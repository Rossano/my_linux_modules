#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>    // printk()
#include <linux/slab.h>        // kmalloc()
#include <linux/fs.h>        // anything
#include <linux/errno.h>    // Error codes
#include <linux/types.h>    // size_t
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    // 0_ACCMODE
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>    // copy_*_user()

#include "scull.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rossano Pantaleoni");

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = NUM_DEVICES;
int scull_quantum = QUANTUM_SIZE;
int scull_qset = SCULL_QSET;

// Module Parameters
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
  int devno = MKDEV(scull_major, scull_minor + index);
  int err;
 
  cdev_init(&dev->cdev, &scull_fops);    // here is the link between the CDEV and fops structures
 
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &scull_fops;
  err = cdev_add(&dev->cdev, devno, 1);    // Add the device
  if (err)
  {
    printk(KERN_ALERT"Error %d adding scull%d", err, index);
  }
  else
  {
    printk(KERN_NOTICE"Added scull%d", index);
  }
}

int scull_open(struct inode *inode, struct file *pfile)
{
  struct scull_dev *dev;        // Char device

  // Since we need cdev inside dev but we want to keep the higher container we use the kernel
  // macro container_of
  dev = container_of(inode->i_cdev, struct scull_dev, cdev);
  pfile->private_data = dev;        // for other methods
 
  // Now trim to 0 the lenght of the device if open in write only
  if ((pfile->f_flags & O_ACCMODE) == O_WRONLY)
  {
    scull_trim(dev);            // ignore errors
  }
 
  return 0;
}

int scull_release(struct inode *inode, struct file *pfile)
{
  return 0;
}

int scull_trim(struct scull_dev *dev)
{
  struct scull_qset *qset, *next;
  int qset_null = dev->qset;            // qset not NULL
  int i;
 
  // Parse all the device and free the memory
  for(qset=dev->data; qset; qset=next)
  {
    if (qset->data)
    {
      for (i=0; i<qset_null; i++) kfree(qset->data[i]);
      kfree(qset->data);
      qset->data = NULL;
    }
    next = qset->next;
    kfree(qset);
  }
  // Updata value
  dev->size = 0;
  dev->quantum = scull_quantum;
  dev->qset = scull_qset;
  dev->data = NULL;
 
  return 0;
}

// Align to the right qset
struct scull_qset * scull_align(struct scull_dev *dev, int pd)
{
  struct scull_qset * qs = dev->data;
 
  if (!qs)
  {
    qs = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
    if (qs == NULL) return NULL;
    memset(qs, 0, sizeof(struct scull_qset));
  }
 
  while (pd--)
  {
    if (!qs->next)
    {
      qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
      if (qs->next == NULL) return NULL;
      memset(qs->next, 0, sizeof(struct scull_qset));
    }
    qs = qs->next;
    continue;
  }
 
  return 0;
}

void scull_cleanup_module(void)
{
  int i;
  dev_t devno = MKDEV(scull_major, scull_minor);
 
  if(scull_devices)
  {
    for (i=0; i<scull_nr_devs; i++)
    {
      scull_trim(scull_devices + i);
      cdev_del(&scull_devices[i].cdev);
    }
    kfree(scull_devices);
  }
}
 
ssize_t scull_read(struct file *pfile, char __user *buf, size_t count, loff_t *ofs)
{
  struct scull_dev *dev = pfile->private_data;
  struct scull_qset *qs;
  int quantum = dev->quantum;
  int qset = dev->qset;
  int itemsize = quantum * qset;
  ssize_t retval = 0;
  int item, s_ofs, q_ofs, rem;        // Data for positioning
 
  // Lock the resource
  if (mutex_lock_interruptible(&dev->mutex)) return -ERESTARTSYS;
 
  if (*ofs >= dev->size) goto out;
 
  if (*ofs * count > dev->size) count = dev->size - *ofs;
 
  item = (long)*ofs / itemsize;        // Find qset
  rem = (long)*ofs % itemsize;
  s_ofs = rem / quantum;        // Find item in qset
  q_ofs = rem % quantum;        // Offset in item
 
  // Goto the right quantum_set
  qs = scull_align(dev, item);
 
  // Checks for errors
  if (qs == NULL || !qs->data || !qs->data[s_ofs]) goto out;
 
  // Read only up to the end of this quantum
  if (count > quantum - q_ofs) count = quantum - q_ofs;
 
  // Read the data
  if (copy_to_user(buf, qs->data[s_ofs]+q_ofs, count))
  {
    retval = -EFAULT;
    goto out;
  }
 
  // Update the file offset
  *ofs += count;
  retval = count;
 
out:
  mutex_unlock(&dev->mutex);        // Unlock the mutex previously locked
  return retval;
}

ssize_t scull_write (struct file *pfile, const char __user *buf, size_t count, loff_t *ofs)
{
  struct scull_dev *dev = pfile->private_data;
  struct scull_qset *qs;
  int quantum = dev->quantum;
  int qset = dev->qset;
  int itemsize = qset * quantum;
  int item, rem, s_ofs, q_ofs;
  ssize_t retval = -ENOMEM;
 
  if (mutex_lock_interruptible(&dev->mutex)) return -ERESTARTSYS;
     
  item = (long)*ofs / itemsize;
  rem = (long)*ofs % itemsize;
  s_ofs = rem / quantum;
  q_ofs = rem % quantum;
 
  qs = scull_align(dev, item);
 
  if (!qs->data)
  {
    // Allocate new memory for new data
    qs->data = kmalloc(sizeof(char *)*qset, GFP_KERNEL);
    if (!qs->data) goto out;
    memset (qs->data, 0, qset * sizeof(char *));
  }
 
  if (!qs->data[s_ofs])
  {
    // New quantum
    qs->data[s_ofs] = kmalloc(quantum, GFP_KERNEL);
    if (!qs->data[s_ofs])
    {
      goto out;
    }
  }
 
  // Write only to the end of the quantum
  if (count > quantum - q_ofs)
  {
    count = quantum - q_ofs;
  }
 
  // Write the data
  if (copy_from_user(qs->data[s_ofs]+q_ofs, buf, count))
  {
    retval = -EFAULT;
    goto out;
  }
 
  *ofs += count;
  retval = count;
 
  // Update new sizes
  if (dev->size < *ofs)
  {
    dev->size = *ofs;
  }
 
out:
  mutex_unlock(&dev->mutex);
  return retval;
}

long scull_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
  return 0;
}

loff_t scull_llseek(struct file *pfile, loff_t off, int whence)
{
  return 0;
}

static int scull_init_module(void)
{
 
#ifdef __DEBUG__
  //printk (KERNEL_NOTICE "scull: Ciao inizializazione scull\n");
  printk (KERN_NOTICE"scull: Ciao inizializazione scull\n");
#endif
 
  int result, i;
  dev_t dev = 0;
 
  if (scull_major)
  {
    dev = MKDEV(scull_major, scull_minor);
    result = register_chrdev_region(dev, scull_nr_devs, "scull");
  }
  else
  {
    result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
    scull_major = MAJOR(dev);
  }
 
  if (result < 0)
  {
    printk(KERN_WARNING"scull: cannot open major %d\n", scull_major);
    return result;
  }
 
  scull_devices = kmalloc(scull_nr_devs*sizeof(struct scull_dev), GFP_KERNEL);
  if (!scull_devices)
  {
    result = -ENOMEM;
    goto fail;
  }
  memset(scull_devices, 0, scull_nr_devs*sizeof(struct scull_dev));
 
  for(i=0; i<scull_nr_devs; i++)
  {
    scull_devices[i].quantum = scull_quantum;
    scull_devices[i].qset = scull_qset;
    mutex_init(&scull_devices[i].mutex);
    scull_setup_cdev(&scull_devices[i], i);
  }
 
  dev = MKDEV(scull_major, scull_minor + scull_nr_devs);
//  dev += scull_p_init(dev);
//  dev += scull_access_init(dev);
 
  return 0;
 
fail:
  scull_cleanup_module();
  return result;
}

static int scull_exit(void)
{
#ifdef __DEBUG__
  //printk (KERNEL_INFO "scull: Addio");
  printk (KERN_NOTICE"scull: Addio");
#endif 
  return 0;
}

init_module(scull_init_module);
exit_module(scull_exit);
/*
 *    scull.h
*/

#ifndef __SCULL_H__
#define __SCULL_H__

#include <linux/ioctl.h>

#define QUANTUM_SIZE	4000
//#define ARRAY_SIZE       
#define NUM_DEVICES		4
#define SCULL_QSET		1000
#define SCULL_MAJOR		0

/*
 * Data structures
 */

struct scull_qset {
  void **data;
  struct scull_qset *next;
};

struct scull_dev {
  struct scull_qset *data;    // Pointer to quantum set FIFO
  int quantum;            // Quantum size
  int qset;            // Array size
  unsigned long size;        // Amount of stored data
  unsigned int access_key;    // For sculluid & scullpriv
  struct mutex mutex;        // For access sync
  struct cdev cdev;        // Char device structure
};

struct scull_dev *scull_devices;

/*
 * Function Prototypes
 */
int scull_open(struct inode *inode, struct file *pfile);
int scull_release(struct inode *inode, struct file *pfile);
int scull_trim(struct scull_dev *dev);
//int scull_trim(struct scull_dev *dev);
ssize_t scull_read(struct file *pfile, char __user *buf, size_t count, loff_t *ofs);
ssize_t scull_write(struct file *pfile, const char __user *buf, size_t count, loff_t *ofs);
loff_t scull_llseek(struct file *pfile, loff_t off, int whence);
long scull_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg);

/*
 * File Operations Structure
 */
struct file_operations scull_fops = {
  .owner = THIS_MODULE,
  .llseek = scull_llseek,
  .read = scull_read,
  .write = scull_write,
  .unlocked_ioctl = scull_ioctl,
  .open = scull_open,
  .release = scull_release,
//  .flush = scull_flush,
//  .fsync = scull_fsync
};

#endif

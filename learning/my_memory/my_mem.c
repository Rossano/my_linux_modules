/*
 * my_mem.c
 *
 *  Created on: Oct 23, 2014
 *      Author: ross
 */

#include <linux/init.h>
#include <linux/configfs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
//#include <asm/system.h>
#include <asm/uaccess.h>

#include "my_mem.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rossano Pantaleoni 2014");

int mem_init(void)
{
	int result;

	//result = register_chrdev(mem_major, "my_mem", &mem_fops);
	result = register_chrdev(0, "my_mem", &mem_fops);

	if(result < 0)
	{
		printk(KERN_ALERT"Cannot obtain major\n");
		return result;
	}

	mem_buf = kmalloc(LEN_BUFFER, GFP_KERNEL);
	if(!mem_buf)
	{
		result = -ENOMEM;
		goto fail;
	}

	memset(mem_buf, 0, LEN_BUFFER);
	printk(KERN_ALERT"Insert my_memory module with major = %d\n", mem_major);
	return 0;

fail:
	mem_exit();
	return result;
}

void mem_exit(void)
{
	unregister_chrdev(mem_major, "my_memory");
	if(mem_buf)
	{
		kfree(mem_buf);
		printk(KERN_ALERT"Removing my_memory module\n");
	}
}

int mem_open(struct inode *pinode, struct file *pfile)
{
	return 0;
}

int mem_release(struct inode *pinode, struct file *pfile)
{
	return 0;
}

ssize_t mem_write(struct file *pfile, const char *buf, size_t len, loff_t *ofs)
{
	char *ch;
	int bytes_to_write;

	if (len <= 0)
	{
		printk(KERN_ALERT"my_mem trying to write NULL or inconsistend data\n");
		return 0;
	}
	if (len < LEN_BUFFER)
	{
		bytes_to_write = len; //LEN_BUFFER;
	}
	else
	{
		bytes_to_write = len - LEN_BUFFER;
	}
	//ch = buf + len - 1;
	ch = buf + bytes_to_write;
	copy_from_user(mem_buf, ch, bytes_to_write); //1);
	return bytes_to_write; //1;
}

ssize_t mem_read(struct file *pfile, char __user *buf, size_t len, loff_t * ofs)
{
	int count = 0;
	if (len > LEN_BUFFER)
	{
		len = LEN_BUFFER;
	}
	copy_to_user(buf, mem_buf, len); //1);

	/*
	if(*ofs == 0)
	{
		(*ofs)++;
		return 1;
	}
	else
	{
		return 0;
	}
	*/
	return len;
}

module_init(mem_init);
module_exit(mem_exit);

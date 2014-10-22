/*
 * myCdev.c
 *
 *  Created on: Oct 10, 2014
 *      Author: ross
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple char device");
MODULE_AUTHOR("ross");

#define BUFFER_LEN	100
#define _MAJOR		89
#define _MINOR		0

static char msg[BUFFER_LEN];
static short readPos = 0;
static int times = 0;

static int dev_open(struct inode*, struct file *);
static int dev_rls(struct inode*, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
		.read = dev_read,
		.write = dev_write,
		.open = dev_open,
		.release = dev_rls,
};

int init_mod(void)
{
	int t = register_chrdev(_MAJOR, "myCdev", &fops);

	if(t < 0)
	{
		printk(KERN_ALERT"myCdev: Device registration failed\n");
	}
	else
	{
		printk(KERN_ALERT"myCdev: Device registred\n");
	}

	return t;
};

void cleanup_mod(void)
{
	unregister_chrdev(_MAJOR, "myCdev");
}

static int dev_open(struct inode *pinode, struct file *pfile)
{
	times++;
	printk(KERN_ALERT"Device opened %d times\n", times);
	return 0;
}


static int dev_rls(struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT"Device closed\n");
	return 0;
}

static ssize_t dev_read(struct file *pfile, char *buf, size_t len, loff_t *ofs)
{
	short count = 0;
	while (len && (msg[readPos] != 0))
	{
		put_user(msg[readPos], buf++);
		count++;
		len--;
		readPos++;
	}
	return count;
}

static ssize_t dev_write(struct file *pfile, const char *buf, size_t len, loff_t *ofs)
{
	short ind = len-1;
	short count = 0;

	memset(msg, 0, BUFFER_LEN);
	readPos = 0;
	while (len > 0)
	{
		msg[count++] = buf[ind--];
		len--;
	}
	return count;
}

module_init(init_mod);
module_exit(cleanup_mod);

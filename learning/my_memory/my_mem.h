/*
 * my_mem.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ross
 */

#ifndef MY_MEM_H_
#define MY_MEM_H_

#define LEN_BUFFER		5
#define _MAJOR 			91
#define _MINOR 			0

static int mem_major = _MAJOR;
static int mem_minor = _MINOR;

char *mem_buf;

int mem_open(struct inode *, struct file *);
int mem_release(struct inode *, struct file *);
ssize_t mem_read(struct file *, char __user *, size_t, loff_t *);
ssize_t mem_write(struct file *, const char __user *, size_t, loff_t *);
int mem_init(void);
void mem_exit(void);

struct file_operations mem_fops = {
		read : mem_read,
		write : mem_write,
		open : mem_open,
		release : mem_release,
};
#endif /* MY_MEM_H_ */

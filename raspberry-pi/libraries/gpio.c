/*
 * gpio.c
 *
 *  Created on: Jan 22, 2015
 *      Author: ross
 */

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include "gpio.h"

int mmap_fd = 0;
volatile unsigned *PI_gpio = NULL;		// volatile to avoid compiler optimization
unsigned long long int GPIO_used = 0;

unsigned *PI_IOmmap(off_t where)
{
	void *map;

	// if /dev/mem is not already open, open it
	if(mmap_fd < 0)
	{
		// Open /dev/mem
		if((mmap_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) error("cannot open /dev/mem");
	}

	// Link GPIO register in user memory
	map = mmap(
			NULL,						// location left to the kernel
			BLOCK_SIZE,
			PROT_READ | PROT_WRITE,		// R/W
			MAP_SHARED,					// share memory with other processes
			mmap_fd,					// user memory
			where						// address to acces
	);

	if (map == MAP_FAILED) error("mmap failed!");

	return (unsigned *) map;
}

void PI_GPIO_config(int port, int mode)
{
	int reg;
	int offset;
	int temp;

	// First of all launch mmap to avoid forgetting it
	if(PI_gpio == NULL) PI_gpio = PI_IOmmap(GPIO_BASE);

	if((port >= 0) && (port < 32) && (mode >= 0) && (mode < 8))
	{
		// The code works until 54 GPIO, but macros support only 32
		reg = port/10;
	}

	// Read the register
	temp = *(PI_gpio + reg);
	// Carry out the offset
	offset = port - (reg * 10);
	offset *= 3;					// 3 bits per port (GPIO)
	// Delete previous bits
	temp &= ~(7 << offset);
	// Add desired mode
	temp |= mode << offset;
	// Write back result
	*(PI_gpio + reg) = temp;

#ifndef GPIO_NO_ATEXIT
	if(mode != 0)
	{
		// Register the function for disabling
		if(mode != 0)
		{
			// Register the functions for disabling
			if (GPIO_used == 0)
			{
				atexit(GPIO_restore);
				// Register the exit functions for all exiting signals
				signal(SIGHUP, exit);
				signal(SIGINT, exit);
				signal(SIGQUIT, exit);
				signal(SIGSTOP, exit);
				signal(SIGTERM, exit);
				signal(SIGABRT, exit);
				signal(SIGKILL, exit);
			}
			GPIO_used |= 1UL << port;
		}
#endif
	}
	else
	{
		fprintf(stderr, "bad port number (%d) or mode (%d)\n", port, mode);
		exit(EXIT_FAILURE);
	}
}

#ifndef GPIO_NO_ATEXIT

void GPIO_restore()
{
	unsigned int GPIO_cache = GPIO_used;
	unsigned int mask = 1;
	int i = 0;

	fputs("\nRestore GPIO", stderr);
	while(GPIO_cache)
	{
		if(GPIO_cache & mask)
		{
			PI_GPIO_config(i, BCM_GPIO_IN);
			GPIO_cache &= ~mask;
			fprintf(stderr, "%d", i);
		}
		mask += mask;			// shift to the left
		i++;
	}
	fputs("in inputs\n", stderr);
}

#endif

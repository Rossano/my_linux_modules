/*
 * gpio.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ross
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_BASE			(0x20200000)	// GPIO memory controller
#define BLOCK_SIZE			(4 * 1024)		// MMU page size

#define RPI_GPSET0			(7)
#define RPI_GPCLR0			(10)
#define RPI_GPLEV0			(13)
#define GPIO_SET			*(PI_gpio + RPI_GPSET0)
#define GPIO_CLR			*(PI_gpio + RPI_GPCLR0)
#define GPIO_LEV			*(PI_gpio + RPI_GPLEV0)
#define GPIO_SET_N(N)		GPIO_SET = (1 << N)
#define GPIO_CLR_N(N)		GPIO_CLR = (1 << N)
#define GPIO_LEV_N(N)		(((GPIO_LEV) >> N) & 1)

#define BCM_GPIO_IN			(0)
#define BCM_GPIO_OUT		(1)
#define BCM_GPIO_ALT0		(4)
#define BCM_GPIO_ALT1		(5)
#define BCM_GPIO_ALT2		(6)
#define BCM_GPIO_ALT3		(7)
#define BCM_GPIO_ALT4		(3)
#define BCM_GPIO_ALT5		(2)
#define BCM_GPIO_ALT6		(7)


// Provide error function if not already provided
#ifdef PI_GPIO_ERR
#include <errno.h>

// Output with contextual error message
void error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void err(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

#endif

//volatile unsigned *PI_gpio = NULL;		// volatile to avoid compiler optimization
extern volatile unsigned *PI_gpio;		// volatile to avoid compiler optimization

#ifndef GPIO_NO_ATEXIT

//unsigned long long int GPIO_used = 0;
extern unsigned long long int GPIO_used;

void GPIO_restore();

#include <signal.h>

#endif

unsigned *PI_IOmmap(off_t);
void PI_GPIO_config(int, int);


#endif /* GPIO_H_ */

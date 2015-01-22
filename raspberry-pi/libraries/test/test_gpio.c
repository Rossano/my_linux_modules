/*
 * test.c
 *
 *  Created on: Jan 22, 2015
 *      Author: ross
 *
 *      HW: short GPIO 4 and GPIO 25
 *      Test is performed as follow:
 *      GPIO 4 is printed and checked: if 1 GPIO 25 is set to 0
 *      and if 0 GPIO 25 is set to 1.
 *      Since the 2 GPIO are shorted, we should see GPIO 4 value changing
 *      every second
 */

#include <stdio.h>
#include <unistd.h>
#include "../gpio.h"

#define PI_GPIO_ERR

#define PI_IN		(4)
#define PI_OUT		(25)

int main(int argc, char *argv[])
{
	// Configure GPIO
	PI_GPIO_config(PI_IN, BCM_GPIO_IN);
	PI_GPIO_config(PI_OUT, BCM_GPIO_OUT);

	while(1)
	{
		printf("GPIO %d = %d\n", PI_IN, GPIO_LEV_N(PI_IN));
		// Check GPIO IN value and set the GPIO OUT to the other value
		if(GPIO_LEV_N(PI_IN)) GPIO_CLR_N(PI_OUT);
		else GPIO_SET_N(PI_OUT);
		sleep(1);
	}

	return 0;
}

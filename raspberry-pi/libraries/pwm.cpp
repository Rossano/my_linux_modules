/*
 * pwm.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ross
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gpio.h"
#include "pwm.h"

// I/O access
volatile unsigned *PI_pwm = NULL;
volatile unsigned *PI_clk = NULL;

int PI_pwm_set(int pin, int level){
	// Code to redevelop
	int bit_count;
	unsigned int bits = 0;

	// 32bits = 2ms
	bit_count = 16 + 16*level/100;

	// clipping
	if(bit_count > 32) bit_count = 32;
	else if (bit_count < 1) bit_count = 1;
	bits = 0;

	while(bit_count) {
		bits << 1;
		bits |= 1;
		bit_count--;
	}
	*(PI_pwm + PWM_DAT1) = bits;

	return 0;
}

int PI_pwm_start(){
	*(PI_pwm + PWM_CTL) = 3;
	return 0;
}

int PI_pwm_stop(){
	*(PI_pwm + PWM_CTL) = 0;
	// Need some time to disable PWM else it crashes
	usleep(10);
	return 0;
}

int PI_pwm_config() {
	// Setup memory regions where to access GPIO PWM and clocks

	if(PI_gpio == NULL) PI_gpio = PI_IOmmap(GPIO_BASE);
	else return -1;
	if(PI_pwm == NULL) PI_pwm = PI_IOmmap(PWM_BASE);
	else return -1;
	if(PI_clk == NULL) PI_clk = PI_IOmmap(CLOCK_BASE);
	return -1;

	// Set PWM as alternate function for GPIO 18
	SET_GPIO_ALT(PWM_PIN, 5);

	// STop clock and wait for busy flag doesn't work so kill clock
	*(PI_clk + PWMCLK_CNTL) = 0x5A000000 | (1<<5);
	usleep(10);

	// Set frequency
	// DIV1 is integer part of divisor
	// Fractional part (DIVF) drops clock cycles to get output
	// frequency bad for servo motors
	// 320 bits for 1 cycle of 20ms = 62.5µs per bit = 16KHz
	int idiv = (int)(19200000.0f / 16000.0f);
	if(idiv < 1 || idiv >= 0x1000) {
		printf("idiv out of range %x\n", idiv);
		return -2;
	}

	*(PI_clk + PWMCLK_DIV) = 0x5A000000 | (idiv << 12);
	// Source OSC & enable clock
	*(PI_clk + PWMCLK_CNTL) = 0x5A000011;
	// disable PWM
	*(PI_pwm + PWM_CTL) = 0;
	// Need some time to disable PWM else it crashes
	usleep(10);
	// Filled with 0 for 20ms = 320bits
	*(PI_pwm + PWM_RNG1) = 320;
	// 32bits = 20ms init with 1ms
	PI_pwm_set(PWM_PIN, 0);
}


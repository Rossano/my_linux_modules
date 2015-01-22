/*
 * pwm.h
 *
 *  Created on: Jan 22, 2015
 *      Author: ross
 */

#ifndef PWM_H_
#define PWM_H_

#define BCM2708_PERIPH_BASE		(0x20000000)
#define PWM_BASE				(BCM2708_PERIPH_BASE + 0x0020C000)
#define CLOCK_BASE				(BCM2708_PERIPH_BASE + 0x00101000)

#define PWM_CTL					0
#define PWM_RNG1				4
#define PWM_DAT1				5
#define PWMCLK_CNTL				40
#define PWMCLK_DIV				41
#define PWM_PIN					18

// I/O access
extern volatile unsigned *PI_pwm;
extern volatile unsigned *PI_clk;

// Function prototypes
int PI_pwm_set(int pin, int level);
int PI_pwm_start();
int PI_pwm_stop();
int PI_pwm_config();

#endif /* PWM_H_ */

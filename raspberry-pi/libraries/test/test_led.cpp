/*
 * test_led.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ross
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../gpio.h"
#include "../pwm.h"

bool running = true;
int wind = 9;
time_t t;

int brigthness();
int flicker();

int main() {
	PI_GPIO_config(BCM_GPIO_OUT, PWM_PIN);
	int err;
	if (!(err = PI_pwm_config())) {
		printf("error during PWM initialization (%d)\n", err);
		return -1;
	}
	printf("start to light-up the LED\npress CTRL+C to stop\n");

	srand((unsigned)time(&t));
	while (running) {
		PI_pwm_start();
		PI_pwm_set(PWM_PIN, brigthness());
		usleep(1000*flicker());
	}
	PI_pwm_stop();
	PI_GPIO_restore();
}

int brigthness() {
	return (5 + rand() % 10);
}

int flicker() {
	return rand() / wind;
}

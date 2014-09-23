/*
 * mvf-pwm-democ.c
 *
 *  Created on: 08-Dec-2013
 *      Author: bhuvan
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "mvf-pwm.h"

#define CPWM 	1
#define EPWM	0

int main(void)
{
	int fileDescriptor = 0, count = 0, i = 0;
	fileDescriptor = open("/dev/pwm", O_RDWR);
	if(fileDescriptor < 0)
	{
		perror("Device open failed");
		return -1;
	}

	pwm_init(fileDescriptor, 3);
	pwm_init(fileDescriptor, 1);
	pwm_config(fileDescriptor, 3, 50, 128, EPWM);

	for(count = 1; count < 5; count++)
	{
		for(i = 3; i<=12; i++)
		{
			pwm_enable(fileDescriptor, 3, 3, i);
			printf("DutyPercent is: %d percent\n", i);
			sleep(1);
		}

		for(i = 12; i>=3; i--)
		{
			pwm_enable(fileDescriptor, 3, 3, i);
			printf("DutyPercent is: %d percent\n", i);
			sleep(1);
		}

		sleep(2);
	}
	pwm_disable(fileDescriptor, 3, 0);

	pwm_config(fileDescriptor, 1, 5000, 128, EPWM);

	for(count = 100; count > 1; count--)
	{
		pwm_enable(fileDescriptor, 1, 0, count);
		sleep(1);
	}
	pwm_disable(fileDescriptor, 3, 0);

	pwm_release(fileDescriptor, 3);
	pwm_release(fileDescriptor, 1);
	close(fileDescriptor);
    return 0;
}

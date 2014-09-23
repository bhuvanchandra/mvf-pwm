/*
 * mvf-pwm.c
 *
 *  Created on: 06-Dec-2013
 *      Author: bhuvan
 */

#include <sys/ioctl.h>

#include "mvf-pwm.h"

int pwm_init(int fileDescriptor, int pwmChipNo)
{
	pwm_ops options;

	options.pwm_chip = pwmChipNo;

	if (ioctl(fileDescriptor, PWM_INIT, &options) == -1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int pwm_config(int fileDescriptor, int pwmChipNo, unsigned long period, int prescale, bool mode)
{
	pwm_ops options;

	options.pwm_chip = pwmChipNo;
	options.prescale = prescale;
	options.period_ns = period;
	options.cpwm = mode;

	if (ioctl(fileDescriptor, PWM_CONFIG, &options) == -1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int pwm_enable(int fileDescriptor, int pwmChipNo, int channel, unsigned long dutyCycle)
{
	pwm_ops options;

	options.pwm_chip = pwmChipNo;
	options.pwm_channel = channel;
	options.dutycycle_ns = dutyCycle;

	if (ioctl(fileDescriptor, PWM_ENABLE, &options) == -1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int pwm_disable(int fileDescriptor, int pwmChipNo, int channel)
{
	pwm_ops options;

	options.pwm_chip = pwmChipNo;
	options.pwm_channel = channel;

	if (ioctl(fileDescriptor, PWM_DISABLE, &options) == -1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
int  pwm_release(int fileDescriptor, int pwmChipNo)
{
	pwm_ops options;

	options.pwm_chip = pwmChipNo;

	if (ioctl(fileDescriptor, PWM_RELEASE, &options) == -1)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

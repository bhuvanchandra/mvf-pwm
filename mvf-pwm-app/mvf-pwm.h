/*
 * mvf-pwm.h
 *
 *  Created on: 06-Dec-2013
 *      Author: bhuvan
 */

#ifndef MVF_PWM_H_
#define MVF_PWM_H_

#include <stdbool.h>

#define PWM_INIT		_IOW('b', 2, pwm_ops *)
#define PWM_CONFIG		_IOW('b', 3, pwm_ops *)
#define PWM_ENABLE		_IOW('b', 4, pwm_ops *)
#define PWM_DISABLE		_IOW('b', 5, pwm_ops *)
#define PWM_RELEASE		_IOW('b', 6, pwm_ops *)

typedef struct
{
	unsigned long period_ns, dutycycle_ns;
	unsigned int prescale, pwm_chip, pwm_channel;
	bool cpwm;
} pwm_ops;

int pwm_init(int fileDescriptor, int pwmChipNo);
int pwm_config(int fileDescriptor, int pwmChipNo, unsigned long period, int prescale, bool mode);
int pwm_enable(int fileDescriptor, int pwmChipNo, int channel, unsigned long dutyCycle);
int pwm_disable(int fileDescriptor, int pwmChipNo, int channel);
int pwm_release(int fileDescriptor, int pwmChipNo);

#endif /* MVF_PWM_H_ */

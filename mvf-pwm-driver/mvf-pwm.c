/* Copyright 2013 Toradex
 *
 * mvf-pwm driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
*/

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/stat.h>
#include <linux/gpio.h>
#include <linux/types.h>
#include <linux/cdev.h> 
#include <linux/device.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/io.h>

#include "mvf-pwm.h"

#define DRIVER_NAME "mvf-pwm"
#define DRV_VERSION "1.0"

static dev_t pwm_dev;   
static struct cdev pwm_cdev; 
static struct class *pwm_cl;
void __iomem *ftm0_base, *ftm1_base, *ftm3_base, *iomux_base;
unsigned long period = 0, duty_cycle = 0;
static unsigned int ps = 0;

struct file_operations pwm_fops = {
	.owner = THIS_MODULE,
	.read = NULL,
	.write = NULL,
	.unlocked_ioctl = pwm_ioctl,
	.open = pwm_open,
	.release = pwm_close,
};

static unsigned long frequency_to_ns(unsigned long freq)
{
	/* nsec = [ (cycles/frequency) * 1000000000 ] */
	unsigned long long ns;
	ns = (1000000000UL / freq);
	return ns;
}

static int pwm_iomux(int chip)
{
	u32 l = 0;
	iomux_base = (uint32_t *)ioremap(IOMUXC_BASE, IOMUX_SIZE);
	if(iomux_base == NULL)
	{
		printk(KERN_ALERT "ioremap for IOMUX failed\n");
		return -1;
	}

	switch(chip)
	{
		case PWM_CHIP0:
			l = __raw_readl(iomux_base + IOMUXC_FTM0_0);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_0));

			l = __raw_readl(iomux_base + IOMUXC_FTM0_1);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_1));

			l = __raw_readl(iomux_base + IOMUXC_FTM0_3);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_3));

			l = __raw_readl(iomux_base + IOMUXC_FTM0_4);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_4));

			l = __raw_readl(iomux_base + IOMUXC_FTM0_5);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_5));

			l = __raw_readl(iomux_base + IOMUXC_FTM0_6);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_6));

			l = __raw_readl(iomux_base + IOMUXC_FTM0_7);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM0_7));
		break;
		
		case PWM_CHIP1:
			l = __raw_readl(iomux_base + IOMUXC_FTM1_0);
			l |= ALT_FN2;
			__raw_writel(l, (iomux_base + IOMUXC_FTM1_0));

			l = __raw_readl(iomux_base + IOMUXC_FTM1_1);
			l |= ALT_FN1;
			__raw_writel(l, (iomux_base + IOMUXC_FTM1_1));
		break;
		
		case PWM_CHIP2:
			printk(KERN_INFO "not available\n");
		break;
	
		case PWM_CHIP3:
			l = __raw_readl(iomux_base + IOMUXC_FTM3_0);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_0));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_1);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_1));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_2);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_2));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_3);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_3));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_4);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_4));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_5);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_5));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_6);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_6));

			l = __raw_readl(iomux_base + IOMUXC_FTM3_7);
			l |= ALT_FN4;
			__raw_writel(l, (iomux_base + IOMUXC_FTM3_7));

		break;
		
		default:
			return ENODEV;
		break;
	}

	return 0;
}

static int pwm_request(struct pwm_device *pwmDev)
{
	switch(pwmDev->chip)
	{
		case PWM_CHIP0:
			ftm0_base = (uint32_t *)ioremap(FTM0_BASE_ADDR, PWM_SIZE);
			pwm_iomux(pwmDev->chip);
		break;
		
		case PWM_CHIP1:
			ftm1_base = (uint32_t *)ioremap(FTM1_BASE_ADDR, PWM_SIZE);
			pwm_iomux(pwmDev->chip);
		break;
		
		case PWM_CHIP2:
			printk(KERN_INFO "not available\n");
		break;
	
		case PWM_CHIP3:
			ftm3_base = (uint32_t *)ioremap(FTM3_BASE_ADDR, PWM_SIZE);
			pwm_iomux(pwmDev->chip);
		break;
		
		default:
			return ENODEV;
		break;
	}
	
	return 0;
}

static int pwm_release(int pwmChip)
{
	u32 reg;

	switch(pwmChip)
	{
		case PWM_CHIP0:
			__raw_writel(0xFF,ftm0_base + PWM_FTM_OUTMASK);
			reg = __raw_readl(ftm0_base + PWM_FTM_SC);
			reg &= ~(0x03 << 3);
			__raw_writel(reg, ftm0_base + PWM_FTM_SC);
			
		break;
		
		case PWM_CHIP1:
			__raw_writel(0xFF,ftm1_base + PWM_FTM_OUTMASK);
			reg = __raw_readl(ftm1_base + PWM_FTM_SC);
			reg &= ~(0x03 << 3);
			__raw_writel(reg, ftm1_base + PWM_FTM_SC);
		break;
		
		case PWM_CHIP2:
			printk(KERN_INFO "not available\n");
		break;
	
		case PWM_CHIP3:
			__raw_writel(0xFF,ftm3_base + PWM_FTM_OUTMASK);
			reg = __raw_readl(ftm3_base + PWM_FTM_SC);
			reg &= ~(0x03 << 3);
			__raw_writel(reg, ftm3_base + PWM_FTM_SC);
		break;
		
		default:
			return ENODEV;
		break;
	}

	return 0;
}

static int pwm_config(struct pwm_device *pwmDev)
{
	u32 reg = 0;
	unsigned long long c = 66;
	unsigned long period_cycles;
	u32 p_s;
	period_cycles = frequency_to_ns(pwmDev->periodValue);
	ps = (pwmDev->prescaleValue);
	p_s =  ps * 1000;
	c = c * period_cycles;
	do_div(c, p_s);
	period_cycles = c;
	
	if(period_cycles > 0xFFFF)
	{
		period_cycles = 0xFFFF;
		printk(KERN_ALERT"required PWM period cycles(%lu) overflow 16-bit counter!\n", period_cycles);	
	}

	switch(pwmDev->chip)
	{
		case PWM_CHIP0:
			switch(pwmDev->prescaleValue)
			{
				case 1:
					ps = PWM_FTMSC_PS1;
				break;

				case 2:
					ps = PWM_FTMSC_PS2;
				break;

				case 4:
					ps = PWM_FTMSC_PS4;
				break;

				case 8:
					ps = PWM_FTMSC_PS8;
				break;

				case 16:
					ps = PWM_FTMSC_PS16;
				break;

				case 32:
					ps = PWM_FTMSC_PS32;
				break;

				case 64:
					ps = PWM_FTMSC_PS64;
				break;

				case 128:
					ps = PWM_FTMSC_PS128;
				break;	
			}
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C0SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C1SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C3SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C4SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C5SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C6SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm0_base + PWM_FTM_C7SC);

			if(pwmDev->cpwm)
			{
				__raw_writel((period_cycles - 1)/2, ftm0_base + PWM_FTM_MOD);
			}
			else
			{
				__raw_writel(period_cycles -1, ftm0_base + PWM_FTM_MOD);
			}

			__raw_writel(0x0, ftm0_base + PWM_FTM_CNTIN);

			reg = __raw_readl(ftm0_base + PWM_FTM_SC);
			reg &= ~((PWM_FTMSC_CLK_MASK << PWM_FTMSC_CLK_OFFSET) | (PWM_FTMSC_PS_MASK << PWM_FTMSC_PS_OFFSET));
			reg |= ((PWM_FTMSC_CLKSYS << PWM_FTMSC_CLK_OFFSET) | ps);
			__raw_writel(reg, ftm0_base + PWM_FTM_SC);

		break;

		case PWM_CHIP1:
			switch(pwmDev->prescaleValue)
			{
				case 1:
					ps = PWM_FTMSC_PS1;
				break;

				case 2:
					ps = PWM_FTMSC_PS2;
				break;

				case 4:
					ps = PWM_FTMSC_PS4;
				break;

				case 8:
					ps = PWM_FTMSC_PS8;
				break;

				case 16:
					ps = PWM_FTMSC_PS16;
				break;

				case 32:
					ps = PWM_FTMSC_PS32;
				break;

				case 64:
					ps = PWM_FTMSC_PS64;
				break;

				case 128:
					ps = PWM_FTMSC_PS128;
				break;
				
			}
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm1_base + PWM_FTM_C0SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm1_base + PWM_FTM_C1SC);

			if(pwmDev->cpwm)
			{
				__raw_writel((period_cycles / 2), ftm1_base + PWM_FTM_MOD);
			}

			else
			{			
				__raw_writel((period_cycles - 1), ftm1_base + PWM_FTM_MOD);
			}

			__raw_writel(0x0, ftm1_base + PWM_FTM_CNTIN);
			reg = __raw_readl(ftm1_base + PWM_FTM_SC);
			reg &= ~((PWM_FTMSC_CLK_MASK << PWM_FTMSC_CLK_OFFSET) | (PWM_FTMSC_PS_MASK << PWM_FTMSC_PS_OFFSET));
			reg |= ((PWM_FTMSC_CLKSYS << PWM_FTMSC_CLK_OFFSET) | ps);
			__raw_writel(reg, ftm1_base + PWM_FTM_SC);
	
		break;
	
		case PWM_CHIP2:
				printk(KERN_INFO "Not available\n");
		break;

		case PWM_CHIP3:
			
			switch(pwmDev->prescaleValue)
			{
				case 1:
					ps = PWM_FTMSC_PS1;
				break;

				case 2:
					ps = PWM_FTMSC_PS2;
				break;

				case 4:
					ps = PWM_FTMSC_PS4;
				break;

				case 8:
					ps = PWM_FTMSC_PS8;
				break;

				case 16:
					ps = PWM_FTMSC_PS16;
				break;

				case 32:
					ps = PWM_FTMSC_PS32;
				break;

				case 64:
					ps = PWM_FTMSC_PS64;
				break;

				case 128:
					ps = PWM_FTMSC_PS128;
				break;
				
			}

			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C0SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C1SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C2SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C3SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C4SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C5SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C6SC);
			__raw_writel(FTM_PWMMODE | FTM_PWM_HIGH_TRUE, ftm3_base + PWM_FTM_C7SC);
			
			if(pwmDev->cpwm)
			{
				__raw_writel((period_cycles / 2), ftm3_base + PWM_FTM_MOD);
			}
			else
			{
				__raw_writel((period_cycles - 1), ftm3_base + PWM_FTM_MOD);				
			}

			__raw_writel(0x0, ftm3_base + PWM_FTM_CNTIN);
			reg = __raw_readl(ftm3_base + PWM_FTM_SC);
			reg &= ~((PWM_FTMSC_CLK_MASK << PWM_FTMSC_CLK_OFFSET) | (PWM_FTMSC_PS_MASK << PWM_FTMSC_PS_OFFSET));
			reg |= ((PWM_FTMSC_CLKSYS << PWM_FTMSC_CLK_OFFSET) | ps);
			__raw_writel(reg, ftm3_base + PWM_FTM_SC);
	
		break;

		default:
			return ENODEV; 
		break;
	}

	period = period_cycles;
	return 0;
}
static int pwm_enable(struct pwm_device *pwmDev)
{
	unsigned long reg;
	unsigned long duty_cycles;
	duty_cycles = period * (pwmDev->dutyCycle);		
	duty_cycles /= 100;

	if(duty_cycles >= 0xFFFF)
	{
		duty_cycles = 0xFFFF - 1;
		printk(KERN_ALERT"required PWM duty cycles(%lu) overflow 16-bit counter!\n", duty_cycles);
	}

	switch(pwmDev->chip)
	{
		case PWM_CHIP0:

		switch(pwmDev->channel)
		{
			case PWM_CHN0:
				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN0_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					/* Center-aligend PWM */
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					/* Edge-aligend PWM */
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C0V);
				}
			break;

			case PWM_CHN1:
				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg |= PWM_CHN1_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);
				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C1V);

				}
			break;

			case PWM_CHN2:
				printk(KERN_INFO "Not available\n");
			break;

			case PWM_CHN3:
				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN3_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);
				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C3V);
				}
			break;

			case PWM_CHN4:
				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN4_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);
				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C4V);
				}
			break;

			case PWM_CHN5:
				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN5_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);
				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C5V);
				}
			break;

			case PWM_CHN6:

				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN6_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C6V);
				}
			break;

			case PWM_CHN7:

				reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN7_EN;
				__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{
					__raw_writel(duty_cycles, ftm0_base + PWM_FTM_C7V);
				}
			break;

			default:
				return ECHRNG;
			break;
		}			
	
		break;
		
		case PWM_CHIP1:

			switch(pwmDev->channel)
			{
				case PWM_CHN0:
					reg = __raw_readl(ftm1_base + PWM_FTM_OUTMASK);
					reg &= PWM_CHN0_EN;
					__raw_writel(reg, ftm1_base + PWM_FTM_OUTMASK);

					if(pwmDev->cpwm)
					{
						/* Center-aligend PWM */
						__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
					}

					else
					{
						__raw_writel(duty_cycles, ftm1_base + PWM_FTM_C0V);
					}
				break;

				case PWM_CHN1:
					reg = __raw_readl(ftm1_base + PWM_FTM_OUTMASK);
					reg &= PWM_CHN1_EN;
					__raw_writel(reg, ftm1_base + PWM_FTM_OUTMASK);

					if(pwmDev->cpwm)
					{
						__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
					}

					else
					{
						__raw_writel(duty_cycles, ftm1_base + PWM_FTM_C1V);
					}
				break;

				default:
					return ECHRNG;
				break;

			}

		break;
		
		case PWM_CHIP2:
			printk(KERN_INFO "Not available\n");
		break;
	
		case PWM_CHIP3:
			switch(pwmDev->channel)
			{
				case PWM_CHN0:
					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg &= PWM_CHN0_EN;
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

					if(pwmDev->cpwm)
					{
						/* Center-aligend PWM */
						__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
					}

					else
					{
						/* Edge-aligend PWM */
						__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C0V);
					}

				break;

			case PWM_CHN1:

				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN1_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);
				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}
				else
				{
					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C1V);
				}

			break;

			case PWM_CHN2:

				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN2_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{


					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C2V);
				}
			break;

			case PWM_CHN3:

				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN3_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{


					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C3V);
				}

				break;

			case PWM_CHN4:

				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN4_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{


					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C4V);
				}

			break;

			case PWM_CHN5:
				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN5_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{


					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C5V);
				}

				break;

			case PWM_CHN6:

				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN6_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{
					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C6V);
				}

			break;

			case PWM_CHN7:

				reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
				reg &= PWM_CHN7_EN;
				__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);

				if(pwmDev->cpwm)
				{
					__raw_writel((duty_cycles / 2), ftm0_base + PWM_FTM_C0V);
				}

				else
				{
					__raw_writel(duty_cycles, ftm3_base + PWM_FTM_C7V);
				}

			break;

			default:
				return ECHRNG;
			break;
		}
		break;
	
		default:
			return ENODEV;
		break;
	}

	return 0;
}

static int pwm_disable(struct pwm_device *pwmDev)
{
	u32 reg;

	switch(pwmDev->chip)
	{
		case PWM_CHIP0:

			switch(pwmDev->channel)
			{
				case PWM_CHN0:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN0_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);

				break;

				case PWM_CHN1:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN1_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);		
			
				break;

				case PWM_CHN2:

					printk(KERN_INFO "Not available\n");

				break;

				case PWM_CHN3:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN3_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);
					
				break;

				case PWM_CHN4:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN4_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);	
				
				break;

				case PWM_CHN5:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN5_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);
					
				break;

				case PWM_CHN6:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN6_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);					

				break;

				case PWM_CHN7:

					reg = __raw_readl(ftm0_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN7_EN);
					__raw_writel(reg, ftm0_base + PWM_FTM_OUTMASK);	
				
				break;
			}	
		
		break;

		case PWM_CHIP1:

			switch(pwmDev->channel)
			{
				case PWM_CHN0:
					reg = __raw_readl(ftm1_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN0_EN);
					__raw_writel(reg, ftm1_base + PWM_FTM_OUTMASK);					
				break;

				case PWM_CHN1:
					reg = __raw_readl(ftm1_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN1_EN);
					__raw_writel(reg, ftm1_base + PWM_FTM_OUTMASK);					
				break;

				default:
					return ECHRNG;
				break;
			}
		break;

		case PWM_CHIP2:
				printk(KERN_INFO "Not available\n");
		break;

		case PWM_CHIP3:

			switch(pwmDev->channel)
			{

				case PWM_CHN0:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN0_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);	
				
				break;

				case PWM_CHN1:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN1_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);	
				
				break;

				case PWM_CHN2:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN2_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);
						
				break;

				case PWM_CHN3:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN3_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);	
				
				break;

				case PWM_CHN4:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN4_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);
					
				break;

				case PWM_CHN5:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN5_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);
				
				break;

				case PWM_CHN6:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN6_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);	
				
				break;

				case PWM_CHN7:

					reg = __raw_readl(ftm3_base + PWM_FTM_OUTMASK);
					reg |= ~(PWM_CHN7_EN);
					__raw_writel(reg, ftm3_base + PWM_FTM_OUTMASK);
					
				break;
			}
		break;
		
		default:
			return ENODEV;
		break;
	}

	return 0;
}

static int pwm_open(struct inode * ino, struct file * filp)
{	
	return 0;
}

static int pwm_close(struct inode * ino, struct file * filp)
{
	return 0;
}

static long pwm_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	pwm_ops pwm_options;
	struct pwm_device pwm;

	switch (cmd)
	{
		case PWM_INIT:

			if (copy_from_user(&pwm_options, (pwm_ops *)arg, sizeof(pwm_options)))
			{
				return -EACCES;
			}

			pwm.chip = pwm_options.pwm_chip;
			pwm_request(&pwm);

		break;	

		case PWM_CONFIG:

			if (copy_from_user(&pwm_options, (pwm_ops *)arg, sizeof(pwm_options)))
			{
				return -EACCES;
			}

			pwm.chip = pwm_options.pwm_chip;
			pwm.prescaleValue = pwm_options.prescale;
			pwm.periodValue = pwm_options.period_ns;
			pwm.cpwm = pwm_options.cpwm;
			pwm_config(&pwm);

		break;

		case PWM_ENABLE:

			if (copy_from_user(&pwm_options, (pwm_ops *)arg, sizeof(pwm_options)))
			{
				return -EACCES;
			}

			pwm.chip = pwm_options.pwm_chip;
			pwm.channel = pwm_options.pwm_channel;
			pwm.dutyCycle = pwm_options.dutycycle_ns;
			pwm_enable(&pwm);

		break;	

		case PWM_DISABLE:

			if (copy_from_user(&pwm_options, (pwm_ops *)arg, sizeof(pwm_options)))
			{
				return -EACCES;
			}		

			pwm.chip = pwm_options.pwm_chip;
			pwm.channel = pwm_options.pwm_channel;
			pwm_disable(&pwm);	
		break;
	
		case PWM_RELEASE:
			if (copy_from_user(&pwm_options, (pwm_ops *)arg, sizeof(pwm_options)))
			{
				return -EACCES;
			}

			pwm.chip = pwm_options.pwm_chip;
			pwm_release(pwm.chip);
		break;

		default:
		    return -EINVAL;
		break;
	}

	return 0;
}

static __init int pwm_init(void)
{
	if (alloc_chrdev_region(&pwm_dev, 0, 1, "pwm") < 0)
	{
		printk(KERN_DEBUG "Failed to register the pwm\n");
		return 1;
	} 

	pwm_cl = class_create(THIS_MODULE, "pwm-dev");

	device_create(pwm_cl, NULL, pwm_dev, NULL, "pwm");

	cdev_init(&pwm_cdev, &pwm_fops);

	if (cdev_add(&pwm_cdev, pwm_dev, 1) < 0)
	{
		printk(KERN_DEBUG "Failed to add cdev\n");
		return 1;
	} 

	return 0;
}

static __exit void pwm_exit(void)
{		
	cdev_del(&pwm_cdev);
	device_destroy(pwm_cl, pwm_dev);
	class_destroy(pwm_cl);
	unregister_chrdev_region(pwm_dev, 1);

	iounmap(ftm0_base);
	iounmap(ftm1_base);
	iounmap(ftm3_base);
	iounmap(iomux_base); 
}

module_init(pwm_init);
module_exit(pwm_exit);

MODULE_AUTHOR("Bhuvan Chandra.DV");
MODULE_DESCRIPTION(DRIVER_NAME);
MODULE_LICENSE("GPL v2");
MODULE_VERSION(DRV_VERSION);

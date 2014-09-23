/* Vybrid FlexTimerModule register definitions */

#define IOMUXC_BASE	0x40048000
#define IOMUXC_FTM0_0	0x58
#define IOMUXC_FTM0_1	0x5C
//#define IOMUXC_FTM0_2	0x60	
#define IOMUXC_FTM0_3	0x64
#define IOMUXC_FTM0_4	0x68
#define IOMUXC_FTM0_5	0x6C
#define IOMUXC_FTM0_6	0x70
#define IOMUXC_FTM0_7	0x74
#define IOMUXC_FTM1_0	0xB4
#define IOMUXC_FTM1_1	0x7C
#define IOMUXC_FTM3_0	0xFC
#define IOMUXC_FTM3_1	0x100
#define IOMUXC_FTM3_2	0x104
#define IOMUXC_FTM3_3	0x108
#define IOMUXC_FTM3_4	0x10C
#define IOMUXC_FTM3_5	0x110
#define IOMUXC_FTM3_6	0x114
#define IOMUXC_FTM3_7	0x118

#define ALT_FN1		(0x1 << 20)
#define ALT_FN2		(0x2 << 20)
#define ALT_FN3		(0x3 << 20)
#define ALT_FN4		(0x4 << 20)
#define ALT_FN5		(0x5 << 20)
#define ALT_FN6		(0x6 << 20)
#define ALT_FN7		(0x7 << 20)
#define IOMUX_SIZE	0x3ac
#define PWM_SIZE	0x00001000

#define FTM0_BASE_ADDR		0x40038000
#define FTM1_BASE_ADDR		0x40039000
#define FTM2_BASE_ADDR		0x400b8000
#define FTM3_BASE_ADDR		0x400b9000

#define PWM_FTM_SC		0x00	/* Status and Control */
#define	PWM_FTM_CNT 		0x04	/* Counter */
#define	PWM_FTM_MOD 		0x08	/* Modulo */

#define	PWM_FTM_C0SC 		0x0c	/* Channel 0 Status and Control */
#define	PWM_FTM_C0V 		0x10	/* Channel 0 Value */
#define	PWM_FTM_C1SC 		0x14	/* Channel 1 Status and Control */
#define	PWM_FTM_C1V 		0x18	/* Channel 1 Value */
#define	PWM_FTM_C2SC 		0x1c	/* Channel 2 Status and Control */
#define	PWM_FTM_C2V 		0x20	/* Channel 2 Value */
#define	PWM_FTM_C3SC 		0x24	/* Channel 3 Status and Control */
#define	PWM_FTM_C3V 		0x28	/* Channel 3 Value */
#define	PWM_FTM_C4SC 		0x2c	/* Channel 4 Status and Control */
#define	PWM_FTM_C4V 		0x30	/* Channel 4 Value */
#define	PWM_FTM_C5SC 		0x34	/* Channel 5 Status and Control */
#define	PWM_FTM_C5V 		0x38	/* Channel 5 Value */
#define	PWM_FTM_C6SC 		0x3c	/* Channel 6 Status and Control */
#define	PWM_FTM_C6V 		0x40	/* Channel 6 Value */
#define	PWM_FTM_C7SC 		0x44	/* Channel 7 Status and Control */
#define	PWM_FTM_C7V 		0x48	/* Channel 7 Value */

#define	PWM_FTM_CNTIN 		0x4c	/* Counter Initial Value */
#define	PWM_FTM_STATUS 		0x50	/* Capture and Compare Status */
#define	PWM_FTM_MODE 		0x54	/* Features Mode Selection */
#define	PWM_FTM_SYNC 		0x58	/* Syncronization */
#define	PWM_FTM_OUTINIT 	0x5c	/* Initial State For Channels Output */
#define	PWM_FTM_OUTMASK 	0x60	/* Outut Mask */
#define	PWM_FTM_COMBINE 	0x64	/* Function for linked channels */
#define	PWM_FTMDEADTIME 	0x68	/* Deadtime insertion contro */
#define	PWM_FTM_EXTTRIG 	0x6c	/* PWM_FTM external trigger */
#define	PWM_FTM_POL 		0x70	/* Channels ploarity */
#define	PWM_FTMFMS 		0x74	/* Fault mode status */
#define	PWM_FTM_FILTER 		0x78	/* Input capture filter control */
#define	PWM_FTM_FLTCTRL 	0x7c	/* Fault control */
#define	PWM_FTMQDCTRL 		0x80	/* Quadrature decoder control and status */
#define	PWM_FTM_CONF 		0x84	/* Configuration */
#define	PWM_FTM_FLTPOL 		0x88	/* PWM_FTM fault input polarity */
#define	PWM_FTM_SYNCONF 	0x8c	/* Synchronization configuration */
#define	PWM_FTM_INVCTRL		0x90	/* PWM_FTM inverting control */
#define	PWM_FTM_SWOCTRL 	0x94	/* PWM_FTM software output control */
#define	PWM_FTM_PWMLOAD 	0x98	/* PWM_FTM PWM load */

#define PWM_MODE_EPWM		0x01	/* Edge alligned PWM */	
#define PWM_MODE_CPWM		0x02	/* Center alligned PWM */
#define PWM_FTMCnSC_MSB		(0x1 << 5)
#define PWM_FTMCnSC_ELSB	(0x1 << 3)
#define FTM_PWMMODE		(PWM_FTMCnSC_MSB)
#define FTM_PWM_HIGH_TRUE	(PWM_FTMCnSC_ELSB)

#define PWM_FTMSC_CLK_MASK	0x03
#define	PWM_FTMSC_CLK_OFFSET	3
#define PWM_FTMSC_CLKSYS	0x01
#define PWM_FTMSC_PS_MASK	0x7
#define PWM_FTMSC_PS_OFFSET	0
#define PWM_FTMSC_PS1		0x0
#define PWM_FTMSC_PS2		0x1
#define PWM_FTMSC_PS4		0x2
#define PWM_FTMSC_PS8		0x3
#define PWM_FTMSC_PS16		0x4
#define PWM_FTMSC_PS32		0x5
#define PWM_FTMSC_PS64		0x6
#define PWM_FTMSC_PS128		0x7

#define PWM_CHN0_EN		~(0x1 << 0)	/* Channel 0 enable */
#define PWM_CHN1_EN		~(0x1 << 1)	/* Channel 1 enable */
#define PWM_CHN2_EN		~(0x1 << 2)	/* Channel 2 enable */
#define PWM_CHN3_EN		~(0x1 << 3)	/* Channel 3 enable */
#define PWM_CHN4_EN		~(0x1 << 4)	/* Channel 4 enable */
#define PWM_CHN5_EN		~(0x1 << 5)	/* Channel 5 enable */
#define PWM_CHN6_EN		~(0x1 << 6)	/* Channel 6 enable */
#define PWM_CHN7_EN		~(0x1 << 7)	/* Channel 7 enable */

enum pwm_chip{
	PWM_CHIP0,
	PWM_CHIP1,
	PWM_CHIP2,
	PWM_CHIP3,
};

enum pwm_channel{
	PWM_CHN0,
	PWM_CHN1,
	PWM_CHN2,
	PWM_CHN3,
	PWM_CHN4,
	PWM_CHN5,
	PWM_CHN6,
	PWM_CHN7,
};

typedef struct
{
	unsigned long period_ns, dutycycle_ns;	
	unsigned int prescale, pwm_chip, pwm_channel;
	bool cpwm;
} pwm_ops;

#define PWM_INIT		_IOW('b', 2, pwm_ops *)
#define PWM_CONFIG		_IOW('b', 3, pwm_ops *)
#define PWM_ENABLE		_IOW('b', 4, pwm_ops *)
#define PWM_DISABLE		_IOW('b', 5, pwm_ops *)
#define PWM_RELEASE		_IOW('b', 6, pwm_ops *)

struct pwm_device
{
	enum pwm_chip chip;
	enum pwm_channel channel;
	unsigned int prescaleValue;
	unsigned long periodValue;
	unsigned long dutyCycle;
	bool cpwm;
};

static int pwm_open(struct inode * ino, struct file * filp);
static int pwm_close(struct inode * ino, struct file * filp);
static long pwm_ioctl(struct file *f, unsigned int cmd, unsigned long arg);

static int pwm_config(struct pwm_device *pwmDev);
static int pwm_enable(struct pwm_device *pwmDev);
static int pwm_disable(struct pwm_device *pwmDev);

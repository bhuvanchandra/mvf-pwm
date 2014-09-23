CROSS_COMPILE ?= ${CROSS_COMPILE}
ARCH        ?= arm
#Add appropriate kernel source tree path
SOURCE_DIR ?= $(HOME)/linux-toradex

AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
STRIP       = $(CROSS_COMPILE)strip
OBJCOPY     = $(CROSS_COMPILE)objcopy
OBJDUMP     = $(CROSS_COMPILE)objdump

obj-m += mvf-pwm.o
ccflags-y += -I$(SOURCE_DIR)/arch/arm


all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(SOURCE_DIR) M=$(PWD) modules

clean:
	rm -f *.o *.ko *.symvers *.mod.* *.order *.symvers *~ 

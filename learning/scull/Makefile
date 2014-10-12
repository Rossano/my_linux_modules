# KDIR := /lib/modules/`uname -r`/build
ifeq ($(ARCH),arm)
	KDIR := ~/projects/raspberry/linux
	CWD := ../../scull # `pwd`
	CCPREFIX := ~/projects/raspberry/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-
else
	KDIR := /lib/modules/`uname -r`/build
	CWD := `pwd`
endif

obj-m += scull.o

modules:
ifeq (${ARCH},arm)
#    make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C /home/cowboy/rpi/linux-rpi-3.2.27 M=$(PWD) modules
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C ${KDIR} M=${CWD} modules
else
	make -C${KDIR} M=${CWD} modules
endif

clean:
	make -C ${KDIR} M=$(CWD) clean
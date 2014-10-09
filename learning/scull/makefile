# KDIR := /lib/modules/`uname -r`/build
KDIR := ~/projects/raspberry/linux
CWD := ../../scull # `pwd`
CCPREFIX := ~/projects/raspberry/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-

obj-m += scull.o

modules:
#    make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C /home/cowboy/rpi/linux-rpi-3.2.27 M=$(PWD) modules
    make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C ${KDIR} M=${CWD} modules

clean:
    make -C ${KDIR} M=$(CWD) clean
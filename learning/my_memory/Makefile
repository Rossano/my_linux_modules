ifeq (${ARCH},arm)
#Raspberry architecture
	KDIR := ~/projects/raspberry/linux
	CWD := ../../../my_linux_modules/learning/my_memory
	CCPREFIX := ~/projects/raspberry/tools/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-
else	
# x86 architecture
# For linux mint
#KDIR := ~/linux-3.13.1
# For linux OpenSUSE
KDIR := /usr/src/linux
CWD := `pwd`
endif

MODULE := my_mem#ory
_MAJOR := 91
_MINOR := 0

obj-m += $(MODULE).o

all:
ifeq (${ARCH},arm)
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C ${KDIR} M=${CWD} modules
	${CCPREFIX}gcc -o my_mem_test test.c
else
	make -C ${KDIR} M=${CWD} modules
	gcc -o my_mem_test test.c
endif
	
	
modules:
ifeq (${ARCH},arm)
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C ${KDIR} M=${CWD} modules
else
	make -C ${KDIR} M=${CWD} modules
endif
	
clean:
	make -C ${KDIR} M=${CWD} clean
	rm -f test
	
install:
	sudo insmod $(MODULE).ko $*
	major=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)
	sudo mknod /dev/$(MODULE) c $major $(_MINOR)
	sudo chmod a+r+w /dev/$(MODULE)	
	
load:
	sudo insmod $(MODULE).ko
	
unload:
	sudo rmmod $(MODULE)
	
test:
ifeq (${ARCH},arm)
	${CCPREFIX}gcc -o my_mem_test test.c
else
	gcc -o my_mem_test test.c
endif

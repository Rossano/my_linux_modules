obj-m += myCdev.o

# For linux mint
KDIR := ~/linux-3.13.1
CWD := `pwd`
MODULE := myCdev
_MAJOR := 89
_MINOR := 0

all:
	make -C ${KDIR} M=${CWD} modules
#	gcc -o test test.c
	
modules:
	make -C ${KDIR} M=${CWD} modules
	
clean:
	make -C ${KDIR} M=${CWD} clean
	rm -f test
	
install:
	sudo mknod /dev/$(MODULE) c $(_MAJOR) $(_MINOR)
	sudo chmod a+r+w /dev/$(MODULE)
	sudo insmod $(MODULE).ko
	
load:
	sudo insmod $(MODULE).ko
	
unload:
	sudo rmmod $(MODULE)
	
test:
	gcc -o test test.c
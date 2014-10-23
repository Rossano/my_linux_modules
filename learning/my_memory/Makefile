# For linux mint
KDIR := ~/linux-3.13.1
CWD := `pwd`
MODULE := my_mem#ory
_MAJOR := 91
_MINOR := 0

obj-m += $(MODULE).o

all:
	make -C ${KDIR} M=${CWD} modules
	gcc -o test test.c
	
modules:
	make -C ${KDIR} M=${CWD} modules
	
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
	gcc -o test test.c
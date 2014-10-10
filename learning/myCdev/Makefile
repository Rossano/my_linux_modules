obj-m += myCdev.o

# For linux mint
KDIR := ~/linux-3.13.0

all:
	make -C ${KDIR} M=${CWD} modules
	gcc -o test test.c
	
modules:
	make -C ${KDIR} M=${CWD} modules
	
clean:
	make -C ${KDIR} M=${CWD} clean
	
test:
	gcc -o test test.c
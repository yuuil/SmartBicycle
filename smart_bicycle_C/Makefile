#obj-m:= PI3.o
obj-m:= MODULE_PI_C.o

KDIR:=~/linux-rpi/
ARM:= ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o PROCESS_PI_C PROCESS_PI_C.c -pthread
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
copy: 
	scp PROCESS_PI_C.c MODULE_PI_C.ko MODULE_PI_C.c mknod.sh Makefile pi@192.168.0.10:~/smart_bicycle_C/


KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD := $(shell pwd)

obj-m := MODULE_PI_A.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o PROCESS_PI_A PROCESS_PI_A.c -pthread

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
copy: 
	scp PROCESS_PI_A.c MODULE_PI_A.ko MODULE_PI_A.c mknod.sh Makefile pi@192.168.0.7:~/smart_bicycle_A/



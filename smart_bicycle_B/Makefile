KDIR := ~/linux-rpi
ARM := ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
PWD := $(shell pwd)

obj-m := MODULE_PI_B.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) modules
	arm-linux-gnueabi-gcc -o PROCESS_PI_B PROCESS_PI_B.c -pthread

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) $(ARM) clean
copy:
	scp PROCESS_PI_B.c MODULE_PI_B.ko MODULE_PI_B.c mknod.sh Makefile pi@192.168.0.9:~/smart_bicycle_B/



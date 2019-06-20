#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/timer.h>

#include <linux/sched.h>
#include <linux/wait.h>

#include <linux/slab.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL");

#define DEV_NAME "pi_A_dev"

//for motor
#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26


#define STEPS 9

static dev_t dev_num;
static struct cdev * cd_cdev;
spinlock_t my_lock;
int p;


int sensor_data[8] = {0,0,0,0,0,0,0,0};

int step[STEPS][4] = { //for motor
	{1,0,0,0},
	{1,1,0,0},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,1,0},
	{0,0,1,1},
	{0,0,0,1},
	{1,0,0,1}

};


void setStep(int blue, int pink, int yellow, int orange){
	gpio_set_value(PIN1, blue);
	gpio_set_value(PIN2, pink);
	gpio_set_value(PIN3, yellow);
	gpio_set_value(PIN4, orange);
}
static void leftTurn(void){
	//front motor 5.625
	int i=0;
	int j=0;
	
	for(i=0; i<8; i++){
		for(j=0; j<STEPS; j++){

			setStep(step[j][0],step[j][1],step[j][2],step[j][3]);
			mdelay(1);

		}
	}

	gpio_set_value(PIN1, 0);
	gpio_set_value(PIN2, 0);
	gpio_set_value(PIN3, 0);
	gpio_set_value(PIN4, 0);


}

static void rightTurn(void){
	//front motor 5.625
	int i=0;
	int j=0;

	
	for(i=0; i<8; i++){
		for(j=STEPS-1; j>=0; j--){

			setStep(step[j][0],step[j][1],step[j][2],step[j][3]);
			mdelay(1);


		}
	}

	gpio_set_value(PIN1, 0);
	gpio_set_value(PIN2, 0);
	gpio_set_value(PIN3, 0);
	gpio_set_value(PIN4, 0);




}

//signal out
static int signal_write(struct file * file, const char * buf, size_t len, loff_t * loff){
        int ret=0;
	int i,j;
        
	ret = copy_from_user(sensor_data, buf, sizeof(int)*8);
	if(ret<0){
		printk("fail: copy from user\n");
	}

	//motor control
	if(sensor_data[4]==1){
		rightTurn();
	}

	if(sensor_data[5] ==1){
		leftTurn();
	}

	return ret;
	
}

//
static int simple_block_open(struct inode * inode, struct file * file){
	printk("simple block open\n");
	return 0;
}

static int simple_block_release(struct inode * inode, struct file * file){
        return 0;
}


struct file_operations pi_fops =
{

	.write = signal_write,
	.open = simple_block_open,
	.release = simple_block_release
};


static int __init piTwo_init(void){
	printk("Init Module\n");
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");


	alloc_chrdev_region(&dev_num, 0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &pi_fops);
	int add;
	add = cdev_add(cd_cdev, dev_num,1);

	if(add<0){
		printk("fail to add character device\n");
		return -1;
	}
	else{
	printk("success to add character device: %d\n",add);
	}
	return 0;


}

static void __exit piTwo_exit(void){
	printk("Exit Module\n");
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);


	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);

}

module_init(piTwo_init);
module_exit(piTwo_exit);

#include<linux/init.h>
#include<linux/uaccess.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/gpio.h>
#include<linux/interrupt.h>
#include<linux/cdev.h>
#include<asm/delay.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "pi_C_dev"

#define ULTRA_TRIG 17
#define ULTRA_ECHO 18

#define MOTOR_PIN1 6 //B1
#define MOTOR_PIN2 13 //B2
#define MOTOR_PIN3 19 //A1
#define MOTOR_PIN4 26 //A2

static int irq_num;
static int echo_valid_flag = 3;
static int error_count = 0;

static ktime_t echo_start;
static ktime_t echo_stop;

int motor_data[8] = { 0, 0, 0 ,0, 0,0 ,0, 0};
int prev_data[8] = { 0, 0, 0 ,0, 0,0 ,0, 0};
int ultra_data[2] = {0,0};


void backward(void) {
	gpio_set_value(MOTOR_PIN1, 1);
	gpio_set_value(MOTOR_PIN3, 1);
	gpio_set_value(MOTOR_PIN2, 0);
	gpio_set_value(MOTOR_PIN4, 0);
}
void forward(void) {
	gpio_set_value(MOTOR_PIN2, 1);
	gpio_set_value(MOTOR_PIN4, 1);
	gpio_set_value(MOTOR_PIN1, 0);
	gpio_set_value(MOTOR_PIN3, 0);
}
void right(void) {
	gpio_set_value(MOTOR_PIN1, 1);
	gpio_set_value(MOTOR_PIN4, 1);
	gpio_set_value(MOTOR_PIN2, 0);
	gpio_set_value(MOTOR_PIN3, 0);
}
void left(void) {
	gpio_set_value(MOTOR_PIN1, 0);
	gpio_set_value(MOTOR_PIN2, 1);
	gpio_set_value(MOTOR_PIN3, 1);
	gpio_set_value(MOTOR_PIN4, 0);
}
void stop(void) {
	gpio_set_value(MOTOR_PIN1, 0);
	gpio_set_value(MOTOR_PIN2, 0);
	gpio_set_value(MOTOR_PIN3, 0);
	gpio_set_value(MOTOR_PIN4, 0);
}
	

static int pi_C_open(struct inode *inode, struct file *file) {
	
//	enable_irq(irq_num);
//
//

//	echo_valid_flag = 3;
//	printk("flag = %d\n",echo_valid_flag);

	udelay(100);
	
	if( echo_valid_flag == 3) {
		echo_start = ktime_set(0,1);
		echo_stop = ktime_set(0,1);
		echo_valid_flag = 0;
		
		gpio_set_value(ULTRA_TRIG,1);
		udelay(10);
		gpio_set_value(ULTRA_TRIG,0);

		echo_valid_flag = 1;
	}
	

	return 0;

}

static int pi_C_release(struct inode *inode, struct file* file) {
	printk("pi_C close \n");
//	disable_irq(irq_num);
	return 0;
}

static ssize_t pi_C_write(struct file *filp, const char *buf, size_t len, loff_t *lof) {
	int ret;
	ret = copy_from_user(motor_data, buf, len);
	printk("pi_C_WRITE : Get %d %d %d %d\n", motor_data[0], motor_data[1], motor_data[2], motor_data[3]);

	if(motor_data[0] == 1) {
		forward();
	} else if (motor_data[1]) {
		backward();
	} else if (motor_data[2]) {
		left();
	} else if (motor_data[3]) {
		right();
	} else
		stop();

	prev_data[0] = motor_data[0];
	prev_data[1] = motor_data[1];
	prev_data[2] = motor_data[2];
	prev_data[3] = motor_data[3];

	return 0;
}

static ssize_t pi_C_read(struct file *filp, char *buf, size_t len, loff_t *lof) {
	int ret;
	ret = copy_to_user(buf, ultra_data, len);
	printk("pi_C_READ : SEND %d -> %dcm \n",ultra_data[0], ultra_data[1]);

	if(error_count > 100) {
		echo_valid_flag = 3;
		udelay(50);
	}

	if( echo_valid_flag == 3) {
		echo_start = ktime_set(0,1);
		echo_stop = ktime_set(0,1);
		echo_valid_flag = 0;
		
		gpio_set_value(ULTRA_TRIG,1);
		udelay(20);
		gpio_set_value(ULTRA_TRIG,0);
	

		echo_valid_flag = 1;
		error_count = 0;

	}else {
		error_count++;
		printk("error %d \n", error_count);
	}
	return 0;
}

struct file_operations pi_C_fops = {
	.open = pi_C_open,
	.release = pi_C_release,
	.read = pi_C_read,
	.write = pi_C_write
};

static irqreturn_t ultra_isr(int irq, void* dev_id) {

	ktime_t tmp_time;
	s64 time;
	int cm;

	tmp_time = ktime_get();
	if(echo_valid_flag == 1) {
			
		printk("detect UP\n");
		if(gpio_get_value(ULTRA_ECHO) == 1) {
			echo_start = tmp_time;
			echo_valid_flag = 2;
		} 
		
		
		
	} else if (echo_valid_flag == 2) {
		
		printk("detect Down\n");
		if(gpio_get_value(ULTRA_ECHO) == 0) {

			echo_stop = tmp_time;
			echo_valid_flag = 0;
			time = ktime_to_us(ktime_sub(echo_stop, echo_start));
			cm = (int) time / 58;
			printk("%lld us  %d cm\n",time, cm);
			ultra_data[0] = ultra_data[1];
			ultra_data[1] = cm;
			if(cm < 20)
				stop();
			echo_valid_flag = 3;
		}

	}
	return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init pi_C_init(void) {
	int ret;

	printk("Init Moudle \n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &pi_C_fops);
	cdev_add(cd_cdev, dev_num, 1);

	gpio_request_one(ULTRA_TRIG, GPIOF_OUT_INIT_LOW, "ULTRA_TRIG");
	gpio_request_one(ULTRA_ECHO, GPIOF_IN, "ULTRA_ECHO");

	// MOTOR
	gpio_request_one(MOTOR_PIN1, GPIOF_OUT_INIT_LOW, "p1");
        gpio_request_one(MOTOR_PIN2, GPIOF_OUT_INIT_LOW, "p2");
        gpio_request_one(MOTOR_PIN3, GPIOF_OUT_INIT_LOW, "p3");
        gpio_request_one(MOTOR_PIN4, GPIOF_OUT_INIT_LOW, "p4");

	irq_num = gpio_to_irq(ULTRA_ECHO);
	ret = request_irq(irq_num, ultra_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "ULTRA_ECHO", NULL);
	if(ret) {
		printk(KERN_ERR "Uable to rquest IRQ: %d\n", ret);
		free_irq(irq_num,NULL);
	}
	else {
//		disable_irq(irq_num);
	}

	return 0;
}

static void __exit pi_C_exit(void){
	printk("Exit Module \n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);

	free_irq(irq_num, NULL);
	gpio_free(ULTRA_TRIG);
	gpio_free(ULTRA_ECHO);
	gpio_free(MOTOR_PIN1);
        gpio_free(MOTOR_PIN2);
        gpio_free(MOTOR_PIN3);
        gpio_free(MOTOR_PIN4);

}

module_init(pi_C_init);
module_exit(pi_C_exit);


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

#define DEV_NAME "pi_B_dev"

#define MAX_TIMING 85
#define DHT11 16 //not definite
#define SPEAKER 12  //not definite
#define LIGHT 24

//for motor
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4
#define IOCTL_NUM5 IOCTL_START_NUM+5

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_UP _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_START _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define IOCTL_DOWN _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
#define IOCTL_LIGHT_UP _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM4, unsigned long *)
#define IOCTL_LIGHT_DOWN _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM5, unsigned long *)

#define SPI_BIT 8

#define LLED 4
#define RLED 17
#define BIGLED 23

#define STEPS 8

static dev_t dev_num;
static struct cdev * cd_cdev;
spinlock_t my_lock;
int p;
int h;

int humid;
int temp;

int threshold = 30;

int sensor_data[8] = {0,0,0,0,0,0,0,0};

struct box { int value; };

static int dht11_data[5] = {0,};

static struct timer_list my_timer;

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
        switch ( cmd ) {
        case IOCTL_UP:
		gpio_set_value(SPI_BIT, 1);
                break;
	case IOCTL_LIGHT_UP:
		printk("BIG LED UP\n");
		gpio_set_value(BIGLED, 1);
		break;
	case IOCTL_LIGHT_DOWN:
		printk("BIG LED DOWN\n");
		gpio_set_value(BIGLED, 0);
		break;
        case IOCTL_DOWN:
		gpio_set_value(SPI_BIT, 0);
                break;
        default:
                return -1;
        }

        return 0;
}


static void play(int note){
	int i=0;

	for(i=0; i<100; i++){
		gpio_set_value(SPEAKER,1);
		udelay(note);
		gpio_set_value(SPEAKER, 0);
		udelay(note);
	}
}



//backward alarm, for eliese
static void backSound(void){
	
	int noteWhole[] = {758,803,758,803,758,1012,851,955,1136,
				1911,1516,1136,1012, 1516,1203,1012,955,
				1516,758,803,758,803,758,1012,851,955,1136,
				1911,1516,1136,1012, 1516,955,1012,1136};
				
		

		int i;
		gpio_request_one(SPEAKER, GPIOF_OUT_INIT_LOW, "SPEAKER");

		for (i= p ; i < p+4 ; i++) {
			if((i==9)||(i==13)||(i==17)||(i==27)||(i==31)){
				mdelay(100);
			}
			mdelay(100);
			play(noteWhole[i]);
		}

		if(i==34){
		p = 0;
		}
		else{
		p = p + 4;
		}

		gpio_set_value(SPEAKER, 0);

	

}

//for humidity check
static void dht11_read(void){
	int last_state = 1;
	int counter = 0;
	int i=0, j=0;

	dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;

	gpio_direction_output(DHT11, 0);
	gpio_set_value(DHT11, 0);
	mdelay(20);
	gpio_set_value(DHT11, 1);
	udelay(40);
	gpio_direction_input(DHT11);

	for(i=0; i < MAX_TIMING; i++) {
		counter = 0;
		while(gpio_get_value(DHT11) == last_state) {
			counter++; //
			udelay(1);
			if(counter == 255) break;
		}
		last_state = gpio_get_value(DHT11);

		if(counter == 255) break;

		if( (i >= 4) && (i % 2 == 0) ) {
			dht11_data[j / 8] <<= 1;
			if(counter > 16) {
				dht11_data[j / 8] |= 1;
			}
			j++;
		}
	}


	if( (j >= 40) &&
			(dht11_data[4] == ( (dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF ) ) ) {
		if((dht11_data[0] != humid) || (dht11_data[2] != temp)){ 
			humid = dht11_data[0];
			temp = dht11_data[2];
		}
		
		printk("Humidity: %d Temperature = %d C\n", humid,temp);
		
	}
	
	
}


static void homeAlarm(void){

  	int home1[] = {1516,1275,1275,1275,1431,1516};
        int home2[] = {1702,1702,1702,1516,1431,1516,1702};
        int home3[] = {1702,1702,1516,1431,1516,1702,1516};
        int i;

       // if (h == 0) {
                for(i=0; i<6; i++){
                        play(home1[i]);
                        mdelay(30);
                }

       // }else if(h == 1){
                for(i=0; i<7; i++){
                        play(home2[i]);
                        mdelay(30);
                }
       // }else if(h==2){
                for(i=0; i<7; i++){
                        play(home3[i]);
                        mdelay(30);
                }
       // }
	
	gpio_set_value(SPEAKER, 0);
	threshold = 40;
//	h++;
//	if( h == 3)
//		h = 0;
}


/*
static int timer_ioctl(struct box *arg){
	int ret;
	int i = 0;
	int l = 0;
	struct box geted;

	ret = copy_from_user(&geted, arg, sizeof(struct box));

	if( ret < 0) {
		printk("copy_FROM_USEer error ");
	}	
	printk("get light value : %d\n", geted.value);

	if(geted.value < 1600)
		gpio_set_value(BIGLED, 1);
	else 
		gpio_set_value(BIGLED, 0);
	if(humid > 35 || temp > 30) {
		//sounds
		printk("SOUND");

	}
	printk("FINISH IOCTL");	
	return -3;
}
*/
static void my_timer_func(unsigned long data){
        int i = 0;

	printk("timer\n");

 //      	 	my_timer.data = data+1; //not sure
   //     	my_timer.expires = jiffies + (2*HZ);
	
//       	else {
 
 	       for(i = 0; i < 5; i++) {
       		         dht11_read();
       		         mdelay(100);
       	       }

		if(humid > threshold || temp > 33 ) 
			homeAlarm();

      	 	my_timer.data = data+1; //not sure
	        my_timer.expires = jiffies + (10*HZ);
//	}

        add_timer(&my_timer);

}



static void thiefAlarm(void){ 
	int i=0;
	for(i=0; i<9; i++){
			play(758);
			mdelay(100);
		}
		mdelay(100);

}



//signal out
static int signal_write(struct file * file, const char * buf, size_t len, loff_t * loff){
	int i=0;
	int j=0;

      	int ret=0;

        // 8bit copy from user...
	ret = copy_from_user(sensor_data, buf, sizeof(int)*8);
	if(ret<0){
		printk("fail: copy from user\n");
	}
	
	if(sensor_data[7] == 1){
		thiefAlarm();
		return 7;
	}
	if(sensor_data[6] == 1){
		//speaker led off
		gpio_set_value(SPEAKER,0);
		gpio_set_value(LLED, 0);
		gpio_set_value(BIGLED, 0);
		gpio_set_value(RLED, 0);


		return 6;
	}

	

	// left right
	gpio_set_value(LLED, sensor_data[2]);
	gpio_set_value(RLED, sensor_data[3]);



	//forward backward
	if((sensor_data[0] == 1) && (sensor_data[1] == 0)){
		//
	}
	else if((sensor_data[0] == 0) && (sensor_data[1] == 1)){
		backSound();
	}

	if(sensor_data[1] == 0) {
		p = 0 ; // speaker init
	}	


      

        return ret;
}

//
static int simple_block_open(struct inode * inode, struct file * file){
//	printk("simple block open\n");
	return 0;
}

static int simple_block_release(struct inode * inode, struct file * file){
        return 0;
}



struct file_operations pi_fops = 
{
	.unlocked_ioctl = simple_ioctl,
	.write = signal_write,
	.open = simple_block_open,
	.release = simple_block_release
};



static int __init piTwo_init(void){
	int add;
	printk("Init Module\n");
	gpio_request_one(SPEAKER, GPIOF_OUT_INIT_LOW, "SPEAKER");
	gpio_request(DHT11, "DHT11");


	gpio_request_one(LLED, GPIOF_OUT_INIT_LOW, "LLED");
	gpio_set_value(LLED,0);

	gpio_request_one(RLED, GPIOF_OUT_INIT_LOW, "RLED");
	gpio_set_value(RLED,0);

	
	gpio_request_one(LIGHT, GPIOF_IN, "LIGHT");
	gpio_request_one(BIGLED, GPIOF_OUT_INIT_LOW, "BIGLED");
	gpio_set_value(BIGLED,0);

	gpio_request_one(SPI_BIT, GPIOF_OUT_INIT_HIGH, "SPI_BIT");

	alloc_chrdev_region(&dev_num, 0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &pi_fops);


	add = cdev_add(cd_cdev, dev_num,1);

	if(add<0){
		printk("fail to add character device\n");
		return -1;
	}
else{
printk("success to add character device: %d\n",add);
}

	init_timer(&my_timer);
	my_timer.function = my_timer_func;
	my_timer.data = 1L;
	my_timer.expires = jiffies + (10*HZ);
	add_timer(&my_timer);


	return 0;

}

static void __exit piTwo_exit(void){
	printk("Exit Module\n");
	gpio_set_value(SPEAKER,0);
	gpio_free(SPEAKER);

	gpio_set_value(DHT11, 0);
	gpio_free(DHT11);



	gpio_set_value(LLED, 0);
	gpio_free(LLED);
	
	gpio_set_value(LIGHT,0);
	gpio_free(LIGHT);

	gpio_set_value(BIGLED, 0);
	gpio_free(BIGLED);



	gpio_set_value(RLED, 0);
	gpio_free(RLED);
	
	gpio_set_value(SPI_BIT, 0);
	gpio_free(SPI_BIT);



	del_timer(&my_timer); //delete timer

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(piTwo_init);
module_exit(piTwo_exit);


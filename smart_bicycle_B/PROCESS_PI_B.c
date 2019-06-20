#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/spi/spidev.h>

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

void client_thread(void *arg);

int data[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
int stop_data[8] = { 0, 0, 0, 0, 0, 0, 0, 0};

static int spifd = 0;

struct box {int value;};

unsigned char  spiBPW = 8;

int dev;

int SPI_DATA(int channel, unsigned char *data, int len) {
        struct spi_ioc_transfer spi;

        channel &= 1;

        memset(&spi, 0, sizeof(spi));

        spi.tx_buf = (unsigned long)data;
        spi.rx_buf = (unsigned long)data;
        spi.len = len;
        spi.delay_usecs = (unsigned short) 0;
        spi.speed_hz = 1000000;
        spi.bits_per_word = spiBPW;

        return ioctl (spifd, SPI_IOC_MESSAGE(1), &spi);
}

void SPI_SETUP(int channel, int speed, int mode) {
        if(ioctl(spifd, SPI_IOC_WR_MODE, &mode) < 0) {
                printf("MODE error\n");
        }

        if(ioctl(spifd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0) {
                printf("BIT error\n");
        }

        if(ioctl(spifd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0 ) {
                printf("SPEED error\n");
        }

}

void timer_thread(void *arg) {
	
	struct box get;
        unsigned char buf[3], channel = 0;
        int value=0;
        int ret;
        
	if( (spifd = open("/dev/spidev0.0", O_RDWR)) < 0 ) {
       	        	 printf("/dev/spidev error\n");
	//		 continue;
       		}
 
       	SPI_SETUP(channel, 1000000,0);
	while(1) {
		
		
		ioctl(dev, IOCTL_DOWN, &value);

        	buf[0] = 0x06 | ((channel & 0x07) >> 2);
        	buf[1] = ((channel & 0x07) << 6);
        	buf[2] = 0x00;

       	 	ret =SPI_DATA(0, buf, 3);

        	buf[1] = 0x0F & buf[1];
        	value = (buf[1] << 8) | buf[2];

        	printf("get value %d \n", value);
		
		if(value < 1600)
			ioctl(dev,IOCTL_LIGHT_UP,NULL);
		else
			ioctl(dev,IOCTL_LIGHT_DOWN,NULL);
	
		ioctl(dev, IOCTL_UP, &value);

		sleep(3);
	}
    	close(spifd);
}



int main(int argc, char *argv[]) {
	int tid, timerid, status; 
	
	if(argc != 2) {
                printf("Use %s IP \n", argv[0]);
                exit(0);
        }

	if((status = pthread_create(&tid, NULL, &client_thread, (void *)argv[1])) != 0 ) {
                        printf("thread create error : %s\n", strerror(status));
                        exit(0);
        }
	
	if((status = pthread_create(&timerid, NULL, &timer_thread, (void *)argv[1])) != 0 ) {
                        printf("thread create error : %s\n", strerror(status));
                        exit(0);
        }



	//for( int i = 0 ; i < 1000 ; i++ ){
	while(1){
		dev = open("/dev/pi_B_dev", O_RDWR);
		
		if(data[6] == 0) 
			write(dev, data, sizeof(data));
		else 
			write(dev, stop_data, sizeof(stop_data));
	
		close(dev);
//		sleep(1);
	}

	pthread_join(tid, NULL);
	pthread_join(timerid, NULL);


	return 0; 
}

void client_thread(void *arg) {
  struct sockaddr_in servaddr;
  char *serverIP = (char *) arg;
  int strlen = sizeof(servaddr);
  int sockfd, buf, cNum;

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
    perror("socket fail");
    exit(0);
  }

  memset(&servaddr, 0, strlen);
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, serverIP, &servaddr.sin_addr);
  servaddr.sin_port = htons(50);

  //printf("port: %s, IP: %s \n", port, serverIP);

  if(connect(sockfd, (struct sockaddr *)&servaddr, strlen) < 0 ) {
    perror("conect fail");
    exit(0);
  }
 
  printf("Connect with server\n");

  //for (int i = 0 ; i < 10 ; i++ ) {
	while(1) {
  	read(sockfd, data, sizeof(int)*8);

 	printf("Get From server : %d, %d, %d, %d, %d, %d, %d, %d  \n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
  }

  close(sockfd);
}

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

void client_thread(void *arg);

int data[8] = { 0, 0 ,0, 0, 0, 0, 0, 0};
int stop_data[8] = { 0, 0 ,0, 0, 0, 0, 0, 0};
int prev_data[4] = { 0, 0 , 0, 0};
int ultra_data[2] = {0, 0};;

int main(int argc, char *argv[]) {
	int dev;
	int tid, status; 
	
	if(argc != 2) {
                printf("Use %s IP \n", argv[0]);
                exit(0);
        }

	if((status = pthread_create(&tid, NULL, &client_thread, (void *)argv[1])) != 0 ) {
                        printf("thread create error : %s\n", strerror(status));
                        exit(0);
        }


//	for( int i = 0 ; i < 1000 ; i++ ){
	dev = open("/dev/pi_C_dev", O_RDWR);

	while(1) {
	//	scanf("%d %d %d %d", &data[0], &data[1], &data[2], &data[3]);
	

		read(dev, ultra_data, sizeof(int)*2);
	
		printf("GET %d -> %dcm \n", ultra_data[0], ultra_data[1]);

		if(data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0)
		{	
			write(dev, data, sizeof(data));
			usleep(50000);
			continue;
		
		} 

		if( (data[0] == 1 && ultra_data[1] < 20 )|| data[6] == 1) {
			write(dev,stop_data,sizeof(stop_data));
			usleep(50000);
			continue;
		}	
/*
		if((data[2] == 1 && prev_data[2] == 0)||(data[3]==1 && prev_data[3]==0)) {
			
			write(dev, data, sizeof(data));
	//		usleep(30000);
	//		write(dev, stop_data, sizeof(stop_data));
	
			prev_data[2] = data[2];
			prev_data[3] = data[3];

			continue;

		}else if((data[2] == 1 && prev_data[2] == 1)||(data[3]==1 && prev_data[3]==1)) {
			
			prev_data[2] = data[2];
			prev_data[3] = data[3];

			usleep(50000);
			continue;
		}
*/
		write(dev, data, sizeof(data));

		//usleep(30000);
		prev_data[2] = data[2];
		prev_data[3] = data[3];


	}

	close(dev);

	pthread_join(tid, NULL);


	return 0; 
}

void client_thread(void *arg) {
  struct sockaddr_in servaddr;
  char *serverIP = (char *) arg;
  int strlen = sizeof(servaddr);
  int sockfd, buf, cNum;
  int dev;

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

//  dev = open("/dev/pi_C_dev", O_RDWR);
  
  while(1) {
  	read(sockfd, data, sizeof(int)*8);

 	printf("GET: %d, %d, %d, %d | %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4],  data[5], data[6], data[7]);

/*	read(dev, ultra_data, sizeof(int)*2);
	
	printf("Distance %d -> %dcm \n", ultra_data[0], ultra_data[1]);

	if(data[0] == 1 && ultra_data[1] < 20) {
			continue;
	}	

	if((data[2] == 1 && prev_data[2] == 0)||(data[3]==1 && prev_data[3]==0)) {
			
			write(dev, data, sizeof(data));
			usleep(90000);
			write(dev, stop_data, sizeof(stop_data));
	
			prev_data[2] = data[2];
			prev_data[3] = data[3];

			continue;

	}else if((data[2] == 1 && prev_data[2] == 1)||(data[3]==1 && prev_data[3]==1)) {
			
			continue;
	}

	write(dev, data, sizeof(data));

	prev_data[2] = data[2];
	prev_data[3] = data[3];
*/
  }

//	close(dev);
  	close(sockfd);
}

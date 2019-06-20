#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/fcntl.h>


#define MAXCNT 10
#define MAXBUF 1024
#define LISTENQ 10

void *thrfunc(void *arg);

int out[8] = {0, 0, 0, 0,0,0,0,0};

int result = 0;
int cntNum = 0;
int change = 0;


int main(int argc, char *argv[]) {
	struct sockaddr_in servaddr, cliaddr;
	int listen_sock, accp_sock[2];
	int i, status ;
	unsigned int clintaddrlen;
	pthread_t tid[2];
	pid_t pid;
	FILE *cfp, *pfp;


	
	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("socket fail");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(50);

	clintaddrlen = sizeof(cliaddr);

	int val = 0;
	if(setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &val , sizeof(val))) {
		perror("setsocketopt");
		close(listen_sock);
		exit(0);
	}

	if(bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind fail");
		exit(0);
	}

	listen(listen_sock, LISTENQ);

		
  	printf("Wait for First client\n");
	accp_sock[0] = accept(listen_sock, (struct sockaddr *)&cliaddr, &clintaddrlen);
	if(accp_sock[0] < 0 ) {
			perror("accpet fail");
			exit(0);
	}

	if((status = pthread_create(&tid[0], NULL, &thrfunc, (void *) &accp_sock[0])) != 0 ) {
		printf("%d thread create error : %s\n", cntNum, strerror(status));
		exit(0);
	}
	
	
 	printf("Wait for Second client\n");
	accp_sock[1] = accept(listen_sock, (struct sockaddr *)&cliaddr, &clintaddrlen);
	if(accp_sock[1] < 0 ) {
			perror("accpet fail");
			exit(0);
	}

	if((status = pthread_create(&tid[1], NULL, &thrfunc, (void *) &accp_sock[1])) != 0 ) {
		printf("%d thread create error : %s\n", cntNum, strerror(status));
		exit(0);
	}

	

	//for(int  i = 0 ; i< 100000 ; i++) { 
	while(1){
		/* Read Child File */
		cfp =fopen("/home/pi/node/esw1/web-controller/child.txt", "r"); 

		if(cfp == NULL) {
			 perror("child file open error\n");
			 exit(0);
		} else {
			for( i = 0; i < 4 ; i++)
				fscanf(cfp, "%d\n", &out[i]);
		}
  	
		fclose(cfp);
	
		/* Read Parenet File */ 
		pfp =fopen("/home/pi/node/esw1/web-controller/parent.txt", "r"); 
	
		if(pfp == NULL) {
			 perror("parent file open error\n");
			 exit(0);
		} else {
			for( i = 4; i < 8 ; i++)
				fscanf(pfp, "%d\n", &out[i]);
		}
  	
		fclose(pfp);

		for(int k = 0 ; k < 8; k++) 
			printf("%d, ",out[k]);
		printf("\n");
		
		//signal_write
		int ret;
		int dev;
		dev = open("/dev/pi_A_dev", O_RDWR);
		ret = write(dev, out, sizeof(out));
		close(dev);

		usleep(50000);
	
	}

	
/*
	for (int i = 0; i< 10 ; i++) {
		printf("Enter Move : ");
		scanf("%d %d %d %d", &out[0], &data[1], &data[2], &data[3]);
		printf("Enserted %d %d %d %d \n", out[0], data[1], data[2], data[3]);
	}

*/
	
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);


	
	close(listen_sock);

	return 0;
}

void *thrfunc(void *arg) {
	int accpt_sock = (int) *((int*) arg);
	int buf;
	
	//for (int i = 0; i< 10000 ; i++) {
	while(1) {
		write(accpt_sock, out, sizeof(int)*8);
	  	usleep(50000);
	}
	close(accpt_sock);

}

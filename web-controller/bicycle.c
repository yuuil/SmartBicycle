#include <stdio.h>
#include <sys/fcntl.h>
#include <string.h>
#include <stdlib.h>



int out[8] = {0,0,0,0,0,0,0,0}; //child 4bit + parent 2bit

int main(void){
	int dev;
	int ret;
	int i;

	
	
	dev = open("/dev/signal_out_dev", O_RDWR);



	//child file read
	FILE * cfp = fopen("child.txt", "r"); //

	if(cfp == NULL){
		printf("child file open error\n");
	}
	else{
		for(i=0; i<4; i++){
			fscanf(cfp,"%d\n",&out[i]); 
	}
}

	fclose(cfp);

	
	//parent file read
	FILE * pfp = fopen("parent.txt", "r"); //

	if(pfp == NULL){
		printf("parent file open error\n");
	}
	else{
		for(i=4; i<8; i++){
			fscanf(pfp,"%d\n",&out[i]); 
	}

}
	fclose(pfp);



	ret = write(dev,out,sizeof(out)); //signal_write



	close(dev);
}

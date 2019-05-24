#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV_NAME "/dev/stopwatch"

int main(int argc, char **argv){

	int data = 0, dev;


	dev = open(DEV_NAME, O_WRONLY);

	if (dev < 0){
		printf("Device open error!\n");
		return -1;
	}

	write(dev, &data, sizeof(data));

	close(dev);
}

#include <stdio.h>
#include <stdlib.h>
#include <linux/unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEVICE_NAME "/dev/dev_driver"

int main(int argc, char **argv){
	int dev;
	int interval, count, option, ret;
	unsigned int gdata;
	
	if (argc != 4){
		printf("invalid argument error <interval> <count> <start option>\n");
		return -1;
	}

	interval = atoi(argv[1]);
	count = atoi(argv[2]);
	option = atoi(argv[3]);

	// get gdata
	ret = syscall(376, interval, count, option, &gdata);

	if (ret != 0){
		printf("syscall error. check argument\n");
		return -1;
	}

	//open

	dev = open(DEVICE_NAME, O_WRONLY);
	if (dev < 0){
		printf("device open error\n");
		return -1;
	}


	//ioctl
	ioctl(dev, 0, &gdata);


	//close
	close(dev);

	return 0;
}



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
	int gdata;
	int interval, count, option, ret;
	int a, b, c, d;

	if (argc != 4){
		printf("invalid argument error <interval> <count> <start option>\n");
		return -1;
	}

	interval = atoi(argv[1]);
	count = atoi(argv[2]);
	option = atoi(argv[3]);

	printf("%d %d %d\n", interval, count, option);
	
	ret = syscall(376, interval, count, option, &gdata);

	if (ret != 0){
		printf("syscall error. check argument\n");
		return -1;
	}

	printf("[%d]\n", gdata);

	a = (gdata & 0xFF000000) >> 24;
	b = (gdata & 0x00FF0000) >> 16;
	c = (gdata & 0x0000FF00) >> 8;
	d = gdata & 0x000000FF;

	printf("%d %d %d %d", a, b, c, d);

	return 0;


}



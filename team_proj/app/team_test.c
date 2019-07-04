#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define OUTPUT_MODE 1
#define INPUT_MODE 0

#define DEV_NAME "/dev/elevator"

#define ELEV_UP 1
#define ELEV_DOWN 3

int make_data(int updown, int a[]){
	
	char ud, floor = 0;
	int res, i = 0;

	ud = (char)updown;

	while(i < 7){
		floor = floor << 1;
		if (a[i] == 1){
			floor = floor | 0x01;
		}
		i++;
	}

	floor= floor << 1;

	res = 0;
	res = ud;
	res = res << 24;
	res = res | floor;

	return res;

}


int main(int argc, char **argv){

	int inout, data = 0, dev;

	int i, updown, a[7];

	dev = open(DEV_NAME, O_WRONLY);

	if (dev < 0){
		printf("Device open error!\n");
		return -1;
	}

	int qq;
	scanf("%d", &qq);

	while (qq > 0){
		printf("up? down? 1 == up, 3 == down, 0 == stop\n");
		scanf("%d", &updown);
		printf("1 to 7 floor, 1 -> go, 0 -> no\n");
		for (i = 0 ; i < 7 ; i++)
			scanf("%d", a+i);

		data = make_data(updown, a);


			/* make test data*/

		inout = OUTPUT_MODE;
		/*
			int ii = 0;
			while(ii < 10){
				ioctl(dev, inout, &data);	
				sleep(1);

				
				printf("%d", data);

				if (inout == 1) inout = INPUT_MODE;
				else 			inout = OUTPUT_MODE;

				ii += 1;
			}
		*/
			
		ioctl(dev, inout, &data);

		qq -= 1;
	}


	sleep(3);
	close(dev);

	return 0;
}

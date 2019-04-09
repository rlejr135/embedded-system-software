#include "20141602.h"

#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

int output_main(key_t key_mo){

	extern int poweroff_flag;
	struct mo_msgbuf mobuf;

	int fnd_dev, led_mmap;
	unsigned long *fpga_addr = 0;
	unsigned char *led_addr = 0;

	//**** open fnd ****//
	if ((fnd_dev = open("/dev/fpga_fnd", O_RDWR)) < 0){
		printf("fnd device open error\n");
		exit(1);
	}


	//**** open led ****//
	if ((led_mmap = open("/dev/mem", O_RDWR|O_SYNC)) < 0){
		perror("/dev/mem open error");
		exit(1);
	}
	fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, led_mmap, FPGA_BASE_ADDRESS);
	if (fpga_addr == MAP_FAILED){
		printf("mmap error!\n");
		close(led_mmap);
		exit(1);
	}
	led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
	*led_addr = LED_NONE;

	//**** open dot ****//



	//**** open text lcd ****//
	
	while(1){
		if (-1 == msgrcv(key_mo, &mobuf, sizeof(struct mo_msgbuf) - sizeof(long), 3, 0)){
			perror("msgrcv() fail\n");
			exit(1);
		}
		printf("rcv led : %d\n", mobuf.led_data);

		output_fnd(fnd_dev, mobuf.fnd_data);
		*led_addr = mobuf.led_data;

		printf("in output : %d\n", mobuf.poweroff);
		if (mobuf.poweroff == POWER_OFF) break;
	}

	close(fnd_dev);
	munmap(led_addr, 4096);
	close(led_mmap);

	printf("output bye\n");
}

void output_fnd(int fnd_dev, unsigned char *output_data){
	unsigned char data[4];
	unsigned char retval;
	int i = 0;
//	printf("[%s]\n", output_data);
	while (i < 4){
		data[i] = output_data[i] - 0x30;
		i++;
	}
	retval = write(fnd_dev, &data, 4);
}

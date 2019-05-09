#include "20141602.h"
#
#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

#define MAX_BUFF 32
#define LINE_BUFF 8


int output_main(key_t key_mo){

	extern int poweroff_flag;
	struct mo_msgbuf mobuf;

	int fnd_dev, led_mmap, text_dev, dot_dev, buz_dev;
	unsigned char buz_data;
	unsigned long *fpga_addr = 0;
	unsigned char *led_addr = 0;
	unsigned char ret;

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
	//**** mapping led ****//
	fpga_addr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, led_mmap, FPGA_BASE_ADDRESS);
	if (fpga_addr == MAP_FAILED){
		printf("mmap error!\n");
		close(led_mmap);
		exit(1);
	}
	led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
	*led_addr = LED_NONE;

	//**** open dot ****//
	if ((dot_dev = open("/dev/fpga_dot", O_WRONLY)) < 0){
		printf("dot device open error\n");
		exit(1);
	}


	//**** open text lcd ****//
	if ((text_dev = open("/dev/fpga_text_lcd", O_WRONLY)) < 0){
		printf("text lcd device open error\n");
		exit(1);
	}

	//**** open buzzer dev ****//
	if ((buz_dev = open("/dev/fpga_buzzer", O_RDWR)) < 0){
		printf("buz device open error\n");
		exit(1);
	}
	
	while(1){
		if (-1 == msgrcv(key_mo, &mobuf, sizeof(struct mo_msgbuf) - sizeof(long), 3, 0)){
			perror("msgrcv() fail\n");
			exit(1);
		}
		
		//**** print fnd ****//
		output_fnd(fnd_dev, mobuf.fnd_data);

		//**** print led ****//
		*led_addr = mobuf.led_data;

		//**** print dot ****//
		output_dot(dot_dev, mobuf.dot_map);

		//**** print text lcd ****//
		output_text_lcd(text_dev, mobuf.text_string);

		//**** buz dev ****//
		if (mobuf.buzz == TRUE){
			buz_data = 1;
			ret = write(buz_dev, &buz_data, 1);
			usleep(500000);
			buz_data = 0;
			ret = write(buz_dev, &buz_data, 1);
		}


		if (mobuf.poweroff == POWER_OFF) break;
	}

	close(fnd_dev);
	munmap(led_addr, 4096);
	close(led_mmap);
	close(dot_dev);
	close(text_dev);
	close(buz_dev);

	printf("output bye\n");
}

void output_fnd(int fnd_dev, unsigned char *output_data){
	unsigned char data[4];
	unsigned char retval;
	int i = 0;
	while (i < 4){
		data[i] = output_data[i] - 0x30;
		i++;
	}
	retval = write(fnd_dev, &data, 4);
}

void output_dot(int dot_dev, unsigned char *dot_value){\
	int value_size = 10;
	write(dot_dev, dot_value, value_size);
}

void output_text_lcd(int text_dev, char* output_string){
	unsigned char string[32];
	int str_size = strlen(output_string), i = 0;

	while(i < str_size){
		string[i] = output_string[i];
		i++;
	}
	while(i < MAX_BUFF){
		string[i] = ' ';
		i++;
	}

	write(text_dev, string, MAX_BUFF);
}

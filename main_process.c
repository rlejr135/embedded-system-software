#include "20141602.h"

int poweroff_flag = 0;

unsigned char fpga_number[11][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
	{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03}, // 9
	{0x08,0x1C,0x36,0x36,0x77,0x7F,0x7F,0x63,0x63,0x63}  // A
};

unsigned char fpga_set_blank[10] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


void main_msgsnd(struct mo_msgbuf mobuf, key_t key_mo){

	msgsnd(key_mo, &mobuf, sizeof(struct mo_msgbuf) - sizeof(long),0);
}
int main_main(key_t key_im, key_t key_mo){

	struct im_msgbuf imbuf;
	struct mo_msgbuf mobuf;
	unsigned int now_mode = 0, ex_mode = 0;

	imbuf.msgtype = 1;
	mobuf.msgtype = 3;
	mobuf.poweroff = POWER_ON;

	//**** in first, mode1 ****//
	mode1_construct(key_mo);
	usleep(10000);
	while(1){
		if (-1 == msgrcv(key_im, &imbuf, sizeof(struct im_msgbuf) - sizeof(long), 1, 0)){
			perror("msgrcv() fail\n");
			exit(1);
		}
		if (imbuf.key[0] == 1) 		poweroff_flag = 1;
		else if (imbuf.key[1] == 1)	now_mode = main_mode_change(1, now_mode);
		else if (imbuf.key[2] == 1)	now_mode = main_mode_change(-1, now_mode);
		
		if (now_mode != ex_mode){
			//**** destroy ex mode's status ****//
			switch(ex_mode){
				case PROG_MODE_CLOCK: 	{ mode1_destroy(); break; }
				case PROG_MODE_COUNTER:	{ mode2_destroy(); break; }
				case PROG_MODE_TEXT: 	{ mode3_destroy(); break; }
				case PROG_MODE_DRAW:	{ mode4_destroy(); break; }
				case PROG_MODE_USER: 	{ break; }
				// reset board function call
			}
			//**** construct now mode's status ****//
			switch(now_mode){
				case PROG_MODE_CLOCK: 	{ mode1_construct(key_mo); break; }
				case PROG_MODE_COUNTER:	{ mode2_construct(key_mo); break; }
				case PROG_MODE_TEXT: 	{ mode3_construct(key_mo); break; }
				case PROG_MODE_DRAW: 	{ mode4_construct(key_mo); break; }
				case PROG_MODE_USER: 	{ break; }
			}
		}
		//**** execute now mode ****//
		switch(now_mode){
			case PROG_MODE_CLOCK: 		{ mode1_main(imbuf.swi, key_mo); break; }
			case PROG_MODE_COUNTER:		{ mode2_main(imbuf.swi, key_mo); break; }
			case PROG_MODE_TEXT: 		{ mode3_main(imbuf.swi, key_mo); break; }
			case PROG_MODE_DRAW: 		{ mode4_main(imbuf.swi, key_mo); break; }
			case PROG_MODE_USER: 		{ break; }
		}
		ex_mode = now_mode;

		//**** init msgbuf ****//
		main_mobuf_init(&mobuf);


		if (poweroff_flag) {
			mobuf.poweroff = POWER_OFF;
			main_msg_clear(&mobuf);
			main_msgsnd(mobuf, key_mo);
			switch(now_mode){
				case PROG_MODE_CLOCK: 	{ mode1_destroy(); break; }
				case PROG_MODE_COUNTER:	{ mode2_destroy(); break; }
				case PROG_MODE_TEXT: 	{ mode3_destroy(); break; }
				case PROG_MODE_DRAW:	{ mode4_destroy(); break; }
				case PROG_MODE_USER: 	{ break; }
			}
	
			break;
		}
	}

	printf("main close\n");
	wait(NULL);
	wait(NULL);


	printf("bye\n");
	return 0;
}


int main_mode_change(int how, unsigned int now_mode){

	if (now_mode == 0) now_mode = 5;
	now_mode += how;
	now_mode %= 5;
	
	printf("hi %d %d\n", now_mode, how);
	return now_mode;
}

void main_mobuf_init(struct mo_msgbuf *msg){
	int i = 0;
	msg->msgtype = 3;
	msg->poweroff = POWER_ON;

	while(i < 4){
		msg->fnd_data[i] = 0;
		i++;
	}
	msg->led_data = LED_NONE;
}

void main_msg_clear(struct mo_msgbuf *msg){
	int i = 0;
	msg->msgtype = MO_MSGTYPE;

	msg->led_data = LED_NONE;
	while (i < 4){
		msg->fnd_data[i] = 0x30;
		i++;
	}

	i = 0;
	while (i < 33){
		msg->text_string[i] = ' ';
		i++;
	}

	i = 0;
	while (i < 10){
		msg->dot_map[i] = fpga_set_blank[i];
		i++;
	}
}

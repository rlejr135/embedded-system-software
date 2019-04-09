#include "20141602.h"

int poweroff_flag = 0;
void main_msgsnd(struct mo_msgbuf mobuf, key_t key_mo){

	msgsnd(key_mo, &mobuf, sizeof(struct mo_msgbuf) - sizeof(long),0);
	printf("send\n");
}
int main_main(key_t key_im, key_t key_mo){

	struct im_msgbuf imbuf;
	struct mo_msgbuf mobuf;
	unsigned int now_mode = 0, ex_mode = 0;
	int change_flag = 0;

	imbuf.msgtype = 1;
	mobuf.msgtype = 3;
	mobuf.poweroff = POWER_ON;

	//**** in first, mode1 ****//
	mode1_construct(key_mo);

	while(1){
		if (-1 == msgrcv(key_im, &imbuf, sizeof(struct im_msgbuf) - sizeof(long), 1, 0)){
			perror("msgrcv() fail\n");
			exit(1);
		}
		if (imbuf.key[0] == 1) poweroff_flag = 1;
		else if (imbuf.key[1] == 1)	now_mode = main_mode_change(1, now_mode);
		else if (imbuf.key[2] == 1)	now_mode = main_mode_change(-1, now_mode);
		
		if (now_mode != ex_mode){
			//**** destroy ex mode's status ****//
			switch(ex_mode){
				change_flag = 1;
				case PROG_MODE_CLOCK: { mode1_destroy(); break; }
				case PROG_MODE_COUNTER:	{ break; }
				case PROG_MODE_TEXT: { break; }
				case PROG_MODE_DRAW: { break; }
				case PROG_MODE_USER: { break; }
				// reset board function call
			}
			//**** construct now mode's status ****//
			switch(now_mode){
				case PROG_MODE_CLOCK:
					
					mode1_construct(key_mo);
					break;
				case PROG_MODE_COUNTER:
					break;
				case PROG_MODE_TEXT:
					break;
				case PROG_MODE_DRAW:
					break;
				case PROG_MODE_USER:
					break;
			}
		}
		//**** construct now mode's status ****//
		switch(now_mode){
			case PROG_MODE_CLOCK:
				mode1_main(imbuf.swi, key_mo);	
				break;
			case PROG_MODE_COUNTER:
				break;
			case PROG_MODE_TEXT:
				break;
			case PROG_MODE_DRAW:
				break;
			case PROG_MODE_USER:
				break;
		}
		ex_mode = now_mode;

		//**** init msgbuf ****//
		main_mobuf_init(&mobuf);


		if (poweroff_flag) {
			mobuf.poweroff = POWER_OFF;
			main_msgsnd(mobuf, key_mo);
			mode1_destroy();
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

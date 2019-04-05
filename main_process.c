#include "20141602.h"


int main_main(key_t key_im, key_t key_mo){

	struct im_msgbuf imbuf;
	struct mo_msgbuf mobuf;
	unsigned int now_mode = 0, ex_mode = 0;

	imbuf.msgtype = 1;
	mobuf.msgtype = 2;
	mobuf.poweroff = POWER_ON;

	while(1){
		if (-1 == msgrcv(key_im, &imbuf, sizeof(struct im_msgbuf) - sizeof(long), 1, 0)){
			perror("msgrcv() fail\n");
			exit(1);
		}

		if (imbuf.key[0] == 1) mobuf.poweroff = POWER_OFF;
		else if (imbuf.key[1] == 1)	now_mode = main_mode_change(1, now_mode);
		else if (imbuf.key[2] == 1)	now_mode = main_mode_change(-1, now_mode);
		

		if (now_mode != ex_mode){

			//**** destroy ex mode's status ****//
			switch(ex_mode){
				case PROG_MODE_CLOCK:
					mode1_destroy()
					break;
				case PROG_MODE_COUNTER:
					break;
				case PROG_MODE_TEXT:
					break;
				case PROG_MODE_DRAW:
					break;
				case PROG_MODE_USER:
					break;
				// reset board function call

			}
		
			//**** construct now mode's status ****//
			switch(now_mode){
				case PROG_MODE_CLOCK:
					mode1_construct();
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


		mobuf.mode = ex_mode = now_mode;
		
//		main_set_mobuf(&mobuf, now_mode);
		msgsnd(key_mo, &mobuf, sizeof(struct mo_msgbuf) - sizeof(long), 0);


		//**** if power off button is pushed ****//
		if (mobuf.poweroff == POWER_OFF) break;
	}

	printf("main close\n");
	wait(NULL);
	wait(NULL);


	printf("bye\n");
	return 0;
}

void main_set_mobuf(struct mo_msgbuf* mobuf, int now_mode){
	mobuf->mode = now_mode;
}

int main_mode_change(int how, unsigned int now_mode){

	if (now_mode == 0) now_mode = 5;
	now_mode += how;
	now_mode %= 5;
	
	printf("hi %d %d\n", now_mode, how);
	return now_mode;
}

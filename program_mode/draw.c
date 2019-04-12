#include "../20141602.h"

#define UP 		1
#define DOWN	7
#define LEFT	3
#define RIGHT	5
#define RESET	0
#define CURSOR	2
#define SELECT	4
#define CLEAR	6
#define REVERSE	8


struct mode4_state{
	unsigned int count_num;			// how many times switch pushed
	unsigned char draw_map[10];		// map
	int row, col;					// cursor's location
	int cursor_flag;				// TRUE == flicker
	int change_flag;				// for flicker. copymap

	int terminate;
};
struct mode4_state *draw_state;

unsigned char dot_space[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

pthread_t mode4_background_thread;
int mode4_thread_id;

pthread_mutex_t mutex_lock;

void mode4_construct(key_t key_mo){
	int i = 0;
	struct mo_msgbuf msg;

	draw_state = (struct mode4_state*)malloc(sizeof(struct mode4_state));
	draw_state->terminate = FALSE;

	while (i < 10){
		draw_state->draw_map[i] = 0x00;
		i++;
	} i = 0;
	draw_state->count_num = 0;
	draw_state->row = 0;
	draw_state->col = 1;
	draw_state->cursor_flag = TRUE;
	draw_state->change_flag = TRUE;

	pthread_mutex_init(&mutex_lock, NULL);
	mode4_thread_id = pthread_create(&mode4_background_thread, NULL, mode4_background, (void *)key_mo);

	mode4_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}

void mode4_destroy(){
	void *sta;

	draw_state->terminate = TRUE;
	pthread_join(mode4_background_thread, &sta);
	pthread_mutex_destroy(&mutex_lock);
	free(draw_state);
}

void mode4_main(unsigned char *swinum, key_t key_mo){
	struct mo_msgbuf msg;
	int i = 0, switch_number = -1;

	while (i < 9){
		if (swinum[i] == 1) { draw_state->count_num += 1; switch_number = i; break;}
		i++;
	} i = 0;
	
	switch(switch_number){
		case UP:
			if (draw_state->row > 0) { draw_state->row -= 1; draw_state->change_flag = TRUE; }
			break;
		case DOWN:
			if (draw_state->row < 9) { draw_state->row += 1; draw_state->change_flag = TRUE; }
			break;
		case LEFT:
			if (draw_state->col > 1) { draw_state->col -= 1; draw_state->change_flag = TRUE; }
			break;
		case RIGHT:
			if (draw_state->col < 7) { draw_state->col += 1; draw_state->change_flag = TRUE; }
			break;
		case RESET:
			mode4_destroy();
			mode4_construct(key_mo);
			break;
		case CURSOR:
			draw_state->cursor_flag += 1;
			draw_state->cursor_flag %= 2;
			break;
		case SELECT:
			draw_state->draw_map[draw_state->row] |= dot_space[draw_state->col];
			break;
		case CLEAR:
			while (i<10){
				draw_state->draw_map[i] = 0x00;
				i++;
			} i = 0;
			draw_state->change_flag = TRUE;
			break;
		case REVERSE:
			while (i < 10){
				draw_state->draw_map[i] ^= 0xFF;
				i++;
			}i = 0;
			draw_state->change_flag = TRUE;
			break;
	}
	mode4_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}

void *mode4_background(void * key){
	int i = 0;
	int copyr = draw_state->row, copyc = draw_state->col;
	int change_flag = FALSE;
	extern int poweroff_flag;
	unsigned char copy_map[10];
	struct mo_msgbuf msg;
	key_t key_mo = (key_t)key;

	while (i < 10){
		copy_map[i] = draw_state->draw_map[i];
		i++;
	} i = 0;

	while(draw_state->terminate == FALSE && poweroff_flag == POWER_ON){
		if (draw_state->cursor_flag == FALSE){
			usleep(500000);
			continue;
		}
		pthread_mutex_lock(&mutex_lock);
		//**** if cursor moved ****//
		if (draw_state->change_flag == TRUE) {
			copyr = draw_state->row;
			copyc = draw_state->col;

			while (i < 10){
				copy_map[i] = draw_state->draw_map[i];
				i++;
			} i = 0;
			draw_state->change_flag = FALSE;
		}

		copy_map[copyr] ^= dot_space[copyc];
		///////////////////////////////////////////////
		// **** for copy msg **** //
				msg.msgtype = MO_MSGTYPE;
				msg.poweroff = POWER_ON;

				//**** do not use led_data and text lcd ****//
				msg.led_data = LED_NONE;
			
				while(i < 33){
					msg.text_string[i] = ' ';
					i++;
				} i = 0;
			
				//**** set fnd ****//
				msg.fnd_data[0] = (draw_state->count_num / 1000) % 10 + 0x30;
				msg.fnd_data[1] = (draw_state->count_num / 100) % 10 + 0x30;
				msg.fnd_data[2] = (draw_state->count_num / 10) % 10 + 0x30;
				msg.fnd_data[3] = (draw_state->count_num) % 10 + 0x30;

				//**** set dot matrix ****//
				while (i < 10){
					msg.dot_map[i] = copy_map[i];
					i++;
				} i = 0;
		////////////////////////////////////////////////

		main_msgsnd(msg, key_mo);

		pthread_mutex_unlock(&mutex_lock);
		usleep(1000000);
	}

	pthread_exit(0);
}

void mode4_set_msg(struct mo_msgbuf *msg){
	int swi_count = draw_state->count_num;
	int i = 0;

	msg->msgtype = MO_MSGTYPE;
	msg->poweroff = POWER_ON;

	//**** do not use led_data and text lcd ****//
	msg->led_data = LED_NONE;
	
	while(i < 33){
		msg->text_string[i] = ' ';
		i++;
	} i = 0;
	
	//**** set fnd ****//
	msg->fnd_data[0] = (swi_count / 1000) % 10 + 0x30;
	msg->fnd_data[1] = (swi_count / 100) % 10 + 0x30;
	msg->fnd_data[2] = (swi_count / 10) % 10 + 0x30;
	msg->fnd_data[3] = (swi_count) % 10 + 0x30;

	//**** set dot matrix ****//
	while (i < 10){
		msg->dot_map[i] = draw_state->draw_map[i];
		i++;
	} i = 0;
}

#include "../20141602.h"

#define CHANGE_SET 0
#define RESET      1
#define HOUR_INC   2
#define MIN_INC    3

struct mode1_state{
	unsigned int hour, min, sec;
	unsigned int reset_hour, reset_min;
	unsigned char led;
	int terminate;
	int sw1_flag;
};
struct mode1_state *clock_state ;

pthread_t mode1_background_thread;
int mode1_thread_id;


void mode1_destroy(){
	void *sta;

	clock_state->terminate = 1;
	pthread_join(mode1_background_thread, &sta);
	free(clock_state);
}

void mode1_construct(key_t key_mo){
	time_t timer;
	static struct tm *t;
	struct mo_msgbuf msg;

	clock_state = (struct mode1_state*)malloc(sizeof(struct mode1_state));
	clock_state->terminate = 0;
	clock_state->sw1_flag = FALSE;

	timer = time(0);
	t = localtime(&timer);
	clock_state->reset_hour = clock_state->hour = t->tm_hour;
	clock_state->reset_min = clock_state->min = t->tm_min;
	clock_state->sec = t->tm_sec;
	clock_state->led = LED_1;

	//**** make background thread ****//
	mode1_thread_id = pthread_create(&mode1_background_thread, NULL, mode1_background, (void *)key_mo);

	msg.msgtype = 3;
	mode1_set_fnd(&msg);
	msg.led_data = LED_1;	
	main_msgsnd(msg, key_mo);
}


void mode1_main(unsigned char *swinum, key_t key_mo){
	struct mo_msgbuf output_msg;
	int i= 0, switch_number = -1;

	while(i < 4){
		if (swinum[i] == 1)
			switch_number = i;
		i++;
	}
	main_mobuf_init(&output_msg);
	switch (switch_number){
		case CHANGE_SET:
			clock_state->sw1_flag  = (clock_state->sw1_flag + 1) & 1;

			//**** set new time. sw1 flag is change 1 to 0****//
			if (clock_state->sw1_flag == FALSE){
				clock_state->led = LED_1;
			}
			break;
		case RESET:
			clock_state->hour = clock_state->reset_hour;
			clock_state->min = clock_state->reset_min;
			break;
		case HOUR_INC:
			if (clock_state->sw1_flag == FALSE) break;
			clock_state->hour += 1;
			clock_state->hour = clock_state->hour % 24;
			break;
		case MIN_INC:
			if (clock_state->sw1_flag == FALSE) break;
			clock_state->min += 1;
			if (clock_state->min >= 60){
				clock_state->min -= 60;
				clock_state->hour +=1;

				if (clock_state->hour >= 24)
					clock_state->hour -=24;
			}
			break;
		default:
			break;
	}

	mode1_set_fnd(&output_msg);
	output_msg.led_data = clock_state->led;

	main_msgsnd(output_msg, key_mo);
}

void *mode1_background(void *key){
	
	extern int poweroff_flag;
	struct mo_msgbuf msg;
	key_t key_mo = (key_t)key;
	
	time_t now_time;
	static struct tm *nt;

	//**** msg initialize ****//
	mode1_set_fnd(&msg);
	msg.led_data = LED_1;
	msg.msgtype = 3;

	while(!(clock_state->terminate) && !poweroff_flag){
		// check now time

		mode1_set_fnd(&msg);
		if(clock_state->sec > 60 && clock_state->sw1_flag == FALSE){
			clock_state->sec = 0;
			if (clock_state->min + 1 > 59)
				clock_state->hour = (clock_state->hour + 1) % 24;
			clock_state->min = (clock_state->min + 1) % 60;
			mode1_set_fnd(&msg);
			main_msgsnd(msg, key_mo);
		}
		else if (clock_state->sw1_flag == FALSE) clock_state->sec +=1;

		//**** if time change ****//
		if (clock_state->sw1_flag == TRUE){
			if (msg.led_data == LED_1 && clock_state->sw1_flag == TRUE) clock_state->led = msg.led_data = LED_3;
			else if (msg.led_data == LED_3 && clock_state->sw1_flag == TRUE) clock_state->led = msg.led_data = LED_4;
			else if (clock_state->sw1_flag == TRUE) clock_state->led = msg.led_data = LED_3;

			main_msgsnd(msg, key_mo);
		}

		// if swi 1 is pressed, turn on and off led
		usleep(1000000);
	}
}

void mode1_set_fnd(struct mo_msgbuf *msg){
	msg->fnd_data[0] = (clock_state->hour / 10) + 0x30;
	msg->fnd_data[1] = (clock_state->hour % 10) + 0x30;
	msg->fnd_data[2] = (clock_state->min / 10) + 0x30;
	msg->fnd_data[3] = (clock_state->min % 10) + 0x30;
}

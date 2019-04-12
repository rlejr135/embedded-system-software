#include "../20141602.h"

#define EASY 	0
#define NORMAL 	1
#define HARD	2

#define EASY_LIFE 	10
#define NORMAL_LIFE 5
#define HARD_LIFE 	2

#define EASY_SPEED  0
#define NORMAL_SPEED 1
#define HARD_SPEED	2

#define BEFORE_START 0
#define RUNNING 	1
#define FINISH		2

#define MAX_STRING_LEN 32

#define BUTTON_1 3
#define BUTTON_2 6
#define BUTTON_3 7
#define BUTTON_4 8
#define BUTTON_5 5
#define BUTTON_SELECT 4


const unsigned char starting_text[] = "Piano tile      diff:";
const unsigned char running_text[] = "Start!!         Score:";
const unsigned char finish_text[] = "Finish!!        Score:";

#define BEFORE_START_TEXT_LEN 21
#define RUNNING_TEXT_LEN 22
#define FINISH_TEXT_LEN  22

struct mode5_state{
	unsigned int life;
	unsigned int difficulty;
	unsigned int speed;
	int score;

	int game_state;

	unsigned char note[10];
	unsigned char game_text[MAX_STRING_LEN + 1];
	int text_len;

	int terminate;
};

struct mode5_state *piano_tile_state;

pthread_t mode5_background_thread;
int mode5_thread_id;

pthread_mutex_t mutex_lock;

void mode5_construct(key_t key_mo){
	int i = 0;
	struct mo_msgbuf msg;

	piano_tile_state = (struct mode5_state*)malloc(sizeof(struct mode5_state));

	// **** initialize state **** //
	piano_tile_state->terminate = FALSE;

	while(i < 10){
		piano_tile_state->note[i] = 0x41;
		i++;
	} i = 0;

	while(i<sizeof(starting_text)){
		piano_tile_state->game_text[i] = starting_text[i];
		i++;
	} 
	piano_tile_state->text_len = i;
	i = 0;
	mode5_strcat();

	piano_tile_state->life = NORMAL_LIFE;
	piano_tile_state->difficulty = NORMAL;
	piano_tile_state->speed = NORMAL_SPEED;
	piano_tile_state->score = 0;
	piano_tile_state->game_state = BEFORE_START;

	// **** make background thread and mutex lock **** //
	pthread_mutex_init(&mutex_lock, NULL);
	mode5_thread_id = pthread_create(&mode5_background_thread, NULL, mode5_background, (void *)key_mo);

	mode5_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}


void mode5_destroy(){
	void *sta;

	piano_tile_state->terminate = TRUE;
	pthread_join(mode5_background_thread, &sta);
	pthread_mutex_destroy(&mutex_lock);
	free(piano_tile_state);
}

void mode5_main(unsigned char *swinum, key_t key_mo){
	struct mo_msgbuf msg;
	int i = 0, switch_number = -1;
	while (i < 9){
		if (swinum[i] == 1) { switch_number = i; break; }
		i++;
	} i = 0;

	if (piano_tile_state->game_state == BEFORE_START){
		switch(switch_number){
			case EASY: piano_tile_state->difficulty = EASY;
					mode5_strcat();
					break;
			case NORMAL: piano_tile_state->difficulty = NORMAL;
					mode5_strcat();
					break;
			case HARD: piano_tile_state->difficulty = HARD;
					mode5_strcat();
					break;
			case BUTTON_SELECT: piano_tile_state->game_state = RUNNING;
					mode5_strcat();
					break;
		}
	}
	else if (piano_tile_state->game_state == RUNNING){
		switch(switch_number){
			case BUTTON_1: mode5_check(BUTTON_1); break;
			case BUTTON_2: mode5_check(BUTTON_2); break;
			case BUTTON_3: mode5_check(BUTTON_3); break;
			case BUTTON_4: mode5_check(BUTTON_4); break;
			case BUTTON_5: mode5_check(BUTTON_5); break;
		}
	}
	else if (piano_tile_state->game_state == FINISH){
		if (switch_number == BUTTON_SELECT){
			mode5_destroy();
			mode5_construct(key_mo);
		}
	}
	mode5_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}


void *mode5_background(void *key){
	key_t key_mo = (key_t)key;
	int i = 0;
	extern int poweroff_flag;

	while(piano_tile_state->terminate == FALSE && poweroff_flag == POWER_ON){
		if (piano_tile_state->game_state == BEFORE_START){
			usleep(500000);
			continue;
		}
	}


	pthread_exit(0);
}

void mode5_set_msg(struct mo_msgbuf* msg){
	int i = 0;

	msg->msgtype = MO_MSGTYPE;
	msg->poweroff = POWER_ON;

	// **** set fnd ****//
	msg->fnd_data[0] = (piano_tile_state->life / 1000) % 10 + 0x30;
	msg->fnd_data[1] = (piano_tile_state->life / 100) % 10 + 0x30;
	msg->fnd_data[2] = (piano_tile_state->life / 10) % 10 + 0x30;
	msg->fnd_data[3] = (piano_tile_state->life) % 10 + 0x30;


	// **** set dot matrix ****//
	while(i < 10){
		msg->dot_map[i] = piano_tile_state->note[i];
		i++;
	} i = 0;


	// **** set led **** //
	switch (piano_tile_state->speed){
		case 0: msg->led_data = LED_1; break;
		case 1: msg->led_data = LED_2; break;
		case 2: msg->led_data = LED_3; break;
		case 3: msg->led_data = LED_4; break;
		case 4: msg->led_data = LED_5; break;
		case 5: msg->led_data = LED_6; break;
		case 6: msg->led_data = LED_7; break;
		case 7: msg->led_data = LED_8; break;
	}

	// **** set text lcd **** //
	while(i < piano_tile_state->text_len + 1){
		msg->text_string[i] = piano_tile_state->game_text[i];
		i++;
	}
}

void mode5_strcat(){
	int i = 0, j = 0;
	char c_score[10] = {0,};

	if (piano_tile_state->game_state == BEFORE_START){

		i = BEFORE_START_TEXT_LEN;
		switch (piano_tile_state->difficulty){
			case EASY: 
				piano_tile_state->game_text[i] = 'E'; 
				break;
			case NORMAL:
				piano_tile_state->game_text[i] = 'N'; 
				break;
			case HARD:
				piano_tile_state->game_text[i] = 'H'; 
				break;
		}
		piano_tile_state->text_len = i+1;
		piano_tile_state->game_text[i+1] = '\0';
	}
	else{
		// **** make int score to char score, strcat at game text
		sprintf(c_score, "%d", piano_tile_state->score);
		
		if (piano_tile_state->game_state == RUNNING) {
			i = RUNNING_TEXT_LEN;

			j = 0;
			while (j< i){
				piano_tile_state->game_text[j] = running_text[j];
				j++;
			}j = 0;
		}
		else if (piano_tile_state->game_state == FINISH) {
			i = FINISH_TEXT_LEN;

			j = 0;
			while (j< i){
				piano_tile_state->game_text[j] = running_text[j];
				j++;
			}j = 0;
		}

		while (j < strlen(c_score)){
			piano_tile_state->game_text[i] = c_score[j];
			i++;
			j++;
		}
		piano_tile_state->text_len = i;
	}
}

void mode5_check(int where){
	

	mode5_strcat();
}

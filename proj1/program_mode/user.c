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
#define MAX_SPEED 7

#define BUTTON_1 3
#define BUTTON_2 6
#define BUTTON_3 7
#define BUTTON_4 8
#define BUTTON_5 5
#define BUTTON_SELECT 4

#define CHECK_B_1 0x61
#define CHECK_B_2 0x51
#define CHECK_B_3 0x49
#define CHECK_B_4 0x45
#define CHECK_B_5 0x43


const unsigned char starting_text[] = "Piano tile      difficulty:";
const unsigned char running_text[] = "Start!!         Score:";
const unsigned char finish_text[] = "end. combo:     Score:";

#define BEFORE_START_TEXT_LEN 27
#define RUNNING_TEXT_LEN 22
#define FINISH_TEXT_LEN  22
#define COMBO_PRINT_LOC  12

struct mode5_state{
	int life;
	unsigned int difficulty;
	unsigned int speed;
	int score;
	int combo;
	int max_combo;

	int game_state;									// ex or running or after

	unsigned char note[10];							// map
	unsigned char game_text[MAX_STRING_LEN + 1];	// text lcd
	int text_len;									

	int check_flag;									// if user push switch?
	int wrong_flag;									// if pushed switch is wrong?

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

	piano_tile_state->life = NORMAL_LIFE;
	piano_tile_state->difficulty = NORMAL;
	piano_tile_state->speed = NORMAL_SPEED;
	piano_tile_state->score = 0;
	piano_tile_state->combo = 0;
	piano_tile_state->max_combo = 0;
	piano_tile_state->game_state = BEFORE_START;
	piano_tile_state->check_flag = TRUE;
	piano_tile_state->wrong_flag = FALSE;

	// **** make background thread and mutex lock **** //
	pthread_mutex_init(&mutex_lock, NULL);
	mode5_thread_id = pthread_create(&mode5_background_thread, NULL, mode5_background, (void *)key_mo);

	mode5_strcat();
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

	// **** handling each case by 3 state. **** //
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
		mode5_set_msg(&msg);
	}
	else if (piano_tile_state->game_state == RUNNING){
		switch(switch_number){
			case BUTTON_1: mode5_check(BUTTON_1); break;
			case BUTTON_2: mode5_check(BUTTON_2); break;
			case BUTTON_3: mode5_check(BUTTON_3); break;
			case BUTTON_4: mode5_check(BUTTON_4); break;
			case BUTTON_5: mode5_check(BUTTON_5); break;

			piano_tile_state->check_flag = TRUE;
		}
		if (piano_tile_state->life <= 0){
			piano_tile_state->game_state = FINISH;
			mode5_strcat();
		}
	}
	else if (piano_tile_state->game_state == FINISH){
		// **** new game **** //
		if (switch_number == BUTTON_SELECT){
			mode5_destroy();
			mode5_construct(key_mo);
			return;
		}
	}
	mode5_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}


void *mode5_background(void *key){
	key_t key_mo = (key_t)key;
	int i = 0;
	struct mo_msgbuf msg;
	extern int poweroff_flag;
	int random;
	int tick_num = 0;
	int mu = 10000;

	i = -1;
	while (i < piano_tile_state->speed){
		mu += 5000;
		i++;
	}i = 0;

	srand(time(NULL));

	while(piano_tile_state->terminate == FALSE && poweroff_flag == POWER_ON){
		// **** state is not running, continue **** //
		if (piano_tile_state->game_state != RUNNING){
			mode5_strcat();
			mode5_set_msg(&msg);
			main_msgsnd(msg, key_mo);

			usleep(500000);
			continue;
		}

		pthread_mutex_lock(&mutex_lock);

		// **** note down ****//
		i = 8;
		while (i >= 0){
			piano_tile_state->note[i+1] = piano_tile_state->note[i];
			i--;
		}

		// **** make random note **** //
		random = (rand() % 5);
		switch(random){
			case 0: piano_tile_state->note[0] = 0x61; break;
			case 1: piano_tile_state->note[0] = 0x51; break;
			case 2: piano_tile_state->note[0] = 0x49; break;
			case 3: piano_tile_state->note[0] = 0x45; break;
			case 4: piano_tile_state->note[0] = 0x43; break;
		}
		mode5_strcat();
		mode5_set_msg(&msg);
		main_msgsnd(msg, key_mo);

		// **** check user mistakes **** //
		if (tick_num > 9 && piano_tile_state->check_flag == FALSE){

			if (piano_tile_state->life >= 1){
				piano_tile_state->life -= 1;
				piano_tile_state->wrong_flag = TRUE;
			}
			if (piano_tile_state->max_combo < piano_tile_state->combo) {
				piano_tile_state->max_combo = piano_tile_state->combo;
			}
			piano_tile_state->combo = 0;
			if (piano_tile_state->life <= 0){
				piano_tile_state->game_state = FINISH;
				mode5_strcat();
			}
		}

		pthread_mutex_unlock(&mutex_lock);
		piano_tile_state->check_flag = FALSE;
		usleep(1000000 - (piano_tile_state->speed * mu));
		tick_num += 1;

		// **** if time is gone, speed up **** //
		if (tick_num >= 30){
			tick_num = 11;
			if (piano_tile_state->speed < MAX_SPEED){
				piano_tile_state->speed += 1;
				mu += 5000;
			}
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
	while (i < MAX_STRING_LEN){
		msg->text_string[i] = ' ';
		i++;
	}

	if (piano_tile_state->wrong_flag == TRUE){
		piano_tile_state->wrong_flag = FALSE;
		msg->buzz = TRUE;
	}
	else{
		msg->buzz = FALSE;
	}
}

void mode5_strcat(){
	int i = 0, j = 0, k = 0;
	char c_score[10] = {0,};
	char c_combo[10] = {0,};

	// **** set each difficulty case **** //
	if (piano_tile_state->game_state == BEFORE_START){
		i = BEFORE_START_TEXT_LEN;
		switch (piano_tile_state->difficulty){
			case EASY: 
				piano_tile_state->game_text[i] = 'E';
				piano_tile_state->life = EASY_LIFE;
				piano_tile_state->speed = EASY_SPEED;
				break;
			case NORMAL:
				piano_tile_state->game_text[i] = 'N'; 
				piano_tile_state->life = NORMAL_LIFE;
				piano_tile_state->speed = NORMAL_SPEED;
				break;
			case HARD:
				piano_tile_state->game_text[i] = 'H'; 
				piano_tile_state->life = HARD_LIFE;
				piano_tile_state->speed = HARD_SPEED;
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
			sprintf(c_combo, "%d", piano_tile_state->combo);

			j = 0;
			while (j< i){
				piano_tile_state->game_text[j] = running_text[j];
				j++;
			}
		}
		else if (piano_tile_state->game_state == FINISH) {
			i = FINISH_TEXT_LEN;
			sprintf(c_combo, "%d", piano_tile_state->max_combo);

			j = 0;
			while (j< i){
				piano_tile_state->game_text[j] = finish_text[j];
				j++;
			}
		}

		// **** for text lcd, write information **** //
		j = COMBO_PRINT_LOC;
		k = 0;
		while (k < strlen(c_combo)){
			piano_tile_state->game_text[j] = c_combo[k];
			j++; k++;
		}j = 0;
		while (j < strlen(c_score)){
			piano_tile_state->game_text[i] = c_score[j];
			i++;
			j++;
		}
		piano_tile_state->text_len = i;
		while (i < MAX_STRING_LEN){
			piano_tile_state->game_text[i] = ' ';
			i++;
		}
	}
}

void mode5_check(int where){
	int check_flag = FALSE;
	// **** check user type switch and note is same **** //
	switch(where){
		case BUTTON_1:
			if (CHECK_B_1 == piano_tile_state->note[9]){
				check_flag = TRUE;	
			}
			break;
		case BUTTON_2: 
			if (CHECK_B_2 == piano_tile_state->note[9]){
				check_flag = TRUE;	
			}
			break;
		case BUTTON_3: 
			if (CHECK_B_3 == piano_tile_state->note[9]){
				check_flag = TRUE;	
			}
			break;
		case BUTTON_4:
			if (CHECK_B_4 == piano_tile_state->note[9]){
				check_flag = TRUE;	
			}
			break;
		case BUTTON_5: 	
			if (CHECK_B_5 == piano_tile_state->note[9]){
				check_flag = TRUE;	
			}
			break;
	}

    // **** it's ok, score and combo up **** //
	if (check_flag == TRUE){
		piano_tile_state->check_flag = TRUE;
		piano_tile_state->combo += 1;
		piano_tile_state->score += 5 * (piano_tile_state->combo/10) + 1;
	}
	else{
		// **** if not, life minus **** //
		if (piano_tile_state->life >= 1){
			piano_tile_state->life -= 1;
			piano_tile_state->wrong_flag = TRUE;
		}
		if (piano_tile_state->max_combo < piano_tile_state->combo) {
			piano_tile_state->max_combo = piano_tile_state->combo;
		}
		piano_tile_state->combo = 0;
	}

	mode5_strcat();
}

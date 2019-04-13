#include "../20141602.h"

#define ALPHA 10
#define NUMBER 	1

#define CHANGE  0
#define SET 	1
#define MAX_STRING_LEN 32

extern unsigned char fpga_number[11][10];
const char swi_text[9][3] = {
	{'.', 'Q', 'Z'},	// 1
	{'A', 'B', 'C'},	// 2
	{'D', 'E', 'F'},	// 3
	{'G', 'H', 'I'},	// 4
	{'J', 'K', 'L'},	// 5
	{'M', 'N', 'O'},	// 6
	{'P', 'R', 'S'},	// 7
	{'T', 'U', 'V'},	// 8
	{'W', 'X', 'Y'}		// 9
};

struct mode3_state{
	unsigned int count_num;						// how many times does switch pushed
	char display_string[MAX_STRING_LEN + 1];
	int string_size;
	char type;									// A or 1
	int click_flag[9];							// if type is A, what order
	int first_typing;

	int terminate;
};

struct mode3_state *text_state;

void mode3_construct(key_t key_mo){
	struct mo_msgbuf msg;
	int i = 0;

	text_state = (struct mode3_state*)malloc(sizeof(struct mode3_state));

	text_state->count_num = 0;
	text_state->type = ALPHA;
	text_state->terminate = FALSE;
	text_state->string_size = 0;

	//**** initialize string ****//
	while(i < MAX_STRING_LEN){
		mode3_set_string(' ', SET);
		i++;
	} 
	text_state->display_string[MAX_STRING_LEN] = '\0';
	i = 0;
	text_state->string_size = 0;

	//**** if first. ****//
	text_state->first_typing = TRUE;

	while(i < 9){
		text_state->click_flag[i] = -1;
		i++;
	}

	mode3_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}


void mode3_destroy(){
	free(text_state);
}

void mode3_main(unsigned char *swinum, key_t key_mo){
	struct mo_msgbuf msg;
	int i = 0, switch_number = -1;
	int input_flag;

	while (i < 9){
		if (swinum[i] == 1){ switch_number = i; text_state->count_num +=1;}
		i++;
	} i = 0;

	//**** clear text lcd ****//
	if (swinum[1] == 1 && swinum[2] == 1){	
		i = 0;
		while(i < MAX_STRING_LEN){
			mode3_set_string(' ', SET);
			i++;
		} 
		text_state->display_string[MAX_STRING_LEN] = '\0';
		i = 0;
		text_state->string_size = 0;
	}
	//**** change eng -> num, num -> eng ****//
	else if (swinum[4] == 1 && swinum[5] == 1){
		if (text_state->type == ALPHA) text_state->type = NUMBER;
		else if (text_state->type == NUMBER) text_state->type = ALPHA;

		while (i < 9){
			text_state->click_flag[i] = -1;
			i++;
		}
		text_state->first_typing = TRUE;
	}
	//**** set space ****//
	else if (swinum[7] == 1 && swinum[8] == 1){
		mode3_set_string(' ', SET);
	}
	//**** just one switch pushed ****//
	else if (switch_number != -1){
		//**** define how switch pushed. ****//
		//*	SET : swi(3) -> swi(4) 		res : DG
		//*	CHANGE : swi(3) -> swi(3)	res : E 
		//////////////////////////////////////
		if (text_state->type == ALPHA){
			while (i < 9){
				if (i == switch_number){
					i += 1;
					continue;
				}
				if (text_state->click_flag[i] != -1){
					input_flag = SET;
					text_state->click_flag[i] = -1;
					break;
				}
				i++;
			}
			if (i == 9) input_flag = CHANGE;
			//**** at first, always set ****//
			if (text_state->first_typing == TRUE){
				text_state->first_typing = FALSE;
				input_flag = SET;
			}
			i = 0;


			//**** setting string ****//
			if (input_flag == SET){
				text_state->click_flag[switch_number] = 0;
				mode3_set_string(swi_text[switch_number][text_state->click_flag[switch_number]], input_flag);
			}
			else if (input_flag == CHANGE){
				text_state->click_flag[switch_number] = (text_state->click_flag[switch_number] + 1) % 3;
				mode3_set_string(swi_text[switch_number][text_state->click_flag[switch_number]], input_flag);
			}
		}
		else if (text_state->type == NUMBER){
			mode3_set_string(switch_number + 1, SET);
		}
	}
	// **** send msg **** //
	mode3_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}

// **** setting string in text_state ****//
void mode3_set_string(char input_char, int flag){
	int str_size = text_state->string_size;
	int i = 0;

	//**** if input character is number ****//
	if (input_char != ' ' && text_state->type == NUMBER) input_char += 0x30;
		
	if (flag == SET){
		if (str_size < MAX_STRING_LEN){
			text_state->display_string[str_size] = input_char;
			text_state->string_size +=1;
		}
		else{
			while (i < MAX_STRING_LEN - 1){
				text_state->display_string[i] = text_state->display_string[i+1];
				i+=1;
			}
			text_state->display_string[MAX_STRING_LEN - 1] = input_char;
		}
	}
	else if (flag == CHANGE && str_size != 0){
		text_state->display_string[str_size - 1] = input_char;
	}
}

// **** setting msg **** //
void mode3_set_msg(struct mo_msgbuf *msg){
	int i = 0;
	int str_size = text_state->string_size;
	int swi_count = text_state->count_num;

	msg->msgtype = MO_MSGTYPE;
	msg->poweroff = POWER_ON;

	//**** do not use led data and buzzer ****//
	msg->led_data = LED_NONE;
	msg->buzz = FALSE;

	//**** set dot matrix A or 1 ****//
	while (i < 10){
		msg->dot_map[i] = fpga_number[text_state->type][i];
		i++;
	}
	i = 0;

	//**** set string ****//
	while (i < str_size){
		msg->text_string[i] = text_state->display_string[i];
		i++;
	}
	while (i< MAX_STRING_LEN){
		msg->text_string[i] = ' ';
		i++;
	}

	//**** set fnd ****//
	msg->fnd_data[0] = (swi_count / 1000) % 10 + 0x30;
	msg->fnd_data[1] = (swi_count / 100) % 10 + 0x30;
	msg->fnd_data[2] = (swi_count / 10) % 10 + 0x30;
	msg->fnd_data[3] = (swi_count) % 10 + 0x30;
}

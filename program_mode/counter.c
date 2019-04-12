#include "../20141602.h"

#define DECIMAL	 	0
#define OCTAL 		1
#define QUADRATIC 	2
#define BINARY 		3


#define CHANGE		0
#define HUND_INC	1
#define TEN_INC		2
#define ONE_INC		3

#define DECIMAL_MAX 1000
#define OCTAL_MAX 	512
#define QUADRATIC_MAX 64
#define BINARY_MAX 	8

struct mode2_state{
	unsigned int counting_number;
	unsigned int type;

	int terminate;
};
struct mode2_state *counter_state;


void mode2_construct(key_t key_mo){
	struct mo_msgbuf msg;

	counter_state = (struct mode2_state*)malloc(sizeof(struct mode2_state));
	counter_state->terminate = FALSE;

	//**** set first state ****//
	counter_state->counting_number = 0;
	counter_state->type = DECIMAL;

	//**** send msg ****//
	mode2_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}

void mode2_destroy(){
	counter_state->terminate = TRUE;

	free(counter_state);
}

void mode2_main(unsigned char *swinum, key_t key_mo){
	struct mo_msgbuf msg;
	int i = 0, switch_number = -1;
	int method_num;

	while (i < 4){
		if (swinum[i] == 1) switch_number = i;
		i++;	
	}

	method_num = mode2_find_method();

	// **** each case handling ****//
	switch (switch_number){
		case CHANGE:
			counter_state->type = (counter_state->type + 1) % 4;
			mode2_check_maximum();
			break;
		case HUND_INC:
			counter_state->counting_number += 1*method_num*method_num;
			mode2_check_maximum();	
			break;
		case TEN_INC:
			counter_state->counting_number += 1*method_num;
			mode2_check_maximum();	
			break;
		case ONE_INC:
			counter_state->counting_number += 1;
			mode2_check_maximum();	
			break;
	}

	mode2_set_msg(&msg);
	main_msgsnd(msg, key_mo);
}

//** find what type and number ****//
int mode2_find_method(){
	int res = counter_state->type;

	if (res == DECIMAL) 		return 10;
	else if (res == OCTAL) 		return 8;
	else if (res == QUADRATIC) 	return 4;
	else if (res == BINARY) 	return 2;
	else 						return FALSE;
}

//**** if over max number, cut ****//
void mode2_check_maximum(){
	int res = counter_state->type;

	if (res == DECIMAL) 		counter_state->counting_number %= DECIMAL_MAX;
	else if (res == OCTAL) 		counter_state->counting_number %= OCTAL_MAX;
	else if (res == QUADRATIC) 	counter_state->counting_number %= QUADRATIC_MAX;
	else if (res == BINARY) 	counter_state->counting_number %= BINARY_MAX;
}

//**** set msgbuf ****//
void mode2_set_msg(struct mo_msgbuf *msg){
	int number = counter_state->counting_number;
	int jinsu = counter_state->type;
	int i = 0;

	msg->msgtype = MO_MSGTYPE;
	msg->poweroff = POWER_ON;

	//**** do not use text, buzzer and dot map ****//
	while (i<33){
		msg->text_string[i] = ' ';
		i++;
	}
	i = 0;

	while (i<10){
		msg->dot_map[i] = 0x00;
		i++;
	}
	i = 0;

	msg->buzz = FALSE;

	// **** set led data **** //
	if (jinsu == DECIMAL) 		{ jinsu = 10; msg->led_data = LED_2; }
	else if (jinsu == OCTAL) 	{ jinsu = 8; msg->led_data = LED_3; }
	else if (jinsu == QUADRATIC)	{ jinsu = 4; msg->led_data = LED_4; }
	else if (jinsu == BINARY) 	{ jinsu = 2; msg->led_data = LED_1; }

	// **** set fnd data **** //
	msg->fnd_data[0] = 0 + 0x30;	
	msg->fnd_data[1] = ((number / jinsu / jinsu) % jinsu) + 0x30;	
	msg->fnd_data[2] = ((number / jinsu) % jinsu) + 0x30;	
	msg->fnd_data[3] = (number % jinsu) + 0x30;
}


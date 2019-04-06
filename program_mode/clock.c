#include "../20141602.h"


#define CHANGE_SET 0
#define RESET      1
#define HOUR_INC   2
#define MIN_INC    3

struct mode1_state{
	unsigned int hour, min;
	unsigned int reset_hour, reset_min;
	unsigned char led[8];


};

struct mode1_state *clock_state ;

void mode1_destroy(){


	printf("helloo!!\n");

	free(clock_state);
}

void mode1_construct(){
	time_t timer;
	static struct tm *t;

	clock_state = (struct mode1_state*)malloc(sizeof(struct mode1_state));
	timer = time(0);
	t = localtime(&timer);
	clock_state->reset_hour = clock_state->hour = t->tm_hour;
	clock_state->reset_min = clock_state->min = t->tm_min;

	printf("%d %d\n",clock_state->hour, clock_state->min);

}


struct mo_msgbuf mode1_main(unsigned char *swinum){
	static unsigned int sw1_flag = 0;
	int i= 0, switch_number = -1;
	while(i < 4){
		if (swinum[i] == 1)
			switch_number = i;
		i++;
	}

	switch (switch_number){
		case CHANGE_SET:
			sw1_flag  = (sw1_flag + 1) & 1;
			break;
		case RESET:
			clock_state->hour = clock_state->reset_hour;
			clock_state->min = clock_state->reset_min;
			break;
		case HOUR_INC:
			if (sw1_flag == 0) break;
			clock_state->hour += 1;
			clock_state->hour = clock_state->hour % 24;
			break;
		case MIN_INC:
			if (sw1_flag == 0) break;
			clock_state->min += 1;
			clock_state->min =clock_state->min % 60;
			break;
	}
	printf("flag : %d number : %d\n", sw1_flag, switch_number);
	printf("%d %d\n", clock_state->hour, clock_state->min);
}

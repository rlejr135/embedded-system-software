#include "../20141602.h"


#define CHANGE_SET 0;
#define RESET      1;
#define HOUR_INC   2;
#define MIN_INC    3;

struct mode1_state{
	int hour, min;
	unsigned char led[8];


}

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
	clock_state->hour = t->tm_hour;
	clock_state->min = t->tm_min;
}


int mode1_main(int swinum){
		static unsigned char sw1_flag = 0;

		switch (swinum){
			case CHANGE_SET:
							break;
			case RESET:
							break;
			case HOUR_INC:
							break;
			case MIN_INC:
							break;
		}
}

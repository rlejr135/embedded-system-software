#include "20141602.h"

int output_main(key_t key_mo){

	struct mo_msgbuf mobuf;

	mobuf.msgtype = 2;

	while(1){
		if (-1 == msgrcv(key_mo, &mobuf, sizeof(struct mo_msgbuf) - sizeof(long), 2, 0)){
			perror("msgrcv() fail\n");
			exit(1);
		}
		if (mobuf.poweroff == POWER_OFF)break;
	}

	printf("output bye\n");
}

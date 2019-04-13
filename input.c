#include "20141602.h"

int input_main(key_t key_im){

	struct im_msgbuf imbuf;
	int key_dev, swi_dev;

	int input_flag = 0, i;

	if((key_dev = open ("/dev/input/event0", O_RDONLY|O_NONBLOCK)) == -1) {
		printf("readkey device open error\n");
	}
	if ((swi_dev = open("/dev/fpga_push_switch", O_RDWR)) == -1){
		printf("switch device open error\n");
	}

	usleep(10000);

	while(1){
		input_init_imbuf(&imbuf);
		input_flag = input_value(&imbuf, key_dev, swi_dev, imbuf.swi);
		//**** if some is pushed **** //
		if (input_flag > 0){
			msgsnd(key_im, &imbuf, sizeof(struct im_msgbuf) - sizeof(long), 0);

			// **** check user to what switch did user push **** //
			for (i = 0 ; i < 9 ; i++)
				printf("%d ", imbuf.swi[i]);
			printf("\n");
			
			if (input_flag == POWER_OFF) break;
			input_flag = 0;
		}
		usleep(100000);
	}
	close(key_dev);
	close(swi_dev);
	printf("input bye\n");
	return 0;
}

void input_init_imbuf(struct im_msgbuf *msgb){
	int i;
	msgb->msgtype = 1;
	
	for (i = 9 ; i >= 0 ; i--){
		msgb->swi[i] = 0;
	}
	msgb->key[0] = 0;
	msgb->key[1] = 0;
	msgb->key[2] = 0;
}

int input_value(struct im_msgbuf *msgb, int key_dev, int swi_dev, unsigned char *swi)
{
	struct input_event ev[BUFF_SIZE];
	int size = sizeof (struct input_event), i;
	unsigned char swi_buffer[9];
	static unsigned char prev_buffer[9] = {0, }; 
	int flag = FALSE, pressed = FALSE;
	
	// **** check readkey **** //
	if (read (key_dev, ev, size * BUFF_SIZE) >= size)
	{
		if( ev[0].value == KEY_PRESS ) {
			if (ev[0].code == POWER_OFF) {
				msgb->key[0] = 1;
				return POWER_OFF;
			}
			else if (ev[0].code == MODE_UP){
				msgb->key[1] = 1;
				return 1;
			}
			else if (ev[0].code == MODE_DOWN){
				msgb->key[2] = 1;
				return 1;
			}	
		}
	}	
	else{
		read(swi_dev, &swi_buffer, sizeof(swi_buffer));

		//**** check if prev and now is same **** //
		for (i = 0 ; i < 9 ; i++){
			if (swi_buffer[i] && prev_buffer[i]){
				pressed = TRUE;
				break;
			}
		}
		if (pressed) return 0;

		//**** if not, set switch ****//
		for (i = 0 ; i < 9 ; i++){
			if (swi_buffer[i]){
				swi[i] = swi_buffer[i];
				flag = TRUE;
			}
		}
		for (i = 0 ; i < 9 ; i++){
			prev_buffer[i] = swi[i];
		}
	}
	
	return flag;
}


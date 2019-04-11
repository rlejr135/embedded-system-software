#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/input.h>


#define TRUE 1
#define FALSE 0

#define IPC_MSGQ_IM 1245
#define IPC_MSGQ_MO 5421

#define MO_MSGTYPE 3

#define BUFF_SIZE 64

#define POWER_ON 0
#define POWER_OFF 158
#define MODE_UP   115
#define MODE_DOWN 114

#define PROG_MODE_CLOCK   0
#define PROG_MODE_COUNTER 1
#define PROG_MODE_TEXT    2
#define PROG_MODE_DRAW    3
#define PROG_MODE_USER    4

#define KEY_RELEASE 0
#define KEY_PRESS   1

#define LED_1 128
#define LED_2 64
#define LED_3 32
#define LED_4 16
#define LED_5 8
#define LED_6 4
#define LED_7 2
#define LED_8 1
#define LED_NONE 0
struct input_event ev[BUFF_SIZE];

struct im_msgbuf{

	//****default****//
	long msgtype;

	//****for msg queue****// 
	//****swi[0~8] -> switch 1~ 9****//
	//****key[0] == back, key[1] == 1 vol+, key[2] == vol-****//
	unsigned char swi[9];
	unsigned char key[3];
};

struct mo_msgbuf{
	long msgtype;

	unsigned char poweroff;

	//**** FND ****//
	unsigned char fnd_data[4];
	//**** LED ****//
	unsigned char led_data;
	//**** Text LCD ****//

	//**** Dot Matrix ****//

};


//**** input process  ****//
int input_main(key_t);
int input_value(struct im_msgbuf *, int, int);
void input_init_imbuf(struct im_msgbuf *);

//**** main process ****//
int main_main(key_t, key_t);
void main_msgsnd(struct mo_msgbuf, key_t);
int main_mode_change(int, unsigned int);
void main_mobuf_init(struct mo_msgbuf *);


//**** output process ****//
int output_main(key_t);
void output_fnd(int, unsigned char *);




//**** program mode 1. Clock ****//

void mode1_construct(key_t);
void mode1_destroy();
void mode1_main(unsigned char*, key_t);
void *mode1_background(void *);
void mode1_set_fnd(struct mo_msgbuf *);

//**** program mode 2. Counter ****//

void mode2_construct(key_t);
void mode2_destroy();
void mode2_main(unsigned char*, key_t);
void mode2_set_msg(struct mo_msgbuf *);
int mode2_find_method();
void mode2_check_maximum();



//**** program mode 3. Text ****//

void mode3_construct();
void mode3_destroy();



//**** program mode 4. Draw ****//

void mode4_construct();
void mode4_destroy();



//**** program mode 5. User ****//

void mode5_construct();
void mode5_destroy();




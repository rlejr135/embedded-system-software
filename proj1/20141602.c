#include "20141602.h"


int main(){
	int count=0;
	pid_t pid_input, pid_output;
	key_t key_im, key_mo;

	int i, j;


	//****make message queue****//
	//****1 == input to main****//
	//****2 == main to output****//

	key_im = msgget((key_t)IPC_MSGQ_IM, IPC_CREAT|0666);
	key_mo = msgget((key_t)IPC_MSGQ_MO, IPC_CREAT|0666);
	

	//****make input process****//
	if ((pid_input = fork()) == -1){
		perror("fork");
		exit(1);
	}
	else count++;
	

	//****make output process****//
	if (pid_input > 0){
		if ((pid_output = fork()) == -1){
			perror("fork");
			exit(1);
		}
		else count++;
	}


	//****input process****//
	if (pid_input == 0){
		input_main(key_im);
	}
	//****main process****//
	else if (pid_input > 0 && pid_output > 0){
		main_main(key_im, key_mo);
	}
	//****output process****//
	else if (pid_input > 0 && pid_output == 0){
		output_main(key_mo);
	}
	

	return 0;
}

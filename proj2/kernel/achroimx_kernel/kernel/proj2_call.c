#include <linux/kernel.h>
#include <asm/uaccess.h>

asmlinkage int sys_newcall(int interval, int count, int option, unsigned int *ret_value){
	char start_loc, start_val;
	int i, where = 10000, temp;
	int res;

	if (option < 1 || option > 8000){
		printk("error: option is not [0001 ~ 8000]\n");
		return -1;
	}

	for (i = 1 ; i <= 4 ; i++){
		temp = (option % where) / (where/10);
		if (temp != 0){
			start_loc = i;
			start_val = temp;
			break;
		}
		where /= 10;
	}

	res = (start_loc << 24 | start_val << 16 | interval << 8 | count);

	copy_to_user(ret_value, &res, sizeof(int));

	return 0;
}

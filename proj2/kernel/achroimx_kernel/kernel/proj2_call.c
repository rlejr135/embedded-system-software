#include <linux/kernel.h>
#include <asm/uaccess.h>

asmlinkage unsigned int sys_newcall(int interval, int count, int option, unsigned int *err_value){
	char start_loc, start_val;
	int i, where = 10000, temp;
	int error = 0,flag = 0;
	unsigned int res;

	// check error
	if (option < 1 || option > 8000){
		printk("error: option is not [0001 ~ 8000]\n");
		error = 1;
	}
	else if (interval < 1 || interval > 100){
		printk("error: interval is not [1 ~ 100]\n");
		error = 1;
	}
	else if (count < 1 || count > 100){
		printk("error: count is not [1 ~ 100]\n");
		error = 1;	
	}
	else{
		for (i = 1 ; i <= 4 ; i++){
			temp = (option % where) / (where/10);
			if (temp != 0 && flag == 0){
				start_loc = i;
				start_val = temp;
				flag = 1;
			}
			else if(temp != 0 && flag == 1){
				printk("error: option must have one nonzero value\n");
				error = 1;
			}
			where /= 10;
		}

		// make data
		res = (start_loc << 24 | start_val << 16 | interval << 8 | count);
	}
	copy_to_user(err_value, &error, sizeof(int));

	return res;
}

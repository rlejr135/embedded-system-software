
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>

#include "fpga_dot_font.h"


#define IOM_DEV_MAJOR 242
#define IOM_DEV_NAME "dev_driver"		

#define IOM_FPGA_DOT_ADDRESS 0x08000210		
#define IOM_LED_ADDRESS 0x08000016 
#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090

#define LINE_BUF 16
#define MAX_BUF 32

#define LEFT_TO_RIGHT 0
#define RIGHT_TO_LEFT 1


static int fpga_dev_usage = 0;

static unsigned char *iom_fpga_text_lcd_addr;
static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_dot_addr;

int iom_fpga_dev_init(void);
void iom_fpga_dev_exit(void);
int iom_fpga_dev_open(struct inode *mynode, struct file *myfile);
int iom_fpga_dev_release(struct inode *mynode, struct file *myfile);
long iom_fpga_dev_ioctl(struct file *myfile, unsigned int ioctl_num, unsigned long ioctl_param);
ssize_t iom_fpga_dev_write(struct file *inode, const char *buf, size_t length, loff_t *offset);
void callback_handler(unsigned long arg);
void start_timer(const char *buf);


struct file_operations iom_fpga_dev_fops = {
	.owner = 			THIS_MODULE,
	.open = 			iom_fpga_dev_open,
	.release=			iom_fpga_dev_release,
	.write=				iom_fpga_dev_write,
	.unlocked_ioctl=	iom_fpga_dev_ioctl,
};

// **** add for timer and output **** //

struct timer_data{
	int pos;					//	where fnd
	int now_val;				// what number
	int interval;				// interval
	int start_val;				// start at where
	int expire;					// finsih time
	struct timer_list timer;	// timer
};

static struct timer_data my_timer;
static int timer_set;

const unsigned short led_num[9] = {0, 128, 64, 32, 16, 8, 4, 2, 1};

unsigned char text_string[33] = {"20141602        Kideok Hwang    "};

void print_output(struct timer_data *);
void init_output(void);


////////////////////////////////////////






int iom_fpga_dev_init(void){
	int res;
	res = register_chrdev(IOM_DEV_MAJOR, IOM_DEV_NAME, &iom_fpga_dev_fops);

	if (res < 0){
		printk("Registering char device failed\n");
		return res;
	}

	// **** mmap each device **** //
	iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);

	printk("success register. dev name : %s, major number : %d\n", IOM_DEV_NAME, IOM_DEV_MAJOR);

	return 0;
}

void iom_fpga_dev_exit(void){
	
	if (timer_set == 1){
		del_timer_sync(&my_timer.timer);
	}

	// **** unmap each device **** //
	iounmap(iom_fpga_dot_addr);
	iounmap(iom_fpga_fnd_addr);
	iounmap(iom_fpga_led_addr);
	iounmap(iom_fpga_text_lcd_addr);

	unregister_chrdev(IOM_DEV_MAJOR, IOM_DEV_NAME);
}

int iom_fpga_dev_open(struct inode *mynode, struct file *myfile){
	if (fpga_dev_usage != 0) return -EBUSY;

	fpga_dev_usage = 1;
	timer_set = 0;

	return 0;
}

int iom_fpga_dev_release(struct inode *mynode, struct file *myfile){
	
	fpga_dev_usage = 0;
	
	return 0;
}

long iom_fpga_dev_ioctl(struct file *myfile, unsigned int ioctl_num, unsigned long ioctl_param){
	switch(ioctl_num){
		case 0:
			start_timer((char*)ioctl_param);
			break;
		default:
			break;
	}
	return 0;
}


ssize_t iom_fpga_dev_write(struct file *inode, const char *buf, size_t length, loff_t *offset){
	
		start_timer(buf);
	return 0;
}

void start_timer(const char *buf){	

	my_timer.pos = buf[3];
	my_timer.now_val = buf[2];
	my_timer.interval = buf[1];
	my_timer.start_val = buf[2];
	my_timer.expire = buf[0];

	if (timer_set == 1){
		del_timer_sync(&my_timer.timer);
	}

	init_timer(&my_timer.timer);
	init_output();

	my_timer.timer.expires = get_jiffies_64() + my_timer.interval * HZ / 10;
	my_timer.timer.data = (unsigned long)&my_timer;
	my_timer.timer.function = callback_handler;


	timer_set = 1;
	add_timer(&my_timer.timer);


}

void callback_handler(unsigned long arg){
	struct timer_data *tmp_data;
	tmp_data = (struct timer_data *)arg;

	if (tmp_data->expire == 0){
		timer_set = 0;
		init_output();
		return;
	}

	print_output(tmp_data);

	tmp_data->timer.expires = get_jiffies_64() + tmp_data->interval * HZ / 10;
	tmp_data->timer.data = (unsigned long) tmp_data;
	tmp_data->timer.function = callback_handler;

	add_timer(&tmp_data->timer);
}

void init_output(void){
	
	int i;
	unsigned short int _s_value;
	unsigned char fnd_val[4] = {0,0,0,0};
	char tmp = ' ';

	// **** fnd init **** //
	_s_value = fnd_val[0] << 12 | fnd_val[1] << 8 | fnd_val[2] << 4 | fnd_val[3];
	outw(_s_value, (unsigned int)iom_fpga_fnd_addr);


	// **** led init **** //
	_s_value = 0;
	outw(_s_value, (unsigned int)iom_fpga_led_addr);


	// **** dot init **** //
	for (i = 0 ; i < 10 ; i++){
		_s_value = 0x00;
		outw(_s_value, (unsigned int)iom_fpga_dot_addr + i*2);
	}


	// **** text init **** //

	text_string[MAX_BUF] = 0;
	
	for (i = 0 ; i < MAX_BUF ; i++){
		_s_value = (tmp & 0xFF) << 8 | (tmp & 0xFF);
		outw(_s_value, (unsigned int)iom_fpga_text_lcd_addr+i);
		i++;
	}


}

void print_output(struct timer_data *now_timer){

	int i;
	unsigned short int _s_value;
	unsigned char dot_num[10], fnd_val[4] = {0, 0, 0, 0};
	static int up = LEFT_TO_RIGHT, down = LEFT_TO_RIGHT;


	// **** print fnd **** //
	fnd_val[now_timer->pos - 1] = (unsigned char)now_timer->now_val;
	_s_value = fnd_val[0] << 12 | fnd_val[1] << 8 | fnd_val[2] << 4 | fnd_val[3];
	outw(_s_value, (unsigned int)iom_fpga_fnd_addr);



	// **** print led **** //
	_s_value = led_num[now_timer->now_val] & 0xFFFF;
	outw(_s_value, (unsigned int)iom_fpga_led_addr);


	// **** print dot **** //
	for (i = 0 ; i < 10 ; i++){
		dot_num[i] = fpga_number[now_timer->now_val][i];
	}
	for (i = 0 ; i < 10 ; i++){
		_s_value = dot_num[i] & 0x7F;
		outw(_s_value, (unsigned int)iom_fpga_dot_addr + i*2);
	}


	// **** print text **** //
	text_string[MAX_BUF] = 0;
	for (i = 0 ; i < MAX_BUF ; i++){
		_s_value = (text_string[i] & 0xFF) << 8 | (text_string[i+1] & 0xFF);
		outw(_s_value, (unsigned int)iom_fpga_text_lcd_addr + i);
		i++;
	}

	
	// **** calculate for next **** //

	now_timer->now_val++;
	if (now_timer->now_val == 9) now_timer->now_val = 1;

	if (now_timer->now_val == now_timer->start_val){
		now_timer->pos++;
		if (now_timer->pos == 5) now_timer->pos = 1;
	}
	now_timer->expire--;


	// **** move string to left or right **** //
	if (up == LEFT_TO_RIGHT && text_string[LINE_BUF - 1] != ' ') up = RIGHT_TO_LEFT;
	else if (up == RIGHT_TO_LEFT && text_string[0] != ' ') up = LEFT_TO_RIGHT;

	if (down == LEFT_TO_RIGHT && text_string[MAX_BUF - 1] != ' ') down = RIGHT_TO_LEFT;
	else if (down == RIGHT_TO_LEFT && text_string[LINE_BUF] != ' ') down = LEFT_TO_RIGHT;


	if (up == LEFT_TO_RIGHT){
		for (i = LINE_BUF-1 ; i> 0 ; i--){
			text_string[i] = text_string[i-1];
		}
		text_string[0] = ' ';
	}
	else if (up == RIGHT_TO_LEFT){
		for (i = 0 ; i < LINE_BUF-1 ; i++){
			text_string[i] = text_string[i+1];
		}
		text_string[LINE_BUF-1] = ' ';
	}


	if (down == LEFT_TO_RIGHT){
		for (i = MAX_BUF-1 ; i > LINE_BUF ; i--){
			text_string[i] = text_string[i-1];
		}
		text_string[LINE_BUF] = ' ';
	}
	else if (down == RIGHT_TO_LEFT){
		for (i = LINE_BUF ; i < MAX_BUF - 1; i++){
			text_string[i] = text_string[i+1];
		}
		text_string[MAX_BUF - 1] = ' ';
	}

}


module_init(iom_fpga_dev_init);
module_exit(iom_fpga_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kideok Hwang");

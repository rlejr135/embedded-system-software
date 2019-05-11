
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


static int fpga_dev_usage = 0;

static unsigned char *iom_fpga_text_lcd_addr;
static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_dot_addr;

int iom_fpga_dev_init(void);
void iom_fpga_dev_exit(void);
int iom_fpga_dev_open(struct inode *mynode, struct file *myfile);
int iom_fpga_dev_release(struct inode *mynode, struct file *myfile);
int iom_fpga_dev_ioctl(struct file *myfile, unsigned int ioctl_num, unsigned long ioctl_param);
ssize_t iom_fpga_dev_write(struct file *inode, const char *buf, size_t length, loff_t *offset);

struct file_operations iom_fpga_dev_fops = {
	.owner = 			THIS_MODULE,
	.open = 			iom_fpga_dev_open,
	.release=			iom_fpga_dev_release,
	.write=				iom_fpga_dev_write,
	.unlocked_ioctl=	iom_fpga_dev_ioctl,
};








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
	iom_fpga_text_lcd_addr = ioremap(IOM_LED_ADDRESS, 0x32);

	printk("success register. dev name : %s, major number : %d\n", IOM_DEV_NAME, IOM_DEV_MAJOR);

	return 0;
}

void iom_fpga_dev_exit(void){
	
	// **** unmap each device **** //
	iounmap(iom_fpga_dot_addr);
	iounmap(iom_fpga_fnd_addr);
	iounmap(iom_fpga_led_addr);
	iounmap(iom_fpga_text_lcd_addr);

	unregister_chrdev(IOM_DEV_MAJOR, IOM_DEV_NAME);
}

int iom_fpga_dev_open(struct inode *mynode, struct file *myfile){
//	if (fpga_dev_usage != 0) return -EBUSY;

	fpga_dev_usage = 1;

	return 0;
}

int iom_fpga_dev_release(struct inode *mynode, struct file *myfile){
	
	fpga_dev_usage = 0;
	
	return 0;
}

int iom_fpga_dev_ioctl(struct file *myfile, unsigned int ioctl_num, unsigned long ioctl_param){
	int number = (int)ioctl_num;
	switch(number){
		case 0:
			iom_fpga_dev_write(myfile, (char*)ioctl_param, 4, 0);
			break;
		default:
			break;
	}
	return 0;
}


ssize_t iom_fpga_dev_write(struct file *inode, const char *buf, size_t length, loff_t *offset){
	unsigned char gdata[4];
	int i = 0;
	while(i < 4){
		gdata[3 - i] = buf[i];
		i++;
	}

	printk("%d %d %d %d\n", gdata[0], gdata[1], gdata[2], gdata[3]);

	return 0;
}


module_init(iom_fpga_dev_init);
module_exit(iom_fpga_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kideok Hwang");

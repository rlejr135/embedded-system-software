
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

#define ION_FPGA_DOT_ADDRESS 0x08000210		
#define IOM_LED_ADDRESS 0x08000016 
#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090


static int fpga_dev_usage = 0;

static unsigned char *iom_fpga_text_lcd_addr;
static unsigned char *iom_fpga_fnd_addr;
static unsigned char *iom_fpga_led_addr;
static unsigned char *iom_fpga_dot_addr;

int iom_fpga_dev_open(struct inode *mynode, struct file *myfile);
int iom_fpga_dev_release(struct inode *mynode, struct file *myfile);
int iom_fpga_dev_ioctl(struct )

struct file_operations iom_fpga_dev_fops = {
	owner: 		THIS_MODULE,
	open: 		iom_fpga_dev_open,
	release:	iom_fpga_dev_release,
	write:		iom_fpga_dev_write,
};




module_init(iom_fpga_dev_open);
module_release(iom_fpga_dev_release);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kideok Hwang");

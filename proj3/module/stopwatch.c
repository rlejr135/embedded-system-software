#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <linux/platform_device.h>
#include <asm/gpio.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/cdev.h>

#define P3_DEV_MAJOR 242
#define P3_DEV_NAME "stopwatch"

#define TRUE 1
#define FALSE 0


/////////////////////////////////////
// **** For fnd output device **** //
/////////////////////////////////////
#define P3_FND_ADDRESS 0x08000004
static unsigned char *p3_fpga_fnd_addr;



//////////////////////////////
// **** For dev driver **** //
//////////////////////////////
static int inter_major=P3_DEV_MAJOR, inter_minor=0;
static int result;
static dev_t inter_dev;
static struct cdev inter_cdev;
static int inter_open(struct inode *, struct file *);
static int inter_release(struct inode *, struct file *);
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int inter_usage = 0;



////////////////////////////////////
// **** For interrupt regist **** //
////////////////////////////////////
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg);


//////////////////////////////
// **** For wait queue **** //
//////////////////////////////
wait_queue_head_t proj3_waitq;
DECLARE_WAIT_QUEUE_HEAD(proj3_waitq);


/////////////////////////////
// **** Add for timer **** //
/////////////////////////////


#define TIME_INTERVAL HZ			// 1 per 1 sec

struct timer_data{
	unsigned char min;
	unsigned char sec;
	unsigned int pause_flag;			// default == false. If it pause, set true
	unsigned int rest_time;				// Store (expires - now_jiffies) value when pause is set
	struct timer_list timer;
};

static struct timer_data my_timer;
static int timer_set;

void callback_handler(unsigned long arg);
void start_timer(void);
void print_fnd(struct timer_data *);
void init_fnd_and_data(void);





static struct file_operations inter_fops =
{
	.open = inter_open,
	.write = inter_write,
	.release = inter_release,
};
	
///////////////////////////////
// **** Start stopwatch **** //
///////////////////////////////
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg) {
	printk(KERN_ALERT "interrupt1!!! = %x\n", gpio_get_value(IMX_GPIO_NR(1, 11)));

	// **** Start, or re - start timer **** //
	if (timer_set == FALSE){
		timer_set = TRUE;
		start_timer();

	}

	return IRQ_HANDLED;
}

///////////////////////////////
// **** Pause stopwatch **** //
///////////////////////////////
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg) {
    printk(KERN_ALERT "interrupt2!!! = %x\n", gpio_get_value(IMX_GPIO_NR(1, 12)));

	// **** Set pause flag and rest time, delete timer **** //
	if (timer_set == TRUE){
		my_timer.rest_time = my_timer.timer.expires - get_jiffies_64();
		my_timer.pause_flag = TRUE;

		del_timer(&my_timer.timer);
		timer_set = FALSE;
	}

    return IRQ_HANDLED;
}

///////////////////////////////
// **** Reset stopwatch **** //
///////////////////////////////
irqreturn_t inter_handler_volup(int irq, void* dev_id,struct pt_regs* reg) {
    printk(KERN_ALERT "interrupt3!!! = %x\n", gpio_get_value(IMX_GPIO_NR(2, 15)));

	if (timer_set == TRUE){
		del_timer(&my_timer.timer);
		timer_set = FALSE;
	}
	init_fnd_and_data();

    return IRQ_HANDLED;
}

//////////////////////////////////////////////////////////////////////
// **** Finish program when this button has pushed until 3 sec **** //
//////////////////////////////////////////////////////////////////////
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg) {

	static int check_finish = 0;
	static int prev_val = 0, now_val = 0;

    printk(KERN_ALERT "interrupt4!!! = %x\n", gpio_get_value(IMX_GPIO_NR(5, 14)));


	if (prev_val == 0){
		
		prev_val = get_jiffies_64();

	}


	printk(KERN_ALERT, "hi\n");
	// **** Finish timer and wake up **** //
	if (timer_set == TRUE){
		del_timer(&my_timer.timer);
		timer_set = FALSE;

	
	init_fnd_and_data();
	__wake_up(&proj3_waitq, 1, 1, NULL);
	}

	printk("hi\n");
 	return IRQ_HANDLED;
}




void callback_handler(unsigned long arg){
	
	struct timer_data *tmp_data;

	tmp_data = (struct timer_data *)arg;

	print_fnd(tmp_data);

	tmp_data->timer.expires = get_jiffies_64() + TIME_INTERVAL;
	tmp_data->timer.data = (unsigned long) tmp_data;
	tmp_data->timer.function = callback_handler;

	add_timer(&tmp_data->timer);
}

void start_timer(void){

	// **** If timer is start first time (not pause and re - start) **** //
	if (my_timer.pause_flag == FALSE){
		// **** Init timer and data **** //
		init_fnd_and_data();
		init_timer(&my_timer.timer);


		print_fnd(&my_timer);

		my_timer.timer.expires = get_jiffies_64() +  TIME_INTERVAL;
		my_timer.timer.data = (unsigned long)&my_timer;
		my_timer.timer.function = callback_handler;

		// **** add timer **** //
		add_timer(&my_timer.timer);
	}
	// **** If timer is paused and re - start **** //
	else if (my_timer.pause_flag == TRUE){

		my_timer.timer.expires = get_jiffies_64() + my_timer.rest_time;
		my_timer.timer.data = (unsigned long)&my_timer;
		my_timer.timer.function = callback_handler;


		my_timer.pause_flag = FALSE;
		my_timer.rest_time = 0;

		add_timer(&my_timer.timer);
	}
}

void print_fnd(struct timer_data *now_timer){

	unsigned short int _s_value;
	unsigned char fnd_val[4] = {0, 0, 0, 0};


	// **** Setting fnd value **** //

	fnd_val[0] = now_timer->min / 10;
	fnd_val[1] = now_timer->min % 10;
	fnd_val[2] = now_timer->sec / 10;
	fnd_val[3] = now_timer->sec % 10;
	
	_s_value = fnd_val[0] << 12 | fnd_val[1] << 8 | fnd_val[2] << 4 | fnd_val[3];
	outw(_s_value, (unsigned int)p3_fpga_fnd_addr);


	// **** Recalculate min and sec value **** //

	now_timer->sec += 1;
	if (now_timer->sec >= 60){
		now_timer->sec = 0;
		now_timer->min +=1;
		if (now_timer->min >= 60){
			now_timer->min = 0;
		}
	}

}

void init_fnd_and_data(void){
	
	unsigned short int _s_value;
	unsigned char fnd_val[4] = {0, 0, 0, 0};

	// **** Initialize fnd dev **** //
	_s_value = fnd_val[0] << 12 | fnd_val[1] << 8 | fnd_val[2] << 4 | fnd_val[3];
	outw(_s_value, (unsigned int)p3_fpga_fnd_addr);

	my_timer.min = 0;
	my_timer.sec = 0;
	my_timer.rest_time = 0;
	my_timer.pause_flag = FALSE;

}



////////////////////////////////////////
// **** regist interrupt handler **** //
////////////////////////////////////////
static int inter_open(struct inode *minode, struct file *mfile){
	int ret;
	int irq;

	printk(KERN_ALERT "Open Module\n");

	// interrupt home button
	gpio_direction_input(IMX_GPIO_NR(1,11));
	irq = gpio_to_irq(IMX_GPIO_NR(1,11));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, &inter_handler_home, IRQF_TRIGGER_FALLING, "home", NULL);

	// interrupt back button
	gpio_direction_input(IMX_GPIO_NR(1,12));
	irq = gpio_to_irq(IMX_GPIO_NR(1,12));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, &inter_handler_back, IRQF_TRIGGER_FALLING, "back", NULL);

	// interrupt volume up button
	gpio_direction_input(IMX_GPIO_NR(2,15));
	irq = gpio_to_irq(IMX_GPIO_NR(2,15));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, &inter_handler_volup, IRQF_TRIGGER_FALLING, "volup", NULL);

	// interrupt volume down button
	gpio_direction_input(IMX_GPIO_NR(5,14));
	irq = gpio_to_irq(IMX_GPIO_NR(5,14));
	printk(KERN_ALERT "IRQ Number : %d\n",irq);
	ret = request_irq(irq, &inter_handler_voldown, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "voldown", NULL);

	inter_usage = 1;
	return 0;
}

// **** Dev driver release **** //
static int inter_release(struct inode *minode, struct file *mfile){
	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
	free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);
	
	printk(KERN_ALERT "Release Module\n");
	inter_usage = 0;
	return 0;
}


// **** Sleep when write function is called **** //
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos ){
	printk("write\n");
	interruptible_sleep_on(&proj3_waitq);

	return 0;
}



//////////////////////////////////
// **** init device driver **** //
//////////////////////////////////

static int inter_register_cdev(void)
{
	int error;
//	if(inter_major) {
		inter_dev = MKDEV(inter_major, inter_minor);
		error = register_chrdev_region(inter_dev,1,"inter");
//	}else{
//		error = alloc_chrdev_region(&inter_dev,inter_minor,1,"inter");
//		inter_major = MAJOR(inter_dev);
//	}
	if(error<0) {
		printk(KERN_WARNING "inter: can't get major %d\n", inter_major);
		return result;
	}
	printk(KERN_ALERT "major number = %d\n", inter_major);
	cdev_init(&inter_cdev, &inter_fops);
	inter_cdev.owner = THIS_MODULE;
	inter_cdev.ops = &inter_fops;
	error = cdev_add(&inter_cdev, inter_dev, 1);
	if(error)
	{
		printk(KERN_NOTICE "inter Register Error %d\n", error);
	}
	return 0;
}


static int __init inter_init(void) {
	int result;
	if((result = inter_register_cdev()) < 0 )
		return result;

	// **** Mapping fnd device **** //
	p3_fpga_fnd_addr = ioremap(P3_FND_ADDRESS, 0x4);


	printk(KERN_ALERT "Init Module Success \n");
	printk(KERN_ALERT "Device : /dev/stopwatch, Major Num : 242 \n");
	return 0;
}
/////////////////////////////////
/////////////////////////////////


static void __exit inter_exit(void) {
	cdev_del(&inter_cdev);
	unregister_chrdev_region(inter_dev, 1);

	// **** Unmap fnd device **** //
	iounmap(p3_fpga_fnd_addr);

	printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kideok Hwang");

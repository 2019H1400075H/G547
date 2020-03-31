#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>
#include<linux/random.h>




static dev_t one_;		

static struct cdev c_dev;	
static struct class *my_cls;	


#define MAGIC_NUMBER 'A'
#define CHANNEL_SELECT_SEQNO 0x01
#define RESULT_ALIGNMENT_SEQNO 0x02

#define CHANNEL_SELECT _IOW(MAGIC_NUMBER, CHANNEL_SELECT_SEQNO, unsigned long)
#define RESULT_ALIGNMENT _IOW(MAGIC_NUMBER, RESULT_ALIGNMENT_SEQNO, unsigned long)


static unsigned long chl = 0;
static unsigned long align = 0;

//Driver callback functions

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "ADC8 device will now open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "ADC8 device will now close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	unsigned int result;
	printk(KERN_INFO "ADC8 device is being read()\n");
	get_random_bytes(&result, sizeof(result));	
	printk(KERN_INFO "ADC8 device := Random numbers are being generated - %d\n",result);
	result=result%1024;
	copy_to_user(buf, result, sizeof(result));
	printk(KERN_INFO "ADC8 device := Value read from Channel - %d is %d\n",chl, result);
	return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{

	switch(cmd){
		case CHANNEL_SELECT:
		printk(KERN_INFO "ADC8 device : IOCTL - CHANNEL_SELECT\n");
		copy_from_user(&chl, arg, sizeof(chl));
		printk(KERN_INFO "ADC8 device : Selected Channel - %d\n",chl);
		break;
		case RESULT_ALIGNMENT:
		printk(KERN_INFO "ADC8 device : IOCTL - RESULT_ALIGNMENT\n");
		copy_from_user(&align, arg, sizeof(align));
		printk(KERN_INFO "ADC8 device : Selected Alignment - %d\n",align);
		break;
		default:
		return -ENOTTY;
		}
	return 0;
}

static struct file_operations fops = 
				{
				.owner	= THIS_MODULE,
				.open	= my_open,
				.release= my_close,
				.read	= my_read,
				.unlocked_ioctl = my_ioctl
				};

static int __init adc_init(void)
{
	printk(KERN_INFO "8-Channel ADC Device Registration\n\n");

	//Step-1 : Reserve Major ans Minor Numbers for ADC8
	if(alloc_chrdev_region(&one_, 0, 1, "adc8") < 0)
	{
		return -1;
	}
	printk(KERN_INFO "<Major , Minor>: <%d, %d>\n", MAJOR(one_), MINOR(one_));

	//Step-2 : Creation of ADC8 device file
	if((my_cls = class_create(THIS_MODULE, "chardev")) == NULL)
	{
		unregister_chrdev_region(one_,1);
		return -1;
	}
	
	if(device_create(my_cls, NULL, one_, NULL, "adc8") == NULL)
	{
		class_destroy(my_cls);
		unregister_chrdev_region(one_,1);
		return -1;
	}

	//Step-3 : Link fops and cdev to the device node
	cdev_init(&c_dev, &fops);
	if(cdev_add(&c_dev, one_, 1) == -1)
	{
		device_destroy(my_cls, one_);
		class_destroy(my_cls);
		unregister_chrdev_region(one_,1);
		return -1;
	}

	return 0;
}

static void __exit adc_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(my_cls, one_);
	class_destroy(my_cls);
	unregister_chrdev_region(one_, 1);
	printk(KERN_INFO "8 bit Channel ADC Device Unregistrated");	
}

module_init(adc_init);
module_exit(adc_exit);

MODULE_DESCRIPTION("8 bit Channel ADC");
MODULE_AUTHOR("Prathamesh Gujar");
MODULE_LICENSE("GPL");

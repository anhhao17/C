#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/sysfs.h>
#include <linux/timer.h>

#define IRQ_NO 1
unsigned int i=0;

unsigned long handler_run_count=0;
DEFINE_SPINLOCK(cdd_spinlock);

static irqreturn_t irq_handler(int irq,void *dev_id){
    spin_lock_irq($cdd_spinlock);
    handler_run_count++;
    pr_info("Handler count %lu\n",handler_run_count);
    spin_unlock_irq(&cdd_spinlock);
    pr_info("Inside Interrupt handle\n");
    return IRQ_HANDLED;
}

dev_t dev=0;
static struct class *dev_class;
static struct cdev chr_dev;

static int __init cdd_init(void);
static int __exit cdd_exit(void);

static int cdd_open(struct inode *inode,struct file *file);
static int cdd_release(struct inode *inode,struct file *file);
static ssize_t cdd_read(struct *filp,char __user *buf,size_t len,loff_t *off);
static ssize_t cdd_write(struct *filp,char __user *buf,size_t len,loff_t *off);

static struct file_operations fops = {
    .open=cdd_open,
    .release=cdd_release,
    .read=cdd_read,
    .write=cdd_write,
    .owner=THIS_MODULE,
};
static int cdd_open(struct inode *inode,struct file *file){
    pr_info("Open file\n");
    return 0;
}
static int cdd_release(struct inode *inode,struct file *file){
    pr_info("Release file\n");
    return 0;
}
static ssize_t cdd_read(struct *filp,char __user *buf,size_t len,loff_t *off){
    pr_info("Read file\n");
    return 0;
}
static ssize_t cdd_write(struct *filp,char __user *buf,size_t len,loff_t *off){
    pr_info("Write file\n");
    return 0;
}   

static int __init cdd_init(void){
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
        pr_info("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&chr_cdev,&fops);

    /*Adding character device to the system*/
    if((cdev_add(&chr_dev,dev,1)) < 0){
        pr_info("Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_info("Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        pr_info("Cannot create the Device \n");
        goto r_device;
    }
    pr_info("Device Driver Insert...Done!!!\n");
        return 0;
r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    cdev_del(&chr_cdev);
    return -1;
}

static void __exit cdd_exit(void){
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&chr_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!\n");
}
module_init(cdd_init);
module_exit(cdd_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - Spinlock");
MODULE_VERSION("1.18");

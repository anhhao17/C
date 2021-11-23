#include <linux/module.h>
#include <linux/init.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>


#define DEV_MEM_SIZE 10
#define CLASS_NAME gpio_class
#define DEVICE_NAME gpio_dev
char device_buffer[DEV_MEM_SIZE];

dev_t dev;

struct *class gpio_class;

struct *device gpio_device;

ssize_t read (struct file *filp, char __user *buf, size_t len, loff_t *off)
{   

}
ssize_t write (struct file *filp, const char __user *buf, size_t len, loff_t *off)
{

}
int open (struct inode *inode, struct file *filp)
{
    pr_info("%s : Opened file\n",__func__);
    return 0;
}
int release (struct inode *inode, struct file *filp)
{
    pr_info("%s : closed file\n",__func__);
    return 0;
}

struct file_operations fops={
    
};
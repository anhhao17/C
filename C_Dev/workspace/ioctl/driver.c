#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>
#include <linux/ioctl.h>


#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__


#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

dev_t dev;

uint32_t value=0;
static struct class *ioc_class;
static struct cdev ioc_cdev;

static ssize_t ioc_read (struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    pr_info("Read func\n");
    return 0;
}
static ssize_t ioc_write (struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("Write func\n");
    return 0;
}
static int ioc_open (struct inode *inode, struct file *filp)
{
    pr_info("Open func\n");
    return 0;
}
static int ioc_release (struct inode *inode, struct file *filp)
{
    pr_info("Release func\n");
    return 0;
}
static long ioc_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case WR_VALUE :
        if(copy_from_user(&value,(uint32_t *)arg,sizeof(value))){
            pr_err("Data Write: Err !!\n");
            
        }
        pr_info("Value = %d\n",value);
        break;
    case RD_VALUE:
        if(copy_to_user((uint32_t *)arg,&value,sizeof(value))){
            pr_err("Data Read: Err !!\n");
            
        }
        //pr_info("Value = %d\n",value);
        break;

    default:
        pr_info("Default\n");
        break;
    }
    return 0;
}
static struct file_operations fops = {
    .read=ioc_read,
    .write=ioc_write,
    .open=ioc_open,
    .release=ioc_release,
    .unlocked_ioctl=ioc_ioctl,
    .owner=THIS_MODULE,
};
static int __init ioc_init(void)
{
        /*Allocating Major number*/
    if((alloc_chrdev_region(&dev,0,1,"etx_Dev"))<0){
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n",MAJOR(dev),MINOR(dev));
    cdev_init(&ioc_cdev,&fops);

    if((cdev_add(&ioc_cdev,dev,1))<0){
        pr_err("Cannot add device to system\n");
        goto r_class;
    }

    if((ioc_class=class_create(THIS_MODULE,"ioc_class"))==NULL){
        pr_err("cannot create struct class\n");
        goto r_class;
    }

    if((device_create(ioc_class,NULL,dev,NULL,"ioc_device"))==NULL){
        pr_err("cannot create device\n");
        goto r_device;
    }
    pr_info("Insert Module Successfully\n");

    return 0;
 
r_device:
        class_destroy(ioc_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

/*
** Module exit function
*/
static void __exit ioc_exit(void)
{
        device_destroy(ioc_class,dev);
        class_destroy(ioc_class);
        cdev_del(&ioc_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(ioc_init);
module_exit(ioc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anh Hao");
MODULE_DESCRIPTION("Example Ioctl");
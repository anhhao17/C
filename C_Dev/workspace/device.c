#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

#define DEV_SIZE 512
char dev_buf[DEV_SIZE];

dev_t device_number;
struct class *dev_class;
struct cdev dev;

ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    
    pr_info("read requested for %zu bytes\n",len);
	pr_info("Current file position = %lld\n",*off);

	
	/* Adjust the 'count' */
	if((*off + len) > DEV_SIZE)
		len = DEV_SIZE - *off;

	

	/*copy from user */
	if(copy_to_user(buf,&dev_buf[*off],len)){
		return -1;
	}

	

	/*update the current file postion */
	*off += len;

	pr_info("Number of bytes successfully read = %zu\n",len);
	pr_info("Updated file position = %lld\n",*off);

	/*Return number of bytes which have been successfully written */
	return len;
}
ssize_t dev_write(struct file *flie, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("write requested for %zu bytes\n",len);
	pr_info("Current file position = %lld\n",*off);

	
	/* Adjust the 'count' */
	if((*off + len) > DEV_SIZE)
		len = DEV_SIZE - *off;

	

	/*copy from user */
	if(copy_from_user(&dev_buf[*off],buf,len)){
		return -1;
	}

	

	/*update the current file postion */
	*off += len;

	pr_info("Number of bytes successfully written = %zu\n",len);
	pr_info("Updated file position = %lld\n",*off);

	/*Return number of bytes which have been successfully written */
	return len;
}
int dev_open(struct inode *inode, struct file *file)
{
    pr_info("open was successful\n");
    return 0;
}
int dev_release(struct inode *inode, struct file *file)
{
    pr_info("release successfull\n");
    return 0;
}

struct file_operations fops = {
    .owner=THIS_MODULE,
    .read=dev_read,
    .write=dev_write,
    .open=dev_open,
    .release=dev_release,
};
static int etx_driver_init(void){
    pr_info("Init module driver\n");
    int ret;
    ret=alloc_chrdev_region(&device_number,0,1,"dev");
    if(ret<0){
        pr_err("cannot allocate major number\n");
        return -1;
    }

    cdev_init(&dev,&fops);

    ret=cdev_add(&dev,device_number,1);
    if(ret<0){
        pr_err("Cannot add device to system\n");
        goto unr_chr;
    }
    dev_class=class_create(THIS_MODULE,"dev_class");
    if(dev_class==NULL){
        pr_err("cannot creat class in /sys/class\n");
        goto un_cdev;
    }
    if(device_create(dev_class,NULL,device_number,NULL,"dev_device")==NULL){
        pr_err("cannot create device\n");
        goto un_class;
    }
    pr_info("module load Succesfull\n");
    return 0;
un_class:
    class_destroy(dev_class);
un_cdev:
    cdev_del(&dev);
unr_chr:
    unregister_chrdev_region(device_number,1);
    pr_info("Module insertion failed\n");
    return ret;
}
static void __exit etx_driver_exit(void){
    device_destroy(dev_class,device_number);
    class_destroy(dev_class);
    cdev_del(&dev);
    unregister_chrdev_region(device_number,1);
    
    pr_info("Device driver unloaded\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anh Hao");
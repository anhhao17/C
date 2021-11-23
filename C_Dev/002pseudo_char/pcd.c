#include <linux/module.h>
#include <linux/init.h> 
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#define DEV_MEM_SIZE 512

char device_buffer[DEV_MEM_SIZE];

/* this hold the device numbers*/
dev_t device_number;

/*cdev variable*/
struct cdev  pcd_cdev;
/*holds the class pointer */
struct class *class_pcd;

struct device *device_pcd;



off_t  pcd_lseek (struct file * filp, loff_t offset, int whence)
{
    loff_t temp;

	pr_info("lseek requested \n");
	pr_info("Current value of the file position = %lld\n",filp->f_pos);

	switch(whence)
	{
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0))
				return -EINVAL;
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
	
	pr_info("New value of the file position = %lld\n",filp->f_pos);

	return filp->f_pos;
}

ssize_t  pcd_read (struct file * filp, char __user * buff, size_t count, loff_t * f_pos)
{
	pr_info("read requested for %zu byte\n",count);
	/*Adjust the count*/
	if((*f_pos+count)>DEV_MEM_SIZE){
		count=DEV_MEM_SIZE-*f_pos;
	}
	if(copy_to_user(buff,&device_buffer[*f_pos],count))
		return -EFAULT;

	
	/*update the current file postion */
	*f_pos += count;

	pr_info("Number of bytes successfully read = %zu\n",count);
	pr_info("Updated file position = %lld\n",*f_pos);

	/*Return number of bytes which have been successfully read */
	return count;
	

}
ssize_t  pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("write requested for %zu bytes\n",count);
	pr_info("Current file position = %lld\n",*f_pos);

	
	/* Adjust the 'count' */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;

	

	/*copy from user */
	if(copy_from_user(&device_buffer[*f_pos],buff,count)){
		return -EFAULT;
	}

	

	/*update the current file postion */
	*f_pos += count;

	pr_info("Number of bytes successfully written = %zu\n",count);
	pr_info("Updated file position = %lld\n",*f_pos);

	/*Return number of bytes which have been successfully written */
	return count;
	
}
int  pcd_release (struct inode *inode, struct file *filp)
{

	pr_info("release successful\n");
	return 0;
}
int  pcd_open (struct inode *inode, struct file *filp)
{
	pr_info("open successful\n");
	return 0;
}
/* file operations  of driver */
struct file_operations  pcd_fops = 
{
	.open=pcd_open,
	.write=pcd_write,
	.read=pcd_read,
	.release=pcd_release,
	.owner=THIS_MODULE

};



static int __init pcd_init(void)
{
	int ret;
    /*1. Dynamically allocate a device number */
	ret=alloc_chrdev_region(&device_number,0,1,"pcd_devices");
	if(ret<0){
		pr_err("cannot allocate char device\n");
		goto out;
	} 	// cat /proc/device "pcd_devices"
	pr_info("%s : Device number <major:minor> = <%d:%d>\n",__func__,MAJOR(device_number),MINOR(device_number));

	/*2. Initialize cdev structure with fops*/

	cdev_init(&pcd_cdev,&pcd_fops);

	/*3. Register cdev structure via VFS */
	pcd_cdev.owner=THIS_MODULE;
	ret=cdev_add(&pcd_cdev,device_number,1);
	if(ret<0){
		pr_err("cannot register cdev struct via VFS\n");
		goto unreg_chrdev;
	}
	/*4 create device class under sys/class/ */
	class_pcd=class_create(THIS_MODULE,"pcd_class");
	if(IS_ERR(class_pcd)){
		pr_err("cannot create class\n");
		ret=PTR_ERR(class_pcd);
		goto cdev_del;
	}
	/*5 device file creation*/
	device_pcd=device_create(class_pcd,NULL,device_number,NULL,"pcd_dev");
	if(IS_ERR(device_pcd)){
		pr_err("cannot create device file\n");
		ret=PTR_ERR(device_pcd);
		goto class_del;
	}
	pr_info("MODULE init SUCCESSFULL\n");
	return 0;
class_del:
	class_destroy(class_pcd);
cdev_del:
	cdev_del(&pcd_cdev);
unreg_chrdev:
	unregister_chrdev_region(device_number,1);
out:
	pr_info("Module insertion failed\n");
	return ret;

}

static void __exit pcd_exit(void)
{
	device_destroy(class_pcd,device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number,1);
	pr_info("module unloaded\n");

}

module_init(pcd_init);
module_exit(pcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("awa");
MODULE_DESCRIPTION("a pseudo char device");
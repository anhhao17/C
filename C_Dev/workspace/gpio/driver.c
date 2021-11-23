#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

#define GPIO_21 (4)

dev_t dev;
static struct class *gpio_class;
static struct cdev gpio_cdev;


static int gpio_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}
/*
** This function will be called when we close the Device file
*/
static int gpio_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}

static ssize_t gpio_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
  uint8_t gpio_state = 0;
  
  //reading GPIO value
  gpio_state = gpio_get_value(GPIO_21);
  
  //write to user
  len = 1;
  if( copy_to_user(buf, &gpio_state, len) > 0) {
    pr_err("ERROR: Not all the bytes have been copied to user\n");
  }
  
  pr_info("Read function : GPIO_21 = %d \n", gpio_state);
  
  return 0;
}
static ssize_t gpio_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
    uint8_t rec_buf[10] = {0};
    if(copy_from_user(rec_buf,buf,len)){
        pr_err("not all bytes have been copied from user\n");
    }
    pr_info("Write Function : GPIO_21 Set = %d\n", rec_buf[0]);
    switch (rec_buf[0])
    {
    case 0:
        gpio_set_value(GPIO_21, 0);
        break;
    case 1:
        gpio_set_value(GPIO_21, 1);
        break;
    default:
        pr_err("Unknown command : Please provide either 1 or 0 \n");
        break;
    }
    return len;
  
  
}



static struct file_operations fops = {
    .open=gpio_open,
    .release=gpio_release,
    .read=gpio_read,
    .write=gpio_write,
    .owner=THIS_MODULE,
};
static int __init driver_gpio_init(void){
    if((alloc_chrdev_region(&dev, 0, 1, "gpio_Dev")) <0){
        pr_err("Cannot allocate major number\n");
        goto r_unreg;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
    
    /*Creating cdev structure*/
    cdev_init(&gpio_cdev,&fops);
    
    /*Adding character device to the system*/
    if((cdev_add(&gpio_cdev,dev,1)) < 0){
        pr_err("Cannot add the device to the system\n");
        goto r_del;
    }
    
    /*Creating struct class*/
    if((gpio_class = class_create(THIS_MODULE,"gpio_class")) == NULL){
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
    
    /*Creating device*/
    if((device_create(gpio_class,NULL,dev,NULL,"gpio_device")) == NULL){
        pr_err( "Cannot create the Device \n");
        goto r_device;
    }
    
    //Checking the GPIO is valid or not
    if(gpio_is_valid(GPIO_21) == false){
        pr_err("GPIO %d is not valid\n", GPIO_21);
        goto r_device;
    }
    
    //Requesting the GPIO
    if(gpio_request(GPIO_21,"GPIO_21") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_21);
        goto r_gpio;
    }
    
    //configure the GPIO as output
    gpio_direction_output(GPIO_21, 0);
    
    /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
    ** Now you can change the gpio values by using below commands also.
    ** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
    ** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
    ** cat /sys/class/gpio/gpio21/value  (read the value LED)
    ** 
    ** the second argument prevents the direction from being changed.
    */
    gpio_export(GPIO_21, false);
    
    pr_info("Device Driver Insert...Done!!!\n");
    return 0;
    
r_gpio:
        gpio_free(GPIO_21);
r_device:
        device_destroy(gpio_class,dev);
r_class:
        class_destroy(gpio_class);
r_del:
        cdev_del(&gpio_cdev);
r_unreg:
        unregister_chrdev_region(dev,1);
    
        return -1;
}
static void __exit driver_gpio_exit(void){
    gpio_unexport(GPIO_21);
    gpio_free(GPIO_21);
    device_destroy(gpio_class,dev);
    class_destroy(gpio_class);
    cdev_del(&gpio_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!\n");

}

module_init(driver_gpio_init);
module_exit(driver_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("anh Hao");
MODULE_DESCRIPTION("example gpio");

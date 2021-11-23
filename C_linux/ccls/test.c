#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

//command
static int __init hello_init(void){
	
	/* test basic driver */
	pr_info("hello world from driver\n");
	return 0;
}

static void __exit hello_exit(void){

	pr_info("Unloaded driver\n");

}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");



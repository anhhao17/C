
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define GPIO_BASE		0x3F200000
#define INTERRUPT_BASE	0x3F00B200
#define LED_PIN			17
#define BUTTON_PIN		24

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Truong Van Huy");
MODULE_DESCRIPTION("Dieu khien button led bang interrupt");
MODULE_VERSION("1.0");

static void __iomem *gpio;
static short int irq_gpio;
static bool onLed;
char my_tasklet_data[] = "my_tasklet_function was called";


static void INP_GPIO(u32 pin)
{
	u32 value = 0;
	u32 *address = NULL;

	if (pin > 40) {
		printk(KERN_ALERT "Chan pin khong hop le");
		return;
	}

	printk(KERN_INFO "%s: func start.", __func__);

	address = (u32 *)gpio + pin / 10;
	value = ioread32(address);
	value &= ~(7 << (((pin)%10)*3));
	iowrite32(value, address);
}

static void OUT_GPIO(u32 pin)
{
	u32 value = 0;
	u32 *address = NULL;

	if (pin > 40) {
		printk(KERN_ALERT "Chan pin khong hop le");
		return;
	}

	address = (u32 *)gpio + pin / 10;
	value = ioread32(address);
	value |= (1 << (((pin)%10)*3));
	iowrite32(value, address);
}

static void GPIO_SET(u32 pin)
{
	if (pin > 40) {
		printk(KERN_ALERT "Chan pin khong hop le");
		return;
	}

	iowrite32(1 << pin, (u32 *)gpio + 7);
}

static void GPIO_CLR(u32 pin)
{
	if (pin > 40) {
		printk(KERN_ALERT "Chan pin khong hop le");
		return;
	}

	iowrite32(1 << pin, (u32 *)gpio + 10);
}

static void GPIO_PULL(u32 value)
{
	iowrite32(value, (u32 *)gpio + 37);
}

static void GPIO_PULLCLK0(u32 clock)
{
	iowrite32(clock, (u32 *)gpio + 38);
}


/* Bottom Half Function */
void my_tasklet_function(unsigned long data)
{
	printk( "%s\n", (char *)data );

	onLed = !onLed;
	if(onLed)
	{
		GPIO_CLR(LED_PIN);
	}
	else
	{
		GPIO_SET(LED_PIN);
	}

	return;
}

DECLARE_TASKLET(my_tasklet, my_tasklet_function, (unsigned long) &my_tasklet_data);

/* Top Half Function */
static irqreturn_t  button_irq_handler(int irq, void *dev_id)
{

	printk(KERN_INFO "Interrupt for pin %d was triggered!\n", BUTTON_PIN);

	tasklet_schedule(&my_tasklet);

	return IRQ_HANDLED;
}

static int __init button_init(void)
{
	gpio = ioremap(GPIO_BASE, 1408);
	if (NULL == gpio) {
		printk(KERN_ERR "Khong mapping duoc gpio.\n");
		return PTR_ERR(gpio);
	}

	/*Init led*/
	INP_GPIO(LED_PIN);
	OUT_GPIO(LED_PIN);

	/*Init button*/
	INP_GPIO(BUTTON_PIN);
	GPIO_PULL(2);
	GPIO_PULLCLK0(0x01000000);

	irq_gpio = gpio_to_irq(BUTTON_PIN);
	if (irq_gpio < 0) {
		printk(KERN_INFO "%s: GPIO to IRQ mapping failure\n", __func__);
		return -ENODEV;
	}

	printk(KERN_INFO "%s: Mapped interrupt %d\n", __func__, irq_gpio);

	if (request_irq(irq_gpio, button_irq_handler,
		IRQF_TRIGGER_FALLING,
		"my_pin24",
		NULL)) {
		printk(KERN_INFO "%s: Irq request failure\n", __func__);
		return -ENODEV;
	}

	onLed = false;
	GPIO_SET(LED_PIN);

	return 0;
}

static void __exit button_exit(void)
{
	INP_GPIO(LED_PIN);
	GPIO_SET(LED_PIN);

	INP_GPIO(BUTTON_PIN);

	free_irq(irq_gpio, NULL);
	tasklet_kill(&my_tasklet);
		
	printk(KERN_INFO "Module removed\n");
}

module_init(button_init);
module_exit(button_exit);
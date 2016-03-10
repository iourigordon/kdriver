#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int airport_init(void)
{
    printk(KERN_ALERT "Hello World\n");
    return 0;
}

static void airport_exit(void)
{
    printk(KERN_ALERT "Goodbye\n");
}

module_init(airport_init);
module_exit(airport_exit);


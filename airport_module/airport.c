#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>


#define AIRPORT_MAJOR 39

enum {
    AIRPORT_HANGAR,
    AIRPORT_LAND_STRIP,
    AIRPORT_TAKEOFF_STRIP,
    AIRPORT_MAX
};

MODULE_LICENSE("Dual BSD/GPL");

static dev_t device;

static int airport_init(void)
{

    device = MKDEV(AIRPORT_MAJOR,0);
    if (register_chrdev_region(device,1,"airport")) {
        printk(KERN_ERR "Failed to get chrdev region");
    }
    return 0;
}

static void airport_exit(void)
{
    printk(KERN_ALERT "Unregistering airport devices\n");
    unregister_chrdev_region(device,1); 
}

module_init(airport_init);
module_exit(airport_exit);


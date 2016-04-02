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

static int major_version;

struct file_operations airport_ops = {
    .owner = THIS_MODULE
};

static int airport_init(void)
{
    major_version = register_chrdev(0, "airport", &airport_ops);
    if (major_version < 0)
        printk(KERN_ERR "Failed to register airport device\n");
    class_create(THIS_MODULE,"");
    return 0;
}

static void airport_exit(void)
{
    printk(KERN_ALERT "Unregistering airport devices\n");
    unregister_chrdev(major_version,"airport"); 
}

module_init(airport_init);
module_exit(airport_exit);


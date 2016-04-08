#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>


#define AIRPORT_MAJOR 39

enum {
    AIRPORT_HANGAR,
    AIRPORT_LAND_STRIP,
    AIRPORT_TAKEOFF_STRIP,
    AIRPORT_MAX
};

static char* device_ext[] = {"_hangar","_land_strip","_takeoff_strip"};

MODULE_LICENSE("Dual BSD/GPL");

static int major_versions[AIRPORT_MAX];
static int major_version;

static struct class*    airport_class;
static struct device*   airport_device[AIRPORT_MAX];

struct file_operations airport_ops = {
    .owner = THIS_MODULE
};

static int airport_init(void)
{
    int device_count = 0;

    major_version = register_chrdev(0, "airport", &airport_ops);
    if (major_version < 0){
        printk(KERN_ERR "Failed to register airport device major number\n");
        return major_version;
    }

    airport_class = class_create(THIS_MODULE,"airport");
    if (IS_ERR(airport_class)) {
        printk(KERN_ERR "Failed to create class airport");
        unregister_chrdev(major_version,"airport"); 
        return PTR_ERR(airport_class);
    }
    
    for (device_count=0;device_count<AIRPORT_MAX;device_count++) {
        airport_device[device_count] = device_create(airport_class, NULL, MKDEV(major_version,device_count), NULL, "airport%s",device_ext[device_count]);
        if (IS_ERR(airport_device[device_count])) {
            printk(KERN_ERR "Failed to create airport_hangar device\n");
            for (device_count-=1;device_count>-1;device_count--)
                device_destroy(airport_class,MKDEV(major_version,device_count));
            class_destroy(airport_class);
            unregister_chrdev(major_version,"airport");
            return PTR_ERR(airport_device);
        }
    } 

    printk(KERN_INFO "Airport driver is loaded\n");
    return 0;
}

static void airport_exit(void)
{
    int device_count;
    printk(KERN_ALERT "Unregistering airport devices\n");
    
    for (device_count=0;device_count<AIRPORT_MAX;device_count++)
        device_destroy(airport_class, MKDEV(major_version,device_count));
    class_destroy(airport_class);
    unregister_chrdev(major_version,"airport"); 
}

module_init(airport_init);
module_exit(airport_exit);


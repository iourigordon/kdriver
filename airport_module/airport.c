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
static char* device_names[] = {"airport_hangar","airport_land_strip","airport_takeoff_strip"};

MODULE_LICENSE("Dual BSD/GPL");

static int major_versions[AIRPORT_MAX];

static struct class*    airport_class;
static struct device*   airport_device[AIRPORT_MAX];

extern struct file_operations airport_hangar_ops;
extern struct file_operations airport_land_strip_ops;
extern struct file_operations airport_takeoff_strip_ops;

static struct file_operations* airport_fops[] = {&airport_hangar_ops,&airport_land_strip_ops,&airport_takeoff_strip_ops};

static int airport_init(void)
{
    int device_count = 0;

    printk(KERN_INFO "Loading airport driver ...\n");

    for (device_count = 0; device_count<AIRPORT_MAX;device_count++) {
        printk(KERN_INFO "Registering char dev %s\n",device_names[device_count]);
        major_versions[device_count] = register_chrdev(0, device_names[device_count], airport_fops[device_count]);
        printk(KERN_INFO "major_version = %d\n",major_versions[device_count]);
        if (major_versions[device_count] < 0){
            printk(KERN_ERR "Failed to register airport device major number\n");
            for(device_count-=1;device_count>-1;device_count--)
                unregister_chrdev(major_versions[device_count],device_names[device_count]);
            return major_versions[device_count];
        }
    }

    airport_class = class_create(THIS_MODULE,"airport");
    if (IS_ERR(airport_class)) {
        printk(KERN_ERR "Failed to create class airport");
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            unregister_chrdev(major_versions[device_count],device_names[device_count]); 
        return PTR_ERR(airport_class);
    }
    
    for (device_count=0;device_count<AIRPORT_MAX;device_count++) {
        airport_device[device_count] = device_create(airport_class, NULL, MKDEV(major_version,1), NULL, "airport%s",device_ext[device_count]);
        if (IS_ERR(airport_device[device_count])) {
            printk(KERN_ERR "Failed to create airport_hangar device\n");
            for (device_count-=1;device_count>-1;device_count--)
                device_destroy(airport_class,MKDEV(major_version,1));
            class_destroy(airport_class);
            for (device_count=0;device_count<AIRPORT_MAX;device_count++)
                unregister_chrdev(major_versions[device_count],device_names[device_count]); 
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
        device_destroy(airport_class, MKDEV(major_version,1));
    class_destroy(airport_class);
    for (device_count=0;device_count<AIRPORT_MAX;device_count++)
        unregister_chrdev(major_versions[device_count],device_names[device_count]); 
}

module_init(airport_init);
module_exit(airport_exit);


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
static dev_t dev_ts[AIRPORT_MAX];

MODULE_LICENSE("Dual BSD/GPL");

static struct class*    airport_class;
static struct device*   airport_device[AIRPORT_MAX];

extern struct file_operations airport_hangar_ops;
extern struct file_operations airport_land_strip_ops;
extern struct file_operations airport_takeoff_strip_ops;

static struct file_operations* airport_fops[] = {&airport_hangar_ops,&airport_land_strip_ops,&airport_takeoff_strip_ops};

extern int init_airport_hangar(int number_of_planes, int number_of_passengers);
extern void destroy_airport_hangar(void);

extern int create_land_strip(dev_t dev_num);
extern void destroy_land_strip(void);

extern int create_takeoff_strip(dev_t dev_num);
extern void destroy_takeoff_strip(void);

static int airport_init(void)
{
    int ret;
    int device_count = 0;

    printk(KERN_INFO "Loading airport_sim driver ...\n");

    memset(dev_ts,0,sizeof(dev_t)*AIRPORT_MAX);
    for (device_count = 0; device_count<AIRPORT_MAX;device_count++) {
        ret = alloc_chrdev_region(&dev_ts[device_count],0,1,device_names[device_count]);
        if (ERR_PTR(ret)) {
            for(device_count-=1;device_count>-1;device_count--)
                unregister_chrdev_region(dev_ts[device_count], 1);
            return ret;
        }
    }    

    airport_class = class_create(THIS_MODULE,"airport_sim");
    if (IS_ERR(airport_class)) {
        printk(KERN_ERR "Failed to create class airport");
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            unregister_chrdev_region(dev_ts[device_count], 1);
        return PTR_ERR(airport_class);
    }
    
    for (device_count=0;device_count<AIRPORT_MAX;device_count++) {
        airport_device[device_count] = device_create(airport_class, NULL, dev_ts[device_count], NULL, "airport%s",device_ext[device_count]);
        if (IS_ERR(airport_device[device_count])) {
            printk(KERN_ERR "Failed to create airport%s device\n",device_ext[device_count]);
            for (device_count-=1;device_count>-1;device_count--)
                device_destroy(airport_class,dev_ts[device_count]);
            class_destroy(airport_class);
            for (device_count=0;device_count<AIRPORT_MAX;device_count++)
                unregister_chrdev_region(dev_ts[device_count], 1);
            return PTR_ERR(airport_device);
        }
    } 


    if ((ret=init_airport_hangar(20,100)) != 0) {
        printk(KERN_ERR "Unregistering airport devices\n");
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            device_destroy(airport_class, dev_ts[device_count]);
        class_destroy(airport_class);
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            unregister_chrdev_region(dev_ts[device_count], 1);
        return ret;
    }

    if (create_land_strip(dev_ts[AIRPORT_LAND_STRIP])) {
        printk(KERN_ERR "Failed to register land strip\n");
        destroy_airport_hangar();
        printk(KERN_ERR "Unregistering airport devices\n");
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            device_destroy(airport_class, dev_ts[device_count]);
        class_destroy(airport_class);
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            unregister_chrdev_region(dev_ts[device_count], 1);
        return ret;
    } 

    if (create_takeoff_strip(dev_ts[AIRPORT_TAKEOFF_STRIP])) {
        printk(KERN_ERR "Failed to register takeoff strip\n");
        destroy_airport_hangar();
        destroy_land_strip();
        printk(KERN_ERR "Unregistering airport devices\n");
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            device_destroy(airport_class, dev_ts[device_count]);
        class_destroy(airport_class);
        for (device_count=0;device_count<AIRPORT_MAX;device_count++)
            unregister_chrdev_region(dev_ts[device_count], 1);
        return ret;

    }

    printk(KERN_INFO "airport_sim driver is loaded\n");
    return 0;
}

static void airport_exit(void)
{
    int device_count;

    destroy_takeoff_strip();
    destroy_land_strip();
    destroy_airport_hangar();

    printk(KERN_INFO "Unregistering airport devices\n");
    for (device_count=0;device_count<AIRPORT_MAX;device_count++)
        device_destroy(airport_class, dev_ts[device_count]);
    class_destroy(airport_class);
    for (device_count=0;device_count<AIRPORT_MAX;device_count++)
        unregister_chrdev_region(dev_ts[device_count], 1);

    printk(KERN_INFO "airport_sim Unloaded\n");
}

module_init(airport_init);
module_exit(airport_exit);


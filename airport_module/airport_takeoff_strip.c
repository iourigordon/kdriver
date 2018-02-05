#include <linux/fs.h>
#include <linux/cdev.h>

#define MAX_TAKEOFF_STRIP_RUNWAYS 2

struct file_operations airport_takeoff_strip_ops = {
    .owner = THIS_MODULE
};

struct _takeoff_strip {
    int max_planes;
    atomic_t available_runways;
    struct cdev cdev;
};

static struct _takeoff_strip takeoff_strip;

int takeoff_strip_open(struct inode *inode, struct file *filep)
{
    struct _takeoff_strip *takeoff_strip_dev;

    printk(KERN_INFO "Plane %d ready for take off\n",current->pid);
    return 0; 
    
}

int create_takeoff_strip(dev_t dev_num)
{
    takeoff_strip.max_planes = MAX_TAKEOFF_STRIP_RUNWAYS;
    atomic_set(&takeoff_strip.available_runways,MAX_TAKEOFF_STRIP_RUNWAYS);

    cdev_init(&(takeoff_strip.cdev),&airport_takeoff_strip_ops);
    takeoff_strip.cdev.owner = THIS_MODULE;
    takeoff_strip.cdev.ops = &airport_takeoff_strip_ops;

    if (cdev_add(&(takeoff_strip.cdev),dev_num,1)) {
        printk(KERN_ERR "failed to create airport_takeoff_strip cdev\n");
        return 1;
    }
    return 0;
}

void destroy_takeoff_strip(void)
{
    cdev_del(&(takeoff_strip.cdev));
}

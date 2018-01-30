#include <linux/fs.h>

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

int takeoff_strip_open(stuct inode *inode, struct file *filep)
{
    struc _takeoff_strip *takeoff_stripi_dev;

    printk(KERN_INFO "Plane %d ready for take off\n",current->pid);
    
    
}

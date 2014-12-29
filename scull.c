#include <linux/init.h>
#include <linux/printk.h>
#include <linux/fs.h>

#include <linux/types.h>
#include <linux/kdev_t.h>

#include "scull.h"

dev_t dev;

static int scull_major = SCULL_MAJOR;
static int scull_minor = SCULL_MINOR;
static int scull_nr_devs = SCULL_NR_DEVS;
static int result;

static int __init scull_init(void)
{
    printk(KERN_INFO "scull module init\n");
    if (scull_major)
    {
        dev = MKDEV(scull_major,scull_minor);
        result = register_chrdev_region(dev,scull_nr_devs,"scull");
    } 
    else 
    {
        result = alloc_chrdev_region(&dev,scull_minor,scull_nr_devs,"scull");
        scull_major = MAJOR(dev);
        printk(KERN_INFO "scull major %d\n",scull_major);
        printk(KERN_INFO "scull minor %d\n",scull_minor);
    }

    if (result < 0)
    {
        printk(KERN_WARNING "scull: can't get major %d\n",scull_major);
    }

    return 0;
}
module_init(scull_init);

static void __exit scull_cleanup(void)
{
    printk(KERN_INFO "scull module exit");
}
module_exit(scull_cleanup);

struct file_operations scull_fops = {
    .owner   = THIS_MODULE,
    .llseek  = scull_llseek,
    .read    = scull_read,
    .write   = scull_write,
    .ioctl   = scull_ioctl,
    .open    = scull_open,
    .release = scull_release, 
};

#include <linux/init.h>
#include <linux/printk.h>
#include <linux/fs.h>

#include <linux/types.h>
#include <linux/kdev_t.h>

#include "scull.h"

dev_t dev;

int scull_major     = SCULL_MAJOR;
int scull_minor     = SCULL_MINOR;
int scull_nr_devs   = SCULL_NR_DEVS;
int scull_quantum   = SCULL_QUANTUM;
int scull_qset      = SCULL_QSET;

static int result;

struct scull_dev *scull_devices;	/* allocated in scull_init_module */

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err,devno = MKDEV(scull_major,scull_minor + index);

    cdev_init(&dev->cdev,&scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops   = &scull_fops;
    err = cdev_add(&dev->cdev,devno,1);

    /* Fail gracefully if need be */
    if (err)
        printk(KERN_NOTICE,"Error %d adding scull %d",err,index);
}

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
        return result;
    }

    /* 
	* allocate the devices -- we can't have them static, as the number
	* can be specified at load time
	*/
	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

    /* Initialize each device. */
	for (i = 0; i < scull_nr_devs; i++) {
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		init_MUTEX(&scull_devices[i].sem);
		scull_setup_cdev(&scull_devices[i], i);
	}

    return 0;

    fail:
	    scull_cleanup_module();
	    return result;

}
module_init(scull_init);

static void __exit scull_cleanup(void)
{
    printk(KERN_INFO "scull module exit");

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, scull_nr_devs);

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

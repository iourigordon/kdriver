#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/thread_info.h>
#include <asm/atomic.h>

#define MAX_LAND_STRIP_RUNWAYS 2

struct _land_strip {
    int max_planes;
    atomic_t available_runways;
    struct cdev cdev;
};

static struct _land_strip land_strip;

void* get_cache(void);

ssize_t write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
    void* plane_cache = get_cache();
    if(!plane_cache)
        return -EFAULT;

    if(copy_from_user(plane_cache,buff,count))
        return -EFAULT;

    /*insert into kfifo*/
    if (!add_plane_to_hangar(plane_cache))
        return -EFAULT; 

    return -1;
}

int land_strip_open(struct inode *inode, struct file *filp)
{
    struct _land_strip *land_strip_dev;

    printk(KERN_INFO "Plane %d entering airspace\n",current->pid);

    land_strip_dev = container_of(inode->i_cdev,struct _land_strip,cdev);
    filp->private_data = land_strip_dev;
    
    return 0;
}

int land_strip_release(struct inode *indoe, struct file *filep)
{
    printk(KERN_INFO "Plane %d has landed\n",current->pid);
    return 0;
}

struct file_operations airport_land_strip_ops = {
    .owner = THIS_MODULE,
    .open = land_strip_open,
    .release = land_strip_release
};

int create_land_strip(dev_t dev_num)
{

    land_strip.max_planes = MAX_LAND_STRIP_RUNWAYS;
    atomic_set(&land_strip.available_runways,MAX_LAND_STRIP_RUNWAYS);

    cdev_init(&(land_strip.cdev),&airport_land_strip_ops);
    land_strip.cdev.owner = THIS_MODULE;
    land_strip.cdev.ops = &airport_land_strip_ops;

    if (cdev_add(&(land_strip.cdev),dev_num,1)) {
        printk(KERN_ERR "failed to create airport_land_strip cdev\n");
        return 1;
    }
    return 0;
}

void destroy_land_strip(void)
{
    cdev_del(&(land_strip.cdev));
}


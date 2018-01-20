#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/thread_info.h>

struct _land_strip {
    int max_planes;
    int landed_planes;
    struct cdev cdev;
};

static struct _land_strip land_strip;

int land_strip_open(struct inode *inode, struct file *filp) {
    struct _land_strip *land_strip_dev;

    printk(KERN_INFO "Plane %d entering airspace\n",current->pid);

    land_strip_dev = container_of(inode->i_cdev,struct _land_strip,cdev);
    filp->private_data = land_strip_dev;

    
    return 0;
}

int land_strip_release(struct inode *indoe, struct file *filep) {
    printk(KERN_INFO "Plane %d has landed\n",current->pid);
    return 0;
}

struct file_operations airport_land_strip_ops = {
    .owner = THIS_MODULE,
    .open = land_strip_open,
    .release = land_strip_release
};

int create_land_strip(dev_t dev_num) {
    cdev_init(&(land_strip.cdev),&airport_land_strip_ops);
    land_strip.cdev.owner = THIS_MODULE;
    land_strip.cdev.ops = &airport_land_strip_ops;

    if (cdev_add(&(land_strip.cdev),dev_num,1)) {
        printk(KERN_ERR "failed to create airport_land_strip cdev\n");
        return 1;
    }
    return 0;
}

void destroy_land_strip(void) {
    cdev_del((&land_strip.cdev));
}


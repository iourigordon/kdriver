#include <linux/fs.h>
#include <linux/cdev.h>

struct _land_strip {
    int max_planes;
    int landed_planes;
    struct cdev cdev;
};

static struct _land_strip land_strip;

struct file_operations airport_land_strip_ops = {
    .owner = THIS_MODULE
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


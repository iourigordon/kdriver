#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/slab.h>


#define HANGAR_MAX_PLANES 16

static char* hangar_cache_name = "hangar cache";
static struct kmem_cache* hangar_cache;

typedef struct _plane {
    int plan_id;
    int plane_capacity;
} plane;

DECLARE_KFIFO (plane_hangar,plane*,HANGAR_MAX_PLANES);

struct file_operations airport_hangar_ops = {
    .owner = THIS_MODULE
};

int init_airport_hangar(int number_of_planes, int number_of_passengers) {
    hangar_cache = kmem_cache_create(hangar_cache_name,sizeof(plane),0,SLAB_HWCACHE_ALIGN,NULL);
    if (!hangar_cache) {
        printk(KERN_INFO "Failed to create hangar cache\n");
        return 1;
    }
    return 0;
}

void test_kfifo(void) {
}

void print_kfifo(void) {
}

void delete_kfifo(void) {
}

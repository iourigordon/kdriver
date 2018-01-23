#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/slab.h>


#define HANGAR_MAX_PLANES 16

static char* hangar_cache_name = "hangar cache";
static struct kmem_cache* hangar_cache;

typedef struct _plane {
    u64 plan_id;
    int plane_capacity;
} plane;

DECLARE_KFIFO (plane_hangar,plane*,HANGAR_MAX_PLANES);

struct file_operations airport_hangar_ops = {
    .owner = THIS_MODULE
};

int init_airport_hangar(int number_of_planes, int number_of_passengers)
{
    hangar_cache = kmem_cache_create(hangar_cache_name,sizeof(plane),0,SLAB_HWCACHE_ALIGN,NULL);
    if (!hangar_cache) {
        printk(KERN_INFO "Failed to create hangar cache\n");
        return 1;
    }
    return 0;
}

void destroy_airport_hangar(void)
{
    if (!hangar_cache) {
        printk(KERN_INFO "Hangar cache does not exist\n");
    }
    kmem_cache_destroy(hangar_cache);
}

void* get_cache(void)
{
    void* plane_cache;

    plane_cache = kmem_cache_alloc(hangar_cache,GFP_KERNEL);
    if (!plane_cache)
        return NULL;
    return plane_cache;
}

#include <linux/fs.h>
#include <linux/kfifo.h>
#include <linux/slab.h>


#define HANGAR_MAX_PLANES 16

static char* hangar_cache_name = "hangar cache";
static struct kmem_cache* hangar_cache;

typedef struct _plane {
    u64 plane_id;
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
    /*check if kfifo has any elements inside. if it does something is wrong*/

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

int is_hangar_full(void)
{
    return kfifo_is_full(&plane_hangar);    
}

int add_plane_to_hangar(void* plane)
{
    return kfifo_put(&plane_hangar,plane); 
}

u64 get_plane_ready_to_takeoff(void)
{
    plane* ready_plane;
    if (kfifo_is_empty(&plane_hangar))
        return 1;
    kfifo_peek(&plane_hangar,&ready_plane);
    return ready_plane->plane_id; 
}

void* remove_plane_from_hangar(void)
{
    plane* ready_plane;
    if (kfifo_is_empty(&plane_hangar))
        return NULL;
    kfifo_get(&plane_hangar,&ready_plane);
    return ready_plane;
}

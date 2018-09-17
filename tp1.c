
#include <linux/list.h>
#include <linux/blkdev.h>    // block device operation- Request queue
#include <linux/blk_types.h> // Bio
#include <linux/genhd.h>     // disk representation
#include <linux/vmalloc.h>
#include <linux/errno.h>     // error codes 

#define MY_BLOCK_MAJOR 240
#define MY_BLOCK_MINOR 1 
#define MY_BLKDEV_NAME "myDev"
#define NB_SECTOR 1024
#define SECTOR_SIZE 512

static struct my_device {
	unsigned int size; 
	spinlock_t lock; 
	u8 *data; 
	struct gendisk *gd;
	struct request_queue *queue;
}device;


static int create_blkdev (static struct my_device *dev){

	dev->gd = allod_disk(MY_BLOCK_MINOR); 
	add_disk(dev->gd);
}


static struct block_device_operation rb_fops =
{
	.owner   = THIS_MODULE,
	.open    = rb_open, 
	.release = rb_close,
	.getgeo  = rb_getgeo,
};

static int __init blkdev_init(void){
	
	// Get registration 
	int status; 
	status = register_blkdev(MY_BLOCK_MAJOR, MY_BLKDEV_NAMEM);
	if(status < 0 ){
		printk(KENRN_ERR "unable to register myDev block device \n"); 
		return -EBUSY; 
	}

	// Setup the Device 
	device.size = NB_SECTOR*SECTOR_SIZE ; 
	device.data = vmalloc(device.size);
	if(device.data == NULL)
		printk(KERN_NOTICE "vmalloc faillure !\n");
		return -ENOMEM;

}

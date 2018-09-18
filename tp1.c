
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h> 
#include <linux/module.h>
#include <linux/list.h>
#include <linux/blkdev.h>    // block device operation- Request queue
#include <linux/blk_types.h> // Bio
#include <linux/genhd.h>     // disk representation
#include <linux/vmalloc.h>
#include <linux/errno.h>     // error codes 
//#include <uapi/asm-generic/errno-base.h>


#define MY_BLOCK_MAJOR 240
#define NB_MINOR 16
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

/*
static int create_blkdev (static struct my_device *dev){

	dev->gd = alloc_disk(NB_MINOR); 
	add_disk(dev->gd);
}
*/
static int my_device_open(struct block_device *d, fmode_t mode){
	printk(KERN_INFO "device open\n") ;
	return 0; 
}

static int my_device_close(struct gendisk *disk, fmode_t mode){
	printk(KERN_INFO "my_device: Mydev is closed\n");
	return 0;
}


static struct block_device_operations my_device_fops =
{
	.owner   = THIS_MODULE,
	.open    = my_device_open, 
	/*.release = my_device_close,*/
	
};

static int __init blkdev_init(void){
	
	// Get registration 
	int major; 
	major = register_blkdev(MY_BLOCK_MAJOR, MY_BLKDEV_NAME);
	if(major < 0 ){
		printk(KERN_ERR "my_device: unable to register myDev block device \n"); 
		return -EBUSY; 
	}
	printk(KERN_NOTICE "my_device: registration success !\n");

	// Setup the Device 
	device.size = NB_SECTOR*SECTOR_SIZE ; 
	device.data = vmalloc(device.size);
	if(device.data == NULL){
		printk(KERN_ERR "my_device: vmalloc faillure !\n");
		return -ENOMEM;
	}
	printk(KERN_DEBUG "my_device: vmalloc success\n");

	device.gd = alloc_disk(NB_MINOR); // Request for 16 minor numbers
	if(!device.gd){
		printk(KERN_ERR "my_device: error allocation disk\n");
		unregister_blkdev(major, MY_BLKDEV_NAME); // unregister device from /proc/devices
		vfree(device.data); // free data memory
		return -ENOMEM;
	}
	printk(KERN_DEBUG "my_device: alloc_disk success!\n");

	//Setup device driver;
	device.gd->major = major;
	device.gd->first_minor = 0;
	//device.gd->fops = &my_device_fops ; 
	device.gd->private_data = &device;
	device.gd->queue =device.queue;

	sprintf(device.gd->disk_name, "Mydev");
	set_capacity(device.gd, device.size);
	add_disk(device.gd);
	printk(KERN_DEBUG "my_device : Block driver initialised Units: sectors of %d * %d = %d bytes\n",device.size, SECTOR_SIZE, device.size*NB_SECTOR);

return 0; 
}
static void __exit blkdev_exit(void){

		vfree(device.data); // free data me
}

module_init(blkdev_init);
module_exit(blkdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abdelhadi");
MODULE_DESCRIPTION("Ram Block Driver");
MODULE_ALIAS_BLOCKDEV_MAJOR(0);

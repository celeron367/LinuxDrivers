#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>


#define VIRTUALDISK_SIZE 0x2000
#define MEM_CLEAR 0x1
#define PORT1_SET 0x2
#define PORT2_SET 0x3
#define VIRTUALDISK_MAJOR 200

static int VirtualDisk_Major=VIRTUALDISK_MAJOR;

struct VirtualDisk
{
	struct cdev cdev;
	unsigned char mem[VIRTUALDISK_SIZE];
	int port1;
	long port2;
	int count;
};

int VirtualDisk_Init(void)
{
	int result;
	dev_t devno=MKDEV(VirtualDisk_Major,0); //build device number

	if (VirtualDisk_Major)
		result = register_chrdev_region(devno,1,"VirtualDisk");
	else
	{
		result=alloc_chrdev_region(&devno,0,1,"VirtualDisk");
		VirtualDisk_Major=MAJOR(devno);
	}
	if (result<0)
		return result;


};

module_init(test_init);
module_exit(test_exit);

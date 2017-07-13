#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <asm/io.h>

/* Define Driver Name */
#define DRIVER_NAME "xadc_driver"
#define MODULE_NAME "xadc"


char *base_addr;   /* Vitual Base Address */
struct resource *res;       /* Device Resource Structure */
unsigned long remap_size;   /* Device Memory Size */




/* Write operation for /proc/myled
 * -----------------------------------
 *  When user cat a string to /proc/myled file, the string will be stored in
 *  const char __user *buf. This function will copy the string from user
 *  space into kernel space, and change it to an unsigned long value.
 *  It will then write the value to the register of myled controller,
 *  and turn on the corresponding LEDs eventually.
 */
 static ssize_t xadc_write(struct file *f, unsigned int * buf, size_t
         len, loff_t * off)
  {
	 if (!len==sizeof(unsigned int)*2){
	 	 printk(KERN_INFO "Data  must be 2 (unsigned int)\n", MODULE_NAME);
	      return -1;
	 	 }
	 iowrite32(buf[1],base_addr + buf[0]);



     return len;
 }
 static ssize_t xadc_read(struct file *f, unsigned int * buf, size_t
         len, loff_t * off)
 {

	 if (!len==sizeof(unsigned int)*2){
		 printk(KERN_INFO "Data  must be 2 (unsigned int)\n", MODULE_NAME);
     return -1;
	 }

	 printk(KERN_INFO "buf[0]:%08lx ,buf[1]:%08lx\n",buf[0],buf[1], MODULE_NAME);
	 buf[1]=ioread32(base_addr + buf[0]);
	 printk(KERN_INFO "addr:%08lx , res=%08lx\n",base_addr + buf[0],buf[1], MODULE_NAME);
	 return len;
 }
 /* Open function for /proc/myled
   * ------------------------------------
   *  When user want to read /proc/myled (i.e. cat /proc/myled), the open function
   *  will be called first. In the open function, a seq_file will be prepared and the
   *  status of myled will be filled into the seq_file by proc_myled_show function.
   */
 static int xadc_open(struct inode *inode, struct file *file)
  {
      //---------Add initial code here-----------------------------

      return 0;
  }

  /* File Operations for /proc/myled */
  static const struct file_operations proc_xadc_operations = {
      .owner = THIS_MODULE,
      .open = xadc_open,
      .read = xadc_read,
      .write = xadc_write,
  };

  /* Shutdown function for myled
   * -----------------------------------
   *  Before myled shutdown, turn-off all the leds
  */
 static void xadc_shutdown(struct platform_device *pdev)
 {
    iowrite32(0, base_addr);
 }

 /* Remove function for myled
  * ----------------------------------
  *  When myled module is removed, turn off all the leds first,
  *  release virtual address and the memory region requested.
  */
 static int xadc_remove(struct platform_device *pdev)
 {
     xadc_shutdown(pdev);

     /* Remove /proc/myled entry */
     remove_proc_entry(DRIVER_NAME, NULL);

     /* Release mapped virtual address */
     iounmap(base_addr);

     /* Release the region */
     release_mem_region(res->start, remap_size);

     return 0;
 }

 /* Device Probe function for myled
  * ------------------------------------
  *  Get the resource structure from the information in device tree.
  *  request the memory region needed for the controller, and map it into
  *  kernel virtual memory space. Create an entry under /proc file system
  *  and register file operations for that entry.
  */
 static int xadc_probe(struct platform_device *pdev)
 {
     struct proc_dir_entry *xadc_proc_entry;
     int ret = 0;

     res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
     if (!res) {
         dev_err(&pdev->dev, "No memory resource\n");
         return -ENODEV;
     }

     remap_size = res->end - res->start + 1;
     if (!request_mem_region(res->start, remap_size, pdev->name)) {
         dev_err(&pdev->dev, "Cannot request IO\n");
         return -ENXIO;
     }
     printk(KERN_INFO DRIVER_NAME " map resoure :%X\n", res->start);
     base_addr = ioremap(res->start, remap_size);
     if (base_addr == NULL) {
         dev_err(&pdev->dev, "Couldn't ioremap memory at 0x%08lx\n",
             (unsigned long)res->start);
         ret = -ENOMEM;
         goto err_release_region;
     }

     xadc_proc_entry = proc_create(DRIVER_NAME, 0, NULL,
                        &proc_xadc_operations);
     if (xadc_proc_entry == NULL) {
         dev_err(&pdev->dev, "Couldn't create proc entry\n");
         ret = -ENOMEM;
         goto err_create_proc_entry;
     }

     printk(KERN_INFO DRIVER_NAME " probed at VA 0x%08lx\n",
            (unsigned long) base_addr);

     return 0;

  err_create_proc_entry:
     iounmap(base_addr);
  err_release_region:
     release_mem_region(res->start, remap_size);

     return ret;
 }

 /* device match table to match with device node in device tree */
 static const struct of_device_id xadc_of_match[] = {
     {.compatible = "xadc"},
     {},
 };

 MODULE_DEVICE_TABLE(of, xadc_of_match);

 /* platform driver structure for myled driver */
 static struct platform_driver xadc_driver = {
     .driver = {
            .name = DRIVER_NAME,
            .owner = THIS_MODULE,
            .of_match_table = xadc_of_match},
     .probe = xadc_probe,
     .remove = xadc_remove,
     .shutdown = xadc_shutdown
};

 /* Register myled platform driver */

 module_platform_driver(xadc_driver);


 /* Module Informations */
 MODULE_AUTHOR("Celeron366, DICP");
 MODULE_LICENSE("GPL");
 MODULE_DESCRIPTION(DRIVER_NAME ": MY XADC driver (Simple Version)");
 MODULE_ALIAS(DRIVER_NAME);

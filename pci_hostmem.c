#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <asm/uaccess.h> /* copy_from/to_user */

#define VENDOR_ID 0x1172
#define DEVICE_ID 0xe001

static int dev_open (struct inode *, struct file *);
static int dev_close (struct inode *, struct file *);
static ssize_t dev_read (struct file *, char *, size_t, loff_t *);
static ssize_t dev_write (struct file *, __user const char *, size_t, loff_t *);
static long dev_ioctl (struct file *file, unsigned int cmd,	unsigned long arg);

static struct pci_device_id ids[] = {
  { PCI_DEVICE(VENDOR_ID,DEVICE_ID), },
  { 0, }
};

int major = 60;

MODULE_DEVICE_TABLE(pci, ids);

void *vaddr;
dma_addr_t dma_handle;

static unsigned char get_revision(struct pci_dev *dev)
{
	u8 revision;

	pci_read_config_byte(dev, PCI_REVISION_ID, &revision);
	return revision;
}

static int pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	/* Do probing type stuff here.  
	 * Like calling request_region();
	 */
	
	printk("probe\n");
	pci_enable_device(dev);
	
	printk("revision is %d\n",get_revision(dev));
	
	pci_set_master(dev);
	
	printk("allocate 0x%08lx\n",PAGE_SIZE);
	vaddr = pci_alloc_consistent(dev,PAGE_SIZE,&dma_handle);
	printk("virtual addr is 0x%016lx\n",(unsigned long)vaddr);
	printk("bus addr is 0x%016lx\n",(unsigned long) dma_handle);
	
	return 0;
}

static void pci_remove(struct pci_dev *dev)
{
	/* clean up any allocated resources and stuff here.
	 * like call release_region();
	 */
	 printk("remove\n");
	 
	 pci_free_consistent(dev,PAGE_SIZE,vaddr,dma_handle);
}

static struct file_operations dev_fops =
{
  .owner = THIS_MODULE,
  .unlocked_ioctl = dev_ioctl,
  .open = dev_open,
  .release = dev_close,
  .read = dev_read,
  .write = dev_write,
  
};

static int dev_open (struct inode *inode, struct file *filp)
{
  return (0);
}


static int dev_close (struct inode *inode, struct file *filp)
{
  return (0);
}

static ssize_t dev_read (struct file * filp, char *buff, size_t count, loff_t * ppos)
{
  /* Transfering data to user space */ 
  unsigned long ul;
  printk("read count=0x%08lx\n",count);
  // copy_to_user returns bytes _not_ read
  ul = copy_to_user(buff,vaddr,count);
  printk("read 0x%08lx\n",count - ul);
  return count - ul;
  
}


static ssize_t dev_write (struct file * filp, __user const char *buff, size_t count, loff_t * ppos)
{
  // read from user space
  unsigned long ul;
  printk("write count=0x%08lx\n",count);
  // copy_from_user returns bytes _not_ write
  ul = copy_from_user(vaddr,buff,count);
  printk("write 0x%08lx\n",count - ul);
  return count - ul;
 
}

static long dev_ioctl (struct file *file, unsigned int cmd,	unsigned long arg){
	// return phys address
	printk("ioctl cmd=%d\n",cmd);
	printk("bus addr is 0x%016lx\n",(unsigned long) dma_handle);
	if (cmd == 1) {
		put_user(dma_handle,(unsigned long*)arg);
	} 
	return 0;
}

static struct pci_driver pci_driver = {
	.name = "pci_hostmem",
	.id_table = ids,
	.probe = pci_probe,
	.remove = pci_remove,
};

static int __init pci_init(void)
{
	int rc;
	printk("init\n");
	rc = register_chrdev(major, "hostmem", &dev_fops);
	if ( rc < 0 ) {
		printk("cannot register chrdev\n");
		return rc;
	}
	printk("register chrdev\n");
	rc = pci_register_driver(&pci_driver);
	if ( rc != 0 ){
		 unregister_chrdev(major,"hostmem");
		 printk("cannot register pci\n");
		 return rc;
	}
	printk("register pci\n");
	return rc;
}
	

static void __exit pci_exit(void)
{
	printk("exit\n");
	pci_unregister_driver(&pci_driver);
	unregister_chrdev(major,"hostmem");
}

MODULE_LICENSE("GPL");

module_init(pci_init);
module_exit(pci_exit);

#include <linux/module.h>  
#include <linux/kernel.h>  
#include <linux/fs.h>  
#include <linux/errno.h>  
#include <linux/types.h>  
#include <linux/fcntl.h>  
#include <linux/cdev.h>  
#include <linux/version.h>  
#include <linux/vmalloc.h>  
#include <linux/ctype.h>  
#include <linux/pagemap.h>  
#include "demo.h"  
  
MODULE_AUTHOR("Yangjin");  
MODULE_LICENSE("Dual BSD/GPL");  
  
struct demo_dev *demo_devices;  
  
static unsigned char demo_inc = 0;//全局变量，每次只能打开一个设备  
  
static u8 demo_buffer[256];  
  
int demo_open(struct inode *inode, struct file *filp)  
{  
    struct demo_dev *dev;  
      
    if (demo_inc > 0) return -ERESTARTSYS;  
    demo_inc++;  
    dev = container_of(inode->i_cdev, struct demo_dev, cdev);  
    filp->private_data = dev;  
  
    return 0;  
}  
  
int demo_release(struct inode *inode, struct file *filp)  
{     
    demo_inc--;  
    return 0;  
}  
  
ssize_t demo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)  
{  
    int result;  
    loff_t pos = *f_pos; //pos: offset  
  
    if (pos >= 256) {  
        result = 0;  
        goto out;                                                                                                
    }  
    if (count > (256 - pos))  
        count = 256 - pos;  
    pos += count;  
  
    if (copy_to_user(buf, demo_buffer+*f_pos, count)) {  
        count = -EFAULT;  
        goto out;     
    }  
      
    *f_pos = pos;  
out:  
    return count;  
}  
  
ssize_t  demo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)  
{  
    ssize_t retval = -ENOMEM;  
    loff_t pos = *f_pos;  
  
    if (pos > 256)  
        goto out;  
    if (count > (256 - pos))   
        count = 256 - pos;    
    pos += count;  
    if (copy_from_user(demo_buffer+*f_pos, buf, count)) {  
        retval = -EFAULT;  
        goto out;     
    }  
      
    *f_pos = pos;  
    retval = count;  
out:  
    return retval;  
}  
  
int  demo_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)  
{  
    if (cmd == COMMAND1) {  
        printk("ioctl command 1 successfully\n");  
        return 0;     
    }  
    if (cmd == COMMAND2) {  
        printk("ioctl command 2 successfully\n");  
        return 0;     
    }  
    printk("ioctl error\n");  
    return -EFAULT;  
}  
  
loff_t demo_llseek(struct file *filp, loff_t off, int whence)  
{  
    loff_t pos;  
      
    pos = filp->f_pos;  
    switch (whence) {  
    case 0:  
        pos = off;  
        break;  
    case 1:  
        pos += off;  
        break;  
    case 2:  
    default:  
        return -EINVAL;   
    }  
      
    if ((pos > 256) || (pos < 0))  
        return -EINVAL;  
      
    return filp->f_pos = pos;  
}  
  
struct file_operations demo_fops = {  
    .owner = THIS_MODULE,  
    .llseek = demo_llseek,  
    .read = demo_read,  
    .write = demo_write,  
    .ioctl = demo_ioctl,  
    .open = demo_open,  
    .release = demo_release,  
};  
  
void demo_cleanup_module(void)  
{  
    dev_t devno = MKDEV(DEMO_MAJOR, DEMO_MINOR);  
      
    if (demo_devices) {  
        cdev_del(&demo_devices->cdev);  
        kfree(demo_devices);  
    }  
    unregister_chrdev_region(devno, 1);  
}  
  
// Init module流程：  
// 1）注册设备号MKDEV；  
// 2）注册设备驱动程序,即初始化cdev结构（嵌入到demo_devices结构中）  
int demo_init_module(void)  
{  
    int result;  
    dev_t dev = 0;  
      
    dev = MKDEV(DEMO_MAJOR, DEMO_MINOR);  
    result = register_chrdev_region(dev, 1, "DEMO");  
    if (result < 0) {  
        printk(KERN_WARNING "DEMO: can't get major %d\n", DEMO_MAJOR);  
        return result;  
    }  
    demo_devices = kmalloc(sizeof(struct demo_dev), GFP_KERNEL);  
    if (!demo_devices) {  
        result = -ENOMEM;  
        goto fail;  
    }  
    memset(demo_devices, 0, sizeof(struct demo_dev));  
    cdev_init(&demo_devices->cdev, &demo_fops);    
    demo_devices->cdev.owner = THIS_MODULE;  
    demo_devices->cdev.ops = &demo_fops; //将创建的字符设备与file_operations中各函数操作连接起来  
  
    result = cdev_add(&demo_devices->cdev, dev, 1);  
    if (result) {  
        printk(KERN_NOTICE "error %d adding demo\n", result);  
        goto fail;  
    }  
    return 0;  
fail:  
    demo_cleanup_module();  
    return result;  
}  
  
module_init(demo_init_module);  
module_exit(demo_cleanup_module);  
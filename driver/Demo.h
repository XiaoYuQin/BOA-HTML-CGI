#ifndef _DEMO_H_  
#define _DEMO_H_  
#include <linux/ioctl.h>  
/*Macros to help debuging*/  
#undef PDEBUG  
#ifdef DEMO_DEBUG  
    #ifdef __KERNEL__  
        #define PDEBUG(fmt, args...) printk(KERN_DEBUG "DEMO:" fmt,## args)   
    #else  
        #define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)  
    #endif  
#else  
#define PDEBUG(fmt, args...)   
#endif  
  
#define DEMO_MAJOR 224  
#define DEMO_MINOR 0  
#define COMMAND1 1  
#define COMMAND2 2  
  
struct demo_dev {  
    struct cdev cdev;  
};  
  
ssize_t demo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);  
ssize_t demo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);  
loff_t demo_llseek(struct file *filp, loff_t off, int whence);  
int demo_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);  
  
#endif  
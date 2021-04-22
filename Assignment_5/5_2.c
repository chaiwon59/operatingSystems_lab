#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chaiwon Park");
MODULE_DESCRIPTION("Exercise 5_2: A simple character device driver");
MODULE_VERSION("0.1");

#define DEVICE_NAME "pwm"
#define BUF_LEN 80

// define led output pins
#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

static int Major;
static int Device_Open = 0;
static struct device *Device = NULL;
static struct class *Class = NULL;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, char __user *, size_t, loff_t *);

struct file_operations fOps = {

        .open = device_open,
        .write = device_write,
        .release = device_release,

};


/**
 * Init function
 */
static int __init start(void){

    printk(KERN_INFO "Character device driver initialisation\n");

    Major = register_chrdev(0, DEVICE_NAME, &fOps);

    if(Major < 0){
        printk(KERN_ALERT "Registering char device failed.\n");
        return Major;
    }

    Class = class_create(THIS_MODULE, "5_2");
    Device = device_create(Class, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

    printk(KERN_INFO "Assigned major number %d.\n", Major);

    return 0;
}

/**
 * Exit function
 */
static void __exit end(void) {

    printk(KERN_INFO "Character device driver exit\n");

    device_destroy(Class, MKDEV(Major, 0));
    class_unregister(Class);
    class_destroy(Class);
    unregister_chrdev(Major, DEVICE_NAME);

}

module_init(start);
module_exit(end);

/**
 * Called when a process tries to open the device file.
 * @param inode
 * @param file
 * @return
 */
static int device_open(struct inode *inode, struct file *file){

    if(Device_Open){
        return -1;
    }
    Device_Open++;
    try_module_get(THIS_MODULE);

    return 0;

}

/**
 * Called when a process closes the device file.
 * @param inode
 * @param file
 * @return
 */
static int device_release(struct inode *inode, struct file *file){
    printk(KERN_INFO "Device Release");
    Device_Open--;
    module_put(THIS_MODULE);
    return 0;
}

/**
 * Called when a process writes to dev file.
 * @param file
 * @param __user
 * @return
 */
static ssize_t device_write(struct file *file, char __user * buffer, size_t length, loff_t * offset){

    long tmp;
    char tbuffer[length + 1];

    //tbuffer is a destination address in kernel space
    //buffer is a source address in user space
    //length is a maximum number of bytes to copy, including the trailing NUL
    tmp = strncpy_from_user(tbuffer, buffer, length+1);
    if(tmp > 0){
        printk(KERN_INFO "%s\n", tbuffer);
        return length;
    }

    printk(KERN_ALERT "Invalid Address\n");
    return -1;

}

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chaiwon Park");
MODULE_DESCRIPTION("Exercise 5_4: Dimming a LED using PWM");
MODULE_VERSION("0.1");

#define DEVICE_NAME "pwm"
#define BUF_LEN 80
#define PERIOD 10000000

// define led output pins
#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

static int Major;
static int Device_Open = 0;
static struct device *Device = NULL;
static struct class *Class = NULL;
static struct hrtimer hr_timer;
int static ledSwitch = 0;
static long onT = PERIOD;
static long offT = 0;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, char __user *, size_t, loff_t *);

struct file_operations fOps = {

        .open = device_open,
        .write = device_write,
        .release = device_release,

};


enum hrtimer_restart myTimer(struct hrtimer *timer){

    ktime_t currtime, interval;
    currtime = ktime_get();

    //If the LED was on and it should be off, change it to off and vice versa
    ledSwitch = !ledSwitch;


    if(ledSwitch){
        //If the switch is on, set the interval
        interval = ktime_set(0, onT);
    }else{
        //If the switch is off, set the interval
        interval = ktime_set(0, offT);
    }

    gpio_set_value(LED1, ledSwitch);
    gpio_set_value(LED2, ledSwitch);
    gpio_set_value(LED3, ledSwitch);
    gpio_set_value(LED4, ledSwitch);

    //Move restart timer to a time in the future
    hrtimer_forward(timer, currtime, interval);

    //Restart or return HRTIMER_NORESTART;
    return HRTIMER_RESTART;

}

/**
 * Init function
 */
static int __init start(void){

    //Allocate GPIOS
    gpio_request(LED1, "LED1");
    gpio_request(LED2, "LED2");
    gpio_request(LED3, "LED3");
    gpio_request(LED4, "LED4");

    //Inform that the GPIO core how a line is to be used.
    //In this case, output as we have to turn on the LEDs.
    gpio_direction_output(LED1, 0);
    gpio_direction_output(LED2, 0);
    gpio_direction_output(LED3, 0);
    gpio_direction_output(LED4, 0);

    Major = register_chrdev(0, DEVICE_NAME, &fOps);

    if(Major < 0){
        printk(KERN_ALERT "Registering char device failed.\n");
        return Major;
    }

    Class = class_create(THIS_MODULE, "5_4");
    Device = device_create(Class, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

    //Initialise a hrtimer
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

    //Set my restart timer function to the hr_timer function
    hr_timer.function = &myTimer;

    //Start a hrtimer
    hrtimer_start(&hr_timer, ktime_set(1,0), HRTIMER_MODE_REL);

    //Printing kernel logs
    printk(KERN_INFO "led_on\n");
    return 0;

    return 0;
}

/**
 * Exit function
 */
static void __exit end(void) {

    //Cancel a hrtimer
    hrtimer_cancel(&hr_timer);

    //Setting the values of LEDs to 0 to turn them off
    gpio_direction_output(LED1, 0);
    gpio_direction_output(LED2, 0);
    gpio_direction_output(LED3, 0);
    gpio_direction_output(LED4, 0);

    //Make sure that a zero is returned, which means that the code is successfully ran.
    gpio_free(LED1);
    gpio_free(LED2);
    gpio_free(LED3);
    gpio_free(LED4);

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

    printk(KERN_INFO "Device Open");
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

    tmp = strncpy_from_user(tbuffer, buffer, length+1);
    if(tmp > 0){
        long cycle = 0;
        tmp = kstrtol(tbuffer, 10, &cycle);
        if(tmp == 0){
            printk(KERN_INFO "%ld\n", cycle);
            if(cycle >= 0 && cycle <= 100){
                onT = cycle * PERIOD/100;
                offT = PERIOD - onT;
                return length;
            }
        }
    }ㅜㅜ

    printk(KERN_ALERT "Invalid Address\n");
    return -1;

}

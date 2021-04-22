#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/hrtimer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chaiwon Park");
MODULE_DESCRIPTION("Exercise 5_3: An introduction to timers");
MODULE_VERSION("0.1");

// define led output pins
#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

static struct hrtimer hr_timer;
int static ledSwitch = 0;

enum hrtimer_restart myTimer(struct hrtimer *timer){

    ktime_t currtime, interval;
    currtime = ktime_get();

    //If the LED was on and it should be off, change it to off and vice versa
    ledSwitch = !ledSwitch;


    if(ledSwitch){
        //If the switch is on, set the interval of 1 second
        interval = ktime_set(1,0);
    }else{
        //If the switch is off, set the interval of 2 seconds
        interval = ktime_set(2,0);
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

static int __init led_on(void){

    //Allocate GPIOS
    gpio_request(LED1, "LED1");
    gpio_request(LED2, "LED2");
    gpio_request(LED3, "LED3");
    gpio_request(LED4, "LED4");

    //Inform that the GPIO core how a line is to be used.
    //In this case, output as we have to turn on the LEDs.
    gpio_direction_output(LED1, 1);
    gpio_direction_output(LED2, 1);
    gpio_direction_output(LED3, 1);
    gpio_direction_output(LED4, 1);

    //Initialise a hrtimer
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);

    //Set my restart timer function to the hr_timer function
    hr_timer.function = &myTimer;

    //Start a hrtimer
    hrtimer_start(&hr_timer, ktime_set(1,0), HRTIMER_MODE_REL);
    //Printing kernel logs
    printk(KERN_INFO "led_on\n");
    return 0;
}


static void __exit led_off(void){

    //Printing kernel logs
    printk(KERN_INFO "led_off\n");

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

}


/**
 * __init macro causes the init function to be discarded and its memory freed once
 * init function finishes for built-int drivers.
 */
module_init(led_on);

/**
* __exit macro causes the omission of the function when the module is built into the kernel
* and has no effect
*/
module_exit(led_off);

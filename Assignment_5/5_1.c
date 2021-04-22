#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chaiwon Park");
MODULE_DESCRIPTION("Exercise 5_1: My first kernel module");
MODULE_VERSION("0.1");

// define led output pins
#define LED1 4
#define LED2 17
#define LED3 27
#define LED4 22

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

     //Printing kernel logs
     printk(KERN_INFO "led_on\n");
     return 0;
 }


 static void __exit led_off(void){

     //Printing kernel logs
     printk(KERN_INFO "led_off\n");

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

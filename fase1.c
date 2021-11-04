/*
 * @file fase1.c
 * @author Fernando Porcel Lopez
 * @date 2 Nov 2021
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>      
#include <linux/interrupt.h>  
#include <linux/kobject.h>    
#include <linux/time.h>       
#define  DEBOUNCE_TIME 200 

MODULE_LICENSE("GPL");

static unsigned int gpioRed = 20
static unsigned int gpioBlue = 16

static unsigned int gpioButton1 = 26
static unsigned int gpioButton2 = 19
static unsigned int gpioButton3 = 13
static unsigned int gpioButton4 = 21

static unsigned bool redOn = 0
static unsigned bool blueOn = 0

static unsigned int numberPresses1 = 0;
static unsigned int numberPresses2 = 0;
static unsigned int numberPresses3 = 0;
static unsigned int numberPresses4 = 0;

static int __init ebbgpio_init(void){

   	int result = 0;

   	printk(KERN_INFO "fase1.c: Initializing the fase1.c LKM\n");

   	if (!gpio_is_valid(gpioRed)){
      		printk(KERN_INFO "fase1.c: invalid gpioRed\n");
      		return -ENODEV;
   	}

   	if (!gpio_is_valid(gpioLED)){
      		printk(KERN_INFO "fase1.c: invalid gpioBlue\n");
      		return -ENODEV;
	}

 	redOn = true;
   	gpio_request(gpioRed, "sysfs");         
   	gpio_direction_output(gpioRed, redOn);  
   	gpio_export(gpioRed, false);            

	blueOn = true;
	gpio_request(gpioBlue, "sysfs");
	gpio_direction_output(gpioBlue, blueOn);
	gpio_export(gpioBlue, false);

	gpio_request(gpioButton1, "sysfs");
	gpio_direction_input(gpioButton1);
   	gpio_set_debounce(gpioButton1, 200);
   	gpio_export(gpioButton1, false);

   	gpio_request(gpioButton2, "sysfs");
   	gpio_direction_input(gpioButton2);
   	gpio_set_debounce(gpioButton2, 200);
   	gpio_export(gpioButton2, false);

   	gpio_request(gpioButton3, "sysfs");
   	gpio_direction_input(gpioButton3);
   	gpio_set_debounce(gpioButton3, 200);
   	gpio_export(gpioButton3, false);

   	gpio_request(gpioButton4, "sysfs");
   	gpio_direction_input(gpioButton4);
  	gpio_set_debounce(gpioButton4, 200);
   	gpio_export(gpioButton4, false);

}

static void __exit ebbgpio_exit(void){

  	printk(KERN_INFO "fase1.c: The button 1 state is currently: %d\n", gpio_get_value(gpioButton1));
	printk(KERN_INFO "fase1.c: The button 2 state is currently: %d\n", gpio_get_value(gpioButton2));
	printk(KERN_INFO "fase1.c: The button 3 state is currently: %d\n", gpio_get_value(gpioButton3));
	printk(KERN_INFO "fase1.c: The button 4 state is currently: %d\n", gpio_get_value(gpioButton4));

	printk(KERN_INFO "fase1.c: The button 1 was pressed %d times\n", numberPresses1);
	printk(KERN_INFO "fase1.c: The button 2 was pressed %d times\n", numberPresses2);
	printk(KERN_INFO "fase1.c: The button 3 was pressed %d times\n", numberPresses3);
	printk(KERN_INFO "fase1.c: The button 4 was pressed %d times\n", numberPresses4);

	gpio_set_value(gpioRed, 0);   
   	gpio_set_value(gpioBlue, 0);   
   	gpio_unexport(gpioRed);        
   	gpio_unexport(gpioBlue); 
         
   	free_irq(irqNumber, NULL);  
             
   	gpio_unexport(gpioButton1);      
   	gpio_unexport(gpioButton2);    
   	gpio_unexport(gpioButton3);    
   	gpio_unexport(gpioButton4);   
          
   	gpio_free(gpioRed);   
   	gpio_free(gpioBlue);   
                   
   	gpio_free(gpioButton1);  
   	gpio_free(gpioButton2);  
   	gpio_free(gpioButton3);  
   	gpio_free(gpioButton4);

   	printk(KERN_INFO "fase1.c: Goodbye from the LKM!\n");

}

module_init(ebbgpio_init);
module_exit(ebbgpio_exit);

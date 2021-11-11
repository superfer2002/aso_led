/*
 * @file fase1.c
 * @author Fernando Porcel Lopez
 * @date 2 Nov 2021
*/

//declaracion de librerias necesarias
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>      
#include <linux/interrupt.h>  
#include <linux/kobject.h>    
#include <linux/time.h>       
#include <linux/unistd.h>

MODULE_LICENSE("GPL");

//declaracion de constantes globales asi como de los pins de cada actuador
static unsigned int gpioRed = 16;
static unsigned int gpioBlue = 20;

static unsigned int gpioButton1 = 26;
static unsigned int gpioButton2 = 19;
static unsigned int gpioButton3 = 13;
static unsigned int gpioButton4 = 21;

static bool redOn = 0;
static bool blueOn = 0;

static unsigned int numberPresses1 = 0;
static unsigned int numberPresses2 = 0;
static unsigned int numberPresses3 = 0;
static unsigned int numberPresses4 = 0;

static unsigned int irqNumber1;
static unsigned int irqNumber2;
static unsigned int irqNumber3;
static unsigned int irqNumber4;

static irq_handler_t ebbgpio_irq_handler1(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t ebbgpio_irq_handler2(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t ebbgpio_irq_handler3(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t ebbgpio_irq_handler4(unsigned int irq, void *dev_id, struct pt_regs *regs);

//llamadas a otros scripts
int call_usermodehelper (const char * path, char ** argv, char ** envp, int wait);

static int __init ebbgpio_init(void){

   	int result = 0;

   	printk(KERN_INFO "fase1.c: Initializing the fase1.c LKM\n");

   	if (!gpio_is_valid(gpioRed)){  //error de asignacion de pin no valido
      		printk(KERN_INFO "fase1.c: invalid gpioRed\n");
      		return -ENODEV;
   	}

   	if (!gpio_is_valid(gpioBlue)){  //error de asignacion de pin no valido
      		printk(KERN_INFO "fase1.c: invalid gpioBlue\n");
      		return -ENODEV;
	}

	//inicializamos los leds apagados por defecto al inicio
 	redOn = false;
   	gpio_request(gpioRed, "sysfs");
   	gpio_direction_output(gpioRed, redOn);  //asignamos como pin de salida
   	gpio_export(gpioRed, false);

	blueOn = false;
	gpio_request(gpioBlue, "sysfs");
	gpio_direction_output(gpioBlue, blueOn);
	gpio_export(gpioBlue, false);

	//inicializamos los botones y los declaramos como pines de entrada
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

	//mapeo del gpio del boton a su irq correspondiente
	irqNumber1 = gpio_to_irq(gpioButton1);
	irqNumber2 = gpio_to_irq(gpioButton2);
	irqNumber3 = gpio_to_irq(gpioButton3);
	irqNumber4 = gpio_to_irq(gpioButton4);

	//vinculamos este irq con su funcion predeterminada
	result = request_irq(irqNumber1, (irq_handler_t) ebbgpio_irq_handler1, IRQF_TRIGGER_RISING, "ebb_gpio_handler1", NULL);
	result = request_irq(irqNumber2, (irq_handler_t) ebbgpio_irq_handler2, IRQF_TRIGGER_RISING, "ebb_gpio_handler2", NULL);
	result = request_irq(irqNumber3, (irq_handler_t) ebbgpio_irq_handler3, IRQF_TRIGGER_RISING, "ebb_gpio_handler3", NULL);
	result = request_irq(irqNumber4, (irq_handler_t) ebbgpio_irq_handler4, IRQF_TRIGGER_RISING, "ebb_gpio_handler4", NULL);

	return result;
}

static void __exit ebbgpio_exit(void){

	//mostramos mensajes de estado de los botones
  	printk(KERN_INFO "fase1.c: The button 1 state is currently: %d\n", gpio_get_value(gpioButton1));
	printk(KERN_INFO "fase1.c: The button 2 state is currently: %d\n", gpio_get_value(gpioButton2));
	printk(KERN_INFO "fase1.c: The button 3 state is currently: %d\n", gpio_get_value(gpioButton3));
	printk(KERN_INFO "fase1.c: The button 4 state is currently: %d\n", gpio_get_value(gpioButton4));

	//mostramos cuantas veces se ha pulsado cada boton
	printk(KERN_INFO "fase1.c: The button 1 was pressed %d times\n", numberPresses1);
	printk(KERN_INFO "fase1.c: The button 2 was pressed %d times\n", numberPresses2);
	printk(KERN_INFO "fase1.c: The button 3 was pressed %d times\n", numberPresses3);
	printk(KERN_INFO "fase1.c: The button 4 was pressed %d times\n", numberPresses4);

	//desvinculamos todos los pines y liberamos la memoria generada para el irq
	gpio_set_value(gpioRed, 0);
   	gpio_set_value(gpioBlue, 0);
   	gpio_unexport(gpioRed);
   	gpio_unexport(gpioBlue);
         
   	free_irq(irqNumber1, NULL);
      	free_irq(irqNumber2, NULL);
	free_irq(irqNumber3, NULL);
	free_irq(irqNumber4, NULL);
             
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

static irq_handler_t ebbgpio_irq_handler1(unsigned int irq, void *dev_id, struct pt_regs *regs) {  //boton 1
	redOn = true;  //encendemos
	gpio_set_value(gpioRed, redOn);
	numberPresses1++;  //aumentamos el contador
	printk("Button 1 pressed!");

	char *argv[] = {"/home/pi/practica/aso_led/button1.sh", NULL};
	char *envp[] = {"HOME=/", NULL};
	call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

	return (irq_handler_t) IRQ_HANDLED;
}

static irq_handler_t ebbgpio_irq_handler2(unsigned int irq, void *dev_id, struct pt_regs *regs){  //boton 2
	redOn = false;  //apagamos
	gpio_set_value(gpioRed, redOn);
	printk("Button 2 pressed!");
	numberPresses2++;  //aumentamos el contador

	char *argv[] = {"/home/pi/practica/aso_led/button2.sh", NULL};
	char *envp[] = {"HOME=/", NULL};
	call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

	return (irq_handler_t) IRQ_HANDLED;
}

static irq_handler_t ebbgpio_irq_handler3(unsigned int irq, void *dev_id, struct pt_regs *regs){  //boton 3
	blueOn = true;  //encendemos
	gpio_set_value(gpioBlue, blueOn);
	printk("Button 3 pressed!");
	numberPresses3++;  //aumentamos el contador

	char *argv[] = {"/home/pi/practica/aso_led/button3.sh", NULL};
	char *envp[] = {"HOME=/", NULL};
	call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

	return (irq_handler_t) IRQ_HANDLED;
}

static irq_handler_t ebbgpio_irq_handler4(unsigned int irq, void *dev_id, struct pt_regs *regs){  //boton 4
	blueOn = false;  //apagamos
	gpio_set_value(gpioBlue, blueOn);
	printk("Button 4 pressed!");
	numberPresses4++;  //aumentamos el contador

	char *argv[] = {"/home/pi/practica/aso_led/button4.sh", NULL};
	char *envp[] = {"HOME=/", NULL};
	call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);

	return (irq_handler_t) IRQ_HANDLED;
}

module_init(ebbgpio_init);
module_exit(ebbgpio_exit);

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

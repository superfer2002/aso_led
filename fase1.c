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

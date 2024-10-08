/* Name: main.c
 * Author: <Jason LI>
 * Copyright: <adapted from ATmega32u4 datasheet and class notes>
 * used ChatGPT to debug syntax errors
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file

int main(void)
{
	_clockdivide(0); //set the clock speed to 16Mhz
	set(DDRC,6); //set PC6 pin to output
	int blink_delay = 200; //blink delay in ms

/* insert your hardware initialization here */
for(;;){
	//toggle high/low on 6 bit of portc (PC6)
	//this turns the LED on and off to blink
	toggle(PORTC,6);
	_delay_ms(blink_delay); //blinking delay
  }
  return 0;   /* never reached */
}
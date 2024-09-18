/* Name: main.c
 * Author: <Jason LI>
 * Copyright: <adapted from ATmega32u4 datasheet and class notes>
 * used ChatGPT to debug syntax errors
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file

// this macro calculates the compare value for 1 single LED blink
// it is calculated as 16Mhz/prescaler multiplied by the blink period (or 1/blink frequency)
#define total_compare_value(blink_frequency,prescaler_val) ((16000000)/(prescaler_val*blink_frequency))-1
// this macro calculates the compare value for LED on time
// it is calcualted as a duty cycle multiplied by the total compare value
#define on_compare_value(blink_frequency,duty_cycle,prescaler_val) ((16000000 * 0.01 * duty_cycle) / (prescaler_val*blink_frequency))-1

int main(void)
{
	_clockdivide(0); //set the clock speed to 16Mhz
	set(DDRC,6); //set PB6 pin to output

	int duty_cycle = 50;  //duty cycle in %
	int blink_frequency = 20; //in hz taken from question
	int prescaler_val = 256; //timer prescaler
  
	//turn counter on, prescaler 256
	set(TCCR3B,CS32);
	clear(TCCR3B,CS30);
	clear(TCCR3B,CS31);
  
	//stores on compare value and total compare value
	int on_compare_val = on_compare_value(blink_frequency,duty_cycle,prescaler_val);
	int total_compare_val = total_compare_value(blink_frequency,prescaler_val);

  /* insert your hardware initialization here */
  for(;;){
	  if (duty_cycle == 0){
		  clear(PORTC,6);
	  }
	  else{
		  if (TCNT3 <= on_compare_val){
			  set(PORTC,6); 
		  }
		  else if ((TCNT3 > on_compare_val) && (TCNT3< total_compare_val)){
			  clear(PORTC,6);
		  }
		  else if (TCNT3 >= total_compare_val){
			  TCNT3 = 0;
		  }
	  }
  }
  return 0;   /* never reached */
}


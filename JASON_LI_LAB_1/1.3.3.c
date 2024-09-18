/* Name: main.c
 * Author: <Jason LI>
 * Copyright: <adapted from ATmega32u4 datasheet, class notes, and MEAM website>
 * used ChatGPT to debug syntax errors
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file


// this macro calculates the compare value for 1 single LED blink
// it is calculated as 16Mhz/prescaler multiplied by the blink period (or 1/blink frequency)
#define total_compare_value(blink_frequency,prescaler_val) ((16000000)/(prescaler_val*blink_frequency))
// this macro calculates the compare value for LED on time
// it is calcualted as a duty cycle multiplied by the total compare value
#define on_compare_value(blink_frequency,duty_cycle,prescaler_val) ((16000000 * 0.01 * duty_cycle) / (prescaler_val*blink_frequency))

int main(void)
{
	_clockdivide(0); //set the clock speed to 16Mhz
	
	set(DDRC,6); //set PB6 pin to output

	//source regarding timer registers
	//https://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-timer3

	int duty_cycle = 0;  //duty cycle in %
	int blink_frequency = 20; //in hz
	int prescaler_val = 256;

	// calculate clock value that LED is on, and total clock time (frequency of blinks)
	int on_compare_val = on_compare_value(blink_frequency,duty_cycle,prescaler_val);
	int total_compare_val = total_compare_value(blink_frequency,prescaler_val);

	//set to mode 14
	//up to ICR3, PWM mode
	//in mode 14 it counds up to ICR3 and then rolls over
	set(TCCR3A,WGM31);
	set(TCCR3B,WGM32);
	set(TCCR3B,WGM33);

	//set toggle
	//clear at OCR3A, set at rollover
	set(TCCR3A,COM3A1);
	clear(TCCR3A,COM3A0);
	
	//if duty cycle is 0, this turns off everything up to OCR3A and to ICR3
	if (duty_cycle == 0){
		clear(TCCR3A,COM3A1);
	}

	//For every cycle of LED blink, the counter cycles up to OCR3A, turns off, 
	//and then continues to cycle to ICR3, where it then turns the LED back on.
	OCR3A = on_compare_val;
	ICR3 = total_compare_val;

	//turn counter on, prescaler 256
	set(TCCR3B,CS32);
	clear(TCCR3B,CS30);
	clear(TCCR3B,CS31);

	while(1);
	return 0;   /* never reached */
}


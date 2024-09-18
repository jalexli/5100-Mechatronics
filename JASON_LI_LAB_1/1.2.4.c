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

	//blink every "x" ms, here I used 200
	//can also be seen as blink period
	int blink_delay = 200; // blink delay in ms
	int duty_cycle = 50; // duty cycle in %

	//the time the LED is on as a portion of blink delay
	//basically duty cycle multiplied by blink delay
	int on_time_delay = blink_delay*duty_cycle*0.01; 
	


/* How The Code Works:
LED turns on
on time delay (calcualted using blinkn delay and duty cycle)
LED turns off
off time delay (calculated by subtracting blink delay and on time delay)
repeat
*/
for(;;){

	set(PORTC,6); //turn on LED (PC6)
	//on time in ms as a delay time
	_delay_ms(on_time_delay);
	clear(PORTC,6); // turn off LED (PC6)
	//off time in ms as a delay time
	//I use subtraction instead of blink_delay*(1-duty_cycle) to avoid 
	//truncation errors; the delay times (on/off times) should add up to blink delay
	int off_time_delay = blink_delay-on_time_delay;
	_delay_ms(off_time_delay);

  }
/* 0 and 100 Duty Cycle Edge Cases:
Verified with Testing

At 0% duty cycle, on time is 0 because 0 * blink delay is 0,
so LED is effectively never on

At 100% duty cycle, on time is the same as blink delay because 100% * on time
equal to blink delay
if the two are equal, blink delay subtracted by on time is 0
the LED is effectively never off
*/

  return 0;   /* never reached */
}
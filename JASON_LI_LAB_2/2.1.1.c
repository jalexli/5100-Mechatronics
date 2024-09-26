/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from code in class notes>
 * License: <insert your license reference here>
 */

/* PC7 has pull up enabled, so in order to input a low I should just have to
ground it with a switch, driving a 1 input is not necessary. that is its
default state
*/

#include "MEAM_general.h"
#include "m_usb.h"

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13);
// char 10 and 13 are carriage return and line feed

//Macros for toggling LED
#define LED_on() set(PORTC,6);
#define LED_off() clear(PORTC,6);

//Checks input (HIIGH OR LOW)
void checkPC7(){
	int pinstate = bit_is_set(PINC,7);
	PRINTNUM(pinstate);
	
	//if low, button is pressed (counnected to ground)
	if (pinstate == 0){
		LED_on();

	}
	//if high, button is released. defalut state because pullup
	else{
		LED_off();
	}
}

int main(){
	m_usb_init();

	//set pinC 7 to inpt
	clear(DDRC,7);

	set(PORTC,7); //turn on internal pullup on PC7
	set(DDRC,6); //set PC^ to output

	while(1) {
		checkPC7();
	}

}

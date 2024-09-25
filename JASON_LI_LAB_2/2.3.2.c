/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from code in class notes>
 * Leo helped me understand the while (!bit_is_set(IFR3, ICF3))) part
 * He explained the logic of a constantly updating ICF3
 * CHATGPT for debugging, comments, fixing syntax errors
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"
#include "m_usb.h"

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13);
#define LED_on() set(PORTC,6);
#define LED_off() clear(PORTC,6);
#define LED2_on() set(PORTB,5);
#define LED2_off() clear(PORTB,5);

#define prescaler 1024
#define timer_max 15625 // 16-bit timer maximum value

unsigned int last_state = 0;
int time_interval = 0;

void init_timer3() {
    // Set prescaler to 1024
    clear(TCCR3B, CS31); // Disable CS31
    set(TCCR3B, CS30);   // Set CS30
    set(TCCR3B, CS32);   // Set CS32

    // Input Capture Mode
    set(TCCR3B, ICES3);  // Capture on rising edge
}

void wait_for_press() {
    // Wait for the input capture event
    while (!bit_is_set(TIFR3, ICF3));

    // Capture current state
    unsigned int current_state = ICR3;
    
    set(TIFR3, ICF3);

    // Calculate the time interval considering overflow
    if ((current_state - last_state)<= 0) {
		time_interval = current_state - last_state + timer_max; // Handle overflow
        
    } else {
        time_interval = current_state - last_state; // No overflow
    }

    // Update last state
    last_state = current_state;
}

int main() {
    m_usb_init();
    _delay_ms(2000); // Give time for USB to initialize

    // Set LED pins as output
    clear(DDRC,7);

    //set(PORTC,7); // Enable internal pullup on PC7
    set(DDRC,6);  // Set PC6 as output for LED1
    set(DDRB,5);  // Set PB5 as output for LED2

    init_timer3();

	//show end of initialization
	set(PORTC,6);
	set(PORTB,5);
	_delay_ms(2000);
	clear(PORTC,6);
	clear(PORTB,5);

    while (1) {
        wait_for_press(); // Wait for the button press and measure time
		int freq = (timer_max/time_interval);
		m_usb_tx_string("\n");
		m_usb_tx_int(freq);
		
        //Not Ideal but && conditionals cause my LEDs to not light up and the 
        //time interval to be extremely off, more than 150hz for 662hz
        //using sliding window method was the only way to have the LEDs light up
        //with reasonable values
        if (freq > 678){
			LED_off();
            LED2_off();	
		}
		else if (freq > 650){
			LED_on();
			LED2_off();
		}
		else if (freq > 25){
			LED2_off();
			LED_off();
		}
		else if (freq > 23){
			LED2_on();
			LED_off();
		}
		else{
			LED2_off();
			LED_off();
		}

			
	}
}

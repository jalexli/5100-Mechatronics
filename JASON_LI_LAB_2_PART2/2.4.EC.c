/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from code in class notes>
 * Leo helped me understand the while(!bit_is_set(IFR3, ICF3))) part
 * He explained the logic of a constantly updating ICF3
 * CHATGPT for debugging, comments, fixing syntax errors
 * Matt helped me with understanding the logic of having a "current_state", "last_state"
 * He also helped me with understanding the logic of when to update those two values
 * Matt and Bryan suggested prescaler
 * Steyn suggested I play with the gain of my Op amp
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"
#include "m_usb.h"

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13);

//Macros for toggling LED
//green LED
#define LED_on() set(PORTC,6);
#define LED_off() clear(PORTC,6);
//red LED
#define LED2_on() set(PORTB,5);
#define LED2_off() clear(PORTB,5);
//yellow LED
#define LED3_on() set(PORTB,7);
#define LED3_off() clear(PORTB,7);

#define prescaler 256
#define timer_max 65536 // max timer value

unsigned int last_state = 0;
int time_interval = 0;

//make it easy to switch between prescaler values
void set_prescaler(){
	if (prescaler == 256){
    	clear(TCCR3B, CS31); // Disable CS31
    	clear(TCCR3B, CS30);   // clear CS30
    	set(TCCR3B, CS32);   // Set CS32
	}
	else if (prescaler == 1024){
		clear(TCCR3B, CS31); // Disable CS31
    	set(TCCR3B, CS30);   // set CS30
    	set(TCCR3B, CS32);   // Set CS32
	}
	else if (prescaler == 64){
		set(TCCR3B, CS31); // set CS31
    	set(TCCR3B, CS30);   // set CS30
    	clear(TCCR3B, CS32);   // clear CS32	
	}
	else if (prescaler == 8){
		set(TCCR3B, CS31); // set CS31
    	clear(TCCR3B, CS30);   // set CS30
    	clear(TCCR3B, CS32);   // clear CS32	
	}
}

void init_timer3() {
	set_prescaler();
   
    // Input Capture Mode
    set(TCCR3B, ICES3);  // Capture on rising edge
}

void wait_for_press() {
    // Wait for the input capture event
    while (!bit_is_set(TIFR3, ICF3));

    // Capture current state
    unsigned int current_state = ICR3;
    
    //clear timer
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

    //make pin 7 c into input
    clear(DDRC,7);

    // Set LED pins as output
    set(DDRC,6);  // Set PC6 as output for LED1
    set(DDRB,5);  // Set PB5 as output for LED2
    set(DDRB,7);  // Set PB7 as output for LED3

    //sets upt timer
    init_timer3();

	//show end of initialization
    //shows LEDs work, helps with debugging
	set(PORTC,6);
	set(PORTB,5);
    set(PORTB,7);
	_delay_ms(2000);
	clear(PORTC,6);
	clear(PORTB,5);
    clear(PORTB,7);

    while (1) {

        wait_for_press(); // Wait for the button press and measure time
		
        //convert ot frequency
        int freq = ((16000000/prescaler)/time_interval);
		//print funtions for debugging
        //m_usb_tx_string("\n");
		//m_usb_tx_int(freq);
		
       //checking for 25hz and 662hz, tolerance range added
        if (freq > 650 && freq < 670){
			LED_on();
            LED2_off();	 
            LED3_off();
		}
		else if (freq > 23 && freq < 26){
			LED_off();
			LED2_on();
            LED3_off();
		}
		else if (freq > 190 && freq < 210){
            LED3_on();
            LED2_off();
			LED_off();
        }
		else{
			LED2_off();
			LED_off();
            LED3_off();
		}

			
	}
}

/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from code in class notes>
 * Leo helped me understand the while (!bit_is_set(IFR3, ICF3))) part
 * He explained the logic of a constantly updating ICF3
 * Matt helped me undderstand the "current state" and "last state" logic
 * CHATGPT for debugging, comments, fixing syntax errors
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"
#include "m_usb.h"

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13);

//LED Macros for toggling on/off
#define LED_on() set(PORTC,6);
#define LED_off() clear(PORTC,6);
#define timer_max 15265  // timer max value
#define prescaler 1024

//time if last press used to calculate interval
int first_press = 0;
int time_interval = 0;

void init_timer3(){
    // Set prescaler to 1024
    clear(TCCR3B, CS31);
    set(TCCR3B, CS30);
    set(TCCR3B, CS32);

    // Input Capture Mode on rising edge
    set(TCCR3B, ICES3);
}

void wait_for_press(){
    // Wait for input capture flag (ICF3) to be set
    while (!bit_is_set(TIFR3, ICF3));

    // Capture current timer value
    unsigned int current_state = ICR3;

    // Clear input capture flag (ICF3)
    set(TIFR3, ICF3);

    // Calculate the time interval, handling overflow
    if (current_state >= first_press) {
        time_interval = current_state - first_press;  // No overflow
    } else {
        time_interval = (timer_max - first_press) + current_state;  // Handle overflow
    }

    //calculating the time in ms from ticks, 102400.0 is float of prescaler * 1000 (ms in s)
	unsigned int time_ms = time_interval * (1024000.0 / 16000000.0);

    // Print the time interval over USB
    m_usb_tx_string("\nTime (ms): ");
    m_usb_tx_uint(time_ms);

    // Update first_press for next press
    first_press = current_state;
}

int main(){
    // Initialize USB
    m_usb_init();
    _delay_ms(2000);  // Give time for USB to initialize

    // Set up LED as output and configure PC7 for input with pull-up
    clear(PORTC, 6);
    clear(DDRC, 7);

    set(DDRC, 6);  // PC6 as output (LED)
    set(PORTC, 7);  // Enable internal pull-up on PC7

    // Initialize Timer 3
    init_timer3();

    // Main loop
    while (1) {
        wait_for_press();  // Wait for button press and measure time
    }
}

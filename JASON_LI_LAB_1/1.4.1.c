/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from ATmega32u4 datasheet and class notes>
 * used ChatGPT to debug syntax errors
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"
#include "m_usb.h"
#include <math.h>

//handles the rise to full itensity
//takes input of number of LED blink cycles going up to know how many times to iterate
//takes input of blink_frequency so that for loop iterates every blink frequency cycle, not clock cycle
//I do this by setting delay equal to blink period * 100 because ms: (1/blink_frequency) * 100 
void rise_LED(int rise_blink_cycles,int blink_frequency) {
    int i;
    for (i = 0; i <= rise_blink_cycles; i++) {

        // Calculate duty cycle as relative to blink cycles or for loop iterations (both should be equal b/c of delay)
        float duty_cycle = (100 * (float)i) / (float)(rise_blink_cycles);
        
        // Ensure duty cycle is within the valid range (0 to 100)
        if (duty_cycle > 100) {
            duty_cycle = 100;
        }
        if (duty_cycle < 0) {
            duty_cycle = 0;
        }

        // sets which clock cycle within the blink cycle to turn off LED
        OCR3A = (int)((duty_cycle * ICR3) / 100);
        
        //delay function below ensures for loop iterates at same frequency as blink frequency
        _delay_ms(100/blink_frequency);

     
    }
}

//handles the fall to 0 intensity
//takes input of number of LED blink cycles going down to know how many times to iterate
//takes input of blink_frequency so that for loop iterates every blink frequency cycle, not clock cycle
//I do this by setting delay equal to blink period * 100 because ms: (1/blink_frequency) * 100 
void fall_LED(int fall_blink_cycles, int blink_frequency) {
    int i;
    for (i = 0; i <= fall_blink_cycles; i++) {

        // Calculate duty cycle as relative to blink cycles or for loop iterations (both should be equal b/c of delay)
        float duty_cycle = (100 * ((float)fall_blink_cycles - (float)i)) / (float)fall_blink_cycles; 

        // Ensure duty cycle is within the valid range (0 to 100)
        if (duty_cycle < 0) {
            duty_cycle = 0;
        }
        if (duty_cycle > 100) {
            duty_cycle = 100;
        }

        // sets which clock cycle within the blink cycle to turn off LED
        OCR3A = (int)((duty_cycle * ICR3) / 100);

        //delay function below ensures for loop iterates at same frequency as blink frequency
        _delay_ms(100/blink_frequency);
    }
}

int main(void) {
    int prescaler_val = 64; //timer prescaler
    _clockdivide(0);  // Set the clock speed to 16MHz
    
    set(DDRC, 6);  // Set PB6 pin to output
    m_usb_init(); // initialize USB
    _
    delay_ms(1000);

    //source regarding timer registers
	//https://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-timer3

    int blink_frequency = 100;  // Blink frequency in Hz
    int rise_time = 300;  // Rise time in ms
    int fall_time = 600;  // Fall time in ms

    // Set up timer to PWM mode 14 (Fast PWM with ICR3 as TOP)
    set(TCCR3A, WGM31);
    set(TCCR3B, WGM32);
    set(TCCR3B, WGM33);

    // Set to clear at OCR3A, set at rollover
    set(TCCR3A, COM3A1);
    clear(TCCR3A, COM3A0);

    // Set prescaler to 64
    //lear(TCCR3B, CS32);
    set(TCCR3B, CS31);
    set(TCCR3B, CS30);
    clear(TCCR3B, CS32);

    // Set the total compare value (ICR3) based on the blink frequency
    int timer_ticks_per_blink = (16000000 / prescaler_val) / (blink_frequency);
    ICR3 = timer_ticks_per_blink;

    // how many times the LED blinks to get to full intensity from 0
    int rise_blink_cycles = rise_time * 0.01 * blink_frequency;

    // how many times the LED blinks to get to 0 intensity from full intensity
    int fall_blink_cycles = fall_time * 0.01 * blink_frequency;

    for (;;) {
        //each blink cycle is one call of rise_LED() and one call of fall_LED
        rise_LED(rise_blink_cycles, blink_frequency);
        fall_LED(fall_blink_cycles, blink_frequency);
    }

    return 0;  // Never reached
}

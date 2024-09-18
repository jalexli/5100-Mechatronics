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
void rise_LED(int rise_blink_cycles,int blink_frequency, int start_intensity_modifier, int final_intensity_modifier) {
    int i;

    //calculates intensity modifier step so intensity can be changed in for loop
    int intensity_modifier_step = (final_intensity_modifier - start_intensity_modifier) / rise_blink_cycles;
    
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

        //this is where intensity modifier is changed every loop
        int intensity_modifier = start_intensity_modifier + intensity_modifier_step;

        // sets which clock cycle within the blink cycle to turn off LED
        // intensity modifier, whe multiplied in OCR3A function affects intensity
        OCR3A = (int)((intensity_modifier * 0.01 * duty_cycle * ICR3) / 100);

        //delay function below ensures for loop iterates at same frequency as blink frequency
        _delay_ms(100/blink_frequency);
    }
}

//handles the fall to 0 intensity
//takes input of number of LED blink cycles going down to know how many times to iterate
//takes input of blink_frequency so that for loop iterates every blink frequency cycle, not clock cycle
//I do this by setting delay equal to blink period * 100 because ms: (1/blink_frequency) * 100 
void fall_LED(int fall_blink_cycles, int blink_frequency,int start_intensity_modifier, int final_intensity_modifier) {
    int i;

    
    //calculates intensity modifier step so intensity can be changed in for loop
    int intensity_modifier_step = (final_intensity_modifier - start_intensity_modifier) / fall_blink_cycles;
    
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

        //this is where intensity modifier is changed every loop
        int intensity_modifier = start_intensity_modifier + intensity_modifier_step;

        // sets which clock cycle within the blink cycle to turn off LED
        // intensity modifier, whe multiplied in OCR3A function affects intensity
        OCR3A = (int)((intensity_modifier * 0.01 * duty_cycle * ICR3) / 100);
        
        //delay function below ensures for loop iterates at same frequency as blink frequency
        _delay_ms(100/blink_frequency);
    }
}

//This functions job is to run the fall_LED and rise_LED functions
//It calculate takes in a starting intensity modifier and a final intensity modifier so that it can transition from one to the other

//it takes in blink frequency as an input to make sure the for loops within the rise_LED and fall_LED functions iterate
//at the same frequency as blink frequency
//I do this by making the delay 1/blinkfrequency seconds long

//It also takes in rise time and fall time, so the rise_LED and fall_LED know how long to go for
void blink_LED(int rise_time, int fall_time, int blink_frequency, int start_intensity_modifier, int final_intensity_modifier){
    // how many times the LED blinks to get to full intensity from 0
    int rise_blink_cycles = rise_time * 0.01 * blink_frequency;

    // how many times the LED blinks to get to 0 intensity from full intensity
    int fall_blink_cycles = fall_time * 0.01 * blink_frequency;

    // fall_blink_cycles and rise_blink_cycles turn rise/fall time into number of cycles for loop needs to iterate
    // in riseLED and fall_LED functions
    rise_LED(rise_blink_cycles, blink_frequency, start_intensity_modifier, final_intensity_modifier);
    fall_LED(fall_blink_cycles, blink_frequency, start_intensity_modifier, final_intensity_modifier);
}

int main(void) {
    int prescaler_val = 64; //timer prescaler

    _clockdivide(0);  // Set the clock speed to 16MHz
    
    set(DDRC, 6);  // Set PB6 pin to output
    m_usb_init(); // initialize USB
    
    _delay_ms(1000);

    //source regarding timer registers
	//https://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-timer3

    int blink_frequency = 100;  // Blink frequency in Hz
    
    //rise time and fall time calculated from table provided in 1.4.3
    int rise_time = 100;  // Rise time in ms
    int fall_time = 400;  // Fall time in ms

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


    for (;;) {

        //intensity variable that goes down in the for loop below
        int intensity = 100;
        int i;

        //for loop runs 21 times (because index from 0), one extra time at 0 intensity
        //each iteration decreases the intensity
        //each cycle includes a high blink and a low blink at 50% intensity
        //2 second delay to follow pattern provided
        for (i = 0; i <= 20; i++) {

            //turn LED back on
            set(TCCR3A, COM3A1);

            intensity = 100 - 5 * (i) ;

            //High blink
            //LED up to "full intensity"
            //"full intensity" is dependent on what iteration i loop is on
            // the higher i is, the lower intensity is
            blink_LED(rise_time,0,blink_frequency,0,intensity);
            //LED down to 0 intensity
            blink_LED(0,fall_time,blink_frequency,intensity,0);

            //low blink
            //LED up to half intensity
            blink_LED(rise_time,0,blink_frequency,0,intensity*0.5);
            //LED down to 0
            blink_LED(0,fall_time,blink_frequency,0.5*intensity,0);

            //turn off LED for rest period
            clear(TCCR3A, COM3A1);
            //LED wait
            _delay_ms(2000);
        }
    }

    return 0;  // Never reached
}

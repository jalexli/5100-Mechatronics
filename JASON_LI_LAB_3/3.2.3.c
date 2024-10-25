/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from code in class notes>
 * recieved help from Matt regarding what prescaler to use in this situation and why
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"
#include "m_usb.h"
#include <math.h>
#include <stdio.h> 

#define ADC_prescaler 128
#define max_pot_angle 270
#define max_ADC_steps 1024
#define timer_prescaler 8
#define motor_freq 50 //hz
#define clock_speed 16000000 //hz
#define max_timer_ticks ((clock_speed / timer_prescaler) / motor_freq)

//adapted from lecture 8 slide 41-42
//adapted from chatGPT prompt: how do I change DIDR based on adc channel
//took basic workflow but the register names were off, and it was using the wrong bits
//I saw the relationship between DIDR0 and DIDR2 with the bitwise of 0-7 by looking at the ADC datasheet
//https://ww1.microchip.com/downloads/en/devicedoc/atmel-7766-8-bit-avr-atmega16u4-32u4_datasheet.pdf
//doing it this way instead of conditionals makes it much more concise
void DIDR_setup(unsigned char ADCchannel){
    unsigned int tmp;
    tmp = 1 << ADCchannel;
    //sets DIDR0 to bitwise of lower 8 bits of ADC channel
    //if over 7 then should be 00000000
    DIDR0 |= tmp & 0x00ff;
    //shift bits to match DIDR2
    //if adc channel over 7, right 8 bits are 0's and left 8 bits contain number
    //once shifted, bits are equivalent to 0-7 bitwise
    DIDR2 |= tmp >> 8;
}

void ADMUX_setup(unsigned char ADCchannel){
    //clears ADMUX, REFS0 set to 0
    ADMUX = (1 << REFS0); 
    clear(ADCSRB, MUX5);
    clear(ADMUX, ADLAR);

    //initial ChatGPT prompt: how do i set up ADMUX in subroutine 
    //with adc channel input
    //needed to be fixed because ChatGPT did not account for MUX5, referred to wrong registers, wrong adc channel values
    //I added conditionals for below 7 and above 7 scenarios (aka Mux5 on or off)
    //I made use of the ADC table from the ATMega32u4 datasheet
    //https://ww1.microchip.com/downloads/en/devicedoc/atmel-7766-8-bit-avr-atmega16u4-32u4_datasheet.pdf
    //ADC channel 2 and 3 don't work, I account for that with conditionals (my own modification)
    if (ADCchannel <= 7 && ADCchannel != 2 && ADCchannel != 3){
        //Set MUX4:0 with the channel number
        //from chatGPT, and consulting ATMega32u4 datasheet
        //prompt: how can I set up ADMUX in a subroutine according to input channel
        ADMUX |= ADCchannel;
    }
    else if (ADCchannel >= 8 && ADCchannel <= 13){
        set(ADCSRB, MUX5);
        //from chatgpt, prompt: how do I take last 3 digits of bit
        //making channel number with 0x07
        //realized while looking at datasheet that ADC8-13
        //was the same as 1-7 except with MUX5 set to 1
        ADMUX |= ADCchannel & 0x07;
    }
}

void ADCprescale_setup(){
    //enable ADC
    set(ADCSRA,ADEN);
    //find power of prescaler
    int power = logf(ADC_prescaler)/logf(2);
    //set ADCSRA to bitwise of power
    //ADCSRA register prescale for 2-128, are represented with bitwise of 0 to 7
    ADCSRA |= (power & 0x07);
}


void ADC_setup(){
    //set up ADC prescale
    ADCprescale_setup();
    DIDR_setup(5);
    DIDR_setup(6);
    DIDR_setup(7);  
}

void move_motor(unsigned int ADCchannel, volatile uint16_t *compare_output){
    //ADMUX set up routine, setting up for specific adc channel for readding
    ADMUX_setup(ADCchannel);

    //start conversion
    set(ADCSRA,ADSC);
    //wait for conversion
    while(!bit_is_set(ADCSRA,ADIF));
    //clear interrupt flag
    set(ADCSRA,ADIF);

    //print angle
    unsigned int adc_value = ADC;

    unsigned int max_adc = (2 * max_ADC_steps) / 3;

    if (adc_value > max_adc) {
        adc_value = max_adc;
    }

    float angle = ((float)adc_value / max_adc) * 180.0;

    //m_usb_tx_string("\nAngle: ");
    //m_usb_tx_uint((unsigned int)angle);    

    //chatgpt prompt: how do i turn angle into sg90 input pulse for Atmega32u4
    //pulse width in ms (1000us(full left) 2000us (full right)), needed to change equation
    //because pulse_width equation was in microseconds and pulse_ticks was in milliseconds
    //replaced all valeus with macros
    float pulse_width = 500.0 + ((angle * 2000.0) / 180.0);  //in micro seconds

    //convert ms to ticks
    unsigned int pulse_ticks = (pulse_width * max_timer_ticks) / 20000.0;

    m_usb_tx_string("\npulseticks: ");
    m_usb_tx_uint(pulse_ticks);

    //m_usb_tx_uint((unsigned int)pulse_ticks);
    *compare_output = pulse_ticks;
}

void timer_setup(){
    
    // copied directly from my 1.4.3 code
    // Set up timer to PWM mode 14 (Fast PWM with ICR3 as TOP)
    set(TCCR1A, WGM11);
    set(TCCR1B, WGM12);
    set(TCCR1B, WGM13);

    // Set to clear at OCR1A, set at rollover
    set(TCCR1A, COM1A1);
    clear(TCCR1A, COM1A0);

    // Set to clear at OCR1B, set at rollover
    set(TCCR1A, COM1B1);
    clear(TCCR1A, COM1B0);

    // Set to clear at OCR1C, set at rollover
    set(TCCR1A, COM1C1);
    clear(TCCR1A, COM1C0);

    // Set prescaler to 8
    set(TCCR1B, CS11);
    clear(TCCR1B, CS10);
    clear(TCCR1B, CS12);

    ICR1 = max_timer_ticks;
}

void servo_pwm_setup(){
    //set up pins as
    set(DDRB,PB5);
    set(DDRB,PB6);
    set(DDRB,PB7);
    
    timer_setup();
}

void test_servo_positions() {
    // Test pulse width for 0 degrees (1000 µs)
    OCR1A = 1000; // Set output compare register to 2000 ticks
    _delay_ms(2000); // Wait for 2 seconds

    // Test pulse width for 180 degrees (2000 µs)
    OCR1A = 1500; // Set output compare register to 4000 ticks
    _delay_ms(2000); // Wait for 2 seconds

    // Test pulse width for 0 degrees (1000 µs)
    OCR1A = 2000; // Set output compare register to 2000 ticks
    _delay_ms(2000); // Wait for 2 seconds

    // Test pulse width for 0 degrees (1000 µs)
    OCR1A = 1500; // Set output compare register to 2000 ticks
    _delay_ms(2000); // Wait for 2 seconds

    OCR1A = 1000; // Set output compare register to 2000 ticks
    _delay_ms(2000); // Wait for 2 seconds
}

int main() {
    m_usb_init();
    ADC_setup();
    servo_pwm_setup();

    test_servo_positions();
 
    while(1){
        //read ADC
        //read_ADC(7);
        move_motor(5, &OCR1A);
        move_motor(6, &OCR1B);
        move_motor(7, &OCR1C);
    }
}

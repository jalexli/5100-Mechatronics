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


void ADC_setup(unsigned char ADCchannel){
    DIDR_setup(ADCchannel);
    //ADMUX_setup(ADCchannel);
    
}

void read_ADC(unsigned int ADCchannel){
    //ADMUX set up routine, setting up for specific adc channel for readding
    ADMUX_setup(ADCchannel);

    //start conversion
    set(ADCSRA,ADSC);
    //wait for conversion
    while(!bit_is_set(ADCSRA,ADIF));

    //print angle
    m_usb_tx_string("\n");
    unsigned int angle = ADC * ((float)max_pot_angle / (float)max_ADC_steps);
    m_usb_tx_uint(angle);
    set(ADCSRA,ADIF);
}

int main() {
    m_usb_init();

    //set up ADC prescale
    ADCprescale_setup();
    //ADC channel set up
    //botttom subroutines not incoporated into top, because need to be run for
    //every additional ADC channel used
    ADC_setup(7);
    ADC_setup(6);

    while(1){
        
        //read ADC
        read_ADC(7);
        read_ADC(6);

    }
}

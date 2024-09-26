/* Name: main.c
 * Author: <Jason Li>
 * Copyright: <adapted from code in class notes>
 * idea to use mode==3 in macro came from here
 *https://stackoverflow.com/questions/12989298/is-it-possible-to-use-a-if-statement-inside-define
 * License: <insert your license reference here>
 */

#include "MEAM_general.h"
#include "m_usb.h"

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13);
// char 10 and 13 are carriage return and line feed


//this came form CHAGPT, prompt: how can i turn A,B,C,etc to DDRx and PORTx
#define DDR(letter) DDR##letter   // Concatenates DDR and the given letter (e.g., DDRA)
#define PORT(letter) PORT##letter // Concatenates PORT and the given letter (e.g., PORTA)


#define INPUT 1
#define INPUT_PULLUP 2
#define OUTPUT 3

#define pinMode(register, bit, mode) (DDR(register) |= ((mode==3) << bit)); (PORT(register) |= ((mode==2)) << bit);

void checkPC6(){
	int pinstate = bit_is_set(PINC,6);
	PRINTNUM(pinstate);
}

int main(){
    m_usb_init();
    _delay_ms(1000);

    while (1) {
    
        //testing to see if macro works
		pinMode(C,6,OUTPUT);
        //lights up LED so I know it works
        set(PORTC,6);
        _delay_ms(2000);
        _delay_ms(2000);
        clear(PORTC,6);
        pinMode(C,6,INPUT);
        checkPC6();
        //prints 0 so i know it works
        _delay_ms(2000);
        _delay_ms(2000);
        pinMode(C,6,INPUT_PULLUP);
		checkPC6();
        //prints 64 (aka 1 or high) so i know it works
	    _delay_ms(2000);
        _delay_ms(2000);



    }
}
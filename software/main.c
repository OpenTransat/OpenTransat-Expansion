/*
Hardware watchdog timer based on PIC16F1825
TEST PROGRAM

OpenTransat
http://opentransat.com
http://github.com/opentransat
http://fb.com/opentransat

Released under the Creative Commons Attribution ShareAlike 4.0 International License
https://creativecommons.org/licenses/by-sa/4.0/
*/

#define _XTAL_FREQ 31000UL
#include "config.h"
#include <xc.h>

#define RELAY PORTCbits.RC0
#define NAVRESET PORTCbits.RC1
#define HEARTBEAT PORTCbits.RC3
#define WARNING PORTCbits.RC4
#define LED PORTCbits.RC5

void init_io() {
    OSCCONbits.IRCF = 0b0000; //31kHz oscillator
    TRISA = 0;
    PORTA = 0;
    TRISC = 0b00001000; //RC0 = RELAY (active high), RC1 = NAVRESET (active high), RC3 = HARTBEAT (input), RC4 = WARNING (active low), RC5 = LED (active high)
    PORTC = 0b00010000;
  
    //disable all analog ports
    ANSELA = 0;
    ANSELC = 0;

    //disable some peripherals
    CM1CON0bits.C1ON = 0;
    CM2CON0bits.C2ON = 0;
    RCSTAbits.SPEN = 0;    
}

void delay_sec(unsigned int n) {
    while (n--) {
        CLRWDT();
        LED = LED?0:1;
        __delay_ms(1000);
    }
}

void navreset() {
    NAVRESET = 1;
    __delay_us(200);    
    NAVRESET = 0;
}

void main(void) {
    init_io();
    while(1) {
        LED = HEARTBEAT;
/*        RELAY = 0;
        delay_sec(300);
        RELAY = 1;
        delay_sec(5);*/
    }
    return;
}

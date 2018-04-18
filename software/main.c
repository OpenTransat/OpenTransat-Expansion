/*
Hardware watchdog timer based on PIC16F1825

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

#define TMR1INIT (65535 - 31000) //timer1 increments +31000 per second

#define RESET_AFTER_SEC 60 //reset after no heartbeat
//#define RESET_AFTER_SEC 30 //TESTING reset after no heartbeat

#define RELAY_AFTER_SEC 8*3600 //periodically turn off the whole boat
//#define RELAY_AFTER_SEC 30 //TESTING periodically turn off the whole boat

#define RESET_US 200

#define RELAY_SEC 15 //turn off time, discharge all capacitors
//#define RELAY_SEC 10 //TESTING turn off time, discharge all capacitors

#define MAX_RETRIES 9 //number of retries after unsuccessful reset (first only reset, next using relay)

volatile unsigned long sectimer1, sectimer2; //incrementing each second in timer1 interrupt

void init_io() {
    OSCCONbits.IRCF = 0b0000; //31kHz oscillator
    TRISA = 0;
    PORTA = 0;
    TRISC = 0b00001000; //RC0 = RELAY (active high), RC1 = NAVRESET (active high), RC3 = HARTBEAT (input), RC4 = WARNING (active low), RC5 = LED (active high)
    PORTC = 0b00010000;
    
    //timer1 for detecting heartbeat
    T1CONbits.TMR1CS = 0b01; //increment on Fosc
    T1CONbits.T1CKPS = 0b00; //prescaler 1:1
    T1CONbits.TMR1ON = 1;
    
    //enable interrupts for timer1
    PIR1bits.TMR1IF = 0; //interrupt flag should be cleared before enabling interrupts (page 195 http://ww1.microchip.com/downloads/en/DeviceDoc/41440A.pdf)
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

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
    __delay_us(RESET_US);    
    NAVRESET = 0;
}

void relay_reset(int sec) {
    RELAY = 1;
    delay_sec(sec);
    RELAY = 0;
}

void interrupt timer1_interrupt(void) {
    if(PIR1bits.TMR1IF && PIE1bits.TMR1IE) {
        sectimer1++;
        sectimer2++;
        TMR1 = TMR1INIT;
        PIR1bits.TMR1IF = 0;
    }
}

void main(void) {

    init_io();

    char hb, hb_prev;
    int reset_retries = 0;
    sectimer1 = 0;
    sectimer2 = 0;
    TMR1 = TMR1INIT;
    
    while(1) {
        CLRWDT();
        
        hb = HEARTBEAT;

        if (hb != hb_prev) { //heartbeat detected: reset sectimer1
            reset_retries = 0;
            sectimer1 = 0;
            LED = 1;
            __delay_ms(10);
            LED = 0;
        }
        
        if (sectimer1 > RESET_AFTER_SEC) { //no heartbeat: reset master CPU
            if (reset_retries > 0 && reset_retries < MAX_RETRIES) { //the previous reset wasn't successful, still no heartbeat => use relay
                relay_reset(RELAY_SEC * reset_retries); //prolonging the period after each unsuccessful reset
                __delay_ms(100);
                navreset();
            } else {
                navreset();
            }
            reset_retries++;
            sectimer1 = 0;
            LED = 1;
            __delay_ms(10);
            LED = 0;
        }
        
        if (sectimer2 > RELAY_AFTER_SEC) { //relay shutdown every RELAY_AFTER_SEC seconds
            relay_reset(RELAY_SEC);
            __delay_ms(100);
            navreset();
            sectimer1 = 0;
            sectimer2 = 0;
            LED = 1;
            __delay_ms(10);
            LED = 0;
        }
        
        hb_prev = hb;

    }
    return;
}

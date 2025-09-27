//#include <PIC18F45K22>
#include <p18cxxx.h>
#include "my_xlcd.h"
#include <delays.h>
#include <stdlib.h>

#pragma config FOSC = INTIO67, PLLCFG = OFF, PRICLKEN = ON,FCMEN = ON, PWRTEN = ON
#pragma config BOREN = SBORDIS, BORV = 250, WDTEN = OFF, WDTPS = 4096, PBADEN = OFF
#pragma config HFOFST = OFF, MCLRE = EXTMCLR, STVREN = ON, LVP = OFF, DEBUG = ON

#define MAX 1023

unsigned long result = 0;
long int output;

unsigned char i = 0;

unsigned char open = 0x01;
unsigned char mid = 0x02;
unsigned char close = 0x03;
unsigned int mark, tempR;

void DelayFor18TCY( void ){
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
Nop();
}
void DelayPORXLCD (void){
Delay1KTCYx(60); // Delay of 15ms
// Cycles = (TimeDelay * Fosc) / 4
// Cycles = (15ms * 16MHz) / 4
// Cycles = 60,000
return;
}
void DelayXLCD (void){
Delay1KTCYx(20); // Delay of 5ms
// Cycles = (TimeDelay * Fosc) / 4
// Cycles = (5ms * 16MHz) / 4
// Cycles = 20,000
return;
}

void main( void ){
    //configure the Oscillator for 1Mhz internal oscillator operation
    OSCCON2 = 0x00; //no 4X PLL
    OSCCON = 0x30; // 16MHZ
    // PORTC LCD
    TRISC= 0x00;
    ANSELC = 0x00;
    // Set PORT A RA0 as analog input
    ANSELA = 0x01;
    TRISA = 0x01;
   
    VREFCON0 = 0xF0;
   
    ADCON0 = 0b00000001;
    ADCON1 = 0b00001000;
    ADCON2 = 0b10110110;
   
    SetCGRamAddr(0x08);
    putcXLCD(0b00001);
    putcXLCD(0b00011);
    putcXLCD(0b00111);
    putcXLCD(0b01111);
    putcXLCD(0b01111);
    putcXLCD(0b00111);
    putcXLCD(0b00011);
    putcXLCD(0b00001);

    SetCGRamAddr(0x10);
    putcXLCD(0b11111);
    putcXLCD(0b11111);
    putcXLCD(0b11111);
    putcXLCD(0b11111);
    putcXLCD(0b11111);
    putcXLCD(0b11111);
    putcXLCD(0b11111);
    putcXLCD(0b11111);

    SetCGRamAddr(0x18);
    putcXLCD(0b10000);
    putcXLCD(0b11000);
    putcXLCD(0b11100);
    putcXLCD(0b11110);
    putcXLCD(0b11110);
    putcXLCD(0b11100);
    putcXLCD(0b11000);
    putcXLCD(0b10000);

    OpenXLCD(FOUR_BIT & LINES_5X7);
   
    WriteCmdXLCD(SHIFT_DISP_LEFT);
   
    SetDDRamAddr(0x00);
    putrsXLCD("Voltage is");
    SetDDRamAddr(0x40);
    Delay10KTCYx(200);
   
    while (1) {
        ADCON0bits.GO=1;
        Delay1KTCYx(80);
        while(ADCON0bits.GO); // wait for ADC to finish
        result = ADRES; // last 2 bits of ADRESH, all 8 bits of ADRESL
        tempR = result & MAX;
        SetDDRamAddr(0x40);
        result = result*400000+5000;
        output = result/100000;
        putcXLCD(output/1000 + '0');
        putcXLCD('.');
        output = output%1000;
        putcXLCD(output/100 + '0');
        output = output%100;
        putcXLCD(output/10 + '0');
        output = output%10;
        putcXLCD(output + '0');
        putrsXLCD(" V");
        // Bonus below
        mark = (unsigned int)(tempR*7/MAX);
        putcXLCD(open);
        for (i = 0; i < mark; i++) {
            putcXLCD(mid);
        }
        for (i = mark; i < 7; i++) {
            putcXLCD(' ');
        }
        putcXLCD(close);
    }
}
//#include <PIC18F45K22> 
#include <p18cxxx.h> 
#include "my_xlcd.h" 
#include <delays.h> 
#include <stdlib.h> 
 
unsigned long result = 0; 
long int output; 
 
void main( void ){
    //configure the Oscillator for 1Mhz internal oscillator operation 
    OSCCON2 = 0x00; //no 4X PLL 
    OSCCON = 0x70; // 16MHZ 
    // PORTC LCD 
    TRISC= 0x00; 
    ANSELC = 0x00; 
    // Set PORT A RA0 as analog input 
    ANSELA = 0x01; 
    TRISA = 0x01; 
    ADCON0 = 0b00000001; 
    ADCON1 = 0b00000000; 
    ADCON2 = 0b10110110; 
    
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
        result = ((unsigned long)ADRESH << 8) | ADRESL; // last 2 bits of ADRESH, all 8 bits of ADRESL 
        SetDDRamAddr(0x40); 
        result = result*488251+5000; 
        output = result/100000; 
        putcXLCD(output/1000 + '0'); 
        putcXLCD('.'); 
        output = output%1000; 
        putcXLCD(output/100 + '0'); 
        output = output%100; 
        putcXLCD(output/10 + '0'); 
        putrsXLCD(“ Volts”); 
    } 
}  
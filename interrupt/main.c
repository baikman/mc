#include <p18cxxx.h>
#include "my_xlcd.h"
#include <delays.h>

#pragma config FOSC=INTIO67, PLLCFG=OFF, PRICLKEN=ON,FCMEN=ON, PWRTEN=OFF
#pragma config BOREN=SBORDIS, BORV=250, WDTEN=OFF, WDTPS=4096, PBADEN=OFF
#pragma config HFOFST=OFF, MCLRE=EXTMCLR, STVREN=ON, LVP=OFF, DEBUG= ON

unsigned char device=0, command=0, value, i;

unsigned int A = 0, B = 0, C = 0, D = 0, E = 0;
unsigned int result, temp, done;
unsigned char buffer[16];

void DelayFor18TCY( void )
{
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
void DelayPORXLCD (void)
{
Delay1KTCYx(60); // Delay of 15ms
// Cycles = (TimeDelay * Fosc) / 4
// Cycles = (15ms * 16MHz) / 4
// Cycles = 60,000
return;
}
void DelayXLCD (void)
{
Delay1KTCYx(20); // Delay of 5ms
// Cycles = (TimeDelay * Fosc) / 4
// Cycles = (5ms * 16MHz) / 4
// Cycles = 20,000
return;
}

void low_ISR(void);
void high_ISR(void);
#pragma code high_vector = 0x08	// force the following statement to start at
void high_interrupt (void)		// 0x08
{
    _asm 
    goto  high_ISR
    _endasm 
}
#pragma code 			//return to the default code section
#pragma interrupt high_ISR 
void high_ISR (void)
{
    //handle high-priority interrupts
    if (PORTBbits.RB4 == 0) TMR0L = 0x0;
    else {
        value = TMR0L;
        if (value > 150) {
            i = 0;
            done = 0;
        } else {
            buffer[i] = value;
            i++;
        }
    }
    if (i > 11) done = 1;
    INTCONbits.RBIF = 0;
}


void main(void) {
    //configure the Oscillator for 1Mhz internal oscillator operation
	OSCCON2 = 0x00; //no 4X PLL
	OSCCON = 0x30;  // 1MHZ
	
    //PORT A
	TRISA = 0xFF;		//Make Port A inputs 
	ANSELA = 0x00;		//Just want Digital inputs on all pins
	
    //PORT B
	TRISB = 0xFF;		//Make Port B inputs 
	ANSELB = 0x00;		//Just want Digital inputs on all pins
	
    //PORT D
	TRISD = 0x00;		//Make Port C Digital outputs 
	ANSELD = 0x00;		//Disable Analog Inputs - Just want Digital inputs
	
    //PORT C
	TRISC = 0x00;		//Make Port C Digital outputs 
	ANSELC = 0x00;		//Disable Analog Inputs - Just want Digital

	
    //set up the Timer 0 for 8 bit operation with Input from internal clock divided by 4
	T0CON = 0xD1;  //divide by 4

    // Enable a high priority “interrupt on change” for RB4.
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 0;
    INTCONbits.RBIE = 1;
    INTCON2bits.RBIP = 1;
    IOCB = 0x10;
	
    // configure external LCD
    OpenXLCD( FOUR_BIT & LINES_5X7 );
    WriteCmdXLCD( SHIFT_DISP_LEFT); //SHIFT_DISP_LEFT 
    while(BusyXLCD());
    WriteCmdXLCD(0x0C); // Turn blink off
    SetDDRamAddr(0x00);
    putrsXLCD("INTERRUPT MODE");
    SetDDRamAddr(0x40);
    putrsXLCD("Dev  Command ");
    for (i=0;i<16;i++){
        buffer[i]= 100;//initialize buffer to see if real data is written to the buffer
    }

    while (1) {
        if (done) {
            result = 0;
            for (i = 0; i < 12; i++) {
                result = result >> 1;
                if (buffer[i] > 70) result |= 0x0800;
            }
            device = result >> 7; //Device is upper 5 bits
            command = result & 0x7f;  //Mask off lower 7 bits to determine command
            PORTD = (unsigned char)result;//cast result to 8bit PORTD
            SetDDRamAddr(0x43);
            putcXLCD(48+(device));  //Since our remote is Device "1" - only handle a single digit
            SetDDRamAddr(0x4D);
            A = (command/100);
            temp = (command % 100);
            B = (temp/10);
            C = (temp % 10);
            putcXLCD(48+(A));
            putcXLCD(48+(B));
            putcXLCD(48+(C));
            Delay10KTCYx(5);  
            done = 0;
        }
    }
}
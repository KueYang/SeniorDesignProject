#include <p32xxxx.h>
#include <plib.h>
#include "IO.h"


void IO_Init(void)
{   
    // Set RB13 and RB15 to Digital IO
//    ANSELBbits.ANSB13 = 0;
//    ANSELBbits.ANSB15 = 0;
    
    mPORTAClearBits(BIT_0 | BIT_1 | BIT_2);
    mPORTBClearBits(BIT_3 | BIT_5);
    
    mPORTASetPinsDigitalIn(BIT_2);             //U1RX
    mPORTASetPinsDigitalOut(BIT_0 | BIT_1);     //LED
    mPORTBSetPinsDigitalOut(BIT_3 | BIT_5);    // U1TX

//    
    // ADC IO
    
    // UART IO
    
    // SPI IO
    
    // I2C IO
}
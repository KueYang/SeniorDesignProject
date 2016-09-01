#include <p32xxxx.h>
#include <plib.h>
#include "IO.h"


void IO_Init(void)
{   
    // Set RB13 and RB15 to Digital IO
    ANSELBbits.ANSB13 = 0;
    ANSELBbits.ANSB15 = 0;
    
    mPORTAClearBits(BIT_0);
    mPORTBClearBits(BIT_5 | BIT_13 | BIT_15);
    mPORTBSetPinsDigitalIn(BIT_13); //U1RX
    mPORTASetPinsDigitalOut(BIT_0); //LED
    mPORTBSetPinsDigitalOut(BIT_5 | BIT_15); //LED | U1TX

//    
    // ADC IO
    
    // UART IO
    
    // SPI IO
    
    // I2C IO
}
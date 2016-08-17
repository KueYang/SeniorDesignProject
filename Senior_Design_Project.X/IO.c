#include <p32xxxx.h>
#include <plib.h>
#include "IO.h"


void IO_Init(void)
{   
    // Clear PORTA bits so there are no unexpected flashes when setting them to output
    mPORTAClearBits(BIT_0);
    // Set PORTA bits as output
    mPORTASetPinsDigitalOut(BIT_0);
    
    // Clear PORTB bits so there are no unexpected flashes when setting them to output
    mPORTBClearBits(BIT_13 | BIT_15);
    // Set PORTB bits as input
    mPORTBSetPinsDigitalIn(BIT_13);
    // Set PORTB bits as output
    mPORTBSetPinsDigitalOut(BIT_15);
//    
    // ADC IO
    
    // UART IO
    
    // SPI IO
    
    // I2C IO
}
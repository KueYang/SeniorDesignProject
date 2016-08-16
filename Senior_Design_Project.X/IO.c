#include <p32xxxx.h>
#include <plib.h>
#include "IO.h"


void IO_Init(void)
{   
    // Clear PORTA bits so there are no unexpected flashes when setting
    // them to output in the next step
    mPORTAClearBits(BIT_0);

    // Set PORTA bits as output
    mPORTASetPinsDigitalOut(BIT_0);
    
    // ADC IO
    
    // UART IO
    
    // SPI IO
    
    // I2C IO
}
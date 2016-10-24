#include <p32xxxx.h>
#include <plib.h>
#include "IO.h"

void IO_Init(void)
{   
    ANSELA = 0; ANSELB = 0;                 // Disable analog inputs
    CM1CON = 0; CM2CON = 0; CM3CON = 0;     // Disable analog comparators
    
    // Digital IO
    
    // ADC IO
    
    // UART IO
    TRISAbits.TRISA2 = 1;  // U1RX
    TRISBbits.TRISB3 = 0;  // U1TX
    
    // SPI IO
//    ANSELBbits.ANSB2 = 0;
//    ANSELBbits.ANSB13 = 0;
//    ANSELBbits.ANSB14 = 0;
//    
//    TRISBbits.TRISB2 = 0;   // SD_CS
//    TRISBbits.TRISB11 = 1;  // SD_SDI
//    TRISBbits.TRISB13 = 0;  // SD_SDO
//    TRISBbits.TRISB14 = 0;  // SD_CLK
    
    // I2C IO
}
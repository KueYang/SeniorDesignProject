/**
 * @file IO.c
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The IO Module.
 */

#include <p32xxxx.h>
#include <plib.h>
#include "IO.h"

/**
 * @brief Initializes the IO module.
 * @details Initializes all pins used for all hardware modules in the application.
 * @return Void
 */
void IO_Init(void)
{   
    ANSELA = 0; ANSELB = 0;                 // Disable analog inputs
    CM1CON = 0; CM2CON = 0; CM3CON = 0;     // Disable analog comparators
    
    // Digital IO
    TRISBbits.TRISB7 = 0;   // Used to test Timer
    
    // ADC IO
    
    // UART IO
    TRISAbits.TRISA2 = 1;   // U1RX
    TRISBbits.TRISB3 = 0;   // U1TX
    
    // SPI IO
    TRISAbits.TRISA3 = 0;   // LDAC
    TRISBbits.TRISB4 = 0;   // CLR
    TRISAbits.TRISA4 = 0;   // SYNC
    TRISBbits.TRISB5 = 0;   // LED SD Card Indication
    TRISBbits.TRISB2 = 1;   // SD_SDI2
    TRISBbits.TRISB1 = 0;   // SD_SDO2
    TRISBbits.TRISB15 = 0;  // SD_CLK2
}

/**
 * @brief Checks and updates IO used for detecting the fret position.
 * @return Void
 */
void IO_Process(void)
{
    
}


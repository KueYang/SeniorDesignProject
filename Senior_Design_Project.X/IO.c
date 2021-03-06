/**
 * @file IO.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The IO module will handle all IO related tasks. The module will be
 * initialize both the analog and digital IOs for all other modules (e.g. UART). 
 * The process of checking for finger placement for each frets is handled in 
 * this module.
 */

#include <p32xxxx.h>
#include <stdio.h>
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "IO.h"

/** @def FRET_GROUP_COUNT 
 * Defines the number of fret groups. */
#define FRET_GROUP_COUNT    4
/** @def FRETS_PER_GROUP 
 * Defines the number of frets per group. */
#define FRETS_PER_GROUP     5

void IO_setGroupOutput(int group);

/**
 * @brief Initializes the IO module.
 * @details Initializes all pins used for all hardware modules in the application.
 * @return Void
 */
void IO_Init(void)
{   
    // Disables all analog pins
    ANSELA = 0x0000; ANSELB = 0x0000; ANSELC = 0x0000;
    ANSELD = 0x0000; ANSELE = 0x0000; ANSELF = 0x0000;
    ANSELG = 0x0000;
    
    // Digital IO
    TRISEbits.TRISE2 = 0;   // LED, ON
    TRISEbits.TRISE3 = 0;   // LED, ERROR
    TRISEbits.TRISE4 = 0;   // LED, INITIALIZATION
    
    // Fret
    TRISGbits.TRISG1 = 1;   // Fret 5
    TRISGbits.TRISG0 = 1;   // Fret 4
    TRISAbits.TRISA6 = 1;   // Fret 3
    TRISAbits.TRISA7 = 1;   // Fret 2
    TRISEbits.TRISE0 = 1;   // Fret 1
    
    // Frets Groups
    TRISEbits.TRISE1 = 0;   // Group 4
    TRISGbits.TRISG14 = 0;  // Group 3
//    TRISGbits.TRISG12 = 0;  // Group 2
    TRISDbits.TRISD4 = 0;   // Group 2
    TRISGbits.TRISG13 = 0;  // Group 1
    
    // UART IO
    TRISCbits.TRISC1 = 1;   // U1RX
    TRISEbits.TRISE5 = 0;   // U1TX
    
    // SPI IO, DAC
    TRISCbits.TRISC4 = 0;   // SYNC
    TRISGbits.TRISG6 = 0;   // DAC_CLK2
    TRISGbits.TRISG7 = 1;   // DAC_SDI2
    TRISGbits.TRISG8 = 0;   // DAC_SDO2
    
    // SPI IO, SD Card
    TRISBbits.TRISB11 = 0;  // CS
    TRISBbits.TRISB8 = 1;   // CD
    TRISBbits.TRISB9 = 1;   // SD_SDI3
    TRISBbits.TRISB10 = 0;  // SD_SDO3
    TRISFbits.TRISF13 = 0;  // SD_CLK3
    
    // ADC 
    TRISGbits.TRISG15 = 1;   // set RG15 as an input
    ANSELGbits.ANSG15 = 1;   // set RG15 (AN28) to analog
    
    // Clears All Digital IO
    PORTACLR = 0xFFFF; PORTBCLR = 0xFFFF; PORTCCLR = 0xFFFF;
    PORTDCLR = 0xFFFF; PORTECLR = 0xFFFF; PORTFCLR = 0xFFFF; 
    PORTGCLR = 0xFFFF;
    
    ON_LED = 0;                 // ON LED
    ERROR_LED = 1;              // ERROR LED
    INITIALIZE_LED = 0;         // INITIALIZATION LED
}

/**
 * @brief Scans a selection of frets.
 * @details Scans all frets groups to determine which fret was pressed. Sets the
 * currently selected fret to the fret that is pressed. Defaults to an open fret.
 * @return Void
 */
int IO_scanFrets(void)
{
    int groupIndex = 0;
    int fretFound = 0;
    int currentFret = 0;
    
    /* Scans through all the fret groups. */
    for(groupIndex = 1; groupIndex <= FRET_GROUP_COUNT; groupIndex++)
    {
        /* Selects the fret group. */
        IO_setGroupOutput(groupIndex);
        
        /* Scans through the five fret inputs. */
        if(FRET1 == 1) {fretFound = 1;}
        else if(FRET2 == 1) {fretFound = 2;}
        else if(FRET3 == 1) {fretFound = 3;}
        else if(FRET4 == 1) {fretFound = 4;}
        else if(FRET5 == 1) {fretFound = 5;}
        
        /* Turn off all output pins. */
        IO_setGroupOutput(0);
        
        /* Checks if any frets were pressed. */
        if(fretFound > 0)
        {
            /* Calculates the fret that is selected*/
            currentFret = (groupIndex-1)*FRETS_PER_GROUP + fretFound;
            break;
        }
    }
    
    char buf[32];
    snprintf(&buf[0] ,32 ,"Fret Selected: %d", currentFret);
    MON_SendString(&buf[0]);
    
    return currentFret;
}

/**
 * @brief Sets the fret group to be scan.
 * @arg group The fret group to be scan.
 * @return Void
 */
void IO_setGroupOutput(int group)
{
    switch(group)
    {
        case 1:
            GROUP1_OUT = 1;
            GROUP2_OUT = 0;
            GROUP3_OUT = 0;
            GROUP4_OUT = 0;
            break;
        case 2:
            GROUP1_OUT = 0;
            GROUP2_OUT = 1;
            GROUP3_OUT = 0;
            GROUP4_OUT = 0;
            break;
        case 3:
            GROUP1_OUT = 0;
            GROUP2_OUT = 0;
            GROUP3_OUT = 1;
            GROUP4_OUT = 0;
            break;
        case 4:
            GROUP1_OUT = 0;
            GROUP2_OUT = 0;
            GROUP3_OUT = 0;
            GROUP4_OUT = 1;
            break;
        case 5:
            GROUP1_OUT = 1;
            GROUP2_OUT = 1;
            GROUP3_OUT = 1;
            GROUP4_OUT = 1;
            break;
        default:
            GROUP1_OUT = 0;
            GROUP2_OUT = 0;
            GROUP3_OUT = 0;
            GROUP4_OUT = 0;
            break;
    }
}
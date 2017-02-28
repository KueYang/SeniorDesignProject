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
#include <plib.h>
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "IO.h"

/** @def FRET_GROUP_COUNT 
 * Defines the number of fret groups. */
#define FRET_GROUP_COUNT    4
/** @def FRETS_PER_GROUP 
 * Defines the number of frets per group. */
#define FRETS_PER_GROUP     5

/** @var currentFret 
 * The current fret that is selected. */
int currentFret;

void IO_setGroupOutput(int group);

/**
 * @brief Initializes the IO module.
 * @details Initializes all pins used for all hardware modules in the application.
 * @return Void
 */
void IO_Init(void)
{   
    // Digital IO
    TRISBbits.TRISB5 = 0;   // LED
    
    // Frets
    TRISAbits.TRISA3 = 1;   // Fret 1, Group 1
    TRISBbits.TRISB4 = 1;   // Fret 2, Group 1
    TRISBbits.TRISB7 = 1;   // Fret 4, Group 1
    TRISBbits.TRISB8 = 0;   // Group 1, Output
    
    // UART IO
    TRISAbits.TRISA2 = 1;   // U1RX
    TRISBbits.TRISB3 = 0;   // U1TX
    
    // SPI IO, DAC
    TRISAbits.TRISA4 = 0;   // SYNC
    TRISBbits.TRISB2 = 1;   // SD_SDI2
    TRISBbits.TRISB1 = 0;   // SD_SDO2
    TRISBbits.TRISB15 = 0;  // SD_CLK2
    
    // SPI IO, SD Card
    TRISBbits.TRISB7 = 0;   // CS
    TRISBbits.TRISB8 = 1;   // CD
    TRISBbits.TRISB11 = 1;  // SD_SDI1
    TRISBbits.TRISB13 = 0;  // SD_SDO1
    TRISBbits.TRISB14 = 0;  // SD_CLK1
    
    // ADC 
    TRISBbits.TRISB0 = 1;   // set RB0 as an input
    ANSELBbits.ANSB0 = 1;   // set RB0 (AN2) to analog
    
    currentFret = 0;        // Sets the current fret to 0, indicating that the string is "opened."
}

/**
 * @brief Checks and updates IO used for detecting the fret position.
 * @return Void
 */
void IO_Process(void)
{
    
}

/**
 * @brief Sets the currently selected fret.
 * @arg fret The fret to select
 * @return Void
 */
void IO_setCurrentFret(int fret)
{
    currentFret = fret;
}

/**
 * @brief Returns the currently selected fret.
 * @return Returns the currently selected fret.
 */
int IO_getCurrentFret(void)
{
    return currentFret;
}

/**
 * @brief Scans a selection of frets.
 * @details Scans all frets groups to determine which fret was pressed. Sets the
 * currently selected fret to the fret that is pressed. Defaults to an open fret.
 * @return Void
 */
void IO_scanFrets(void)
{
    int groupIndex;
    BOOL setNewFret = FALSE;
    int fretFound = 0;
    
    /* Scans through all the fret groups. */
    for(groupIndex = 1; groupIndex <= FRET_GROUP_COUNT; groupIndex+=5)
    {
        /* Selects the fret group. */
        IO_setGroupOutput(groupIndex);
        
        /* Scans through the five fret inputs. */
        if(FRET1 == 1) {fretFound = 1;}
        if(FRET2 == 1) {fretFound = 2;}
        if(FRET3 == 1) {fretFound = 3;}
        if(FRET4 == 1) {fretFound = 4;}
        if(FRET5 == 1) {fretFound = 5;}
        
        /* Checks if any frets were pressed. */
        if(fretFound > 0)
        {
            currentFret = groupIndex + fretFound;
            setNewFret = TRUE;
            break;
        }
    }
    /* If no frets were pressed, set the fret to 0, indicating an open string. */
    if(!setNewFret)
    {
        currentFret = 0;
    }
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
        case 0:
            GROUP1_OUT = 1;
            GROUP2_OUT = 0;
            GROUP3_OUT = 0;
            GROUP4_OUT = 0;
            break;
        case 1:
            GROUP1_OUT = 0;
            GROUP2_OUT = 1;
            GROUP3_OUT = 0;
            GROUP4_OUT = 0;
            break;
        case 2:
            GROUP1_OUT = 0;
            GROUP2_OUT = 0;
            GROUP3_OUT = 1;
            GROUP4_OUT = 0;
            break;
        case 3:
            GROUP1_OUT = 0;
            GROUP2_OUT = 0;
            GROUP3_OUT = 0;
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
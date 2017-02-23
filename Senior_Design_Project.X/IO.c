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

#define FRET_GROUP_COUNT    4
#define FRETS_PER_GROUP     5

void IO_setGroupOutput(int group);
int IO_scanGroupFrets(int index);

int currentFret;

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
    int groupIndex, fretIndex;
    BOOL setNewFret = FALSE;
    
    /* Scans through all the fret groups. */
    for(groupIndex = 1; groupIndex <= FRET_GROUP_COUNT; groupIndex+=5)
    {
        /* Selects the fret group. */
        IO_setGroupOutput(groupIndex); 
        for(fretIndex = 0; fretIndex < FRETS_PER_GROUP; fretIndex++)
        {
            /* Scans through each fret group. */
            if(IO_scanGroupFrets(groupIndex + fretIndex) == 1)
            {
                currentFret = groupIndex + fretIndex;
                setNewFret = TRUE;
                break;
            }
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

/**
 * @brief Sets the fret group to be scan.
 * @arg fret The selected fret group to scan.
 * @return Returns the fret that is selected.
 * @retval 0, if no frets were scanned.
 */
int IO_scanGroupFrets(int fret)
{
    switch(fret)
    {
        /* Group 1 */
        case 1:
            return FRET1;
        case 2:
            return FRET2;
        case 3:
            return FRET3;
        case 4:
            return FRET4;
        case 5:
            return FRET5;
        /* Group 2 */
        case 6:
            return FRET6;
        case 7:
            return FRET7;
        case 8:
            return FRET8;
        case 9:
            return FRET9;
        case 10:
            return FRET10;
        /* Group 3 */
        case 11:
            return FRET11;
        case 12:
            return FRET12;
        case 13:
            return FRET13;
        case 14:
            return FRET14;
        case 15:
            return FRET15;
        /* Group 4 */
        case 16:
            return FRET16;
        case 17:
            return FRET17;
        case 18:
            return FRET18;
        case 19:
            return FRET19;
        case 20:
            return FRET20;
    }
    return FRET0;
}


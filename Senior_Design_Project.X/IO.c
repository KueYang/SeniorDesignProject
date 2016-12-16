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
    TRISAbits.TRISA4 = 0;   // SYNC
    TRISBbits.TRISB4 = 0;   // CLR
    TRISBbits.TRISB5 = 0;   // RSTSEL
    TRISBbits.TRISB2 = 1;   // SD_SDI2
    TRISBbits.TRISB1 = 0;   // SD_SDO2
    TRISBbits.TRISB15 = 0;  // SD_CLK2
    
    currentFret = 0;    // Sets the current fret to 0, indicating that the string is "opened."
}

/**
 * @brief Checks and updates IO used for detecting the fret position.
 * @return Void
 */
void IO_Process(void)
{
    
}

void IO_setCurrentFret(int fret)
{
    currentFret = fret;
}

int IO_getCurrentFret(void)
{
    return currentFret;
}

void IO_scanFrets(void)
{
    int groupIndex, fretIndex;
    BOOL setNewFret = FALSE;
    
    // Scans through all the frets.
    for(groupIndex = 1; groupIndex <= FRET_GROUP_COUNT; groupIndex+=5)
    {
        IO_setGroupOutput(groupIndex); // Sets the output pin high for the group.
        for(fretIndex = 0; fretIndex < FRETS_PER_GROUP; fretIndex++)
        {
            if(IO_scanGroupFrets(groupIndex + fretIndex) == 1)
            {
                currentFret = groupIndex + fretIndex;
                setNewFret = TRUE;
                break;
            }
        }
    }
    // If no frets were pressed, set the fret to 0, indicating an open string.
    if(!setNewFret)
    {
        currentFret = 0;
    }
}

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

int IO_scanGroupFrets(int index)
{
    switch(index)
    {
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
    return 0;
}


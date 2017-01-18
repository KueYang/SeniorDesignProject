/**
 * @file TIMER.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The TIMER module will handle timers and delays used in the 
 * application. 
 */

#include <p32xxxx.h>
#include <plib.h>
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "Audio.h"
#include "TIMER.h"

/**  
 * @privatesection
 * @{
 */

/** @def PERIOD 
 * Timer 1 Period for one ms. */
#define ONE_MS_PERIOD           156
#define TWENTY_US_PERIOD        12

void TIMER1_Init(void);
void TIMER3_Init(void);

/**@var ms_TICK 
 * Millisecond counter. */
UINT32 ms_TICK;
BOOL Timer3_ON;
/** @} */

/**
 * @brief Initializes all timer modules.
 * @return Void
 */
void TIMER_Init(void)
{
    TIMER1_Init();
    TIMER3_Init();
}

/**
 * @brief Runs timer related operations.
 * @return Void
 */
void TIMER_Process(void)
{
    PORTBbits.RB5 = 1;
    TIMER_MSecondDelay(3000);
    PORTBbits.RB5 = 0;
    TIMER_MSecondDelay(3000);
}

/**
 * @brief Initializes Timer 1 module.
 * @return Void
 */
void TIMER1_Init(void)
{
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, ONE_MS_PERIOD);
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T1, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_1);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
}

void TIMER_SetSampleRate(UINT16 sampleRate)
{
    UINT16 period = ((GetPeripheralClock()/sampleRate)-1);
    PR3 = period;
    TMR3 = 0;
}

/**
 * @brief Initializes Timer 3 module.
 * @return Void
 */
void TIMER3_Init(void)
{
//    OpenTimer3(T3_OFF | T3_SOURCE_INT | T3_PS_1_8, SIX_US_PERIOD);
    T3CONbits.ON = 0;
    T3CONbits.SIDL = 0;
    T3CONbits.TGATE = 0;
    T3CONbits.TCKPS = 0b000;    //PS = 1
    T3CONbits.TCS = 0;
    
    PR3 = TWENTY_US_PERIOD;
    TMR3 = 0;
    
    Timer3_ON = FALSE;
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T3, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_1);
    INTSetVectorSubPriority(INT_TIMER_3_VECTOR, INT_SUB_PRIORITY_LEVEL_2);
}

BOOL TIMER3_IsON(void)
{
    return Timer3_ON;
}

void TIMER3_ON(BOOL ON)
{
    if(ON == TRUE)
    {
        T3CONbits.ON = 1;
        Timer3_ON = TRUE;
    }
    else
    {
        T3CONbits.ON = 0;
        Timer3_ON = FALSE;
    }
}

/**
 * @brief Delays the application for a given set time.
 * @arg timeDelay The delay in milliseconds.
 * @return Void
 */
void TIMER_MSecondDelay(int timeDelay)
{
    int msTick = TIMER_GetMSecond();
    while ((ms_TICK-msTick) <= timeDelay);
}

/**
 * @brief Returns the millisecond count.
 * @return Returns the millisecond count of the application since startup.
 */
UINT32 TIMER_GetMSecond(void)
{
    return ms_TICK;
}

/**
 * @brief Timer 1 Interrupt Service Routine.
 * @details The interrupt service routine is used to increment the millisecond
 * counter used for the timer.
 * @return Void.
 */
void __ISR(_TIMER_1_VECTOR, IPL2AUTO) Timer1Handler(void)
{
    // Increments the millisecond counter.
    ms_TICK++;
    
    // Clear the interrupt flag
    INTClearFlag(INT_T1);
}


void __ISR(_TIMER_3_VECTOR, IPL2AUTO) Timer3Handler(void)
{
    /* 
     * Checks if the bytes written is greater than the buffer size. If so, 
     * starts reading from memory again to fill in the buffer. Otherwise, write
     * data to the DAC.
     */
    int bytesWritten = AUDIO_getBytesWritten();
    int bytesRead = AUDIO_getBytesRead();
    
    if((bytesRead == bytesWritten) && (!AUDIO_isDoneReading()))
    {
        AUDIO_ReadDataFromMemory();
    }
    else
    {
        if(AUDIO_isDoneWriting())
        {
            AUDIO_setNewTone(0);        // Resets the note to the open string note
            TIMER3_ON(FALSE);
        }
        else
        {
            AUDIO_WriteDataToDAC();
        }
    }
    
    // Clear the interrupt flag
    INTClearFlag(INT_T3);
}



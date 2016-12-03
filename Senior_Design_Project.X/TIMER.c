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
#include "TIMER.h"

/**  
 * @privatesection
 * @{
 */

/** @def PERIOD 
 * Timer 1 Period for one ms. */
#define PERIOD  156 

void TIMER1_Init(void);
void TIMER2_Init(void);
void TIMER3_Init(void);

/**@var ms_TICK 
 * Millisecond counter. */
UINT32 ms_TICK;
BOOL Timer2_ON;
BOOL Timer3_ON;
/** @} */

/**
 * @brief Initializes all timer modules.
 * @return Void
 */
void TIMER_Init(void)
{
    TIMER1_Init();
    TIMER2_Init();
    TIMER3_Init();
}

/**
 * @brief Runs timer related operations.
 * @return Void
 */
void TIMER_Process(void)
{
    PORTBbits.RB7 = 1;
    TIMER_MSecondDelay(3000);
    PORTBbits.RB7 = 0;
    TIMER_MSecondDelay(3000);
}

/**
 * @brief Initializes Timer 1 module.
 * @return Void
 */
void TIMER1_Init(void)
{
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, PERIOD);
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T1, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_1);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
}

void TIMER_SetSampleRate(UINT16 sampleRate)
{
    PR2 = (GetPeripheralClock()/sampleRate) - 1;
    TMR2 = 0;
}

/**
 * @brief Initializes Timer 2 module.
 * @return Void
 */
void TIMER2_Init(void)
{
    OpenTimer2(T2_OFF | T2_SOURCE_INT | T2_PS_1_256, PERIOD);
    
    Timer2_ON = FALSE;
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T2, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_1);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_1);
}

/**
 * @brief Initializes Timer 3 module.
 * @return Void
 */
void TIMER3_Init(void)
{
    OpenTimer3(T3_OFF | T3_SOURCE_INT | T3_PS_1_256, PERIOD);
    
    Timer3_ON = FALSE;
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T3, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_1);
    INTSetVectorSubPriority(INT_TIMER_3_VECTOR, INT_SUB_PRIORITY_LEVEL_2);
}

BOOL TIMER2_IsON(void)
{
    return Timer2_ON;
}

void TIMER2_ON(BOOL ON)
{
    if(ON == TRUE)
    {
        T2CONbits.ON = 1;
        Timer2_ON = TRUE;
    }
    else
    {
        T2CONbits.ON = 0;
        Timer2_ON = FALSE;
    }
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

void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2Handler(void)
{
    Audio_WriteDataToDAC();
    
    // Clear the interrupt flag
    INTClearFlag(INT_T2);
}

void __ISR(_TIMER_3_VECTOR, IPL2AUTO) Timer3Handler(void)
{
    Audio_ReadDataFromMemory();
    
    // Enables the timer 2 module if it isn't already on.
    if(!Timer2_ON)
    {
        TIMER2_ON(TRUE);
    }
    
    // Clear the interrupt flag
    INTClearFlag(INT_T3);
}



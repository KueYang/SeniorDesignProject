/**
 * @file TIMER.c
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The TIMER module.
 */

#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "TIMER.h"

/**  
 * @privatesection
 * @{
 */
/** 
 * Timer 1 Period, period needed for timer 1 to trigger an interrupt every 1 second.
 * 40 MHz PBCLK / 256 = 156.25 kHz, Timer 1 clock
 */
#define PERIOD  156 

void TIMER1_Init(void);

/**@var Millisecond counter. */
UINT32 ms_TICK;

/**@}*/

/**
 * @brief Initializes all timer modules.
 * @return Void
 */
void TIMER_Init(void)
{
    TIMER1_Init();
}

/**
 * @brief Runs timer related operations.
 * @return None
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

/**
 * @brief Delays the application for a given set time.
 * @param timeDelay The delay in milliseconds.
 * @return Void
 */
void TIMER_MSecondDelay(int timeDelay)
{
    int msTick = TIMER_GetMSecond();
    while ((ms_TICK-msTick) <= timeDelay);
}

/**
 * @brief Returns the millisecond count since the start of the application.
 * @return The millisecond count of the application.
 */
UINT32 TIMER_GetMSecond(void)
{
    return ms_TICK;
}

/**
 * @brief Timer 1 Interrupt Service Routine.
 * @return Void.
 */
void __ISR(_TIMER_1_VECTOR, IPL2AUTO) Timer1Handler(void)
{
    // Increments the millisecond timer.
    ms_TICK++;
    
    // Clear the interrupt flag
    INTClearFlag(INT_T1);
}
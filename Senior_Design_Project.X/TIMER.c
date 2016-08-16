#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "TIMER.h"

// Period needed for timer 1 to trigger an interrupt every 1 second
// (8 MHz PBCLK / 256 = 31250 KHz Timer 1 clock)
#define PERIOD  31250

UINT32 ms_TICK;

void TIMER1_Init(void);

/*
 *  Timers Initialization
 */
void TIMER_Init(void)
{
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, PERIOD);
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T1, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
}

void TIMER_MSecondDelay(int timeDelay)
{
    int msTick = TIMER_GetMSecond();
    while ((ms_TICK-msTick) <= timeDelay);
}

UINT32 TIMER_GetMSecond()
{
    return ms_TICK;
}

void __ISR(_TIMER_1_VECTOR, IPL2AUTO) Timer1Handler(void)
{
//    ms_TICK++;
    
        mPORTAToggleBits(BIT_0);
    // Clear the interrupt flag
    INTClearFlag(INT_T1);
}
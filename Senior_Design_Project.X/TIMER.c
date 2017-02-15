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
#include "./fatfs/diskio.h"
#include "AUDIO.h"
#include "TIMER.h"

/**  
 * @privatesection
 * @{
 */
#define INT32_MAX_NUM           1<<31

/** @def PERIOD 
 * Timer 1 Period for one ms. */
#define ONE_HUNDRED_US_PERIOD   4000
#define ONE_MS_PERIOD           40000
#define TWENTY_US_PERIOD        12

void TIMER1_Init(void);
void TIMER2_Init(void);
void TIMER3_Init(void);

/**@var ms_TICK 
 * Millisecond counter. */
UINT32 one_hundred_us_TICK;
UINT32 ms_TICK;
BOOL Timer1_ON;
BOOL Timer2_ON;
BOOL Timer3_ON;
/** @} */

/**
 * @brief Initializes all timer modules.
 * @return Void
 */
void TIMER_Init(void)
{
    one_hundred_us_TICK = 0;
    ms_TICK = 0;
    
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
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_1, ONE_MS_PERIOD);
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T1, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_1_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_TIMER_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
}

BOOL TIMER1_IsON(void)
{
    return Timer1_ON;
}

void TIMER1_ON(BOOL ON)
{
    if(ON == TRUE)
    {
        T1CONbits.ON = 1;
        Timer1_ON = TRUE;
        INTClearFlag(INT_T1);
    }
    else
    {
        T1CONbits.ON = 0;
        Timer1_ON = FALSE;
        INTClearFlag(INT_T1);
    }
}

void TIMER2_Init(void)
{
    T2CONbits.ON = 0;
    T2CONbits.SIDL = 0;
    T2CONbits.TGATE = 0;
    T2CONbits.TCKPS = 0b000;    //PS = 1
    T2CONbits.TCS = 0;
    
    PR2 = ONE_HUNDRED_US_PERIOD;
    TMR2 = 0;
    
    T2CONbits.ON = 0;
    Timer2_ON = FALSE;
    
    // Set up the timer interrupt with a priority of 2
    INTEnable(INT_T2, INT_ENABLED);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_3);
}

void TIMER2_SetSampleRate(UINT16 sampleRate)
{
    PR2 = ((GetPeripheralClock()/sampleRate)-1);
    TMR2 = 0;
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
        INTClearFlag(INT_T2);
    }
    else
    {
        T2CONbits.ON = 0;
        Timer2_ON = FALSE;
        INTClearFlag(INT_T2);
    }
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
    INTSetVectorPriority(INT_TIMER_3_VECTOR, INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_TIMER_3_VECTOR, INT_SUB_PRIORITY_LEVEL_2);
}

void TIMER3_SetSampleRate(UINT16 sampleRate)
{
    UINT16 period = ((GetPeripheralClock()/sampleRate)-1);
    PR3 = period;//+ (period/3);
    TMR3 = 0;
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
        INTClearFlag(INT_T3);
    }
    else
    {
        T3CONbits.ON = 0;
        Timer3_ON = FALSE;
        INTClearFlag(INT_T3);
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

UINT32 TIMER_GetUSecond(void)
{
    return one_hundred_us_TICK/100;
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
    
    // Resets the ms tick if overflows
    if(ms_TICK >= INT32_MAX_NUM)
    {
        ms_TICK = 0;
    }
    
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
    
    // Clear the interrupt flag
    INTClearFlag(INT_T1);
}

void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2Handler(void)
{
    one_hundred_us_TICK++;
    
    INTClearFlag(INT_T2);
}

void __ISR(_TIMER_3_VECTOR, IPL2AUTO) Timer3Handler(void)
{
    /* 
     * Checks if the bytes written is greater than the buffer size. If so, 
     * starts reading from memory again to fill in the buffer. Otherwise, write
     * data to the DAC.
     */
    AUDIO_WriteDataToDAC();
    
    // Clear the interrupt flag
    INTClearFlag(INT_T3);
}

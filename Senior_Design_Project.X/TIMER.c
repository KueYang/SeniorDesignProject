/**
 * @file TIMER.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The TIMER module will handle timers and delays used in the 
 * application. 
 */

#include <p32xxxx.h>
#include "plib/plib.h"
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "./fatfs/diskio.h"
#include "AUDIO.h"
#include "TIMER.h"

/**  
 * @privatesection
 * @{
 */
/** @def ONE_MS_PERIOD 
 * Timer 1 Period for one ms. */
#define ONE_MS_PERIOD           40000

/**@var ms_TICK 
 * Millisecond counter. */
UINT32 ms_TICK;
/**@var Timer1_ON 
 * Boolean used to indicated if Timer 1 is on/off. */
BOOL Timer1_ON;
/**@var Timer3_ON 
 * Boolean used to indicated if Timer 3 is on/off. */
BOOL Timer3_ON;
/** @} */

void TIMER1_Init(void);
void TIMER3_Init(void);

/**
 * @brief Initializes all timer modules.
 * @return Void
 */
void TIMER_Init(void)
{
    ms_TICK = 0;
    
    TIMER1_Init();
    TIMER3_Init();
}

/**
 * @brief Runs timer related operations.
 * @return Void
 */
void TIMER_Process(void)
{
    PORTEbits.RE3 = 1;
    TIMER_MSecondDelay(5000);
    PORTEbits.RE3 = 0;
    TIMER_MSecondDelay(5000);
}

/**
 * @brief Initializes Timer 1 module.
 * @return Void
 */
void TIMER1_Init(void)
{
    T1CONbits.ON = 0;           // Disable Timer 1
    T1CONbits.SIDL = 0;         // Disables sleep on idle
    T1CONbits.TGATE = 0;        // Gated time accumulation is disabled
    T1CONbits.TCKPS = 0b000;    // Timer 1 Pre-Scalar = 1
    T1CONbits.TCS = 0;
    
    PR1 = ONE_MS_PERIOD;        // Sets Timer 1 Period to 1 ms
    TMR1 = 0;                   // Clears Timer 1 counter
    
    T1CONbits.ON = 1;           // Enable Timer 1
    
    /* Sets up the Timer 1 interrupts. */
    IFS0bits.T1IF = 0;          // Clears Timer 1 Interrupt Flag
    IEC0bits.T1IE = 1;          // Enables Timer 1 Interrupt
    IPC1bits.T1IP = 2;          // Sets Timer 1 Interrupt Priority 2
    IPC1bits.T1IS = 0;          // Sets Timer 1 Interrupt Sub-Priority 0
}

/**
 * @brief Checks if Timer 1 is on/off.
 * @return Returns a boolean indicating if Timer 1 is on/off.
 * @retval TRUE Timer 1 is on.
 * @retval FALSE Timer 1 is off.
 */
BOOL TIMER1_IsON(void)
{
    return Timer1_ON;
}

/**
 * @brief Toggles on/off Timer 1.
 * @arg ON Toggles the timer on/off (TRUE/FALSE).
 * @return Void
 */
void TIMER1_ON(BOOL ON)
{
    if(ON == TRUE)
    {
        T1CONbits.ON = 1;
        Timer1_ON = TRUE;
        IFS0bits.T1IF = 0;
    }
    else
    {
        T1CONbits.ON = 0;
        Timer1_ON = FALSE;
        IFS0bits.T1IF = 0;
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
    
    // Resets the ms tick if overflows
    if(ms_TICK >= INT32_MAX_NUM)
    {
        ms_TICK = 0;
    }
    
    WDTCONbits.WDTCLR = 0x01;       // Clears the watchdog timer flag.
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
    WDTCONbits.WDTCLR = 0x01;       // Clears the watchdog timer flag.
    
    // Clear the interrupt flag
    IFS0bits.T1IF = 0;
}

/**
 * @brief Initializes Timer 3 module.
 * @return Void
 */
void TIMER3_Init(void)
{
    T3CONbits.ON = 0;           // Disable Timer 3
    T3CONbits.SIDL = 0;         // Disables sleep on idle
    T3CONbits.TGATE = 0;        // Gated time accumulation is disabled
    T3CONbits.TCKPS = 0b000;    // Timer 3 Pre-Scalar = 1
    T3CONbits.TCS = 0;
    
    PR3 = ONE_MS_PERIOD;        // Sets Timer 3 Period to 1 ms
    TMR3 = 0;                   // Clears Timer 3 counter
    
    T3CONbits.ON = 0;           // Disable Timer 3
    
    /* Sets up the Timer 1 interrupts. */
    IFS0bits.T3IF = 0;          // Clears Timer 3 interrupt flag
    IEC0bits.T3IE = 1;          // Enables Timer 3 interrupt
    IPC3bits.T3IP = 2;          // Sets Timer 3 priority to 2
    IPC3bits.T3IS = 2;          // Sets Timer 3 sub-priority to 2
    
    Timer3_ON = FALSE;
}

/**
 * @brief Sets the Timer 3 period
 * @arg sampleRate The sample rate to set Timer 3 at.
 * @return Void
 */
void TIMER3_SetSampleRate(UINT16 sampleRate)
{
    UINT16 period = ((GetPeripheralClock()/sampleRate)-1);
    PR3 = period + (period/2);
    TMR3 = 0;
}

/**
 * @brief Checks if Timer 3 is on/off.
 * @return Returns a boolean indicating if Timer 3 is on/off.
 * @retval TRUE Timer 3 is on.
 * @retval FALSE Timer 3 is off.
 */
BOOL TIMER3_IsON(void)
{
    return Timer3_ON;
}

/**
 * @brief Toggles on/off Timer 3.
 * @arg ON Toggles the timer on/off (TRUE/FALSE).
 * @return Void
 */
void TIMER3_ON(BOOL ON)
{
    if(ON == TRUE)
    {
        TMR3 = 0;
        T3CONbits.ON = 1;
        Timer3_ON = TRUE;
        IFS0bits.T3IF = 0;
    }
    else
    {
        TMR3 = 0;
        T3CONbits.ON = 0;
        Timer3_ON = FALSE;
        IFS0bits.T3IF = 0;
    }
}

/**
 * @brief Timer 3 Interrupt Service Routine.
 * @details The interrupt service routine is used to write audio data to the 
 * DAC in a set interval.
 * @return Void.
 */
void __ISR(_TIMER_3_VECTOR, IPL2AUTO) Timer3Handler(void)
{
    /* 
     * Checks if the bytes written is greater than the buffer size. If so, 
     * starts reading from memory again to fill in the buffer. Otherwise, write
     * data to the DAC.
     */
    AUDIO_WriteDataToDAC();
    
    // Clear the interrupt flag
    IFS0bits.T3IF = 0;
}

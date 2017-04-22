/**
 * @file main.c
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The main entry point of the application.
 */

#include <p32xxxx.h>
#include "plib/plib.h"
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
#include "ADC.h"
#include "SPI.h"
#include "UART.h"
#include "DAC.h"
#include "AUDIO.h"

/**
 * @defgroup usbConfig USB configurations
 * @{
 * @details All USB related tasks are disabled and not used in the application.
 */
#pragma config PMDL1WAY = OFF       // Peripheral Module Disable Configuration, disabled
#pragma config IOL1WAY = OFF        // Peripheral Pin Select Configuration, disabled
#pragma config FUSBIDIO = OFF       // USB USID Selection, disabled
#pragma config FVBUSONIO = OFF      // USB VBUS ON Selection, disabled
#pragma config UPLLIDIV = DIV_1     // USB PLL Input Divider
#pragma config UPLLEN = OFF         // USB PLL Enable, disabled
/**@}*/

/**
 * @defgroup clockConfig System and Peripheral Clock Configurations
 * @{
 * @details The configurations used to configure the system and peripheral clocks.
 * The system clock is configured for 40 Mhz using the internal 8 MHz oscillator.
 *  The system clock configuration can be configured as follow:
 *      SYSCLK = 40 MHz (8 MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
 *  where FPLL related configurations are used to adjust the clock.
 * The peripheral clock is configured to the same clock speed as the system clock.
 *  The peripheral clock configuration can be configured as follow:
 *      PBCLK = 40 MHz (SYSCLK / FPBDIV)
 *  where the FPB configuration is used to adjust the clock.
 */
#pragma config FPLLMUL = MUL_20     // PLL Multiplier
#pragma config FPLLIDIV = DIV_2     // PLL Input Divider
#pragma config FPLLODIV = DIV_2     // PLL Output Divider
#pragma config FNOSC = FRCPLL       // Oscillator Selection Bit, Internal Osc (FRC) with PLL
#pragma config POSCMOD = OFF        // Primary Oscillator Configuration, HS osc mode
#pragma config FSOSCEN = OFF        // Secondary Oscillator Disabled
#pragma config OSCIOFNC = OFF       // CLKO Output Signal on OSCO pin Disabled
#pragma config FPBDIV = DIV_1       // Peripheral Clock Divisor, Pb_Clk is Sys_Clk/1
/**@}*/

/**
 * @defgroup programConfig Programming Configurations
 * @{
 * @details The PIC is configured to be programmed on pins ICE3 and ICD3. JTAG
 *  and the internal switch over configurations are disabled. Code protection 
 *  and program flash protection are disabled.
 * 
 */
#pragma config IESO = OFF           // Internal/External Switch Over, disabled
#pragma config JTAGEN = OFF         // JTAG Disabled
#pragma config ICESEL = ICS_PGx1    // ICE3/ICD3 Comm Channel Select
#pragma config PWP = OFF            // Program Flash Write Protect
#pragma config CP = OFF             // Code Protection Disabled
/**@}*/

/**
 * @defgroup watchDog Watch Dog Timer Configurations
 * @{
 * @details The watch dog is enabled in software for this application.
 */
#pragma config FWDTEN = ON         // Watchdog Timer Enabled
#pragma config WDTPS = PS2048         // Watchdog Timer Post-scaler, 2s timeout
/**@}*/

/**
 * @brief The main entry point of the application.
 * @return An integer 0 upon exit success.
 */
int main(void) 
{    
//    DEVCFG1bits.FWDTEN = 0;         // Enable software control of WDT
//    WDTCONbits.ON = 0;              // Disables WatchDog Timer
//    CLEAR_WATCHDOG_TIMER;           // Clears the watchdog timer
//    DEVCFG1bits.WDTPS = 0b00110;    // PostScalar 1:64, 64ms
    
    /* Enable multi-vector interrupts */
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    /* Peripheral Initializations */
    IO_Init();                      // Initializes all analog/digital IO.
    
    /* Checks for a watchdog reset. */
    ((RCONbits.WDTO == 1) ?  (ERROR_LED = 0) : (ERROR_LED = 1));
        
    TIMER_Init();                   // Initializes all timer modules.
    ADC_Init();                     // Initializes all ADC modules.
    SPI_Init();                     // Initializes all SPI modules.
    UART_Init();                    // Initializes all UART modules
    AUDIO_Init();                   // Initializes the Audio module.
    DAC_Init();                     // Initializes the DACs.
    
    TIMER_MSecondDelay(1);
    ERROR_LED = 1;              // ERROR LED
    TIMER_MSecondDelay(1);
    INITIALIZE_LED = 1;         // INITIALIZATION LED
    TIMER_MSecondDelay(1);
    
//    WDTCONbits.ON = 1;              // Enable WatchDog Timer
    
    while(1)
    {
        CLEAR_WATCHDOG_TIMER;           // Clears the watchdog timer
        AUDIO_Process();
    }

    return (0);
}
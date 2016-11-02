#include <p32xxxx.h>
#include <plib.h>
#include "./FIO_Library/HardwareProfile.h"
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
//#include "ADC.h"
//#include "SPI.h"
#include "UART.h"
#include "Audio.h"

/* USB configurations */
#pragma config PMDL1WAY = OFF       // Peripheral Module Disable Configuration, disabled
#pragma config IOL1WAY = OFF        // Peripheral Pin Select Configuration, disabled
#pragma config FUSBIDIO = OFF       // USB USID Selection, disabled
#pragma config FVBUSONIO = OFF      // USB VBUS ON Selection, disabled
#pragma config UPLLIDIV = DIV_1     // USB PLL Input Divider
#pragma config UPLLEN = OFF         // USB PLL Enable, disabled

/* Clock Configuration Bit Settings:
 * SYSCLK = 40 MHz (8 MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
 * PBCLK = 40 MHz (SYSCLK / FPBDIV)
 * Internal RC Osc w/PLL (XT+,HS+,EC+PLL)
 */
#pragma config FPLLMUL = MUL_20     // PLL Multiplier
#pragma config FPLLIDIV = DIV_2     // PLL Input Divider
#pragma config FPLLODIV = DIV_2     // PLL Output Divider
#pragma config FNOSC = FRCPLL       // Oscillator Selection Bit, Internal Osc (FRC) with PLL
#pragma config POSCMOD = OFF        // Primary Oscillator Configuration, HS osc mode
#pragma config FSOSCEN = OFF        // Secondary Oscillator Disabled
#pragma config OSCIOFNC = OFF       // CLKO Output Signal on OSCO pin Disabled
#pragma config FPBDIV = DIV_1       // Peripheral Clock Divisor, Pb_Clk is Sys_Clk/1

/* Programming Configurations */
#pragma config IESO = OFF           // Internal/External Switch Over, disabled
#pragma config JTAGEN = OFF         // JTAG Disabled
#pragma config ICESEL = ICS_PGx1    // ICE1/ICD1 Comm Channel Select
#pragma config PWP = OFF            // Program Flash Write Protect
#pragma config CP = OFF             // Code Protection Disabled

/* Watch dog configurations */
#pragma config FWDTEN = OFF         // Watchdog Timer Disabled
#pragma config WDTPS = PS32         // Watchdog Timer Post-scaler

int main(int argc, char** argv) {
    /* Configures the system. */
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    
    /* Peripheral Initializations */
    IO_Init();
    TIMER_Init();
//    ADC_Init();
//    SPI_Init();
    UART_Init();
    AUDIO_Init();
    
    /* Enable multi-vector interrupts */
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
   
  
    PORTBbits.RB2 = 1;
    while(1)
    {
//        TIMER_Process();
        UART_Process();
//        SPI_Process();
        AUDIO_Process();
    }

    return (0);
}

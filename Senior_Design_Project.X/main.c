#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
//#include "ADC.h"
//#include "SPI.h"
#include "UART.h"
#include "SD_Library/FSDefs.h"
#include "SD_Library/FSconfig.h"
#include "SD_Library/FSIO.h"
#include "SD_Library/SD-SPI.h"

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
    
    /* Enable multi-vector interrupts */
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    
    char sendBuffer[] = "This is test string 1";
    char send2[] = "2";
    char receiveBuffer[50];
    
    while(1)
    {
        UART_Process();
        
        FSFILE * pointer;
        char path[30];
        char count = 30;
        char * pointer2;
        SearchRec rec;
        unsigned char attributes;
        unsigned char size = 0, i;

        // Turn on the interrupts
        mOSCSetPBDIV(OSC_PB_DIV_2);
        //Initialize the RTCC
        RtccInit();
        while(RtccGetClkStat()!=RTCC_CLK_ON);// wait for the SOSC to be actually running and RTCC to have its clock source
                                             // could wait here at most 32ms
        RtccOpen(0x10073000, 0x07011602, 0);

        while (!MDD_SDSPI_MediaDetect());

        // Initialize the library
        while (!FSInit());
    }

    return (0);
}

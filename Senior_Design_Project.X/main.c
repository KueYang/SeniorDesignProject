#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
//#include "ADC.h"
//#include "SPI.h"
//#include "I2C.h"
#include "UART.h"

/* Configuration Bit Settings:
 * SYSCLK = 8 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
 * PBCLK = 8 MHz (SYSCLK / FPBDIV)
 * Primary Osc w/PLL (XT+,HS+,EC+PLL)
 * WDT OFF */
#pragma config FPLLMUL = MUL_24
#pragma config FPLLIDIV = DIV_3
#pragma config FPLLODIV = DIV_8
#pragma config FNOSC = FRCPLL       // Oscillator Selection Bit, Internal Osc (FRC) with PLL
#pragma config POSCMOD = OFF        // Primary Oscillator Configuration, HS osc mode
#pragma config FPBDIV = DIV_1       // Peripheral Clock Divisor, Pb_Clk is Sys_Clk/1
#pragma config FSOSCEN = OFF        // Secondary Oscillator Disabled

#pragma config JTAGEN = OFF         // JTAG Disabled
#pragma config ICESEL = ICS_PGx1    // ICE/ICD Comm Channel Select
#pragma config PWP = OFF            // Program Flash Write Protect
#pragma config CP = OFF             // Code Protection Disabled
#pragma config FWDTEN = OFF         // Watchdog Timer Disabled

int main(int argc, char** argv) {
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    
    /*Initialization*/
    IO_Init();
//    TIMER_Init();
//    ADC_Init();
//    SPI_Init();
//    I2C_Init();
    UART_Init();
    
    /* Enable multi-vector interrupts */
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    
    while(1)
    {
        UART_Process();
    }

    return (0);
}

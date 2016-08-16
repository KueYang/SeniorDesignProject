#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
//#include "IO.h"
#include "TIMER.h"
//#include "ADC.h"
//#include "SPI.h"
//#include "I2C.h"
//#include "UART.h"

// Configuration Bit settings
// SYSCLK = 8 MHz (8MHz Crystal / FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 8 MHz (SYSCLK / FPBDIV)
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
#pragma config FPLLMUL = MUL_20
#pragma config FPLLIDIV = DIV_1
#pragma config FPLLODIV = DIV_1
#pragma config FNOSC = PRIPLL       // Oscillator Selection Bit, Primary Osc (FRC) with PLL
#pragma config POSCMOD = HS         // Primary Oscillator Configuration, HS osc mode
#pragma config FPBDIV = DIV_1       // Peripheral Clock Divisor, Pb_Clk is Sys_Clk/1
#pragma config FSOSCEN = OFF        // Secondary Oscillator Disabled

#pragma config JTAGEN = OFF         // JTAG Disabled
#pragma config ICESEL = ICS_PGx1    // ICE/ICD Comm Channel Select
#pragma config PWP = OFF            // Program Flash Write Protect
#pragma config CP = OFF             // Code Protection Disabled
#pragma config FWDTEN = OFF         // Watchdog Timer Disabled
        
#define SYS_FREQ (8000000L)

int main(int argc, char** argv) {
    
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    
    // Enable multi-vector interrupts
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
    /*Initialization*/
    IO_Init();
    TIMER_Init();
//    ADC_Init();
//    SPI_Init();
//    I2C_Init();
//    UART_Init();
    
    while(1){
//        TIMER_MSecondDelay(5);
//        mPORTAToggleBits(BIT_0);
//        TIMER_MSecondDelay(5);
//        mPORTAToggleBits(BIT_0);
    }

    return (0);
}


#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "TIMER.h"
#include "SPI.h"

void SPI2_Init(void);

void SPI_Init(void)
{
    SPI2_Init();
}

void SPI2_Init(void)
{
    // Re-mapped pins RPA4 and RPB8 pins to SDI2 and SDO2
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(3,SDI2,RPB2);     // Assign RPA4 as input pin for SDI
        PPSOutput(2,RPB1,SDO2);    // Set RPB8 pin as output for SDO
        PPSLock;
    });
    
    SPI2CONbits.ON = 0;         // Disables the SPI Module
    
    SPI2CONbits.FRMEN = 0;      // Framed SPI support disabled
    SPI2CONbits.FRMSYNC = 0;    // Frame sync pulse output (Master Mode) 
    SPI2CONbits.FRMPOL = 0;     // Frame Pulse active-low
    SPI2CONbits.FRMSYPW = 0;    // Frame sync pulse is one clock wide
    SPI2CONbits.FRMCNT = 0b000; // Generate frame sync pulse on every data character
    SPI2CONbits.SPIFE = 0;      // Frame sync pulse precedes the first bit clock
    
    SPI2CONbits.MSSEN = 0;      // Slave select SPI support disabled
    SPI2CONbits.MCLKSEL = 0;    // PBCLK is used by baud rate generator
    SPI2CONbits.ENHBUF = 0;     // Enhanced Buffer mode disabled
    SPI2CONbits.SIDL = 0;       // Continue module in idle mode
    SPI2CONbits.DISSDO = 0;     // SDO2 controlled by module
    SPI2CONbits.MODE16 = 0b00;  // 8-bit communication
    SPI2CONbits.SMP = 1;        // Input data sampled at end of data output
    SPI2CONbits.CKE = 0;        // Serial output data changes from idle to active clk
    SPI2CONbits.SSEN = 0;       // Slave select disable
    SPI2CONbits.CKP = 1;        // Clk is active low
    SPI2CONbits.MSTEN = 1;      // Master mode enabled
    SPI2CONbits.DISSDI = 0;     // SDI2 controlled by module
    SPI2CONbits.STXISEL = 0b00; // Transmit interrupts on last transfer
    SPI2CONbits.SRXISEL = 0b00; // Receive interrupts on last received
    
    SPI2BRG = 2;                // SPI clock speed at PBCLK/8
    SPI2STATbits.SPIROV = 0;    // Clears Receive overflow flag
    
    SPI2CONbits.ON = 1;         // Enable SPI Module
    
    //Enable SPI1 interrupt.
//    INTEnable(INT_SPI1, INT_ENABLED);
//    INTSetVectorPriority(INT_SPI_1_VECTOR, INT_PRIORITY_LEVEL_2);
//    INTSetVectorSubPriority(INT_SPI_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
}

void SPI_Process()
{
    
}

BYTE SPI2_ReadWrite(BYTE ch)
{
    BYTE dummy = 0;

    dummy = SPI2BUF;                //Clears flag to read/write to buffer
    SPI2BUF = ch;                   //Write BYTE to the buffer
    while (!SPI2STATbits.SPIRBF);   //Waits for transfer to be completed
    dummy = SPI2BUF;                //Read a dummy byte from buffer

    return dummy;
}

/* SPI2 ISR */
void __ISR(_SPI_2_VECTOR, IPL4AUTO) IntSpi2Handler(void) 
{
    INTClearFlag(INT_SPI2TX);
}



#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "./FIO_Library/HardwareProfile.h"
#include "TIMER.h"
#include "SPI.h"

#define SPI_BAUD_RATE       20000000
#define SPI_CHANNEL         SPI_CHANNEL1	// the SPI channel to use

void SPI_Init(void)
{
    // Re-mapped pins RPB11 and RPB13 pins to U1RX and U1TX
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(2,SDI1,RPB11);     // Assign RPB11 as input pin for SDI
        PPSOutput(3,RPB13,SDO1);    // Set RPB13 pin as output for SDO
        PPSLock;
    });
    
    SPI1CONbits.ON = 0;
    SPI1CONbits.FRMEN = 0;
    SPI1CONbits.FRMSYNC = 0;
    SPI1CONbits.FRMPOL = 0;
    SPI1CONbits.MSSEN = 1;
    SPI1CONbits.FRMSYPW = 0;
    SPI1CONbits.FRMCNT = 0b000;
    SPI1CONbits.MCLKSEL = 0;
    SPI1CONbits.SPIFE = 0;
    SPI1CONbits.ENHBUF = 0;
    SPI1CONbits.SIDL = 0;
    SPI1CONbits.DISSDO = 0;
    SPI1CONbits.MODE16 = 0;
    SPI1CONbits.SMP = 0;
    SPI1CONbits.CKE = 0;
    SPI1CONbits.SSEN = 0;
    SPI1CONbits.CKP = 0;
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.DISSDI = 0;
    SPI1CONbits.STXISEL = 0b00;
    SPI1CONbits.SRXISEL = 0b00;
    SPI1CONbits.ON = 1;

    //Enable SPI1 interrupt.
//    INTEnable(INT_SPI1, INT_ENABLED);
//    INTSetVectorPriority(INT_SPI_1_VECTOR, INT_PRIORITY_LEVEL_2);
//    INTSetVectorSubPriority(INT_SPI_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
}

void SPI_Process()
{
    PORTBbits.RB2 = 0;
    /* Write a dummy byte to the shift register. */
    BYTE data = SPI1_ReadWrite(0x40);
    data = SPI1_ReadWrite(0x00);
    data = SPI1_ReadWrite(0x00);
    data = SPI1_ReadWrite(0x00);
    data = SPI1_ReadWrite(0x00);
    data = SPI1_ReadWrite(0x95);
    data = SPI1_ReadWrite(0xFF);
    data = SPI1_ReadWrite(0xFF);
    PORTBbits.RB2 = 1;
//    TIMER_MSecondDelay(1000);
}

BYTE SPI1_ReadWrite(BYTE ch)
{
    BYTE dummy = 0;

    dummy = SPI1BUF;                //Clears flag to read/write to buffer
    SPI1BUF = ch;                   //Write BYTE to the buffer
    while (!SPI1STATbits.SPIRBF)    //Waits for transfer to be completed
    dummy = SPI1BUF;                //Read a dummy byte from buffer

    return dummy;
}

/* SPI1 ISR */
void __ISR(_SPI_1_VECTOR, IPL4AUTO) IntSpi1Handler(void) 
{
    
    INTClearFlag(INT_SPI1TX);
}



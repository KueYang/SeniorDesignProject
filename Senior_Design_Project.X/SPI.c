#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "TIMER.h"
#include "SPI.h"

#define SPI_BAUD_RATE   20000000

void SPI_Init(void)
{
    // Re-mapped pins RPB11 and RPB13 pins to U1RX and U1TX
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(2,SDI1,RPB11);     // Assign RPB11 as input pin for SDI
        PPSOutput(3,RPB13,SDO1);    // Set RPB13 pin as output for SDO
        PPSLock;
    });
    
    //Configure SPI in I2S mode with 24-bit stereo audio.
    UINT spi_con1 = 0, spi_con2 = 0;
    spi_con1 = SPI_OPEN_MSTEN |     // Master mode enable
                SPI_OPEN_SSEN |     // Enable slave select function
            SPI_OPEN_CKP_HIGH |     // Clock polarity Idle High Active Low
              SPI_OPEN_MODE16 |     // Data mode: 24b
              SPI_OPEN_MODE32 |     // Data mode: 24b
             SPI_OPEN_MCLKSEL |     // Clock selected is reference clock
             SPI_OPEN_FSP_HIGH;     // Frame Sync Pulse is active high

//    spi_con2 = SPI_OPEN2_AUDEN |    // Enable Audio mode
//            SPI_OPEN2_AUDMOD_I2S;   // Enable I2S mode

    //Configure and turn on the SPI1 module.
    SpiChnOpenEx(SPI_CHANNEL1, spi_con1, spi_con2, (GetPeripheralClock()/SPI_BAUD_RATE));

    //Enable SPI1 interrupt.
//    INTEnable(INT_SPI1, INT_ENABLED);
//    INTSetVectorPriority(INT_SPI_1_VECTOR, INT_PRIORITY_LEVEL_4);
//    INTSetVectorSubPriority(INT_SPI_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    
    SpiChnPutC(SPI_CHANNEL1, 0); //Dummy write to start the SPI
}

void SPI_Process()
{
    /* Write a dummy byte to the shift register. */
    BYTE data = SPI1_ReadWrite(0x00);
    TIMER_MSecondDelay(50);
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



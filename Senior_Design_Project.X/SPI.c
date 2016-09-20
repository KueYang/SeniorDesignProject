#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "SPI.h"

// Definitions.
#define SAMPLE_RATE 8000    //8(kHz)
#define FRAME_SIZE 	(SAMPLE_RATE/1000)
#define RODIV		3
#define REFTRIM		464

typedef struct audio {
    UINT64 audioWord;
}AudioMono;

AudioMono* txBuffer;

void SPI_Init(void)
{
    UINT spi_con1 = 0, spi_con2 = 0;
    UINT baud_rate = SAMPLE_RATE * 64;
    UINT mclk = SAMPLE_RATE * 256;
    
    mOSCREFOTRIMSet(REFTRIM);
    OSCREFConfig(OSC_REFOCON_USBPLL,            // USB-PLL clock output used as REFCLKO source
            OSC_REFOCON_OE | OSC_REFOCON_ON,    // Enable and turn on the REFCLKO
            RODIV);

    //Configure SPI in I2S mode with 24-bit stereo audio.
    spi_con1 = SPI_OPEN_MSTEN |     // Master mode enable
                SPI_OPEN_SSEN |     // Enable slave select function
            SPI_OPEN_CKP_HIGH |     // Clock polarity Idle High Actie Low
              SPI_OPEN_MODE16 |     // Data mode: 24b
              SPI_OPEN_MODE32 |     // Data mode: 24b
             SPI_OPEN_MCLKSEL |     // Clock selected is reference clock
             SPI_OPEN_FSP_HIGH;     // Frame Sync Pulse is active high

    spi_con2 = SPI_OPEN2_AUDEN |    // Enable Audio mode
            SPI_OPEN2_AUDMOD_I2S;   // Enable I2S mode

    //Configure and turn on the SPI1 module.
    SpiChnOpenEx(SPI_CHANNEL1, spi_con1, spi_con2, (mclk / baud_rate));

    //Enable SPI1 interrupt.
    INTEnable(INT_SPI1, INT_ENABLED);
    INTSetVectorPriority(INT_SPI_1_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_SPI_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    
    SpiChnPutC(SPI_CHANNEL1, 0); //Dummy write to start the SPI
}

void SPI_Process()
{
    
}

BYTE SPI1_ReadWrtie(BYTE ch)
{
    BYTE dummy = 0;

    dummy = SPI1BUF;                //Clears flag to read/wrtie to buffer
    SPI1BUF = ch;                   //Write BYTE to the buffer
    while (!SPI1STATbits.SPIRBF)    //Waits for transfer to be completed
    dummy = SPI1BUF;                //Read a dummy byte from buffer

    return dummy;
}

/* SPI1 ISR */
void __ISR(_SPI_1_VECTOR, IPL4AUTO) IntSpi1Handler(void) 
{
    SpiChnPutC(SPI_CHANNEL1, 0);
    
    INTClearFlag(INT_SPI1TX);
}



#include <p32xxxx.h>
#include "STDDEF.h"
#include "SPI.h"


void SPI_Init(void)
{
    
}

void SPI_Process(){
    
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




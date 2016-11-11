#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "SPI.h"
#include "DAC.h"

void DAC_Init(void)
{
    LDAC = 0;
    SYNC = 1;
    DAC_WriteToDAC(POWER_ON_OFF, 0x0000, DAC_A | POWER_ON_NORMAL);
}

BOOL DAC_WriteToDAC(BYTE cmd, BYTE addr, WORD data)
{
   SYNC = 0;    // Shifts the latch low to initiate write
   
   SPI2_ReadWrite(cmd);
   SPI2_ReadWrite(addr);
   SPI2_ReadWrite(data); //Sends the first 12 bits, ignore last 4 bits
   
   SYNC = 1;    // Shifts the latch high to end write
}




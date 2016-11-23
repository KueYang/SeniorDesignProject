/**
 * @file DAC.c
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The DAC module.
 */

#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "SPI.h"
#include "DAC.h"

/**
 * @brief Initializes the DAC.
 * @remark Requires the SPI module to be initialized.
 * @return Void.
 */
void DAC_Init(void)
{
    SYNC = 1;   // Sets the latch high.
    DAC_WriteToDAC(POWER_ON_OFF_DAC_A , 0x000F & DAC_A);    // Turns on DAC A
}

/**
 * @brief Writes data to the DAC.
 * @remark Requires the DAC to be initialized. 
 * @return Returns a boolean indicating if writing to the DAC is successful.
 * @retval TRUE if the file was read successfully
 * @retval FALSE if the file was read unsuccessfully
 */
BOOL DAC_WriteToDAC(BYTE cmd_addr, WORD data)
{
   SYNC = 0;    // Shifts the latch low to initiate write
   
   SPI2_ReadWrite(cmd_addr);
   SPI2_ReadWrite(data << 4);       // Sends the first 8 bits
   SPI2_ReadWrite(data << 8);       // Sends the last 4 bits
   
   SYNC = 1;    // Shifts the latch high to end write
}




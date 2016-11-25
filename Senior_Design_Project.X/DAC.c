/**
 * @file DAC.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The DAC module handles all DAC related operations such as 
 * configuring and writing to the DAC. DAC configurations are defined in the
 * header file. 
 */

#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "SPI.h"
#include "DAC.h"

/**
 * @brief Initializes the DAC.
 * @details Powers on the DAC with only one channel open.
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
 * @details The current DAC can only handle 12-bit data. Therefore, the data that
 * is transmitted are the first 12 MSB.
 * @arg cmd_addr The command and channel of the DAC.
 * @arg data The data that will be written to the DAC.
 * @remark Requires SPI and the DAC to be initialized. 
 * @return Returns a boolean indicating if writing to the DAC is successful.
 * @retval TRUE If the file was read successfully
 * @retval FALSE If the file was read unsuccessfully
 */
BOOL DAC_WriteToDAC(BYTE cmd_addr, WORD data)
{
   SYNC = 0;    // Shifts the latch low to initiate write
   
   SPI2_ReadWrite(cmd_addr);
   SPI2_ReadWrite(data << 4);       // Sends the first 8 bits
   SPI2_ReadWrite(data << 8);       // Sends the last 4 bits
   
   SYNC = 1;    // Shifts the latch high to end write
}




/**
 * @file DAC.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The DAC module handles all DAC related operations such as 
 * configuring and writing to the DAC. DAC configurations are defined in the
 * header file. 
 */

#include <p32xxxx.h>
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
    SYNC = 1;       // Sets the latch high.
    DAC_WriteToDAC(POWER_ON_OFF_CHN_A_B , POWER_ON_DAC_B_A);
    DAC_ZeroOutput();
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
DWORD DAC_WriteToDAC(BYTE cmd_addr, WORD data)
{
    DWORD readBack = 0;
    
    SYNC = 0;    // Shifts the latch low to initiate write
   
    readBack |= SPI2_ReadWrite(cmd_addr) << 16;              // Sends the address BYTES
    readBack |= SPI2_ReadWrite((data & 0xFF00)>>8) << 8;     // Sends the first 2 MSB
    readBack |= SPI2_ReadWrite(data & 0x00FF);               // Sends the last 2 LSB
   
    SYNC = 1;    // Shifts the latch high to end write
    
    return readBack;
}

/**
 * @brief Sets the DAC output to mid-scale.
 * @remark Requires SPI and the DAC to be initialized. 
 * @return Void
 */
void DAC_Zero(void)
{
    DAC_WriteToDAC(WRITE_UPDATE_CHN_A, AC_ZERO);
    DAC_WriteToDAC(WRITE_UPDATE_CHN_B, AC_ZERO);
}

/**
 * @brief Sets the DAC output to zero.
 * @remark Requires SPI and the DAC to be initialized. 
 * @return Void
 */
void DAC_ZeroOutput(void)
{
    DAC_WriteToDAC(WRITE_UPDATE_CHN_A, 0);
    DAC_WriteToDAC(WRITE_UPDATE_CHN_B, 0);
}

/**
 * @brief Reads the DAC registers.
 * @arg channelA The channel to read back data.
 * @return The DAC register value.
 */
DWORD DAC_ReadBack(BOOL channelA)
{
    if(channelA)
    { 
        DAC_WriteToDAC(READ_CHN_A, 0x0000);
    }
    else
    {
        DAC_WriteToDAC(READ_CHN_B, 0x0000);
    }
    return DAC_WriteToDAC(0x00, 0x0000);
}
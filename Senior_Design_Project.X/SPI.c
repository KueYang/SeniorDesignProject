/**
 * @file SPI.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The SPI module will handle all SPI related tasks. The SPI module is
 * used by the DAC module to write to the DAC. 
 */

#include <p32xxxx.h>
#include "plib/plib.h"
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "SPI.h"

/**  
 * @privatesection
 * @{
 */
UINT16 SPI_GetBaudRate(int clk);
void SPI2_Init(void);
/** @}*/

/**
 * @brief Initializes the SPI modules.
 * @return Void.
 */
void SPI_Init(void)
{
    SPI2_Init();
}

/**
 * @brief Initializes the SPI2 module.
 * @return Void.
 */
void SPI2_Init(void)
{
    // Re-mapped pins RPB2 and RPB1 pins to SDI2 and SDO2
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(3,SDI2,RPB2);     // Assign RPB2 as input pin for SDI
        PPSOutput(2,RPB1,SDO2);    // Set RPB1 pin as output for SDO
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
    SPI2CONbits.MODE16 = 0b00;  // 16-bit communication
    SPI2CONbits.SMP = 1;        // Input data sampled at end of data output
    SPI2CONbits.CKE = 0;        // Serial output data changes from idle to active clk
    SPI2CONbits.SSEN = 0;       // Slave select disable
    SPI2CONbits.CKP = 1;        // Clk is active low
    SPI2CONbits.MSTEN = 0;      // Master mode disabled
    SPI2CONbits.DISSDI = 0;     // SDI2 controlled by module
    SPI2CONbits.STXISEL = 0b00; // Transmit interrupts on last transfer
    SPI2CONbits.SRXISEL = 0b00; // Receive interrupts on last received
    
    SPI2CON2bits.AUDEN = 0;                 // Audio CODEC support disabled
    SPI2CON2bits.AUDMOD = 0b00;             // I2S mode
    SPI2CON2bits.AUDMONO = 0;               // Audio Data is stereo
    SPI2CON2bits.FRMERREN = 0;              // Interrupt Events via FRMERR disabled
    SPI2CON2bits.IGNROV = 0;                // Receive overflow is critical, stops SPI
    SPI2CON2bits.IGNTUR = 0;                // Transmit underrun is critical, stops SPI
    SPI2CON2bits.SPIROVEN = 0;              // Receive overflow doesn't trigger error event.
    SPI2CON2bits.SPITUREN = 0;              // Transmit underrun doesn't trigger error event.
    SPI2CON2bits.SPISGNEXT = 0;             // RX Data is not signed-extended
    
    SPI2BRG = SPI_GetBaudRate(8000000);    // SPI clock speed at 8 MHz
    SPI2STATbits.SPIROV = 0;                // Clears Receive overflow flag
    
    SPI2CONbits.ON = 1;                     // Enable SPI Module
}

/**
 * @brief Reads and write data to SPI buffer.
 * @arg ch The data to be written to SPI buffer.
 * @return Returns the data read from the SPI buffer.
 */
BYTE SPI2_ReadWrite(BYTE ch)
{
    BYTE dummy = 0;

    dummy = SPI2BUF;                //Clears flag to read/write to buffer
    SPI2BUF = ch;                   //Write BYTE to the buffer
    while (!SPI2STATbits.SPIRBF);   //Waits for transfer to be completed
    dummy = SPI2BUF;                //Read a dummy byte from buffer

    return dummy;
}

/**
 * @brief Initializes the SPI1 module.
 * @arg clk The clock rate to set the SPI module to.
 * @return Void.
 */
void SPI1_Init(int clk)
{
    // Re-mapped pins RPB11 and RPB13 pins to SDI1 and SDO1
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(2,SDI1,RPB11);     // Assign RPB11 as input pin for SDI
        PPSOutput(3,RPB13,SDO1);    // Set RPB13 pin as output for SDO
        PPSLock;
    });
    
    SPI1CONbits.ON = 0;         // Disable SPI Module
    SPI1CONbits.MSSEN = 0;      // Slave select SPI support disabled
    SPI1CONbits.MCLKSEL = 0;    // PBCLK is used by baud rate generator
    SPI1CONbits.ENHBUF = 0;     // Enhanced Buffer mode disabled
    SPI1CONbits.SIDL = 0;       // Continue module in idle mode
    SPI1CONbits.DISSDO = 0;     // SDO1 controlled by module
    SPI1CONbits.MODE16 = 0b00;  // 8-bit communication
    SPI1CONbits.SMP = 1;        // Input data sampled at end of data output
    SPI1CONbits.CKE = 1;        // Serial output data changes from active to idle clk
    SPI1CONbits.SSEN = 0;       // Slave select disable
    SPI1CONbits.CKP = 0;        // Clk is idle low, active high
    SPI1CONbits.MSTEN = 1;      // Master mode enabled
    SPI1CONbits.DISSDI = 0;     // SDI1 controlled by module
    
    SPI1BRG = SPI_GetBaudRate(clk); // SPI clock config
    SPI1STATbits.SPIROV = 0;        // Clears Receive overflow flag
    
    SPI1CONbits.ON = 1;             // Enable SPI Module
}

/**
 * @brief Returns the SPIBRG value corresponding to the specified baud rate.
 * @arg clk The clock rate to set.
 * @return Returns the SPIBRG value corresponding to the specified clk value.
 */
UINT16 SPI_GetBaudRate(int clk)
{
    return GetPeripheralClock()/(2*clk) - 1;
}

/**
 * @brief Reads and write data to SPI buffer.
 * @arg ch The data to be written to SPI buffer.
 * @return Returns the data read from the SPI buffer.
 */
BYTE SPI1_ReadWrite(BYTE ch)
{
    BYTE dummy = 0;

    dummy = SPI1BUF;                //Clears flag to read/write to buffer
    SPI1BUF = ch;                   //Write BYTE to the buffer
    while (!SPI1STATbits.SPIRBF);   //Waits for transfer to be completed
    dummy = SPI1BUF;                //Read a dummy byte from buffer

    return dummy;
}

/**
 * @brief Writes multiple data blocks to SPI buffer.
 * @arg buff The data buffer to send.
 * @arg cnt The number of bytes to write.
 * @return Void
 */
void SPI1_MultiWrite (const BYTE* buff,	UINT16 cnt )
{
	do {
		SPI1BUF = *buff++; while(!SPI1STATbits.SPIRBF) ; SPI1BUF; 
		SPI1BUF = *buff++; while(!SPI1STATbits.SPIRBF) ; SPI1BUF; 
	} while (cnt -= 2);
}

/**
 * @brief Reads multiple data blocks to SPI buffer.
 * @arg buff The data buffer to store the received values.
 * @arg cnt The number of bytes to receive.
 * @return Void
 */
void SPI1_MultiRead ( BYTE* buff, UINT16 cnt)
{
	do {
		SPI1BUF = 0xFF; while(!SPI1STATbits.SPIRBF) ; *buff++ = SPI1BUF; 
		SPI1BUF = 0xFF; while(!SPI1STATbits.SPIRBF) ; *buff++ = SPI1BUF; 
	} while (cnt -= 2);
}


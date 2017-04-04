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
void SPI1_Init(void);
void SPI2_Init(void);
/** @}*/

/**
 * @brief Initializes the SPI modules.
 * @return Void.
 */
void SPI_Init(void)
{
//    SPI1_Init();
    SPI2_Init();
}

void SPI1_Init(void)
{
    // Re-mapped pins RPG7 and RPG8 pins to SDI2 and SDO2
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(2,SDI1,RPD11);     // Assign RPG7 as input pin for SDI
        PPSOutput(2,RPC13,SDO1);    // Set RPG8 pin as output for SDO
        PPSLock;
    });
    
    SPI1CONbits.ON = 0;         // Disables the SPI Module
    
    SPI1CONbits.FRMEN = 0;      // Framed SPI support disabled
    SPI1CONbits.FRMSYNC = 0;    // Frame sync pulse output (Master Mode) 
    SPI1CONbits.FRMPOL = 0;     // Frame Pulse active-low
    SPI1CONbits.FRMSYPW = 0;    // Frame sync pulse is one clock wide
    SPI1CONbits.FRMCNT = 0b000; // Generate frame sync pulse on every data character
    SPI1CONbits.SPIFE = 0;      // Frame sync pulse precedes the first bit clock
    
    SPI1CONbits.MSSEN = 0;      // Slave select SPI support disabled
    SPI1CONbits.MCLKSEL = 0;    // PBCLK is used by baud rate generator
    SPI1CONbits.ENHBUF = 0;     // Enhanced Buffer mode disabled
    SPI1CONbits.SIDL = 0;       // Continue module in idle mode
    SPI1CONbits.DISSDO = 0;     // SDO2 controlled by module
    SPI1CONbits.MODE16 = 0b00;  // 16-bit communication
    SPI1CONbits.SMP = 1;        // Input data sampled at end of data output
    SPI1CONbits.CKE = 0;        // Serial output data changes from idle to active clk
    SPI1CONbits.SSEN = 0;       // Slave select disable
    SPI1CONbits.CKP = 1;        // Clk is active low
    SPI1CONbits.MSTEN = 1;      // Master mode enabled
    SPI1CONbits.DISSDI = 0;     // SDI2 controlled by module
    SPI1CONbits.STXISEL = 0b00; // Transmit interrupts on last transfer
    SPI1CONbits.SRXISEL = 0b00; // Receive interrupts on last received
    
    SPI1CON2bits.AUDEN = 0;                 // Audio CODEC support disabled
    SPI1CON2bits.AUDMOD = 0b00;             // I2S mode
    SPI1CON2bits.AUDMONO = 0;               // Audio Data is stereo
    SPI1CON2bits.FRMERREN = 0;              // Interrupt Events via FRMERR disabled
    SPI1CON2bits.IGNROV = 0;                // Receive overflow is critical, stops SPI
    SPI1CON2bits.IGNTUR = 0;                // Transmit underrun is critical, stops SPI
    SPI1CON2bits.SPIROVEN = 0;              // Receive overflow doesn't trigger error event.
    SPI1CON2bits.SPITUREN = 0;              // Transmit underrun doesn't trigger error event.
    SPI1CON2bits.SPISGNEXT = 0;             // RX Data is not signed-extended
    
    SPI1BRG = SPI_GetBaudRate(8000000);     // SPI clock speed at 8 MHz
    SPI1STATbits.SPIROV = 0;                // Clears Receive overflow flag
    
    SPI1CONbits.ON = 1;                     // Enable SPI Module
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
 * @brief Initializes the SPI2 module.
 * @return Void.
 */
void SPI2_Init(void)
{
    // Re-mapped pins RPG7 and RPG8 pins to SDI2 and SDO2
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(2,SDI2,RPG7);     // Assign RPG7 as input pin for SDI
        PPSOutput(1,RPG8,SDO2);    // Set RPG8 pin as output for SDO
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
    SPI2CONbits.MSTEN = 1;      // Master mode enabled
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
    
    SPI2STATbits.SPIROV = 0;                // Clears Receive overflow flag
    SPI2BRG = SPI_GetBaudRate(10000000);    // SPI clock speed at 8 MHz
    
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
 * @brief Initializes the SPI3 module.
 * @arg clk The clock rate to set the SPI module to.
 * @return Void.
 */
void SPI3_Init(int clk)
{
    // Re-mapped pins RPB9 and RPB10 pins to SDI3 and SDO3
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(1,SDI3,RPB9);      // Assign RPB9 as input pin for SDI
        PPSOutput(1,RPB10,SDO3);    // Set RPB10 pin as output for SDO
        PPSLock;
    });
    
    SPI3CONbits.ON = 0;         // Disable SPI Module
    SPI3CONbits.MSSEN = 0;      // Slave select SPI support disabled
    SPI3CONbits.MCLKSEL = 0;    // PBCLK is used by baud rate generator
    SPI3CONbits.ENHBUF = 0;     // Enhanced Buffer mode disabled
    SPI3CONbits.SIDL = 0;       // Continue module in idle mode
    SPI3CONbits.DISSDO = 0;     // SDO3 controlled by module
    SPI3CONbits.MODE16 = 0b00;  // 8-bit communication
    SPI3CONbits.SMP = 1;        // Input data sampled at end of data output
    SPI3CONbits.CKE = 1;        // Serial output data changes from active to idle clk
    SPI3CONbits.SSEN = 0;       // Slave select disable
    SPI3CONbits.CKP = 0;        // Clk is idle low, active high
    SPI3CONbits.MSTEN = 1;      // Master mode enabled
    SPI3CONbits.DISSDI = 0;     // SDI3 controlled by module
    
    SPI3BRG = SPI_GetBaudRate(clk); // SPI clock config
    SPI3STATbits.SPIROV = 0;        // Clears Receive overflow flag
    
    SPI3CONbits.ON = 1;             // Enable SPI Module
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
BYTE SPI3_ReadWrite(BYTE ch)
{
    BYTE dummy = 0;

    dummy = SPI3BUF;                //Clears flag to read/write to buffer
    SPI3BUF = ch;                   //Write BYTE to the buffer
    while (!SPI3STATbits.SPIRBF);   //Waits for transfer to be completed
    dummy = SPI3BUF;                //Read a dummy byte from buffer

    return dummy;
}

/**
 * @brief Writes multiple data blocks to SPI buffer.
 * @arg buff The data buffer to send.
 * @arg cnt The number of bytes to write.
 * @return Void
 */
void SPI3_MultiWrite (const BYTE* buff,	UINT16 cnt )
{
	do 
    {
		SPI3BUF = *buff++; while(!SPI3STATbits.SPIRBF) ; SPI3BUF; 
		SPI3BUF = *buff++; while(!SPI3STATbits.SPIRBF) ; SPI3BUF; 
	} while (cnt -= 2);
}

/**
 * @brief Reads multiple data blocks to SPI buffer.
 * @arg buff The data buffer to store the received values.
 * @arg cnt The number of bytes to receive.
 * @return Void
 */
void SPI3_MultiRead ( BYTE* buff, UINT16 cnt)
{
	do 
    {
		SPI3BUF = 0xFF; while(!SPI3STATbits.SPIRBF) ; *buff++ = SPI3BUF; 
		SPI3BUF = 0xFF; while(!SPI3STATbits.SPIRBF) ; *buff++ = SPI3BUF; 
	} while (cnt -= 2);
}


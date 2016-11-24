/**
 * @file Audio.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Defines System Configurations.
 */

#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

// System Clock Configurations
#define SYS_FREQ                (40000000L)    // 40MHz
#define	GetPeripheralClock()    (SYS_FREQ/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock()	(SYS_FREQ)

// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI

/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

/* SD Card definitions: Change these to fit your application when using
   an SD-card-based physical layer                                   */
#ifdef USE_SD_INTERFACE_WITH_SPI

// Registers for the SPI module you want to use
#define MDD_USE_SPI_1
//#define MDD_USE_SPI_2

//SPI Configuration
#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
#define SPI_START_CFG_2     (SPI_ENABLE)

// Define the SPI frequency
#define SPI_FREQUENCY			(20000000)

// Description: SD-SPI Chip Select Output bit
#define SD_CS               LATBbits.LATB10
// Description: SD-SPI Chip Select TRIS bit
#define SD_CS_TRIS          TRISBbits.TRISB10

// Description: SD-SPI Card Detect Input bit
#define SD_CD               PORTBbits.RB9
// Description: SD-SPI Card Detect TRIS bit
#define SD_CD_TRIS          TRISBbits.TRISB9

// Description: SD-SPI Write Protect Check Input bit
#define SD_WE               PORTBbits.RB8
// Description: SD-SPI Write Protect Check TRIS bit
#define SD_WE_TRIS          TRISBbits.TRISB8

// Description: The main SPI control register
#define SPICON1             SPI1CON
// Description: The SPI status register
#define SPISTAT             SPI1STAT
// Description: The SPI Buffer
#define SPIBUF              SPI1BUF
// Description: The receive buffer full bit in the SPI status register
#define SPISTAT_RBF         SPI1STATbits.SPIRBF
// Description: The bitwise define for the SPI control register (i.e. _____bits)
#define SPICON1bits         SPI1CONbits
// Description: The bitwise define for the SPI status register (i.e. _____bits)
#define SPISTATbits         SPI1STATbits
// Description: The enable bit for the SPI module
#define SPIENABLE           SPICON1bits.ON
// Description: The definition for the SPI baud rate generator register (PIC32)
#define SPIBRG			    SPI1BRG

// Tris pins for SCK/SDI/SDO lines
// Description: The TRIS bit for the SCK pin
#define SPICLOCK            TRISBbits.TRISB14
// Description: The TRIS bit for the SDI pin
#define SPIIN               TRISBbits.TRISB11
// Description: The TRIS bit for the SDO pin
#define SPIOUT              TRISBbits.TRISB13

//SPI library functions
#define putcSPI             putcSPI1
#define getcSPI             getcSPI1
#define OpenSPI(config1, config2)   OpenSPI1(config1, config2)

#endif

#endif
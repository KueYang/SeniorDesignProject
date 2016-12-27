/**
 * @file SPI_Hardware.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Defines the SPI Configurations used with the MDD library.
 */

#ifndef SPI_HARDWARE_H
#define	SPI_HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif

/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

/**@def MDD_USE_SPI_1 Defines the SPI module that is used. */
#define MDD_USE_SPI_1

/**@def SPI_START_CFG_1 Defines the SPI configurations (e.g. Master mode). */
#define SPI_START_CFG_1     (PRI_PRESCAL_64_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON)
/**@def SPI_START_CFG_2 Defines the SPI enable configuration. */
#define SPI_START_CFG_2     (SPI_ENABLE)

/**@def SPI_FREQUENCY Defines the SPI Clock Speed. */
#define SPI_FREQUENCY		(20000000)

/**@def SD_CS SD Chip Select Output bit. */
#define SD_CS               LATBbits.LATB10
/**@def SD_CS_TRIS SD Chip Select TRIS bit. */
#define SD_CS_TRIS          TRISBbits.TRISB10

/**@def SD_CS_TRIS Description: SD-SPI Card Detect Input bit */
#define SD_CD               PORTBbits.RB9
/**@def SD_CS_TRIS Description: SD-SPI Card Detect TRIS bit */
#define SD_CD_TRIS          TRISBbits.TRISB9

/**@def SD_CS_TRIS Description: SD-SPI Write Protect Check Input bit */
#define SD_WE               PORTBbits.RB8
/**@def SD_CS_TRIS Description: SD-SPI Write Protect Check TRIS bit */
#define SD_WE_TRIS          TRISBbits.TRISB8

/**@def SPICON1 Defines the main SPI control register. */
#define SPICON1             SPI1CON
/**@def SPISTAT Defines the SPI status register. */
#define SPISTAT             SPI1STAT
/**@def SPIBUF Defines the SPI buffer. */
#define SPIBUF              SPI1BUF
/**@def SPISTAT_RBF Defines the receive buffer full bit in the SPI status register. */
#define SPISTAT_RBF         SPI1STATbits.SPIRBF
/**@def SPICON1bits Defines the bitwise define for the SPI control register (i.e. _____bits). */
#define SPICON1bits         SPI1CONbits
/**@def SPISTATbits Defines the bitwise define for the SPI status register (i.e. _____bits). */
#define SPISTATbits         SPI1STATbits
/**@def SPIENABLE Defines the enable bit for the SPI module. */
#define SPIENABLE           SPICON1bits.ON
/**@def SPIBRG Defines the definition for the SPI baud rate generator register (PIC32). */
#define SPIBRG			    SPI1BRG

// Tris pins for SCK/SDI/SDO lines
/**@def SPICLOCK Defines the TRIS bit for the SCK pin. */
#define SPICLOCK            TRISBbits.TRISB14
/**@def SPIIN Defines the TRIS bit for the SDI pin. */
#define SPIIN               TRISBbits.TRISB11
/**@def SPIOUT Defines the TRIS bit for the SDO pin. */
#define SPIOUT              TRISBbits.TRISB13

/**@def putcSPI Defines the function used to transmit data. */
#define putcSPI             putcSPI1
/**@def getcSPI Defines the function used to read data. */
#define getcSPI             getcSPI1
/**@def OpenSPI(config1, config2) Defines the function used to configure the SPI module. */
#define OpenSPI(config1, config2)   OpenSPI1(config1, config2)


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_HARDWARE_H */


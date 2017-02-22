/**
 * @file SPI.h
 * @author Kue Yang
 * @date 11/22/2016 
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif
    
void SPI_Init(void);
BYTE SPI2_ReadWrite(BYTE);

/**@def SD_CS SD Chip Select Output bit. */
#define SD_CS               LATBbits.LATB7
/**@def SD_CS_TRIS SD Chip Select TRIS bit. */
#define SD_CS_TRIS          TRISBbits.TRISB7

/**@def SD_CS_TRIS Description: SD-SPI Card Detect Input bit */
#define SD_CD               PORTBbits.RB8
/**@def SD_CS_TRIS Description: SD-SPI Card Detect TRIS bit */
#define SD_CD_TRIS          TRISBbits.TRISB8

void SPI1_Init(int clk);
BYTE SPI1_ReadWrite(BYTE ch);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */


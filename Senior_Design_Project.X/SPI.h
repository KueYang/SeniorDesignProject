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
BYTE SPI1_ReadWrite(BYTE);
BYTE SPI2_ReadWrite(BYTE);
void SPI3_Init(int clk);
BYTE SPI3_ReadWrite(BYTE ch);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */


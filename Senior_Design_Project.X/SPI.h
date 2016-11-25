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
void SPI_Process();
BYTE SPI2_ReadWrite(BYTE);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */


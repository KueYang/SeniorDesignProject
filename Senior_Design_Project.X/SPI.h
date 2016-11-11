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


#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

void SPI_Init(void);
void SPI_Process();
BYTE SPI1_ReadWrtie(BYTE);


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */


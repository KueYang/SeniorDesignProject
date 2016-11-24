/**
 * @file DAC.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Header for the DAC Module. 
 */

#ifndef DAC_H
#define	DAC_H

#ifdef	__cplusplus
extern "C" {
#endif

/**  
 * @defgroup WAV File Header Definitions and Error Codes.
 * @{
 */
/* DAC Pin Outs */
#define LDAC            PORTAbits.RA3
#define CLR             PORTBbits.RB4
#define SYNC            PORTAbits.RA4

/* DAC Selection Bits */
#define DAC_A                           0b01
#define DAC_B                           0b10

/* DAC A Configuration Bits*/
#define POWER_ON_OFF_DAC_A              0x20
#define WRITE_INPUT_REG_DAC_A           0x00
#define UPDATE_REG_DAC_A                0x07
#define WRITE_UPDATE_DAC_A              0x10

/* DAC B Configuration Bits*/
#define POWER_ON_OFF_DAC_B              0x21
#define WRITE_INPUT_REG_DAC_B           0x01
#define UPDATE_REG_DAC_B                0x08
#define WRITE_UPDATE_DAC_B              0x11

/* DAC A and DAC B Configuration Bits */
#define POWER_ON_OFF_DAC_A_B            0x27
#define WRITE_INPUT_REG_DAC_A_B         0x07
#define UPDATE_REG_DAC_A_B              0x0E
#define WRITE_UPDATE_DAC_A_B            0x17
/** @} */

void DAC_Init(void);
BOOL DAC_WriteToDAC(BYTE cmd_addr, WORD data);

#ifdef	__cplusplus
}
#endif

#endif	/* DAC_H */


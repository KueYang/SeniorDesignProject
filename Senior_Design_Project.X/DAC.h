/**
 * @file DAC.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef DAC_H
#define	DAC_H

#ifdef	__cplusplus
extern "C" {
#endif

/** @def LDAC 
 * Defines the IO port used for updating the DAC. */
#define LDAC                            PORTAbits.RA3
/** @def CLR 
 * Defines the IO port used for clearing data on the DAC. */
#define CLR                             PORTBbits.RB4
/** @def SYNC 
 * Defines the IO port used to write data to DAC. */
#define SYNC                            PORTAbits.RA4

/** @def DAC_A 
 * Defines the selection bit for channel A on the DAC. */
#define DAC_A                           0b01
/** @def DAC_B 
 * Defines the selection bit for channel B on the DAC. */
#define DAC_B                           0b10

/** @def POWER_ON_OFF_DAC_A 
 * Defines the command to turn on DAC and channel A. */
#define POWER_ON_OFF_DAC_A              0x20
/** @def WRITE_INPUT_REG_DAC_A 
 * Defines the command to write to channel A. */
#define WRITE_INPUT_REG_DAC_A           0x00
/** @def UPDATE_REG_DAC_A 
 * Defines the command to update the registers for channel A. */
#define UPDATE_REG_DAC_A                0x07
/** @def WRITE_UPDATE_DAC_A 
 * Defines the command to write and update channel A. */
#define WRITE_UPDATE_DAC_A              0x10

/** @def POWER_ON_OFF_DAC_B 
 * Defines the command to turn on DAC and channel B. */
#define POWER_ON_OFF_DAC_B              0x21
/** @def WRITE_INPUT_REG_DAC_B 
 * Defines the command to write to channel B. */
#define WRITE_INPUT_REG_DAC_B           0x01
/** @def UPDATE_REG_DAC_B 
 * Defines the command to update the registers for channel B. */
#define UPDATE_REG_DAC_B                0x08
/** @def WRITE_UPDATE_DAC_B 
 * Defines the command to write and update channel B. */
#define WRITE_UPDATE_DAC_B              0x11

/** @def POWER_ON_OFF_DAC_A_B 
 * Defines the command to turn on DAC and both channel A and channel B. */
#define POWER_ON_OFF_DAC_A_B            0x27
/** @def WRITE_INPUT_REG_DAC_A_B 
 * Defines the command to write to both channel A and channel B. */
#define WRITE_INPUT_REG_DAC_A_B         0x07
/** @def UPDATE_REG_DAC_A_B 
 * Defines the command to update the registers for both channel A and channel B. */
#define UPDATE_REG_DAC_A_B              0x0E
/** @def WRITE_UPDATE_DAC_A_B 
 * Defines the command to write and update both channel A and channel B. */
#define WRITE_UPDATE_DAC_A_B            0x17
    
void DAC_Init(void);
BOOL DAC_WriteToDAC(BYTE cmd_addr, WORD data);

#ifdef	__cplusplus
}
#endif

#endif	/* DAC_H */


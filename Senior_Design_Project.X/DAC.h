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
    
/**@def DAC12B
 * Selects the DAC to be used. Defaults to the AD5689R */
#define DAC12B

#define AC_ZERO                         32766
    
/** @def LDAC 
 * Defines the IO port used for updating the DAC. */
#define LDAC                            PORTAbits.RA3
/** @def CLR 
 * Defines the IO port used for clearing data on the DAC. */
#define CLR                             PORTBbits.RB4
/** @def SYNC 
 * Defines the IO port used to write data to DAC. */
#define SYNC                            PORTAbits.RA4
    
#ifdef  DAC12B
/** @def DAC_A 
 * Defines the selection bit for channel A on the DAC. */
#define DAC_A                           0b01
/** @def DAC_B 
 * Defines the selection bit for channel B on the DAC. */
#define DAC_B                           0b10
/** @def DAC_B_A 
 * Defines the selection bit for channel B on the DAC. */
#define DAC_B_A                         DAC_A | DAC_B

/** @def POWER_ON_OFF_CHN_A 
 * Defines the command to turn on DAC and channel A. */
#define POWER_ON_OFF_CHN_A              0x20
/** @def WRITE_INPUT_CHN_A 
 * Defines the command to write to channel A. */
#define WRITE_INPUT_CHN_A               0x00
/** @def UPDATE_CHN_A 
 * Defines the command to update the registers for channel A. */
#define UPDATE_CHN_A                    0x07
/** @def WRITE_UPDATE_CHN_A 
 * Defines the command to write and update channel A. */
#define WRITE_UPDATE_CHN_A              0x10

/** @def POWER_ON_OFF_CHN_B 
 * Defines the command to turn on DAC and channel B. */
#define POWER_ON_OFF_CHN_B              0x21
/** @def WRITE_INPUT_CHN_B 
 * Defines the command to write to channel B. */
#define WRITE_INPUT_CHN_B               0x01
/** @def UPDATE_CHN_B 
 * Defines the command to update the registers for channel B. */
#define UPDATE_CHN_B                    0x08
/** @def WRITE_UPDATE_CHN_B 
 * Defines the command to write and update channel B. */
#define WRITE_UPDATE_CHN_B              0x11

/** @def POWER_ON_OFF_CHN_A_B 
 * Defines the command to turn on DAC and both channel A and channel B. */
#define POWER_ON_OFF_CHN_A_B            0x27
/** @def WRITE_INPUT_CHN_A_B 
 * Defines the command to write to both channel A and channel B. */
#define WRITE_INPUT_CHN_A_B             0x07
/** @def UPDATE_CHN_A_B 
 * Defines the command to update the registers for both channel A and channel B. */
#define UPDATE_CHN_A_B                  0x0E
/** @def WRITE_UPDATE_CHN_A_B 
 * Defines the command to write and update both channel A and channel B. */
#define WRITE_UPDATE_CHN_A_B            0x17
    
#else
/** @def RSTSEL 
 * Defines the IO port used for power-on reset. */
#define RSTSEL                          PORTBbits.RB5

/** @def DAC_A 
 * Defines the selection bit for channel A on the DAC. */
#define DAC_A                           0x1
/** @def DAC_B 
 * Defines the selection bit for channel B on the DAC. */
#define DAC_B                           0x8
/** @def DAC_B_A 
 * Defines the selection bit for both channel A and channel B. */  
#define DAC_B_A                         DAC_B | DAC_A

#define CMD_WRITE_TO_DAC                0x1
#define CMD_UPDATE_DAC                  0x2
#define CMD_WRITE_UPDATE_DAC            0x3
#define CMD_POWER_ON_OFF                0x4
#define CMD_LDAC_MASK_REG               0x5
#define CMD_SOFT_RESET                  0x6
#define CMD_INTERN_REF                  0x7
#define CMD_SET_DCEN_REG                0x8
#define CMD_READ_CHN_REG                0x9
    
/** @def WRITE_INPUT_CHN_A 
 * Defines the command to write to channel A. */
#define WRITE_INPUT_CHN_A               (CMD_WRITE_TO_DAC << 4) | DAC_A
/** @def UPDATE_CHN_A 
 * Defines the command to update the registers for channel A. */
#define UPDATE_CHN_A                    (CMD_UPDATE_DAC << 4) | DAC_A
/** @def WRITE_UPDATE_CHN_A 
 * Defines the command to write and update channel A. */
#define WRITE_UPDATE_CHN_A              (CMD_WRITE_UPDATE_DAC << 4) | DAC_A

/** @def WRITE_INPUT_CHN_B 
 * Defines the command to write to channel B. */
#define WRITE_INPUT_CHN_B               (CMD_WRITE_TO_DAC << 4) | DAC_B
/** @def UPDATE_CHN_B 
 * Defines the command to update the registers for channel B. */
#define UPDATE_CHN_B                    (CMD_UPDATE_DAC << 4)  | DAC_B
/** @def WRITE_UPDATE_CHN_B 
 * Defines the command to write and update channel B. */
#define WRITE_UPDATE_CHN_B              (CMD_WRITE_UPDATE_DAC << 4) | DAC_B

/** @def POWER_ON_OFF_CHN_A_B 
 * Defines the command to turn on DAC and both channel A and channel B. */
#define POWER_ON_OFF_CHN_A_B            (CMD_POWER_ON_OFF << 4) | DAC_B_A
/** @def WRITE_INPUT_CHN_A_B 
 * Defines the command to write to both channel A and channel B. */
#define WRITE_INPUT_CHN_A_B             (CMD_WRITE_TO_DAC << 4) | DAC_B_A
/** @def UPDATE_CHN_A_B 
 * Defines the command to update the registers for both channel A and channel B. */
#define UPDATE_CHN_A_B                  (CMD_UPDATE_DAC << 4) | DAC_B_A
/** @def WRITE_UPDATE_CHN_A_B 
 * Defines the command to write and update both channel A and channel B. */
#define WRITE_UPDATE_CHN_A_B            (CMD_WRITE_UPDATE_DAC << 4) | DAC_B_A
#endif
    
void DAC_Init(void);
void DAC_Zero(void);
void DAC_ZeroOutput(void);
BOOL DAC_WriteToDAC(BYTE cmd_addr, WORD data);

#ifdef	__cplusplus
}
#endif

#endif	/* DAC_H */


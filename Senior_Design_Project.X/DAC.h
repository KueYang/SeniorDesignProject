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
//#define DAC12B
    
/** @def AC_ZERO 
 * Defines the mid-scale value for the 16-bit DAC. */
#define AC_ZERO                         0x7777
    
/** @def SYNC 
 * Defines the IO port used to write data to DAC. */
#define SYNC                            PORTAbits.RA4

/** @def DAC_A 
 * Defines the selection bit for channel A on the DAC. */
#define DAC_A                           0x1
/** @def DAC_B 
 * Defines the selection bit for channel B on the DAC. */
#define DAC_B                           0x8
/** @def DAC_B_A 
 * Defines the selection bit for both channel A and channel B. */  
#define DAC_B_A                         DAC_B | DAC_A

/** @def POWER_ON_DAC_B_A 
 * Defines the power on selection for channel A and channel B. */  
#define POWER_ON_DAC_B_A                0x3C

/** @def CMD_WRITE_TO_DAC 
 * Defines the configuration bit to write to DAC, single channel. */  
#define CMD_WRITE_TO_DAC                0x1
/** @def CMD_UPDATE_DAC 
 * Defines the configuration bit to update the DAC, single channel. */  
#define CMD_UPDATE_DAC                  0x2
/** @def CMD_WRITE_UPDATE_DAC 
 * Defines the configuration bit to write and update the DAC, both channels. */  
#define CMD_WRITE_UPDATE_DAC            0x3
/** @def CMD_POWER_ON_OFF 
 * Defines the configuration bit to power on the DAC. */  
#define CMD_POWER_ON_OFF                0x4
/** @def CMD_LDAC_MASK_REG 
 * Defines the configuration bit to mask the LDAC pin. */  
#define CMD_LDAC_MASK_REG               0x5
/** @def CMD_SOFT_RESET 
 * Defines the configuration bit to soft reset the DAC. */  
#define CMD_SOFT_RESET                  0x6
/** @def CMD_INTERN_REF 
 * Defines the configuration bit to enable internal references. */  
#define CMD_INTERN_REF                  0x7
/** @def CMD_SET_DCEN_REG 
 * Defines the configuration bit to enable daisy chain. */  
#define CMD_SET_DCEN_REG                0x8
/** @def CMD_READ_CHN_REG 
 * Defines the configuration bit to read the DAC SDO pin. */  
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
/** @def READ_CHN_A 
 * Defines the command to read channel A. */
#define READ_CHN_A                      (CMD_READ_CHN_REG << 4) | DAC_A

/** @def WRITE_INPUT_CHN_B 
 * Defines the command to write to channel B. */
#define WRITE_INPUT_CHN_B               (CMD_WRITE_TO_DAC << 4) | DAC_B
/** @def UPDATE_CHN_B 
 * Defines the command to update the registers for channel B. */
#define UPDATE_CHN_B                    (CMD_UPDATE_DAC << 4)  | DAC_B
/** @def WRITE_UPDATE_CHN_B 
 * Defines the command to write and update channel B. */
#define WRITE_UPDATE_CHN_B              (CMD_WRITE_UPDATE_DAC << 4) | DAC_B/** @def READ_CHN_A 
/** @def READ_CHN_B 
 * Defines the command to read channel B. */
#define READ_CHN_B                      (CMD_READ_CHN_REG << 4) | DAC_A

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
    
void DAC_Init(void);
void DAC_Zero(void);
void DAC_ZeroOutput(void);
BOOL DAC_WriteToDAC(BYTE cmd_addr, WORD data);

#ifdef	__cplusplus
}
#endif

#endif	/* DAC_H */


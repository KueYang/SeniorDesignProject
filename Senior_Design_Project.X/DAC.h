#ifndef DAC_H
#define	DAC_H

#ifdef	__cplusplus
extern "C" {
#endif

/* DAC Pin Outs */
#define LDAC            PORTAbits.RA3
#define CLR             PORTBbits.RB4
#define SYNC            PORTAbits.RA4

/* DAC Address */
#define DAC_A_ADDR      0b0000
#define DAC_B_ADDR      0b0001
#define ALL_DAC_ADDR    0b0111

/* Power On/Off and Reset Bits */
#define DAC_A                       0b01
#define DAC_B                       0b10
#define POWER_ON_NORMAL             0b00000
#define POWER_OFF_1K_GND            0b01000
#define POWER_OFF_100K_GND          0b10000
#define POWER_OFF_THREE_STATE       0b11000
    
/* DAC Command Definitions */
#define WRITE_INPUT_REG             0b0000
#define UPDATE_REG                  0b0001
#define WRITE_UPDATE_REG            0b0010
#define WRITE_UPDATE_CHAN           0b0011
#define POWER_ON_OFF                0b0100
#define RESET_DAC                   0b0101
#define LDAC_REG_SETUP              0b0110
#define INTERN_REF_SETUP            0b0111

void DAC_Init(void);
BOOL DAC_WriteToDAC(BYTE cmd, BYTE addr, WORD data);

#ifdef	__cplusplus
}
#endif

#endif	/* DAC_H */


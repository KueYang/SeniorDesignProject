/**
 * @file HardwareProfile.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Defines System Configurations.
 */

#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

/**@def SYS_FREQ 
 * Defines the System Clock Speed. */
#define SYS_FREQ                (40000000L)    // 40MHz
/**@def GetPeripheralClock() 
 * Calculates and returns the Peripheral Clock Speed. */
#define	GetPeripheralClock()    (SYS_FREQ/(1 << OSCCONbits.PBDIV))
/**@def GetInstructionClock() 
 * Calculates and returns the Instruction Clock Speed. */
#define	GetInstructionClock()	(SYS_FREQ)


#define PPSUnLock() {SYSKEY=0x0;SYSKEY=0xAA996655;SYSKEY=0x556699AA;CFGCONbits.IOLOCK=0;} 
#define PPSLock() {SYSKEY=0x0;SYSKEY=0xAA996655;SYSKEY=0x556699AA;CFGCONbits.IOLOCK=1;}

#endif
#ifndef CONFIG_H
#define	CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif
        
#define SYS_FREQ                (40000000L)    // 40MHz
#define	GetPeripheralClock()    (SYS_FREQ/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock()	(SYS_FREQ)
    
#ifdef	__cplusplus
}
#endif

#endif	/* CONFIG_H */


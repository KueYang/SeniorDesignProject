#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

void TIMER_Init(void);
void TIMER_Process(void);
UINT32 TIMER_GetMSecond(void);
void TIMER_MSecondDelay(int);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */


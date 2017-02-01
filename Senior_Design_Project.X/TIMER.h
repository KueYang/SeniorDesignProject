/**
 * @file TIMER.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

void TIMER_Init(void);
void TIMER_Process(void);
UINT32 TIMER_GetMSecond(void);
void TIMER_MSecondDelay(int);

BOOL TIMER1_IsON(void);
void TIMER1_ON(BOOL ON);
BOOL TIMER2_IsON(void);
void TIMER2_ON(BOOL ON);
void TIMER2_SetSampleRate(UINT16 sampleRate);
BOOL TIMER3_IsON(void);
void TIMER3_ON(BOOL ON);
void TIMER3_SetSampleRate(UINT16 sampleRate);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */


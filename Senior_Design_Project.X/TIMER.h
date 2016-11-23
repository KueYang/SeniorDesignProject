/**
 * @file TIMER.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The TIMER module handles timer configurations and delays.
 */

#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief Initializes all timer modules.
 * @return Void
 */
void TIMER_Init(void);

/**
 * @brief The main timer module process.
 * @return None
 */
void TIMER_Process(void);

/**
 * @brief Returns the millisecond count since the start of the application.
 * @return The millisecond count of the application.
 */
UINT32 TIMER_GetMSecond(void);

/**
 * @brief Delays the application for a given set time.
 * @param timeDelay The delay in milliseconds
 * @return Void
 */
void TIMER_MSecondDelay(int);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */


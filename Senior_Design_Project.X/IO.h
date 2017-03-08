/**
 * @file IO.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef IO_H
#define	IO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/* Group Output Pins. */
#define GROUP1_OUT  PORTBbits.RB8
#define GROUP2_OUT  PORTBbits.RB5   
#define GROUP3_OUT  PORTBbits.RB5   
#define GROUP4_OUT  PORTBbits.RB5   

/* Open String Pins */
#define FRET0       PORTBbits.RB5
/* Fret Pins */
#define FRET1       PORTAbits.RA3
#define FRET2       PORTBbits.RB4
#define FRET3       PORTBbits.RB7
#define FRET4       PORTBbits.RB5
#define FRET5       PORTBbits.RB5

void IO_Init(void);
void IO_Process(void);
int IO_scanFrets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */


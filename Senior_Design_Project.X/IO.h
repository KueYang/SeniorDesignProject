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
#define GROUP2_OUT  PORTBbits.RB0   
#define GROUP3_OUT  PORTBbits.RB0   
#define GROUP4_OUT  PORTBbits.RB0   

/* Open String */
#define FRET0       PORTBbits.RB0
/* Group 1 */
#define FRET1       PORTAbits.RA3
#define FRET2       PORTBbits.RB4
#define FRET3       PORTBbits.RB5
#define FRET4       PORTBbits.RB7
#define FRET5       PORTBbits.RB4
/* Group 2 */
#define FRET6       PORTBbits.RB5
#define FRET7       PORTBbits.RB5
#define FRET8       PORTBbits.RB7
#define FRET9       PORTBbits.RB8
#define FRET10      PORTBbits.RB9
/* Group 3 */
#define FRET11      PORTBbits.RB10
#define FRET12      PORTBbits.RB11
#define FRET13      PORTBbits.RB11
#define FRET14      PORTBbits.RB13
#define FRET15      PORTBbits.RB14
/* Group 4 */
#define FRET16      PORTBbits.RB15
#define FRET17      PORTBbits.RB15
#define FRET18      PORTBbits.RB15
#define FRET19      PORTBbits.RB15
#define FRET20      PORTBbits.RB15

void IO_Init(void);
void IO_Process(void);

int IO_getCurrentFret(void);
void IO_setCurrentFret(int fret);
void IO_scanFrets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */


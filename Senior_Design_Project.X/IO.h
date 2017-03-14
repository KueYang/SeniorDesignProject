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
/** @def GROUP1_OUT 
 * Defines the output PORT for Group 1 */
#define GROUP1_OUT  PORTBbits.RD4
/** @def GROUP2_OUT 
 * Defines the output PORT for Group 2 */
#define GROUP2_OUT  PORTBbits.RD5   
/** @def GROUP3_OUT 
 * Defines the output PORT for Group 3 */
#define GROUP3_OUT  PORTBbits.RD6  
/** @def GROUP4_OUT 
 * Defines the output PORT for Group 4 */
#define GROUP4_OUT  PORTBbits.RD7   
    
/* Fret Pins */
/** @def FRET1 
 * Defines the input PORT for Fret 1 */
#define FRET1       PORTDbits.RD1
/** @def FRET2 
 * Defines the input PORT for Fret 2 */
#define FRET2       PORTDbits.RD2
/** @def FRET3 
 * Defines the input PORT for Fret 3 */
#define FRET3       PORTDbits.RD3
/** @def FRET4 
 * Defines the input PORT for Fret 4 */
#define FRET4       PORTDbits.RD12
/** @def FRET5 
 * Defines the input PORT for Fret 5 */
#define FRET5       PORTDbits.RD13

void IO_Init(void);
void IO_Process(void);
int IO_scanFrets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */


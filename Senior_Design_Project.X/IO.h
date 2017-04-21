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
/** @def GROUP4_OUT 
 * Defines the output PORT for Group 4 */
#define GROUP4_OUT  PORTEbits.RE1
/** @def GROUP3_OUT 
 * Defines the output PORT for Group 3 */
#define GROUP3_OUT  PORTGbits.RG14   
/** @def GROUP2_OUT 
 * Defines the output PORT for Group 2 */
#define GROUP2_OUT  PORTDbits.RD4  
/** @def GROUP1_OUT 
 * Defines the output PORT for Group 1 */
#define GROUP1_OUT  PORTGbits.RG13   
    
/* Fret Pins */
/** @def FRET5 
 * Defines the input PORT for Fret 5 */
#define FRET5       PORTGbits.RG1
/** @def FRET4 
 * Defines the input PORT for Fret 4 */
#define FRET4       PORTGbits.RG0
/** @def FRET3 
 * Defines the input PORT for Fret 3 */
#define FRET3       PORTAbits.RA6
/** @def FRET2 
 * Defines the input PORT for Fret 2 */
#define FRET2       PORTAbits.RA7
/** @def FRET1 
 * Defines the input PORT for Fret 1 */
#define FRET1       PORTEbits.RE0

/** @def ON_LED 
 * Defines the LED for ON. */
#define ON_LED              PORTEbits.RE2
/** @def ERROR_LED 
 * Defines the LED for ERROR. */
#define ERROR_LED           PORTEbits.RE3
/** @def INITIALIZE_LED 
 * Defines the LED for finishing INITIALIZATION. */
#define INITIALIZE_LED      PORTEbits.RE4
    
void IO_Init(void);
int IO_scanFrets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */


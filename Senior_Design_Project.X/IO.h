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
#define GROUP1_OUT  PORTEbits.RE1
/** @def GROUP2_OUT 
 * Defines the output PORT for Group 2 */
#define GROUP2_OUT  PORTGbits.RG14   
/** @def GROUP3_OUT 
 * Defines the output PORT for Group 3 */
#define GROUP3_OUT  PORTGbits.RG12  
/** @def GROUP4_OUT 
 * Defines the output PORT for Group 4 */
#define GROUP4_OUT  PORTGbits.RG13   
    
/* Fret Pins */
/** @def FRET1 
 * Defines the input PORT for Fret 1 */
#define FRET1       PORTGbits.RG1
/** @def FRET2 
 * Defines the input PORT for Fret 2 */
#define FRET2       PORTGbits.RG0
/** @def FRET3 
 * Defines the input PORT for Fret 3 */
#define FRET3       PORTAbits.RA6
/** @def FRET4 
 * Defines the input PORT for Fret 4 */
#define FRET4       PORTAbits.RA7
/** @def FRET5 
 * Defines the input PORT for Fret 5 */
#define FRET5       PORTEbits.RE0

void IO_Init(void);
void IO_Process(void);
int IO_scanFrets(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */


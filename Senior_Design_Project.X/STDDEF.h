/**
 * @file STDDEF.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef STDDEF_H
#define	STDDEF_H

#ifdef	__cplusplus
extern "C" {
#endif

/**@def TRUE 
 * Boolean datatype. */
#define TRUE    1
/**@def FALSE 
 * Boolean datatype. */    
#define FALSE   0
/**@def BOOL
 * Boolean datatype definition. */
#define BOOL    int

/** @brief Typedef definition for UINT8. */
typedef unsigned char       UINT8;
/** @brief Typedef definition for UINT16. */
typedef unsigned short      UINT16;
/** @brief Typedef definition for UINT32. */
typedef unsigned long       UINT32;

/** @brief Typedef definition for BYTE datatype. */
typedef unsigned char       BYTE;   // 8-bits
/** @brief Typedef definition for WORD datatype. */
typedef unsigned short      WORD;   // 16-bits
/** @brief Typedef definition for DWORD datatype. */
typedef unsigned long       DWORD;  // 32-bits

#ifdef	__cplusplus
}
#endif

#endif	/* STDDEF_H */


/**
 * @file STDDEF.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Defines data types used in the application.
 */

#ifndef STDDEF_H
#define	STDDEF_H

#ifdef	__cplusplus
extern "C" {
#endif

/**@defgroup Standard Data Types 
 * @{
 */
/** Boolean type definitions */
#define TRUE    1
#define FALSE   0
#define BOOL    int

/** @typedef UINT8 datatype. */
typedef unsigned char       UINT8;
/** @typedef UINT16 datatype. */
typedef unsigned short      UINT16;
/** @typedef UINT32 datatype. */
typedef unsigned long       UINT32;

/** @typedef BYTE datatype. */
typedef unsigned char       BYTE;   // 8-bits
/** @typedef WORD datatype. */
typedef unsigned short      WORD;   // 16-bits
/** @typedef DWORD datatype. */
typedef unsigned long       DWORD;  // 32-bits

/** @} */

#ifdef	__cplusplus
}
#endif

#endif	/* STDDEF_H */


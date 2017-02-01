/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT file system module  R0.12b                             /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2016, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/


#include "ff.h"			/* Declarations of FatFs API */
#include "diskio.h"		/* Declarations of device I/O functions */


/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/

#if _FATFS != 68020	/* Revision ID */
#error Wrong include file (ff.h).
#endif


#define	ABORT(fs, res)		{ fp->err = (BYTE)(res); LEAVE_FF(fs, res); }


/* Reentrancy related */
#if _FS_REENTRANT
#if _USE_LFN == 1
#error Static LFN work area cannot be used at thread-safe configuration
#endif
#define	ENTER_FF(fs)		{ if (!lock_fs(fs)) return FR_TIMEOUT; }
#define	LEAVE_FF(fs, res)	{ unlock_fs(fs, res); return res; }
#else
#define	ENTER_FF(fs)
#define LEAVE_FF(fs, res)	return res
#endif



/* Definitions of sector size */
#if (_MAX_SS < _MIN_SS) || (_MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096) || (_MIN_SS != 512 && _MIN_SS != 1024 && _MIN_SS != 2048 && _MIN_SS != 4096)
#error Wrong sector size configuration
#endif
#if _MAX_SS == _MIN_SS
#define	SS(fs)	((UINT16)_MAX_SS)	/* Fixed sector size */
#else
#define	SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif


/* Timestamp */
#if _FS_NORTC == 1
#if _NORTC_YEAR < 1980 || _NORTC_YEAR > 2107 || _NORTC_MON < 1 || _NORTC_MON > 12 || _NORTC_MDAY < 1 || _NORTC_MDAY > 31
#error Invalid _FS_NORTC settings
#endif
#define GET_FATTIME()	((DWORD)(_NORTC_YEAR - 1980) << 25 | (DWORD)_NORTC_MON << 21 | (DWORD)_NORTC_MDAY << 16)
#else
#define GET_FATTIME()	get_fattime()
#endif


/* File lock controls */
#if _FS_LOCK != 0
typedef struct {
	FATFS *fs;		/* Object ID 1, volume (NULL:blank entry) */
	DWORD clu;		/* Object ID 2, directory (0:root) */
	DWORD ofs;		/* Object ID 3, directory offset */
	WORD ctr;		/* Object open counter, 0:none, 0x01..0xFF:read mode open count, 0x100:write mode */
} FILESEM;
#endif



/* DBCS code ranges and SBCS upper conversion tables */

#if _CODE_PAGE == 932	/* Japanese Shift-JIS */
#define _DF1S	0x81	/* DBC 1st byte range 1 start */
#define _DF1E	0x9F	/* DBC 1st byte range 1 end */
#define _DF2S	0xE0	/* DBC 1st byte range 2 start */
#define _DF2E	0xFC	/* DBC 1st byte range 2 end */
#define _DS1S	0x40	/* DBC 2nd byte range 1 start */
#define _DS1E	0x7E	/* DBC 2nd byte range 1 end */
#define _DS2S	0x80	/* DBC 2nd byte range 2 start */
#define _DS2E	0xFC	/* DBC 2nd byte range 2 end */

#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0x80
#define _DS2E	0xFE

#elif _CODE_PAGE == 949	/* Korean */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x41
#define _DS1E	0x5A
#define _DS2S	0x61
#define _DS2E	0x7A
#define _DS3S	0x81
#define _DS3E	0xFE

#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#define _DF1S	0x81
#define _DF1E	0xFE
#define _DS1S	0x40
#define _DS1E	0x7E
#define _DS2S	0xA1
#define _DS2E	0xFE

#elif _CODE_PAGE == 437	/* U.S. */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x45,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
				0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 720	/* Arabic */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 737	/* Greek */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
				0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xEF,0xF5,0xF0,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 771	/* KBL */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDC,0xDE,0xDE, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFE,0xFF}

#elif _CODE_PAGE == 775	/* Baltic */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F, \
				0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 850	/* Latin 1 */
#define _DF1S	0
#define _EXCVT {0x43,0x55,0x45,0x41,0x41,0x41,0x41,0x43,0x45,0x45,0x45,0x49,0x49,0x49,0x41,0x41, \
				0x45,0x92,0x92,0x4F,0x4F,0x4F,0x55,0x55,0x59,0x4F,0x55,0x4F,0x9C,0x4F,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0x41,0x41,0x41,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0x41,0x41,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD1,0xD1,0x45,0x45,0x45,0x49,0x49,0x49,0x49,0xD9,0xDA,0xDB,0xDC,0xDD,0x49,0xDF, \
				0x4F,0xE1,0x4F,0x4F,0x4F,0x4F,0xE6,0xE8,0xE8,0x55,0x55,0x55,0x59,0x59,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 852	/* Latin 2 */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F, \
				0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0xAC, \
				0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}

#elif _CODE_PAGE == 855	/* Cyrillic */
#define _DF1S	0
#define _EXCVT {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F, \
				0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
				0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
				0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF, \
				0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 857	/* Turkish */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x49,0x8E,0x8F, \
				0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
				0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0x49,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 860	/* Portuguese */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0x8F,0x8E,0x91,0x86,0x80,0x89,0x89,0x92,0x8B,0x8C,0x98,0x8E,0x8F, \
				0x90,0x91,0x92,0x8C,0x99,0xA9,0x96,0x9D,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x86,0x8B,0x9F,0x96,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 861	/* Icelandic */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x8B,0x8B,0x8D,0x8E,0x8F, \
				0x90,0x92,0x92,0x4F,0x99,0x8D,0x55,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
				0xA4,0xA5,0xA6,0xA7,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 862	/* Hebrew */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 863	/* Canadian-French */
#define _DF1S	0
#define _EXCVT {0x43,0x55,0x45,0x41,0x41,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x41,0x8F, \
				0x45,0x45,0x45,0x4F,0x45,0x49,0x55,0x55,0x98,0x4F,0x55,0x9B,0x9C,0x55,0x55,0x9F, \
				0xA0,0xA1,0x4F,0x55,0xA4,0xA5,0xA6,0xA7,0x49,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 864	/* Arabic */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x45,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
				0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 865	/* Nordic */
#define _DF1S	0
#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
				0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
				0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 866	/* Russian */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
				0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 869	/* Greek 2 */
#define _DF1S	0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
				0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x86,0x9C,0x8D,0x8F,0x90, \
				0x91,0x90,0x92,0x95,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
				0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
				0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
				0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xA4,0xA5,0xA6,0xD9,0xDA,0xDB,0xDC,0xA7,0xA8,0xDF, \
				0xA9,0xAA,0xAC,0xAD,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xCF,0xCF,0xD0,0xEF, \
				0xF0,0xF1,0xD1,0xD2,0xD3,0xF5,0xD4,0xF7,0xF8,0xF9,0xD5,0x96,0x95,0x98,0xFE,0xFF}

#elif _CODE_PAGE == 1	/* ASCII (for only non-LFN cfg) */
#if _USE_LFN != 0
#error Cannot enable LFN without valid code page.
#endif
#define _DF1S	0

#else
#error Unknown code page

#endif


/* Character code support macros */
#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))

#if _DF1S != 0	/* Code page is DBCS */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E) || ((BYTE)(c) >= _DF2S && (BYTE)(c) <= _DF2E))
#else			/* One 1st byte area */
#define IsDBCS1(c)	((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E)
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E) || ((BYTE)(c) >= _DS3S && (BYTE)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E))
#endif

#else			/* Code page is SBCS */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */


/* File attribute bits (internal use) */
#define	AM_VOL		0x08	/* Volume label */
#define AM_LFN		0x0F	/* LFN entry */
#define AM_MASK		0x3F	/* Mask of defined bits */


/* File access control and file status flags (internal use) */
#define FA_SEEKEND	0x20	/* Seek to end of the file on file open */
#define FA_MODIFIED	0x40	/* File has been modified */
#define FA_DIRTY	0x80	/* FIL.buf[] needs to be written-back */


/* Name status flags */
#define NSFLAG		11		/* Index of name status byte in fn[] */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower case flag (body) */
#define NS_EXT		0x10	/* Lower case flag (ext) */
#define NS_DOT		0x20	/* Dot entry */
#define NS_NOLFN	0x40	/* Do not find LFN */
#define NS_NONAME	0x80	/* Not followed */


/* Limits and boundaries (differ from specs but correct for real DOS/Windows) */
#define MAX_FAT12	0xFF5			/* Maximum number of FAT12 clusters */
#define	MAX_FAT16	0xFFF5			/* Maximum number of FAT16 clusters */
#define	MAX_FAT32	0xFFFFFF5		/* Maximum number of FAT32 clusters */
#define	MAX_EXFAT	0x7FFFFFFD		/* Maximum number of exFAT clusters (limited by implementation) */
#define MAX_DIR		0x200000		/* Maximum size of FAT directory */
#define MAX_DIR_EX	0x10000000		/* Maximum size of exFAT directory */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure members because the structure is not binary compatible between
/ different platforms */

#define BS_JmpBoot			0		/* x86 jump instruction (3-byte) */
#define BS_OEMName			3		/* OEM name (8-byte) */
#define BPB_BytsPerSec		11		/* Sector size [byte] (WORD) */
#define BPB_SecPerClus		13		/* Cluster size [sector] (BYTE) */
#define BPB_RsvdSecCnt		14		/* Size of reserved area [sector] (WORD) */
#define BPB_NumFATs			16		/* Number of FATs (BYTE) */
#define BPB_RootEntCnt		17		/* Size of root directory area for FAT12/16 [entry] (WORD) */
#define BPB_TotSec16		19		/* Volume size (16-bit) [sector] (WORD) */
#define BPB_Media			21		/* Media descriptor byte (BYTE) */
#define BPB_FATSz16			22		/* FAT size (16-bit) [sector] (WORD) */
#define BPB_SecPerTrk		24		/* Track size for int13h [sector] (WORD) */
#define BPB_NumHeads		26		/* Number of heads for int13h (WORD) */
#define BPB_HiddSec			28		/* Volume offset from top of the drive (DWORD) */
#define BPB_TotSec32		32		/* Volume size (32-bit) [sector] (DWORD) */
#define BS_DrvNum			36		/* Physical drive number for int13h (BYTE) */
#define BS_NTres			37		/* Error flag (BYTE) */
#define BS_BootSig			38		/* Extended boot signature (BYTE) */
#define BS_VolID			39		/* Volume serial number (DWORD) */
#define BS_VolLab			43		/* Volume label string (8-byte) */
#define BS_FilSysType		54		/* File system type string (8-byte) */
#define BS_BootCode			62		/* Boot code (448-byte) */
#define BS_55AA				510		/* Signature word (WORD) */

#define BPB_FATSz32			36		/* FAT32: FAT size [sector] (DWORD) */
#define BPB_ExtFlags32		40		/* FAT32: Extended flags (WORD) */
#define BPB_FSVer32			42		/* FAT32: File system version (WORD) */
#define BPB_RootClus32		44		/* FAT32: Root directory cluster (DWORD) */
#define BPB_FSInfo32		48		/* FAT32: Offset of FSINFO sector (WORD) */
#define BPB_BkBootSec32		50		/* FAT32: Offset of backup boot sector (WORD) */
#define BS_DrvNum32			64		/* FAT32: Physical drive number for int13h (BYTE) */
#define BS_NTres32			65		/* FAT32: Error flag (BYTE) */
#define BS_BootSig32		66		/* FAT32: Extended boot signature (BYTE) */
#define BS_VolID32			67		/* FAT32: Volume serial number (DWORD) */
#define BS_VolLab32			71		/* FAT32: Volume label string (8-byte) */
#define BS_FilSysType32		82		/* FAT32: File system type string (8-byte) */
#define BS_BootCode32		90		/* FAT32: Boot code (420-byte) */

#define BPB_ZeroedEx		11		/* exFAT: MBZ field (53-byte) */
#define BPB_VolOfsEx		64		/* exFAT: Volume offset from top of the drive [sector] (QWORD) */
#define BPB_TotSecEx		72		/* exFAT: Volume size [sector] (QWORD) */
#define BPB_FatOfsEx		80		/* exFAT: FAT offset from top of the volume [sector] (DWORD) */
#define BPB_FatSzEx			84		/* exFAT: FAT size [sector] (DWORD) */
#define BPB_DataOfsEx		88		/* exFAT: Data offset from top of the volume [sector] (DWORD) */
#define BPB_NumClusEx		92		/* exFAT: Number of clusters (DWORD) */
#define BPB_RootClusEx		96		/* exFAT: Root directory cluster (DWORD) */
#define BPB_VolIDEx			100		/* exFAT: Volume serial number (DWORD) */
#define BPB_FSVerEx			104		/* exFAT: File system version (WORD) */
#define BPB_VolFlagEx		106		/* exFAT: Volume flags (BYTE) */
#define BPB_ActFatEx		107		/* exFAT: Active FAT flags (BYTE) */
#define BPB_BytsPerSecEx	108		/* exFAT: Log2 of sector size in byte (BYTE) */
#define BPB_SecPerClusEx	109		/* exFAT: Log2 of cluster size in sector (BYTE) */
#define BPB_NumFATsEx		110		/* exFAT: Number of FATs (BYTE) */
#define BPB_DrvNumEx		111		/* exFAT: Physical drive number for int13h (BYTE) */
#define BPB_PercInUseEx		112		/* exFAT: Percent in use (BYTE) */
#define	BPB_RsvdEx			113		/* exFAT: Reserved (7-byte) */
#define BS_BootCodeEx		120		/* exFAT: Boot code (390-byte) */

#define	FSI_LeadSig			0		/* FAT32 FSI: Leading signature (DWORD) */
#define	FSI_StrucSig		484		/* FAT32 FSI: Structure signature (DWORD) */
#define	FSI_Free_Count		488		/* FAT32 FSI: Number of free clusters (DWORD) */
#define	FSI_Nxt_Free		492		/* FAT32 FSI: Last allocated cluster (DWORD) */

#define MBR_Table			446		/* MBR: Offset of partition table in the MBR */
#define	SZ_PTE				16		/* MBR: Size of a partition table entry */
#define PTE_Boot			0		/* MBR PTE: Boot indicator */
#define PTE_StHead			1		/* MBR PTE: Start head */
#define PTE_StSec			2		/* MBR PTE: Start sector */
#define PTE_StCyl			3		/* MBR PTE: Start cylinder */
#define PTE_System			4		/* MBR PTE: System ID */
#define PTE_EdHead			5		/* MBR PTE: End head */
#define PTE_EdSec			6		/* MBR PTE: End sector */
#define PTE_EdCyl			7		/* MBR PTE: End cylinder */
#define PTE_StLba			8		/* MBR PTE: Start in LBA */
#define PTE_SizLba			12		/* MBR PTE: Size in LBA */

#define	DIR_Name			0		/* Short file name (11-byte) */
#define	DIR_Attr			11		/* Attribute (BYTE) */
#define	DIR_NTres			12		/* Lower case flag (BYTE) */
#define DIR_CrtTime10		13		/* Created time sub-second (BYTE) */
#define	DIR_CrtTime			14		/* Created time (DWORD) */
#define DIR_LstAccDate		18		/* Last accessed date (WORD) */
#define	DIR_FstClusHI		20		/* Higher 16-bit of first cluster (WORD) */
#define	DIR_ModTime			22		/* Modified time (DWORD) */
#define	DIR_FstClusLO		26		/* Lower 16-bit of first cluster (WORD) */
#define	DIR_FileSize		28		/* File size (DWORD) */
#define	LDIR_Ord			0		/* LFN entry order and LLE flag (BYTE) */
#define	LDIR_Attr			11		/* LFN attribute (BYTE) */
#define	LDIR_Type			12		/* LFN type (BYTE) */
#define	LDIR_Chksum			13		/* Checksum of the SFN entry (BYTE) */
#define	LDIR_FstClusLO		26		/* Must be zero (WORD) */
#define	XDIR_Type			0		/* Type of exFAT directory entry (BYTE) */
#define	XDIR_NumLabel		1		/* Number of volume label characters (BYTE) */
#define	XDIR_Label			2		/* Volume label (11-WORD) */
#define	XDIR_CaseSum		4		/* Sum of case conversion table (DWORD) */
#define	XDIR_NumSec			1		/* Number of secondary entries (BYTE) */
#define	XDIR_SetSum			2		/* Sum of the set of directory entries (WORD) */
#define	XDIR_Attr			4		/* File attribute (WORD) */
#define	XDIR_CrtTime		8		/* Created time (DWORD) */
#define	XDIR_ModTime		12		/* Modified time (DWORD) */
#define	XDIR_AccTime		16		/* Last accessed time (DWORD) */
#define	XDIR_CrtTime10		20		/* Created time subsecond (BYTE) */
#define	XDIR_ModTime10		21		/* Modified time subsecond (BYTE) */
#define	XDIR_CrtTZ			22		/* Created timezone (BYTE) */
#define	XDIR_ModTZ			23		/* Modified timezone (BYTE) */
#define	XDIR_AccTZ			24		/* Last accessed timezone (BYTE) */
#define	XDIR_GenFlags		33		/* Gneral secondary flags (WORD) */
#define	XDIR_NumName		35		/* Number of file name characters (BYTE) */
#define	XDIR_NameHash		36		/* Hash of file name (WORD) */
#define XDIR_ValidFileSize	40		/* Valid file size (QWORD) */
#define	XDIR_FstClus		52		/* First cluster of the file data (DWORD) */
#define	XDIR_FileSize		56		/* File/Directory size (QWORD) */

#define	SZDIRE				32		/* Size of a directory entry */
#define	LLEF				0x40	/* Last long entry flag in LDIR_Ord */
#define	DDEM				0xE5	/* Deleted directory entry mark set to DIR_Name[0] */
#define	RDDEM				0x05	/* Replacement of the character collides with DDEM */





/*--------------------------------------------------------------------------

   Module Private Work Area

---------------------------------------------------------------------------*/

/* Remark: Variables here without initial value shall be guaranteed zero/null
/  at start-up. If not, either the linker or start-up routine being used is
/  not compliance with C standard. */

#if _VOLUMES < 1 || _VOLUMES > 9
#error Wrong _VOLUMES setting
#endif
static FATFS *FatFs[_VOLUMES];	/* Pointer to the file system objects (logical drives) */
static WORD Fsid;				/* File system mount ID */

#if _FS_RPATH != 0 && _VOLUMES >= 2
static BYTE CurrVol;			/* Current drive */
#endif

#if _FS_LOCK != 0
static FILESEM Files[_FS_LOCK];	/* Open object lock semaphores */
#endif

#if _USE_LFN == 0			/* Non-LFN configuration */
#define	DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define	FREE_NAMBUF()
#else
#if _MAX_LFN < 12 || _MAX_LFN > 255
#error Wrong _MAX_LFN setting
#endif

#if _USE_LFN == 1		/* LFN enabled with static working buffer */
#if _FS_EXFAT
static BYTE	DirBuf[SZDIRE*19];	/* Directory entry block scratchpad buffer (19 entries in size) */
#endif
static WORD LfnBuf[_MAX_LFN+1];	/* LFN enabled with static working buffer */
#define	DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define	FREE_NAMBUF()

#elif _USE_LFN == 2 	/* LFN enabled with dynamic working buffer on the stack */
#if _FS_EXFAT
#define	DEF_NAMBUF		WORD lbuf[_MAX_LFN+1]; BYTE dbuf[SZDIRE*19];
#define INIT_NAMBUF(fs)	{ (fs)->lfnbuf = lbuf; (fs)->dirbuf = dbuf; }
#define	FREE_NAMBUF()
#else
#define	DEF_NAMBUF		WORD lbuf[_MAX_LFN+1];
#define INIT_NAMBUF(fs)	{ (fs)->lfnbuf = lbuf; }
#define	FREE_NAMBUF()
#endif

#elif _USE_LFN == 3 	/* LFN enabled with dynamic working buffer on the heap */
#if _FS_EXFAT
#define	DEF_NAMBUF		WORD *lfn;
#define INIT_NAMBUF(fs)	{ lfn = ff_memalloc((_MAX_LFN+1)*2 + SZDIRE*19); if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE); (fs)->lfnbuf = lfn; (fs)->dirbuf = (BYTE*)(lfn+_MAX_LFN+1); }
#define	FREE_NAMBUF()	ff_memfree(lfn)
#else
#define	DEF_NAMBUF		WORD *lfn;
#define INIT_NAMBUF(fs)	{ lfn = ff_memalloc((_MAX_LFN+1)*2); if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE); (fs)->lfnbuf = lfn; }
#define	FREE_NAMBUF()	ff_memfree(lfn)
#endif

#else
#error Wrong _USE_LFN setting
#endif
#endif

#ifdef _EXCVT
static const BYTE ExCvt[] = _EXCVT;	/* Upper conversion table for SBCS extended characters */
#endif






/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Load/Store multi-byte word in the FAT structure                       */
/*-----------------------------------------------------------------------*/

static
WORD ld_word (const BYTE* ptr)	/*	 Load a 2-byte little-endian word */
{
	WORD rv;

	rv = ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}

static
DWORD ld_dword (const BYTE* ptr)	/* Load a 4-byte little-endian word */
{
	DWORD rv;

	rv = ptr[3];
	rv = rv << 8 | ptr[2];
	rv = rv << 8 | ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}

#if _FS_EXFAT
static
QWORD ld_qword (const BYTE* ptr)	/* Load an 8-byte little-endian word */
{
	QWORD rv;

	rv = ptr[7];
	rv = rv << 8 | ptr[6];
	rv = rv << 8 | ptr[5];
	rv = rv << 8 | ptr[4];
	rv = rv << 8 | ptr[3];
	rv = rv << 8 | ptr[2];
	rv = rv << 8 | ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}
#endif

/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

/* Copy memory to memory */
static
void mem_cpy (void* dst, const void* src, UINT16 cnt) {
	BYTE *d = (BYTE*)dst;
	const BYTE *s = (const BYTE*)src;

	if (cnt) {
		do *d++ = *s++; while (--cnt);
	}
}

/* Fill memory block */
static
void mem_set (void* dst, int val, UINT16 cnt) {
	BYTE *d = (BYTE*)dst;

	do *d++ = (BYTE)val; while (--cnt);
}

/* Compare memory block */
static
int mem_cmp (const void* dst, const void* src, UINT16 cnt) {	/* ZR:same, NZ:different */
	const BYTE *d = (const BYTE *)dst, *s = (const BYTE *)src;
	int r = 0;

	do {
		r = *d++ - *s++;
	} while (--cnt && r == 0);

	return r;
}

/* Check if chr is contained in the string */
static
int chk_chr (const char* str, int chr) {	/* NZ:contained, ZR:not contained */
	while (*str && *str != chr) str++;
	return *str;
}




#if _FS_REENTRANT
/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/
static
int lock_fs (
	FATFS* fs		/* File system object */
)
{
	return ff_req_grant(fs->sobj);
}


static
void unlock_fs (
	FATFS* fs,		/* File system object */
	FRESULT res		/* Result code to be returned */
)
{
	if (fs && res != FR_NOT_ENABLED && res != FR_INVALID_DRIVE && res != FR_TIMEOUT) {
		ff_rel_grant(fs->sobj);
	}
}

#endif



#if _FS_LOCK != 0
/*-----------------------------------------------------------------------*/
/* File lock control functions                                           */
/*-----------------------------------------------------------------------*/

static
FRESULT chk_lock (	/* Check if the file can be accessed */
	DIR* dp,		/* Directory object pointing the file to be checked */
	int acc			/* Desired access type (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT16 i, be;

	/* Search file semaphore table */
	for (i = be = 0; i < _FS_LOCK; i++) {
		if (Files[i].fs) {	/* Existing entry */
			if (Files[i].fs == dp->obj.fs &&	 	/* Check if the object matched with an open object */
				Files[i].clu == dp->obj.sclust &&
				Files[i].ofs == dp->dptr) break;
		} else {			/* Blank entry */
			be = 1;
		}
	}
	if (i == _FS_LOCK) {	/* The object is not opened */
		return (be || acc == 2) ? FR_OK : FR_TOO_MANY_OPEN_FILES;	/* Is there a blank entry for new object? */
	}

	/* The object has been opened. Reject any open against writing file and all write mode open */
	return (acc || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}


static
int enq_lock (void)	/* Check if an entry is available for a new object */
{
	UINT16 i;

	for (i = 0; i < _FS_LOCK && Files[i].fs; i++) ;
	return (i == _FS_LOCK) ? 0 : 1;
}


static
UINT16 inc_lock (	/* Increment object open counter and returns its index (0:Internal error) */
	DIR* dp,	/* Directory object pointing the file to register or increment */
	int acc		/* Desired access (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT16 i;


	for (i = 0; i < _FS_LOCK; i++) {	/* Find the object */
		if (Files[i].fs == dp->obj.fs &&
			Files[i].clu == dp->obj.sclust &&
			Files[i].ofs == dp->dptr) break;
	}

	if (i == _FS_LOCK) {				/* Not opened. Register it as new. */
		for (i = 0; i < _FS_LOCK && Files[i].fs; i++) ;
		if (i == _FS_LOCK) return 0;	/* No free entry to register (int err) */
		Files[i].fs = dp->obj.fs;
		Files[i].clu = dp->obj.sclust;
		Files[i].ofs = dp->dptr;
		Files[i].ctr = 0;
	}

	if (acc && Files[i].ctr) return 0;	/* Access violation (int err) */

	Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1;	/* Set semaphore value */

	return i + 1;
}


static
FRESULT dec_lock (	/* Decrement object open counter */
	UINT16 i			/* Semaphore index (1..) */
)
{
	WORD n;
	FRESULT res;


	if (--i < _FS_LOCK) {	/* Shift index number origin from 0 */
		n = Files[i].ctr;
		if (n == 0x100) n = 0;		/* If write mode open, delete the entry */
		if (n > 0) n--;				/* Decrement read mode open count */
		Files[i].ctr = n;
		if (n == 0) Files[i].fs = 0;	/* Delete the entry if open count gets zero */
		res = FR_OK;
	} else {
		res = FR_INT_ERR;			/* Invalid index nunber */
	}
	return res;
}


static
void clear_lock (	/* Clear lock entries of the volume */
	FATFS *fs
)
{
	UINT16 i;

	for (i = 0; i < _FS_LOCK; i++) {
		if (Files[i].fs == fs) Files[i].fs = 0;
	}
}

#endif	/* _FS_LOCK != 0 */



/*-----------------------------------------------------------------------*/
/* Move/Flush disk access window in the file system object               */
/*-----------------------------------------------------------------------*/
static
FRESULT move_window (	/* Returns FR_OK or FR_DISK_ERROR */
	FATFS* fs,			/* File system object */
	DWORD sector		/* Sector number to make appearance in the fs->win[] */
)
{
	FRESULT res = FR_OK;


	if (sector != fs->winsect) {	/* Window offset changed? */
		if (res == FR_OK) {			/* Fill sector window with new data */
			if (disk_read(fs->drv, fs->win, sector, 1) != RES_OK) {
				sector = 0xFFFFFFFF;	/* Invalidate window if data is not reliable */
				res = FR_DISK_ERR;
			}
			fs->winsect = sector;
		}
	}
	return res;
}

/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/

static
DWORD clust2sect (	/* !=0:Sector number, 0:Failed (invalid cluster#) */
	FATFS* fs,		/* File system object */
	DWORD clst		/* Cluster# to be converted */
)
{
	clst -= 2;
	if (clst >= fs->n_fatent - 2) return 0;		/* Invalid cluster# */
	return clst * fs->csize + fs->database;
}




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of a FAT entry                                */
/*-----------------------------------------------------------------------*/

static
DWORD get_fat (	/* 0xFFFFFFFF:Disk error, 1:Internal error, 2..0x7FFFFFFF:Cluster status */
	_FDID* obj,	/* Corresponding object */
	DWORD clst	/* Cluster number to get the value */
)
{
	UINT16 wc, bc;
	DWORD val;
	FATFS *fs = obj->fs;


	if (clst < 2 || clst >= fs->n_fatent) {	/* Check if in valid range */
		val = 1;	/* Internal error */

	} else {
		val = 0xFFFFFFFF;	/* Default value falls on disk error */

		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT16)clst; bc += bc / 2;
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc = fs->win[bc++ % SS(fs)];
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc |= fs->win[bc % SS(fs)] << 8;
			val = (clst & 1) ? (wc >> 4) : (wc & 0xFFF);
			break;

		case FS_FAT16 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) != FR_OK) break;
			val = ld_word(fs->win + clst * 2 % SS(fs));
			break;

		case FS_FAT32 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK) break;
			val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x0FFFFFFF;
			break;
#if _FS_EXFAT
		case FS_EXFAT :
			if (obj->objsize) {
				DWORD cofs = clst - obj->sclust;	/* Offset from start cluster */
				DWORD clen = (DWORD)((obj->objsize - 1) / SS(fs)) / fs->csize;	/* Number of clusters - 1 */

				if (obj->stat == 2) {	/* Is there no valid chain on the FAT? */
					if (cofs <= clen) {
						val = (cofs == clen) ? 0x7FFFFFFF : clst + 1;	/* Generate the value */
						break;
					}
				}
				if (obj->stat == 3 && cofs < obj->n_cont) {	/* Is it in the contiguous part? */
					val = clst + 1; 	/* Generate the value */
					break;
				}
				if (obj->stat != 2) {	/* Get value from FAT if FAT chain is valid */
					if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK) break;
					val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x7FFFFFFF;
					break;
				}
			}
			/* go next */
#endif
		default:
			val = 1;	/* Internal error */
		}
	}

	return val;
}

#if _USE_FASTSEEK
/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/

static
DWORD clmt_clust (	/* <2:Error, >=2:Cluster number */
	FIL* fp,		/* Pointer to the file object */
	FSIZE_t ofs		/* File offset to be converted to cluster# */
)
{
	DWORD cl, ncl, *tbl;
	FATFS *fs = fp->obj.fs;


	tbl = fp->cltbl + 1;	/* Top of CLMT */
	cl = (DWORD)(ofs / SS(fs) / fs->csize);	/* Cluster order from top of the file */
	for (;;) {
		ncl = *tbl++;			/* Number of cluters in the fragment */
		if (ncl == 0) return 0;	/* End of table? (error) */
		if (cl < ncl) break;	/* In this fragment? */
		cl -= ncl; tbl++;		/* Next fragment */
	}
	return cl + *tbl;	/* Return the cluster number */
}

#endif	/* _USE_FASTSEEK */

/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_sdi (	/* FR_OK(0):succeeded, !=0:error */
	DIR* dp,		/* Pointer to directory object */
	DWORD ofs		/* Offset of directory table */
)
{
	DWORD csz, clst;
	FATFS *fs = dp->obj.fs;


	if (ofs >= (DWORD)((_FS_EXFAT && fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR) || ofs % SZDIRE) {	/* Check range of offset and alignment */
		return FR_INT_ERR;
	}
	dp->dptr = ofs;				/* Set current offset */
	clst = dp->obj.sclust;		/* Table start cluster (0:root) */
	if (clst == 0 && fs->fs_type >= FS_FAT32) {	/* Replace cluster# 0 with root cluster# */
		clst = fs->dirbase;
		if (_FS_EXFAT) dp->obj.stat = 0;	/* exFAT: Root dir has an FAT chain */
	}

	if (clst == 0) {	/* Static table (root-directory in FAT12/16) */
		if (ofs / SZDIRE >= fs->n_rootdir)	return FR_INT_ERR;	/* Is index out of range? */
		dp->sect = fs->dirbase;

	} else {			/* Dynamic table (sub-directory or root-directory in FAT32+) */
		csz = (DWORD)fs->csize * SS(fs);	/* Bytes per cluster */
		while (ofs >= csz) {				/* Follow cluster chain */
			clst = get_fat(&dp->obj, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
			if (clst < 2 || clst >= fs->n_fatent) return FR_INT_ERR;	/* Reached to end of table or internal error */
			ofs -= csz;
		}
		dp->sect = clust2sect(fs, clst);
	}
	dp->clust = clst;					/* Current cluster# */
	if (!dp->sect) return FR_INT_ERR;
	dp->sect += ofs / SS(fs);			/* Sector# of the directory entry */
	dp->dir = fs->win + (ofs % SS(fs));	/* Pointer to the entry in the win[] */

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory table index next                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_next (	/* FR_OK(0):succeeded, FR_NO_FILE:End of table, FR_DENIED:Could not stretch */
	DIR* dp,		/* Pointer to the directory object */
	int stretch		/* 0: Do not stretch table, 1: Stretch table if needed */
)
{
	DWORD ofs, clst;
	FATFS *fs = dp->obj.fs;

	ofs = dp->dptr + SZDIRE;	/* Next entry */
	if (!dp->sect || ofs >= (DWORD)((_FS_EXFAT && fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR)) return FR_NO_FILE;	/* Report EOT when offset has reached max value */

	if (ofs % SS(fs) == 0) {	/* Sector changed? */
		dp->sect++;				/* Next sector */

		if (!dp->clust) {		/* Static table */
			if (ofs / SZDIRE >= fs->n_rootdir) {	/* Report EOT if it reached end of static table */
				dp->sect = 0; return FR_NO_FILE;
			}
		}
		else {					/* Dynamic table */
			if ((ofs / SS(fs) & (fs->csize - 1)) == 0) {		/* Cluster changed? */
				clst = get_fat(&dp->obj, dp->clust);			/* Get next cluster */
				if (clst <= 1) return FR_INT_ERR;				/* Internal error */
				if (clst == 0xFFFFFFFF) return FR_DISK_ERR;		/* Disk error */
				if (clst >= fs->n_fatent) {						/* Reached end of dynamic table */

				}
				dp->clust = clst;		/* Initialize data for new cluster */
				dp->sect = clust2sect(fs, clst);
			}
		}
	}
	dp->dptr = ofs;						/* Current entry */
	dp->dir = fs->win + ofs % SS(fs);	/* Pointer to the entry in the win[] */

	return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* FAT: Directory handling - Load/Store start cluster number             */
/*-----------------------------------------------------------------------*/

static
DWORD ld_clust (	/* Returns the top cluster value of the SFN entry */
	FATFS* fs,		/* Pointer to the fs object */
	const BYTE* dir	/* Pointer to the key entry */
)
{
	DWORD cl;

	cl = ld_word(dir + DIR_FstClusLO);
	if (fs->fs_type == FS_FAT32) {
		cl |= (DWORD)ld_word(dir + DIR_FstClusHI) << 16;
	}

	return cl;
}

#if _USE_LFN != 0
/*------------------------------------------------------------------------*/
/* FAT-LFN: LFN handling                                                  */
/*------------------------------------------------------------------------*/
static
const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};	/* Offset of LFN characters in the directory entry */


/*--------------------------------------------------------*/
/* FAT-LFN: Compare a part of file name with an LFN entry */
/*--------------------------------------------------------*/
static
int cmp_lfn (				/* 1:matched, 0:not matched */
	const WORD* lfnbuf,	/* Pointer to the LFN working buffer to be compared */
	BYTE* dir				/* Pointer to the directory entry containing the part of LFN */
)
{
	UINT16 i, s;
	WORD wc, uc;


	if (ld_word(dir + LDIR_FstClusLO) != 0) return 0;	/* Check LDIR_FstClusLO */

	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	for (wc = 1, s = 0; s < 13; s++) {		/* Process all characters in the entry */
		uc = ld_word(dir + LfnOfs[s]);		/* Pick an LFN character */
		if (wc) {
			if (i >= _MAX_LFN || ff_wtoupper(uc) != ff_wtoupper(lfnbuf[i++])) {	/* Compare it */
				return 0;					/* Not matched */
			}
			wc = uc;
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	}

	if ((dir[LDIR_Ord] & LLEF) && wc && lfnbuf[i]) return 0;	/* Last segment matched but different length */

	return 1;		/* The part of LFN matched */
}


#if _FS_MINIMIZE <= 1 || _FS_RPATH >= 2 || _USE_LABEL || _FS_EXFAT
/*-----------------------------------------------------*/
/* FAT-LFN: Pick a part of file name from an LFN entry */
/*-----------------------------------------------------*/
static
int pick_lfn (			/* 1:succeeded, 0:buffer overflow or invalid LFN entry */
	WORD* lfnbuf,		/* Pointer to the LFN working buffer */
	BYTE* dir			/* Pointer to the LFN entry */
)
{
	UINT16 i, s;
	WORD wc, uc;


	if (ld_word(dir + LDIR_FstClusLO) != 0) return 0;	/* Check LDIR_FstClusLO */

	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	for (wc = 1, s = 0; s < 13; s++) {		/* Process all characters in the entry */
		uc = ld_word(dir + LfnOfs[s]);		/* Pick an LFN character */
		if (wc) {
			if (i >= _MAX_LFN) return 0;	/* Buffer overflow? */
			lfnbuf[i++] = wc = uc;			/* Store it */
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	}

	if (dir[LDIR_Ord] & LLEF) {				/* Put terminator if it is the last LFN part */
		if (i >= _MAX_LFN) return 0;		/* Buffer overflow? */
		lfnbuf[i] = 0;
	}

	return 1;		/* The part of LFN is valid */
}
#endif

#endif	/* _USE_LFN != 0 */

#if _USE_LFN != 0
/*-----------------------------------------------------------------------*/
/* FAT-LFN: Calculate checksum of an SFN entry                           */
/*-----------------------------------------------------------------------*/

static
BYTE sum_sfn (
	const BYTE* dir		/* Pointer to the SFN entry */
)
{
	BYTE sum = 0;
	UINT16 n = 11;

	do sum = (sum >> 1) + (sum << 7) + *dir++; while (--n);
	return sum;
}

#endif	/* _USE_LFN != 0 */



#if _FS_EXFAT
/*-----------------------------------------------------------------------*/
/* exFAT: Checksum                                                       */
/*-----------------------------------------------------------------------*/

static
WORD xdir_sum (			/* Get checksum of the directoly block */
	const BYTE* dir		/* Directory entry block to be calculated */
)
{
	UINT16 i, szblk;
	WORD sum;


	szblk = (dir[XDIR_NumSec] + 1) * SZDIRE;
	for (i = sum = 0; i < szblk; i++) {
		if (i == XDIR_SetSum) {	/* Skip sum field */
			i++;
		} else {
			sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + dir[i];
		}
	}
	return sum;
}



static
WORD xname_sum (		/* Get check sum (to be used as hash) of the name */
	const WORD* name	/* File name to be calculated */
)
{
	WORD chr;
	WORD sum = 0;


	while ((chr = *name++) != 0) {
		chr = ff_wtoupper(chr);		/* File name needs to be ignored case */
		sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr & 0xFF);
		sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr >> 8);
	}
	return sum;
}

#if _FS_MINIMIZE <= 1 || _FS_RPATH >= 2
/*------------------------------------------------------*/
/* exFAT: Get object information from a directory block */
/*------------------------------------------------------*/

static
void get_xdir_info (
	BYTE* dirb,			/* Pointer to the direcotry entry block 85+C0+C1s */
	FILINFO* fno		/* Buffer to store the extracted file information */
)
{
	UINT16 di, si;
	WORD w;
#if !_LFN_UNICODE
	UINT16 nc;
#endif

	/* Get file name */
#if _LFN_UNICODE
	if (dirb[XDIR_NumName] <= _MAX_LFN) {
		for (si = SZDIRE * 2, di = 0; di < dirb[XDIR_NumName]; si += 2, di++) {
			if ((si % SZDIRE) == 0) si += 2;	/* Skip entry type field */
			w = ld_word(dirb + si);				/* Get a character */
			fno->fname[di] = w;					/* Store it */
		}
	} else {
		di = 0;	/* Buffer overflow and inaccessible object */
	}
#else
	for (si = SZDIRE * 2, di = nc = 0; nc < dirb[XDIR_NumName]; si += 2, nc++) {
		if ((si % SZDIRE) == 0) si += 2;	/* Skip entry type field */
		w = ld_word(dirb + si);				/* Get a character */
		w = ff_convert(w, 0);				/* Unicode -> OEM */
		if (w == 0) { di = 0; break; }		/* Could not be converted and inaccessible object */
		if (_DF1S && w >= 0x100) {			/* Put 1st byte if it is a DBC (always false at SBCS cfg) */
			fno->fname[di++] = (char)(w >> 8);
		}
		if (di >= _MAX_LFN) { di = 0; break; }	/* Buffer overflow and inaccessible object */
		fno->fname[di++] = (char)w;
	}
#endif
	if (di == 0) fno->fname[di++] = '?';	/* Inaccessible object? */
	fno->fname[di] = 0;						/* Terminate file name */

	fno->altname[0] = 0;							/* No SFN */
	fno->fattrib = dirb[XDIR_Attr];					/* Attribute */
	fno->fsize = (fno->fattrib & AM_DIR) ? 0 : ld_qword(dirb + XDIR_FileSize);	/* Size */
	fno->ftime = ld_word(dirb + XDIR_ModTime + 0);	/* Time */
	fno->fdate = ld_word(dirb + XDIR_ModTime + 2);	/* Date */
}

#endif	/* _FS_MINIMIZE <= 1 || _FS_RPATH >= 2 */


/*-----------------------------------*/
/* exFAT: Get a directry entry block */
/*-----------------------------------*/

static
FRESULT load_xdir (	/* FR_INT_ERR: invalid entry block */
	DIR* dp			/* Pointer to the reading direcotry object pointing the 85 entry */
)
{
	FRESULT res;
	UINT16 i, nent;
	BYTE* dirb = dp->obj.fs->dirbuf;	/* Pointer to the on-memory direcotry entry block 85+C0+C1s */


	/* Load 85 entry */
	res = move_window(dp->obj.fs, dp->sect);
	if (res != FR_OK) return res;
	if (dp->dir[XDIR_Type] != 0x85) return FR_INT_ERR;
	mem_cpy(dirb, dp->dir, SZDIRE);
	nent = dirb[XDIR_NumSec] + 1;

	/* Load C0 entry */
	res = dir_next(dp, 0);
	if (res != FR_OK) return res;
	res = move_window(dp->obj.fs, dp->sect);
	if (res != FR_OK) return res;
	if (dp->dir[XDIR_Type] != 0xC0) return FR_INT_ERR;
	mem_cpy(dirb + SZDIRE, dp->dir, SZDIRE);

	/* Load C1 entries */
	if (nent < 3 || nent > 19) return FR_NO_FILE;
	i = SZDIRE * 2; nent *= SZDIRE;
	do {
		res = dir_next(dp, 0);
		if (res != FR_OK) return res;
		res = move_window(dp->obj.fs, dp->sect);
		if (res != FR_OK) return res;
		if (dp->dir[XDIR_Type] != 0xC1) return FR_INT_ERR;
		mem_cpy(dirb + i, dp->dir, SZDIRE);
		i += SZDIRE;
	} while (i < nent);

	/* Sanity check */
	if (xdir_sum(dirb) != ld_word(dirb + XDIR_SetSum)) return FR_INT_ERR;

	return FR_OK;
}

#endif	/* _FS_EXFAT */



#if _FS_MINIMIZE <= 1 || _FS_RPATH >= 2 || _USE_LABEL || _FS_EXFAT
/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_read (
	DIR* dp,		/* Pointer to the directory object */
	int vol			/* Filtered by 0:file/directory or 1:volume label */
)
{
	FRESULT res = FR_NO_FILE;
	FATFS *fs = dp->obj.fs;
	BYTE a, c;
#if _USE_LFN != 0
	BYTE ord = 0xFF, sum = 0xFF;
#endif

	while (dp->sect) {
		res = move_window(fs, dp->sect);
		if (res != FR_OK) break;
		c = dp->dir[DIR_Name];	/* Test for the entry type */
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of the directory */
#if _FS_EXFAT
		if (fs->fs_type == FS_EXFAT) {	/* On the exFAT volume */
			if (_USE_LABEL && vol) {
				if (c == 0x83) break;	/* Volume label entry? */
			} else {
				if (c == 0x85) {		/* Start of the file entry block? */
					dp->blk_ofs = dp->dptr;	/* Get location of the block */
					res = load_xdir(dp);	/* Load the entry block */
					if (res == FR_OK) {
						dp->obj.attr = fs->dirbuf[XDIR_Attr] & AM_MASK;	/* Get attribute */
					}
					break;
				}
			}
		} else
#endif
		{	/* On the FAT12/16/32 volume */
			dp->obj.attr = a = dp->dir[DIR_Attr] & AM_MASK;	/* Get attribute */
#if _USE_LFN != 0	/* LFN configuration */
			if (c == DDEM || c == '.' || (int)((a & ~AM_ARC) == AM_VOL) != vol) {	/* An entry without valid data */
				ord = 0xFF;
			} else {
				if (a == AM_LFN) {			/* An LFN entry is found */
					if (c & LLEF) {			/* Is it start of an LFN sequence? */
						sum = dp->dir[LDIR_Chksum];
						c &= (BYTE)~LLEF; ord = c;
						dp->blk_ofs = dp->dptr;
					}
					/* Check LFN validity and capture it */
					ord = (c == ord && sum == dp->dir[LDIR_Chksum] && pick_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF;
				} else {					/* An SFN entry is found */
					if (ord || sum != sum_sfn(dp->dir)) {	/* Is there a valid LFN? */
						dp->blk_ofs = 0xFFFFFFFF;			/* It has no LFN. */
					}
					break;
				}
			}
#else		/* Non LFN configuration */
			if (c != DDEM && c != '.' && a != AM_LFN && (int)((a & ~AM_ARC) == AM_VOL) == vol) {	/* Is it a valid entry? */
				break;
			}
#endif
		}
		res = dir_next(dp, 0);		/* Next entry */
		if (res != FR_OK) break;
	}

	if (res != FR_OK) dp->sect = 0;		/* Terminate the read operation on error or EOT */
	return res;
}

#endif	/* _FS_MINIMIZE <= 1 || _USE_LABEL || _FS_RPATH >= 2 */



/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_find (	/* FR_OK(0):succeeded, !=0:error */
	DIR* dp			/* Pointer to the directory object with the file name */
)
{
	FRESULT res;
	FATFS *fs = dp->obj.fs;
	BYTE c;
#if _USE_LFN != 0
	BYTE a, ord, sum;
#endif

	res = dir_sdi(dp, 0);			/* Rewind directory object */
	if (res != FR_OK) return res;
#if _FS_EXFAT
	if (fs->fs_type == FS_EXFAT) {	/* On the exFAT volume */
		BYTE nc;
		UINT16 di, ni;
		WORD hash = xname_sum(fs->lfnbuf);		/* Hash value of the name to find */

		while ((res = dir_read(dp, 0)) == FR_OK) {	/* Read an item */
			if (ld_word(fs->dirbuf + XDIR_NameHash) != hash) continue;	/* Skip the comparison if hash value mismatched */
			for (nc = fs->dirbuf[XDIR_NumName], di = SZDIRE * 2, ni = 0; nc; nc--, di += 2, ni++) {	/* Compare the name */
				if ((di % SZDIRE) == 0) di += 2;
				if (ff_wtoupper(ld_word(fs->dirbuf + di)) != ff_wtoupper(fs->lfnbuf[ni])) break;
			}
			if (nc == 0 && !fs->lfnbuf[ni]) break;	/* Name matched? */
		}
		return res;
	}
#endif
	/* On the FAT12/16/32 volume */
#if _USE_LFN != 0
	ord = sum = 0xFF; dp->blk_ofs = 0xFFFFFFFF;	/* Reset LFN sequence */
#endif
	do {
		res = move_window(fs, dp->sect);
		if (res != FR_OK) break;
		c = dp->dir[DIR_Name];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
#if _USE_LFN != 0	/* LFN configuration */
		dp->obj.attr = a = dp->dir[DIR_Attr] & AM_MASK;
		if (c == DDEM || ((a & AM_VOL) && a != AM_LFN)) {	/* An entry without valid data */
			ord = 0xFF; dp->blk_ofs = 0xFFFFFFFF;	/* Reset LFN sequence */
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (!(dp->fn[NSFLAG] & NS_NOLFN)) {
					if (c & LLEF) {		/* Is it start of LFN sequence? */
						sum = dp->dir[LDIR_Chksum];
						c &= (BYTE)~LLEF; ord = c;	/* LFN start order */
						dp->blk_ofs = dp->dptr;	/* Start offset of LFN */
					}
					/* Check validity of the LFN entry and compare it with given name */
					ord = (c == ord && sum == dp->dir[LDIR_Chksum] && cmp_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF;
				}
			} else {					/* An SFN entry is found */
				if (!ord && sum == sum_sfn(dp->dir)) break;	/* LFN matched? */
				if (!(dp->fn[NSFLAG] & NS_LOSS) && !mem_cmp(dp->dir, dp->fn, 11)) break;	/* SFN matched? */
				ord = 0xFF; dp->blk_ofs = 0xFFFFFFFF;	/* Reset LFN sequence */
			}
		}
#else		/* Non LFN configuration */
		dp->obj.attr = dp->dir[DIR_Attr] & AM_MASK;
		if (!(dp->dir[DIR_Attr] & AM_VOL) && !mem_cmp(dp->dir, dp->fn, 11)) break;	/* Is it a valid entry? */
#endif
		res = dir_next(dp, 0);	/* Next entry */
	} while (res == FR_OK);

	return res;
}


#if _FS_MINIMIZE <= 1 || _FS_RPATH >= 2
/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/

static
void get_fileinfo (		/* No return code */
	DIR* dp,			/* Pointer to the directory object */
	FILINFO* fno	 	/* Pointer to the file information to be filled */
)
{
	UINT16 i, j;
	TCHAR c;
	DWORD tm;
#if _USE_LFN != 0
	WORD w, lfv;
	FATFS *fs = dp->obj.fs;
#endif


	fno->fname[0] = 0;		/* Invaidate file info */
	if (!dp->sect) return;	/* Exit if read pointer has reached end of directory */

#if _USE_LFN != 0	/* LFN configuration */
#if _FS_EXFAT
	if (fs->fs_type == FS_EXFAT) {	/* On the exFAT volume */
		get_xdir_info(fs->dirbuf, fno);
		return;
	} else
#endif
	{	/* On the FAT12/16/32 volume */
		if (dp->blk_ofs != 0xFFFFFFFF) {	/* Get LFN if available */
			i = j = 0;
			while ((w = fs->lfnbuf[j++]) != 0) {	/* Get an LFN character */
#if !_LFN_UNICODE
				w = ff_convert(w, 0);		/* Unicode -> OEM */
				if (w == 0) { i = 0; break; }	/* No LFN if it could not be converted */
				if (_DF1S && w >= 0x100) {	/* Put 1st byte if it is a DBC (always false at SBCS cfg) */
					fno->fname[i++] = (char)(w >> 8);
				}
#endif
				if (i >= _MAX_LFN) { i = 0; break; }	/* No LFN if buffer overflow */
				fno->fname[i++] = (TCHAR)w;
			}
			fno->fname[i] = 0;	/* Terminate the LFN */
		}
	}

	i = j = 0;
	lfv = fno->fname[i];	/* LFN is exist if non-zero */
	while (i < 11) {		/* Copy name body and extension */
		c = (TCHAR)dp->dir[i++];
		if (c == ' ') continue;				/* Skip padding spaces */
		if (c == RDDEM) c = (TCHAR)DDEM;	/* Restore replaced DDEM character */
		if (i == 9) {						/* Insert a . if extension is exist */
			if (!lfv) fno->fname[j] = '.';
			fno->altname[j++] = '.';
		}
#if _LFN_UNICODE
		if (IsDBCS1(c) && i != 8 && i != 11 && IsDBCS2(dp->dir[i])) {
			c = c << 8 | dp->dir[i++];
		}
		c = ff_convert(c, 1);	/* OEM -> Unicode */
		if (!c) c = '?';
#endif
		fno->altname[j] = c;
		if (!lfv) {
			if (IsUpper(c) && (dp->dir[DIR_NTres] & (i >= 9 ? NS_EXT : NS_BODY))) {
				c += 0x20;			/* To lower */
			}
			fno->fname[j] = c;
		}
		j++;
	}
	if (!lfv) {
		fno->fname[j] = 0;
		if (!dp->dir[DIR_NTres]) j = 0;	/* Altname is no longer needed if neither LFN nor case info is exist. */
	}
	fno->altname[j] = 0;	/* Terminate the SFN */

#else	/* Non-LFN configuration */
	i = j = 0;
	while (i < 11) {		/* Copy name body and extension */
		c = (TCHAR)dp->dir[i++];
		if (c == ' ') continue;				/* Skip padding spaces */
		if (c == RDDEM) c = (TCHAR)DDEM;	/* Restore replaced DDEM character */
		if (i == 9) fno->fname[j++] = '.';	/* Insert a . if extension is exist */
		fno->fname[j++] = c;
	}
	fno->fname[j] = 0;
#endif

	fno->fattrib = dp->dir[DIR_Attr];				/* Attribute */
	fno->fsize = ld_dword(dp->dir + DIR_FileSize);	/* Size */
	tm = ld_dword(dp->dir + DIR_ModTime);			/* Timestamp */
	fno->ftime = (WORD)tm; fno->fdate = (WORD)(tm >> 16);
}

#endif /* _FS_MINIMIZE <= 1 || _FS_RPATH >= 2 */



#if _USE_FIND && _FS_MINIMIZE <= 1
/*-----------------------------------------------------------------------*/
/* Pattern matching                                                      */
/*-----------------------------------------------------------------------*/

static
WORD get_achar (		/* Get a character and advances ptr 1 or 2 */
	const TCHAR** ptr	/* Pointer to pointer to the SBCS/DBCS/Unicode string */
)
{
#if !_LFN_UNICODE
	WORD chr;

	chr = (BYTE)*(*ptr)++;					/* Get a byte */
	if (IsLower(chr)) chr -= 0x20;			/* To upper ASCII char */
#ifdef _EXCVT
	if (chr >= 0x80) chr = ExCvt[chr - 0x80];	/* To upper SBCS extended char */
#else
	if (IsDBCS1(chr) && IsDBCS2(**ptr)) {		/* Get DBC 2nd byte if needed */
		chr = chr << 8 | (BYTE)*(*ptr)++;
	}
#endif
	return chr;
#else
	return ff_wtoupper(*(*ptr)++);			/* Get a word and to upper */
#endif
}


static
int pattern_matching (	/* 0:not matched, 1:matched */
	const TCHAR* pat,	/* Matching pattern */
	const TCHAR* nam,	/* String to be tested */
	int skip,			/* Number of pre-skip chars (number of ?s) */
	int inf				/* Infinite search (* specified) */
)
{
	const TCHAR *pp, *np;
	WORD pc, nc;
	int nm, nx;


	while (skip--) {				/* Pre-skip name chars */
		if (!get_achar(&nam)) return 0;	/* Branch mismatched if less name chars */
	}
	if (!*pat && inf) return 1;		/* (short circuit) */

	do {
		pp = pat; np = nam;			/* Top of pattern and name to match */
		for (;;) {
			if (*pp == '?' || *pp == '*') {	/* Wildcard? */
				nm = nx = 0;
				do {				/* Analyze the wildcard chars */
					if (*pp++ == '?') nm++; else nx = 1;
				} while (*pp == '?' || *pp == '*');
				if (pattern_matching(pp, np, nm, nx)) return 1;	/* Test new branch (recurs upto number of wildcard blocks in the pattern) */
				nc = *np; break;	/* Branch mismatched */
			}
			pc = get_achar(&pp);	/* Get a pattern char */
			nc = get_achar(&np);	/* Get a name char */
			if (pc != nc) break;	/* Branch mismatched? */
			if (pc == 0) return 1;	/* Branch matched? (matched at end of both strings) */
		}
		get_achar(&nam);			/* nam++ */
	} while (inf && nc);			/* Retry until end of name if infinite search is specified */

	return 0;
}

#endif /* _USE_FIND && _FS_MINIMIZE <= 1 */



/*-----------------------------------------------------------------------*/
/* Pick a top segment and create the object name in directory form       */
/*-----------------------------------------------------------------------*/

static
FRESULT create_name (	/* FR_OK: successful, FR_INVALID_NAME: could not create */
	DIR* dp,			/* Pointer to the directory object */
	const TCHAR** path	/* Pointer to pointer to the segment in the path string */
)
{
#if _USE_LFN != 0	/* LFN configuration */
	BYTE b, cf;
	WORD w, *lfn;
	UINT16 i, ni, si, di;
	const TCHAR *p;

	/* Create LFN in Unicode */
	p = *path; lfn = dp->obj.fs->lfnbuf; si = di = 0;
	for (;;) {
		w = p[si++];					/* Get a character */
		if (w < ' ') break;				/* Break if end of the path name */
		if (w == '/' || w == '\\') {	/* Break if a separator is found */
			while (p[si] == '/' || p[si] == '\\') si++;	/* Skip duplicated separator if exist */
			break;
		}
		if (di >= _MAX_LFN) return FR_INVALID_NAME;	/* Reject too long name */
#if !_LFN_UNICODE
		w &= 0xFF;
		if (IsDBCS1(w)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			b = (BYTE)p[si++];			/* Get 2nd byte */
			w = (w << 8) + b;			/* Create a DBC */
			if (!IsDBCS2(b)) return FR_INVALID_NAME;	/* Reject invalid sequence */
		}
		w = ff_convert(w, 1);			/* Convert ANSI/OEM to Unicode */
		if (!w) return FR_INVALID_NAME;	/* Reject invalid code */
#endif
		if (w < 0x80 && chk_chr("\"*:<>\?|\x7F", w)) return FR_INVALID_NAME;	/* Reject illegal characters for LFN */
		lfn[di++] = w;					/* Store the Unicode character */
	}
	*path = &p[si];						/* Return pointer to the next segment */
	cf = (w < ' ') ? NS_LAST : 0;		/* Set last segment flag if end of the path */
#if _FS_RPATH != 0
	if ((di == 1 && lfn[di - 1] == '.') ||
		(di == 2 && lfn[di - 1] == '.' && lfn[di - 2] == '.')) {	/* Is this segment a dot name? */
		lfn[di] = 0;
		for (i = 0; i < 11; i++)		/* Create dot name for SFN entry */
			dp->fn[i] = (i < di) ? '.' : ' ';
		dp->fn[i] = cf | NS_DOT;		/* This is a dot entry */
		return FR_OK;
	}
#endif
	while (di) {						/* Snip off trailing spaces and dots if exist */
		w = lfn[di - 1];
		if (w != ' ' && w != '.') break;
		di--;
	}
	lfn[di] = 0;						/* LFN is created */
	if (di == 0) return FR_INVALID_NAME;	/* Reject nul name */

	/* Create SFN in directory form */
	mem_set(dp->fn, ' ', 11);
	for (si = 0; lfn[si] == ' ' || lfn[si] == '.'; si++) ;	/* Strip leading spaces and dots */
	if (si) cf |= NS_LOSS | NS_LFN;
	while (di && lfn[di - 1] != '.') di--;	/* Find extension (di<=si: no extension) */

	i = b = 0; ni = 8;
	for (;;) {
		w = lfn[si++];					/* Get an LFN character */
		if (!w) break;					/* Break on end of the LFN */
		if (w == ' ' || (w == '.' && si != di)) {	/* Remove spaces and dots */
			cf |= NS_LOSS | NS_LFN; continue;
		}

		if (i >= ni || si == di) {		/* Extension or end of SFN */
			if (ni == 11) {				/* Long extension */
				cf |= NS_LOSS | NS_LFN; break;
			}
			if (si != di) cf |= NS_LOSS | NS_LFN;	/* Out of 8.3 format */
			if (si > di) break;			/* No extension */
			si = di; i = 8; ni = 11;	/* Enter extension section */
			b <<= 2; continue;
		}

		if (w >= 0x80) {				/* Non ASCII character */
#ifdef _EXCVT
			w = ff_convert(w, 0);		/* Unicode -> OEM code */
			if (w) w = ExCvt[w - 0x80];	/* Convert extended character to upper (SBCS) */
#else
			w = ff_convert(ff_wtoupper(w), 0);	/* Upper converted Unicode -> OEM code */
#endif
			cf |= NS_LFN;				/* Force create LFN entry */
		}

		if (_DF1S && w >= 0x100) {		/* Is this DBC? (always false at SBCS cfg) */
			if (i >= ni - 1) {
				cf |= NS_LOSS | NS_LFN; i = ni; continue;
			}
			dp->fn[i++] = (BYTE)(w >> 8);
		} else {						/* SBC */
			if (!w || chk_chr("+,;=[]", w)) {	/* Replace illegal characters for SFN */
				w = '_'; cf |= NS_LOSS | NS_LFN;/* Lossy conversion */
			} else {
				if (IsUpper(w)) {		/* ASCII large capital */
					b |= 2;
				} else {
					if (IsLower(w)) {	/* ASCII small capital */
						b |= 1; w -= 0x20;
					}
				}
			}
		}
		dp->fn[i++] = (BYTE)w;
	}

	if (dp->fn[0] == DDEM) dp->fn[0] = RDDEM;	/* If the first character collides with DDEM, replace it with RDDEM */

	if (ni == 8) b <<= 2;
	if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03) cf |= NS_LFN;	/* Create LFN entry when there are composite capitals */
	if (!(cf & NS_LFN)) {						/* When LFN is in 8.3 format without extended character, NT flags are created */
		if ((b & 0x03) == 0x01) cf |= NS_EXT;	/* NT flag (Extension has only small capital) */
		if ((b & 0x0C) == 0x04) cf |= NS_BODY;	/* NT flag (Filename has only small capital) */
	}

	dp->fn[NSFLAG] = cf;	/* SFN is created */

	return FR_OK;


#else	/* _USE_LFN != 0 : Non-LFN configuration */
	BYTE c, d, *sfn;
	UINT16 ni, si, i;
	const char *p;

	/* Create file name in directory form */
	p = *path; sfn = dp->fn;
	mem_set(sfn, ' ', 11);
	si = i = 0; ni = 8;
#if _FS_RPATH != 0
	if (p[si] == '.') { /* Is this a dot entry? */
		for (;;) {
			c = (BYTE)p[si++];
			if (c != '.' || si >= 3) break;
			sfn[i++] = c;
		}
		if (c != '/' && c != '\\' && c > ' ') return FR_INVALID_NAME;
		*path = p + si;								/* Return pointer to the next segment */
		sfn[NSFLAG] = (c <= ' ') ? NS_LAST | NS_DOT : NS_DOT;	/* Set last segment flag if end of the path */
		return FR_OK;
	}
#endif
	for (;;) {
		c = (BYTE)p[si++];
		if (c <= ' ') break; 			/* Break if end of the path name */
		if (c == '/' || c == '\\') {	/* Break if a separator is found */
			while (p[si] == '/' || p[si] == '\\') si++;	/* Skip duplicated separator if exist */
			break;
		}
		if (c == '.' || i >= ni) {		/* End of body or over size? */
			if (ni == 11 || c != '.') return FR_INVALID_NAME;	/* Over size or invalid dot */
			i = 8; ni = 11;				/* Goto extension */
			continue;
		}
		if (c >= 0x80) {				/* Extended character? */
#ifdef _EXCVT
			c = ExCvt[c - 0x80];		/* To upper extended characters (SBCS cfg) */
#else
#if !_DF1S
			return FR_INVALID_NAME;		/* Reject extended characters (ASCII only cfg) */
#endif
#endif
		}
		if (IsDBCS1(c)) {				/* Check if it is a DBC 1st byte (always false at SBCS cfg.) */
			d = (BYTE)p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(d) || i >= ni - 1) return FR_INVALID_NAME;	/* Reject invalid DBC */
			sfn[i++] = c;
			sfn[i++] = d;
		} else {						/* SBC */
			if (chk_chr("\"*+,:;<=>\?[]|\x7F", c)) return FR_INVALID_NAME;	/* Reject illegal chrs for SFN */
			if (IsLower(c)) c -= 0x20;	/* To upper */
			sfn[i++] = c;
		}
	}
	*path = p + si;						/* Return pointer to the next segment */
	if (i == 0) return FR_INVALID_NAME;	/* Reject nul string */

	if (sfn[0] == DDEM) sfn[0] = RDDEM;	/* If the first character collides with DDEM, replace it with RDDEM */
	sfn[NSFLAG] = (c <= ' ') ? NS_LAST : 0;		/* Set last segment flag if end of the path */

	return FR_OK;
#endif /* _USE_LFN != 0 */
}




/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

static
FRESULT follow_path (	/* FR_OK(0): successful, !=0: error code */
	DIR* dp,			/* Directory object to return last directory and found object */
	const TCHAR* path	/* Full-path string to find a file or directory */
)
{
	FRESULT res;
	BYTE ns;
	_FDID *obj = &dp->obj;
	FATFS *fs = obj->fs;


#if _FS_RPATH != 0
	if (*path != '/' && *path != '\\') {	/* Without heading separator */
		obj->sclust = fs->cdir;				/* Start from the current directory */
	} else
#endif
	{										/* With heading separator */
		while (*path == '/' || *path == '\\') path++;	/* Strip heading separator */
		obj->sclust = 0;					/* Start from the root directory */
	}
#if _FS_EXFAT && _FS_RPATH != 0
	if (fs->fs_type == FS_EXFAT && obj->sclust) {	/* Retrieve the sub-directory status if needed */
		DIR dj;

		obj->c_scl = fs->cdc_scl;
		obj->c_size = fs->cdc_size;
		obj->c_ofs = fs->cdc_ofs;
		res = load_obj_dir(&dj, obj);
		if (res != FR_OK) return res;
		obj->objsize = ld_dword(fs->dirbuf + XDIR_FileSize);
		obj->stat = fs->dirbuf[XDIR_GenFlags] & 2;
	}
#endif

	if ((UINT16)*path < ' ') {				/* Null path name is the origin directory itself */
		dp->fn[NSFLAG] = NS_NONAME;
		res = dir_sdi(dp, 0);

	} else {								/* Follow path */
		for (;;) {
			res = create_name(dp, &path);	/* Get a segment name of the path */
			if (res != FR_OK) break;
			res = dir_find(dp);				/* Find an object with the segment name */
			ns = dp->fn[NSFLAG];
			if (res != FR_OK) {				/* Failed to find the object */
				if (res == FR_NO_FILE) {	/* Object is not found */
					if (_FS_RPATH && (ns & NS_DOT)) {	/* If dot entry is not exist, stay there */
						if (!(ns & NS_LAST)) continue;	/* Continue to follow if not last segment */
						dp->fn[NSFLAG] = NS_NONAME;
						res = FR_OK;
					} else {							/* Could not find the object */
						if (!(ns & NS_LAST)) res = FR_NO_PATH;	/* Adjust error code if not last segment */
					}
				}
				break;
			}
			if (ns & NS_LAST) break;			/* Last segment matched. Function completed. */
			/* Get into the sub-directory */
			if (!(obj->attr & AM_DIR)) {		/* It is not a sub-directory and cannot follow */
				res = FR_NO_PATH; break;
			}
#if _FS_EXFAT
			if (fs->fs_type == FS_EXFAT) {
				obj->c_scl = obj->sclust;		/* Save containing directory information for next dir */
				obj->c_size = ((DWORD)obj->objsize & 0xFFFFFF00) | obj->stat;
				obj->c_ofs = dp->blk_ofs;
				obj->sclust = ld_dword(fs->dirbuf + XDIR_FstClus);	/* Open next directory */
				obj->stat = fs->dirbuf[XDIR_GenFlags] & 2;
				obj->objsize = ld_qword(fs->dirbuf + XDIR_FileSize);
			} else
#endif
			{
				obj->sclust = ld_clust(fs, fs->win + dp->dptr % SS(fs));	/* Open next directory */
			}
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Get logical drive number from path name                               */
/*-----------------------------------------------------------------------*/

static
int get_ldnumber (		/* Returns logical drive number (-1:invalid drive) */
	const TCHAR** path	/* Pointer to pointer to the path name */
)
{
	const TCHAR *tp, *tt;
	UINT16 i;
	int vol = -1;
#if _STR_VOLUME_ID		/* Find string drive id */
	static const char* const str[] = {_VOLUME_STRS};
	const char *sp;
	char c;
	TCHAR tc;
#endif


	if (*path) {	/* If the pointer is not a null */
		for (tt = *path; (UINT16)*tt >= (_USE_LFN ? ' ' : '!') && *tt != ':'; tt++) ;	/* Find ':' in the path */
		if (*tt == ':') {	/* If a ':' is exist in the path name */
			tp = *path;
			i = *tp++ - '0'; 
			if (i < 10 && tp == tt) {	/* Is there a numeric drive id? */
				if (i < _VOLUMES) {	/* If a drive id is found, get the value and strip it */
					vol = (int)i;
					*path = ++tt;
				}
			}
#if _STR_VOLUME_ID
			 else {	/* No numeric drive number, find string drive id */
				i = 0; tt++;
				do {
					sp = str[i]; tp = *path;
					do {	/* Compare a string drive id with path name */
						c = *sp++; tc = *tp++;
						if (IsLower(tc)) tc -= 0x20;
					} while (c && (TCHAR)c == tc);
				} while ((c || tp != tt) && ++i < _VOLUMES);	/* Repeat for each id until pattern match */
				if (i < _VOLUMES) {	/* If a drive id is found, get the value and strip it */
					vol = (int)i;
					*path = tt;
				}
			}
#endif
			return vol;
		}
#if _FS_RPATH != 0 && _VOLUMES >= 2
		vol = CurrVol;	/* Current drive */
#else
		vol = 0;		/* Drive 0 */
#endif
	}
	return vol;
}




/*-----------------------------------------------------------------------*/
/* Load a sector and check if it is an FAT boot sector                   */
/*-----------------------------------------------------------------------*/

static
BYTE check_fs (	/* 0:FAT, 1:exFAT, 2:Valid BS but not FAT, 3:Not a BS, 4:Disk error */
	FATFS* fs,	/* File system object */
	DWORD sect	/* Sector# (lba) to check if it is an FAT-VBR or not */
)
{
	fs->wflag = 0; fs->winsect = 0xFFFFFFFF;		/* Invaidate window */
	if (move_window(fs, sect) != FR_OK) return 4;	/* Load boot record */

	if (ld_word(fs->win + BS_55AA) != 0xAA55) return 3;	/* Check boot record signature (always placed at offset 510 even if the sector size is >512) */

	if (fs->win[BS_JmpBoot] == 0xE9 || (fs->win[BS_JmpBoot] == 0xEB && fs->win[BS_JmpBoot + 2] == 0x90)) {
		if ((ld_dword(fs->win + BS_FilSysType) & 0xFFFFFF) == 0x544146) return 0;	/* Check "FAT" string */
		if (ld_dword(fs->win + BS_FilSysType32) == 0x33544146) return 0;			/* Check "FAT3" string */
	}
#if _FS_EXFAT
	if (!mem_cmp(fs->win + BS_JmpBoot, "\xEB\x76\x90" "EXFAT   ", 11)) return 1;
#endif
	return 2;
}




/*-----------------------------------------------------------------------*/
/* Find logical drive and check if the volume is mounted                 */
/*-----------------------------------------------------------------------*/

static
FRESULT find_volume (	/* FR_OK(0): successful, !=0: any error occurred */
	const TCHAR** path,	/* Pointer to pointer to the path name (drive number) */
	FATFS** rfs,		/* Pointer to pointer to the found file system object */
	BYTE mode			/* !=0: Check write protection for write access */
)
{
	BYTE fmt, *pt;
	int vol;
	DSTATUS stat;
	DWORD bsect, fasize, tsect, sysect, nclst, szbfat, br[4];
	WORD nrsv;
	FATFS *fs;
	UINT16 i;


	/* Get logical drive number */
	*rfs = 0;
	vol = get_ldnumber(path);
	if (vol < 0) return FR_INVALID_DRIVE;

	/* Check if the file system object is valid or not */
	fs = FatFs[vol];					/* Get pointer to the file system object */
	if (!fs) return FR_NOT_ENABLED;		/* Is the file system object available? */

	ENTER_FF(fs);						/* Lock the volume */
	*rfs = fs;							/* Return pointer to the file system object */

	mode &= (BYTE)~FA_READ;				/* Desired access mode, write access or not */
	if (fs->fs_type) {					/* If the volume has been mounted */
		stat = disk_status(fs->drv);
		if (!(stat & STA_NOINIT)) {		/* and the physical drive is kept initialized */
			if (!_FS_READONLY && mode && (stat & STA_PROTECT)) {	/* Check write protection if needed */
				return FR_WRITE_PROTECTED;
			}
			return FR_OK;				/* The file system object is valid */
		}
	}

	/* The file system object is not valid. */
	/* Following code attempts to mount the volume. (analyze BPB and initialize the fs object) */

	fs->fs_type = 0;					/* Clear the file system object */
	fs->drv = LD2PD(vol);				/* Bind the logical drive and a physical drive */
	stat = disk_initialize(fs->drv);	/* Initialize the physical drive */
	if (stat & STA_NOINIT) { 			/* Check if the initialization succeeded */
		return FR_NOT_READY;			/* Failed to initialize due to no medium or hard error */
	}
	if (!_FS_READONLY && mode && (stat & STA_PROTECT)) { /* Check disk write protection if needed */
		return FR_WRITE_PROTECTED;
	}
#if _MAX_SS != _MIN_SS						/* Get sector size (multiple sector size cfg only) */
	if (disk_ioctl(fs->drv, GET_SECTOR_SIZE, &SS(fs)) != RES_OK) return FR_DISK_ERR;
	if (SS(fs) > _MAX_SS || SS(fs) < _MIN_SS || (SS(fs) & (SS(fs) - 1))) return FR_DISK_ERR;
#endif
	/* Find an FAT partition on the drive. Supports only generic partitioning, FDISK and SFD. */
	bsect = 0;
	fmt = check_fs(fs, bsect);			/* Load sector 0 and check if it is an FAT-VBR as SFD */
	if (fmt == 2 || (fmt < 2 && LD2PT(vol) != 0)) {	/* Not an FAT-VBR or forced partition number */
		for (i = 0; i < 4; i++) {			/* Get partition offset */
			pt = fs->win + (MBR_Table + i * SZ_PTE);
			br[i] = pt[PTE_System] ? ld_dword(pt + PTE_StLba) : 0;
		}
		i = LD2PT(vol);						/* Partition number: 0:auto, 1-4:forced */
		if (i) i--;
		do {								/* Find an FAT volume */
			bsect = br[i];
			fmt = bsect ? check_fs(fs, bsect) : 3;	/* Check the partition */
		} while (!LD2PT(vol) && fmt >= 2 && ++i < 4);
	}
	if (fmt == 4) return FR_DISK_ERR;		/* An error occured in the disk I/O layer */
	if (fmt >= 2) return FR_NO_FILESYSTEM;	/* No FAT volume is found */

	/* An FAT volume is found. Following code initializes the file system object */

#if _FS_EXFAT
	if (fmt == 1) {
		QWORD maxlba;

		for (i = BPB_ZeroedEx; i < BPB_ZeroedEx + 53 && fs->win[i] == 0; i++) ;	/* Check zero filler */
		if (i < BPB_ZeroedEx + 53) return FR_NO_FILESYSTEM;

		if (ld_word(fs->win + BPB_FSVerEx) != 0x100) return FR_NO_FILESYSTEM;	/* Check exFAT revision (Must be 1.0) */

		if (1 << fs->win[BPB_BytsPerSecEx] != SS(fs))	/* (BPB_BytsPerSecEx must be equal to the physical sector size) */
			return FR_NO_FILESYSTEM;

		maxlba = ld_qword(fs->win + BPB_TotSecEx) + bsect;	/* Last LBA + 1 of the volume */
		if (maxlba >= 0x100000000) return FR_NO_FILESYSTEM;	/* (It cannot be handled in 32-bit LBA) */

		fs->fsize = ld_dword(fs->win + BPB_FatSzEx);	/* Number of sectors per FAT */

		fs->n_fats = fs->win[BPB_NumFATsEx];			/* Number of FATs */
		if (fs->n_fats != 1) return FR_NO_FILESYSTEM;	/* (Supports only 1 FAT) */

		fs->csize = 1 << fs->win[BPB_SecPerClusEx];		/* Cluster size */
		if (fs->csize == 0)	return FR_NO_FILESYSTEM;	/* (Must be 1..32768) */

		nclst = ld_dword(fs->win + BPB_NumClusEx);		/* Number of clusters */
		if (nclst > MAX_EXFAT) return FR_NO_FILESYSTEM;	/* (Too many clusters) */
		fs->n_fatent = nclst + 2;

		/* Boundaries and Limits */
		fs->volbase = bsect;
		fs->database = bsect + ld_dword(fs->win + BPB_DataOfsEx);
		fs->fatbase = bsect + ld_dword(fs->win + BPB_FatOfsEx);
		if (maxlba < (QWORD)fs->database + nclst * fs->csize) return FR_NO_FILESYSTEM;	/* (Volume size must not be smaller than the size requiered) */
		fs->dirbase = ld_dword(fs->win + BPB_RootClusEx);

		/* Check if bitmap location is in assumption (at the first cluster) */
		if (move_window(fs, clust2sect(fs, fs->dirbase)) != FR_OK) return FR_DISK_ERR;
		for (i = 0; i < SS(fs); i += SZDIRE) {
			if (fs->win[i] == 0x81 && ld_dword(fs->win + i + 20) == 2) break;	/* 81 entry with cluster #2? */
		}
		if (i == SS(fs)) return FR_NO_FILESYSTEM;

		fmt = FS_EXFAT;			/* FAT sub-type */
	} else
#endif	/* _FS_EXFAT */
	{
		if (ld_word(fs->win + BPB_BytsPerSec) != SS(fs)) return FR_NO_FILESYSTEM;	/* (BPB_BytsPerSec must be equal to the physical sector size) */

		fasize = ld_word(fs->win + BPB_FATSz16);			/* Number of sectors per FAT */
		if (fasize == 0) fasize = ld_dword(fs->win + BPB_FATSz32);
		fs->fsize = fasize;

		fs->n_fats = fs->win[BPB_NumFATs];					/* Number of FATs */
		if (fs->n_fats != 1 && fs->n_fats != 2) return FR_NO_FILESYSTEM;	/* (Must be 1 or 2) */
		fasize *= fs->n_fats;								/* Number of sectors for FAT area */

		fs->csize = fs->win[BPB_SecPerClus];				/* Cluster size */
		if (fs->csize == 0 || (fs->csize & (fs->csize - 1))) return FR_NO_FILESYSTEM;	/* (Must be power of 2) */

		fs->n_rootdir = ld_word(fs->win + BPB_RootEntCnt);	/* Number of root directory entries */
		if (fs->n_rootdir % (SS(fs) / SZDIRE)) return FR_NO_FILESYSTEM;	/* (Must be sector aligned) */

		tsect = ld_word(fs->win + BPB_TotSec16);			/* Number of sectors on the volume */
		if (tsect == 0) tsect = ld_dword(fs->win + BPB_TotSec32);

		nrsv = ld_word(fs->win + BPB_RsvdSecCnt);			/* Number of reserved sectors */
		if (nrsv == 0) return FR_NO_FILESYSTEM;				/* (Must not be 0) */

		/* Determine the FAT sub type */
		sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / SZDIRE);	/* RSV + FAT + DIR */
		if (tsect < sysect) return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
		nclst = (tsect - sysect) / fs->csize;				/* Number of clusters */
		if (nclst == 0) return FR_NO_FILESYSTEM;			/* (Invalid volume size) */
		fmt = FS_FAT32;
		if (nclst <= MAX_FAT16) fmt = FS_FAT16;
		if (nclst <= MAX_FAT12) fmt = FS_FAT12;

		/* Boundaries and Limits */
		fs->n_fatent = nclst + 2;							/* Number of FAT entries */
		fs->volbase = bsect;								/* Volume start sector */
		fs->fatbase = bsect + nrsv; 						/* FAT start sector */
		fs->database = bsect + sysect;						/* Data start sector */
		if (fmt == FS_FAT32) {
			if (ld_word(fs->win + BPB_FSVer32) != 0) return FR_NO_FILESYSTEM;	/* (Must be FAT32 revision 0.0) */
			if (fs->n_rootdir) return FR_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
			fs->dirbase = ld_dword(fs->win + BPB_RootClus32);	/* Root directory start cluster */
			szbfat = fs->n_fatent * 4;						/* (Needed FAT size) */
		} else {
			if (fs->n_rootdir == 0)	return FR_NO_FILESYSTEM;/* (BPB_RootEntCnt must not be 0) */
			fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
			szbfat = (fmt == FS_FAT16) ?					/* (Needed FAT size) */
				fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
		}
		if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs)) return FR_NO_FILESYSTEM;	/* (BPB_FATSz must not be less than the size needed) */

	}

	fs->fs_type = fmt;	/* FAT sub-type */
	fs->id = ++Fsid;	/* File system mount ID */
#if _USE_LFN == 1
	fs->lfnbuf = LfnBuf;	/* Static LFN working buffer */
#if _FS_EXFAT
	fs->dirbuf = DirBuf;	/* Static directory block working buuffer */
#endif
#endif
#if _FS_RPATH != 0
	fs->cdir = 0;		/* Initialize current directory */
#endif
#if _FS_LOCK != 0		/* Clear file lock semaphores */
	clear_lock(fs);
#endif
	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Check if the file/directory object is valid or not                    */
/*-----------------------------------------------------------------------*/

static
FRESULT validate (	/* Returns FR_OK or FR_INVALID_OBJECT */
	_FDID* obj,		/* Pointer to the _OBJ, the 1st member in the FIL/DIR object, to check validity */
	FATFS** fs		/* Pointer to pointer to the owner file system object to return */
)
{
	FRESULT res;


	if (!obj || !obj->fs || !obj->fs->fs_type || obj->fs->id != obj->id || (disk_status(obj->fs->drv) & STA_NOINIT)) {
		*fs = 0;				/* The object is invalid */
		res = FR_INVALID_OBJECT;
	} else {
		*fs = obj->fs;			/* Owner file sytem object */
		ENTER_FF(obj->fs);		/* Lock file system */
		res = FR_OK;
	}
	return res;
}




/*---------------------------------------------------------------------------

   Public Functions (FatFs API)

----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
	FATFS* fs,			/* Pointer to the file system object (NULL:unmount)*/
	const TCHAR* path,	/* Logical drive number to be mounted/unmounted */
	BYTE opt			/* Mode option 0:Do not mount (delayed mount), 1:Mount immediately */
)
{
	FATFS *cfs;
	int vol;
	FRESULT res;
	const TCHAR *rp = path;


	/* Get logical drive number */
	vol = get_ldnumber(&rp);
	if (vol < 0) return FR_INVALID_DRIVE;
	cfs = FatFs[vol];					/* Pointer to fs object */

	if (cfs) {
#if _FS_LOCK != 0
		clear_lock(cfs);
#endif
#if _FS_REENTRANT						/* Discard sync object of the current volume */
		if (!ff_del_syncobj(cfs->sobj)) return FR_INT_ERR;
#endif
		cfs->fs_type = 0;				/* Clear old fs object */
	}

	if (fs) {
		fs->fs_type = 0;				/* Clear new fs object */
#if _FS_REENTRANT						/* Create sync object for the new volume */
		if (!ff_cre_syncobj((BYTE)vol, &fs->sobj)) return FR_INT_ERR;
#endif
	}
	FatFs[vol] = fs;					/* Register new fs object */

	if (!fs || opt != 1) return FR_OK;	/* Do not mount now, it will be mounted later */

	res = find_volume(&path, &fs, 0);	/* Force mounted the volume */
	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
	FIL* fp,			/* Pointer to the blank file object */
	const TCHAR* path,	/* Pointer to the file name */
	BYTE mode			/* Access mode and file open mode flags */
)
{
	FRESULT res;
	DIR dj;
	FATFS *fs;
	DEF_NAMBUF


	if (!fp) return FR_INVALID_OBJECT;

	/* Get logical drive */
	mode &= _FS_READONLY ? FA_READ : FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_CREATE_NEW | FA_OPEN_ALWAYS | FA_OPEN_APPEND | FA_SEEKEND;
	res = find_volume(&path, &fs, mode);
	if (res == FR_OK) {
		dj.obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(&dj, path);	/* Follow the file path */

		if (res == FR_OK) {
#if _FS_EXFAT
			if (fs->fs_type == FS_EXFAT) {
				fp->obj.sclust = ld_dword(fs->dirbuf + XDIR_FstClus);		/* Get allocation info */
				fp->obj.objsize = ld_qword(fs->dirbuf + XDIR_FileSize);
				fp->obj.stat = fs->dirbuf[XDIR_GenFlags] & 2;
				fp->obj.c_scl = dj.obj.sclust;
				fp->obj.c_size = ((DWORD)dj.obj.objsize & 0xFFFFFF00) | dj.obj.stat;
				fp->obj.c_ofs = dj.blk_ofs;
			} else
#endif
			{
				fp->obj.sclust = ld_clust(fs, dj.dir);				/* Get allocation info */
				fp->obj.objsize = ld_dword(dj.dir + DIR_FileSize);
			}
#if _USE_FASTSEEK
			fp->cltbl = 0;			/* Disable fast seek mode */
#endif
			fp->obj.fs = fs;	 	/* Validate the file object */
			fp->obj.id = fs->id;
			fp->flag = mode;		/* Set file access mode */
			fp->err = 0;			/* Clear error flag */
			fp->sect = 0;			/* Invalidate current data sector */
			fp->fptr = 0;			/* Set file pointer top of the file */
		}

		FREE_NAMBUF();
	}

	if (res != FR_OK) fp->obj.fs = 0;	/* Invalidate file object on error */

	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
	FIL* fp, 	/* Pointer to the file object */
	void* buff,	/* Pointer to data buffer */
	UINT16 btr,	/* Number of bytes to read */
	UINT16* br	/* Pointer to number of bytes read */
)
{
	FRESULT res;
	FATFS *fs;
	DWORD clst, sect;
	FSIZE_t remain;
	UINT16 rcnt, cc, csect;
	BYTE *rbuff = (BYTE*)buff;


	*br = 0;	/* Clear read byte counter */
	res = validate(&fp->obj, &fs);				/* Check validity of the file object */
	if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK) LEAVE_FF(fs, res);	/* Check validity */
	if (!(fp->flag & FA_READ)) LEAVE_FF(fs, FR_DENIED); /* Check access mode */
	remain = fp->obj.objsize - fp->fptr;
	if (btr > remain) btr = (UINT16)remain;		/* Truncate btr by remaining bytes */

	for ( ;  btr;								/* Repeat until all data read */
		rbuff += rcnt, fp->fptr += rcnt, *br += rcnt, btr -= rcnt) {
		if (fp->fptr % SS(fs) == 0) {			/* On the sector boundary? */
			csect = (UINT16)(fp->fptr / SS(fs) & (fs->csize - 1));	/* Sector offset in the cluster */
			if (csect == 0) {					/* On the cluster boundary? */
				if (fp->fptr == 0) {			/* On the top of the file? */
					clst = fp->obj.sclust;		/* Follow cluster chain from the origin */
				} else {						/* Middle or end of the file */
#if _USE_FASTSEEK
					if (fp->cltbl) {
						clst = clmt_clust(fp, fp->fptr);	/* Get cluster# from the CLMT */
					} else
#endif
					{
						clst = get_fat(&fp->obj, fp->clust);	/* Follow cluster chain on the FAT */
					}
				}
				if (clst < 2) ABORT(fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
				fp->clust = clst;				/* Update current cluster */
			}
			sect = clust2sect(fs, fp->clust);	/* Get current sector */
			if (!sect) ABORT(fs, FR_INT_ERR);
			sect += csect;
			cc = btr / SS(fs);					/* When remaining bytes >= sector size, */
			if (cc) {							/* Read maximum contiguous sectors directly */
				if (csect + cc > fs->csize) {	/* Clip at cluster boundary */
					cc = fs->csize - csect;
				}
				if (disk_read(fs->drv, rbuff, sect, cc) != RES_OK) ABORT(fs, FR_DISK_ERR);
				rcnt = SS(fs) * cc;				/* Number of bytes transferred */
				continue;
			}
#if !_FS_TINY
			if (fp->sect != sect) {			/* Load data sector if not in cache */
				if (disk_read(fs->drv, fp->buf, sect, 1) != RES_OK)	ABORT(fs, FR_DISK_ERR);	/* Fill sector cache */
			}
#endif
			fp->sect = sect;
		}
		rcnt = SS(fs) - (UINT16)fp->fptr % SS(fs);	/* Number of bytes left in the sector */
		if (rcnt > btr) rcnt = btr;					/* Clip it by btr if needed */
#if _FS_TINY
		if (move_window(fs, fp->sect) != FR_OK) ABORT(fs, FR_DISK_ERR);	/* Move sector window */
		mem_cpy(rbuff, fs->win + fp->fptr % SS(fs), rcnt);	/* Extract partial sector */
#else
		mem_cpy(rbuff, fp->buf + fp->fptr % SS(fs), rcnt);	/* Extract partial sector */
#endif
	}

	LEAVE_FF(fs, FR_OK);
}

/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
	FIL* fp		/* Pointer to the file object to be closed */
)
{
	FRESULT res;
	FATFS *fs;

	{
		res = validate(&fp->obj, &fs);	/* Lock volume */
		if (res == FR_OK) {
#if _FS_LOCK != 0
			res = dec_lock(fp->obj.lockid);	/* Decrement file open counter */
			if (res == FR_OK)
#endif
			{
				fp->obj.fs = 0;			/* Invalidate file object */
			}
#if _FS_REENTRANT
			unlock_fs(fs, FR_OK);		/* Unlock volume */
#endif
		}
	}
	return res;
}




#if _FS_RPATH >= 1
/*-----------------------------------------------------------------------*/
/* Change Current Directory or Current Drive, Get Current Directory      */
/*-----------------------------------------------------------------------*/

#if _VOLUMES >= 2
FRESULT f_chdrive (
	const TCHAR* path		/* Drive number */
)
{
	int vol;


	/* Get logical drive number */
	vol = get_ldnumber(&path);
	if (vol < 0) return FR_INVALID_DRIVE;

	CurrVol = (BYTE)vol;	/* Set it as current volume */

	return FR_OK;
}
#endif


FRESULT f_chdir (
	const TCHAR* path	/* Pointer to the directory path */
)
{
	FRESULT res;
	DIR dj;
	FATFS *fs;
	DEF_NAMBUF

	/* Get logical drive */
	res = find_volume(&path, &fs, 0);
	if (res == FR_OK) {
		dj.obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(&dj, path);		/* Follow the path */
		if (res == FR_OK) {					/* Follow completed */
			if (dj.fn[NSFLAG] & NS_NONAME) {
				fs->cdir = dj.obj.sclust;	/* It is the start directory itself */
#if _FS_EXFAT
				if (fs->fs_type == FS_EXFAT) {
					fs->cdc_scl = dj.obj.c_scl;
					fs->cdc_size = dj.obj.c_size;
					fs->cdc_ofs = dj.obj.c_ofs;
				}
#endif
			} else {
				if (dj.obj.attr & AM_DIR) {	/* It is a sub-directory */
#if _FS_EXFAT
					if (fs->fs_type == FS_EXFAT) {
						fs->cdir = ld_dword(fs->dirbuf + XDIR_FstClus);		/* Sub-directory cluster */
						fs->cdc_scl = dj.obj.sclust;						/* Save containing directory information */
						fs->cdc_size = ((DWORD)dj.obj.objsize & 0xFFFFFF00) | dj.obj.stat;
						fs->cdc_ofs = dj.blk_ofs;
					} else
#endif
					{
						fs->cdir = ld_clust(fs, dj.dir);					/* Sub-directory cluster */
					}
				} else {
					res = FR_NO_PATH;		/* Reached but a file */
				}
			}
		}
		FREE_NAMBUF();
		if (res == FR_NO_FILE) res = FR_NO_PATH;
	}

	LEAVE_FF(fs, res);
}


#if _FS_RPATH >= 2
FRESULT f_getcwd (
	TCHAR* buff,	/* Pointer to the directory path */
	UINT16 len		/* Size of path */
)
{
	FRESULT res;
	DIR dj;
	FATFS *fs;
	UINT16 i, n;
	DWORD ccl;
	TCHAR *tp;
	FILINFO fno;
	DEF_NAMBUF


	*buff = 0;
	/* Get logical drive */
	res = find_volume((const TCHAR**)&buff, &fs, 0);	/* Get current volume */
	if (res == FR_OK) {
		dj.obj.fs = fs;
		INIT_NAMBUF(fs);
		i = len;			/* Bottom of buffer (directory stack base) */
		if (!_FS_EXFAT || fs->fs_type != FS_EXFAT) {	/* (Cannot do getcwd on exFAT and returns root path) */
			dj.obj.sclust = fs->cdir;				/* Start to follow upper directory from current directory */
			while ((ccl = dj.obj.sclust) != 0) {	/* Repeat while current directory is a sub-directory */
				res = dir_sdi(&dj, 1 * SZDIRE);	/* Get parent directory */
				if (res != FR_OK) break;
				res = move_window(fs, dj.sect);
				if (res != FR_OK) break;
				dj.obj.sclust = ld_clust(fs, dj.dir);	/* Goto parent directory */
				res = dir_sdi(&dj, 0);
				if (res != FR_OK) break;
				do {							/* Find the entry links to the child directory */
					res = dir_read(&dj, 0);
					if (res != FR_OK) break;
					if (ccl == ld_clust(fs, dj.dir)) break;	/* Found the entry */
					res = dir_next(&dj, 0);
				} while (res == FR_OK);
				if (res == FR_NO_FILE) res = FR_INT_ERR;/* It cannot be 'not found'. */
				if (res != FR_OK) break;
				get_fileinfo(&dj, &fno);		/* Get the directory name and push it to the buffer */
				for (n = 0; fno.fname[n]; n++) ;
				if (i < n + 3) {
					res = FR_NOT_ENOUGH_CORE; break;
				}
				while (n) buff[--i] = fno.fname[--n];
				buff[--i] = '/';
			}
		}
		tp = buff;
		if (res == FR_OK) {
#if _VOLUMES >= 2
			*tp++ = '0' + CurrVol;			/* Put drive number */
			*tp++ = ':';
#endif
			if (i == len) {					/* Root-directory */
				*tp++ = '/';
			} else {						/* Sub-directroy */
				do		/* Add stacked path str */
					*tp++ = buff[i++];
				while (i < len);
			}
		}
		*tp = 0;
		FREE_NAMBUF();
	}

	LEAVE_FF(fs, res);
}

#endif /* _FS_RPATH >= 2 */
#endif /* _FS_RPATH >= 1 */



#if _FS_MINIMIZE <= 2
/*-----------------------------------------------------------------------*/
/* Seek File R/W Pointer                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_lseek (
	FIL* fp,		/* Pointer to the file object */
	FSIZE_t ofs		/* File pointer from top of file */
)
{
	FRESULT res;
	FATFS *fs;
	DWORD clst, bcs, nsect;
	FSIZE_t ifptr;
#if _USE_FASTSEEK
	DWORD cl, pcl, ncl, tcl, dsc, tlen, ulen, *tbl;
#endif

	res = validate(&fp->obj, &fs);		/* Check validity of the file object */
	if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK) LEAVE_FF(fs, res);	/* Check validity */
#if _USE_FASTSEEK
	if (fp->cltbl) {	/* Fast seek */
		if (ofs == CREATE_LINKMAP) {	/* Create CLMT */
			tbl = fp->cltbl;
			tlen = *tbl++; ulen = 2;	/* Given table size and required table size */
			cl = fp->obj.sclust;		/* Origin of the chain */
			if (cl) {
				do {
					/* Get a fragment */
					tcl = cl; ncl = 0; ulen += 2;	/* Top, length and used items */
					do {
						pcl = cl; ncl++;
						cl = get_fat(&fp->obj, cl);
						if (cl <= 1) ABORT(fs, FR_INT_ERR);
						if (cl == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
					} while (cl == pcl + 1);
					if (ulen <= tlen) {		/* Store the length and top of the fragment */
						*tbl++ = ncl; *tbl++ = tcl;
					}
				} while (cl < fs->n_fatent);	/* Repeat until end of chain */
			}
			*fp->cltbl = ulen;	/* Number of items used */
			if (ulen <= tlen) {
				*tbl = 0;		/* Terminate table */
			} else {
				res = FR_NOT_ENOUGH_CORE;	/* Given table size is smaller than required */
			}
		} else {						/* Fast seek */
			if (ofs > fp->obj.objsize) ofs = fp->obj.objsize;	/* Clip offset at the file size */
			fp->fptr = ofs;				/* Set file pointer */
			if (ofs) {
				fp->clust = clmt_clust(fp, ofs - 1);
				dsc = clust2sect(fs, fp->clust);
				if (!dsc) ABORT(fs, FR_INT_ERR);
				dsc += (DWORD)((ofs - 1) / SS(fs)) & (fs->csize - 1);
				if (fp->fptr % SS(fs) && dsc != fp->sect) {	/* Refill sector cache if needed */
#if !_FS_TINY

					if (disk_read(fs->drv, fp->buf, dsc, 1) != RES_OK) ABORT(fs, FR_DISK_ERR);	/* Load current sector */
#endif
					fp->sect = dsc;
				}
			}
		}
	} else
#endif

	/* Normal Seek */
	{
#if _FS_EXFAT
		if (fs->fs_type != FS_EXFAT && ofs >= 0x100000000) ofs = 0xFFFFFFFF;	/* Clip at 4GiB-1 if at FATxx */
#endif
		if (ofs > fp->obj.objsize && (_FS_READONLY || !(fp->flag & FA_WRITE))) {	/* In read-only mode, clip offset with the file size */
			ofs = fp->obj.objsize;
		}
		ifptr = fp->fptr;
		fp->fptr = nsect = 0;
		if (ofs) {
			bcs = (DWORD)fs->csize * SS(fs);	/* Cluster size (byte) */
			if (ifptr > 0 &&
				(ofs - 1) / bcs >= (ifptr - 1) / bcs) {	/* When seek to same or following cluster, */
				fp->fptr = (ifptr - 1) & ~(FSIZE_t)(bcs - 1);	/* start from the current cluster */
				ofs -= fp->fptr;
				clst = fp->clust;
			} else {									/* When seek to back cluster, */
				clst = fp->obj.sclust;					/* start from the first cluster */
				fp->clust = clst;
			}
			if (clst != 0) {
				while (ofs > bcs) {						/* Cluster following loop */
					ofs -= bcs; fp->fptr += bcs;
					{
						clst = get_fat(&fp->obj, clst);	/* Follow cluster chain if not in write mode */
					}
					if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
					if (clst <= 1 || clst >= fs->n_fatent) ABORT(fs, FR_INT_ERR);
					fp->clust = clst;
				}
				fp->fptr += ofs;
				if (ofs % SS(fs)) {
					nsect = clust2sect(fs, clst);	/* Current sector */
					if (!nsect) ABORT(fs, FR_INT_ERR);
					nsect += (DWORD)(ofs / SS(fs));
				}
			}
		}
		if (!_FS_READONLY && fp->fptr > fp->obj.objsize) {		/* Set file change flag if the file size is extended */
			fp->obj.objsize = fp->fptr;
			fp->flag |= FA_MODIFIED;
		}
		if (fp->fptr % SS(fs) && nsect != fp->sect) {	/* Fill sector cache if needed */
#if !_FS_TINY
			if (disk_read(fs->drv, fp->buf, nsect, 1) != RES_OK) ABORT(fs, FR_DISK_ERR);	/* Fill sector cache */
#endif
			fp->sect = nsect;
		}
	}

	LEAVE_FF(fs, res);
}



#if _FS_MINIMIZE <= 1
/*-----------------------------------------------------------------------*/
/* Create a Directory Object                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir (
	DIR* dp,			/* Pointer to directory object to create */
	const TCHAR* path	/* Pointer to the directory path */
)
{
	FRESULT res;
	FATFS *fs;
	_FDID *obj;
	DEF_NAMBUF


	if (!dp) return FR_INVALID_OBJECT;

	/* Get logical drive */
	obj = &dp->obj;
	res = find_volume(&path, &fs, 0);
	if (res == FR_OK) {
		obj->fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(dp, path);			/* Follow the path to the directory */
		if (res == FR_OK) {						/* Follow completed */
			if (!(dp->fn[NSFLAG] & NS_NONAME)) {	/* It is not the origin directory itself */
				if (obj->attr & AM_DIR) {		/* This object is a sub-directory */
#if _FS_EXFAT
					if (fs->fs_type == FS_EXFAT) {
						obj->c_scl = obj->sclust;	/* Save containing directory inforamation */
						obj->c_size = ((DWORD)obj->objsize & 0xFFFFFF00) | obj->stat;
						obj->c_ofs = dp->blk_ofs;
						obj->sclust = ld_dword(fs->dirbuf + XDIR_FstClus);	/* Get object location and status */
						obj->objsize = ld_qword(fs->dirbuf + XDIR_FileSize);
						obj->stat = fs->dirbuf[XDIR_GenFlags] & 2;
					} else
#endif
					{
						obj->sclust = ld_clust(fs, dp->dir);	/* Get object location */
					}
				} else {						/* This object is a file */
					res = FR_NO_PATH;
				}
			}
			if (res == FR_OK) {
				obj->id = fs->id;
				res = dir_sdi(dp, 0);			/* Rewind directory */
#if _FS_LOCK != 0
				if (res == FR_OK) {
					if (obj->sclust) {
						obj->lockid = inc_lock(dp, 0);	/* Lock the sub directory */
						if (!obj->lockid) res = FR_TOO_MANY_OPEN_FILES;
					} else {
						obj->lockid = 0;	/* Root directory need not to be locked */
					}
				}
#endif
			}
		}
		FREE_NAMBUF();
		if (res == FR_NO_FILE) res = FR_NO_PATH;
	}
	if (res != FR_OK) obj->fs = 0;		/* Invalidate the directory object if function faild */

	LEAVE_FF(fs, res);
}




/*-----------------------------------------------------------------------*/
/* Close Directory                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_closedir (
	DIR *dp		/* Pointer to the directory object to be closed */
)
{
	FRESULT res;
	FATFS *fs;


	res = validate(&dp->obj, &fs);			/* Check validity of the file object */
	if (res == FR_OK) {
#if _FS_LOCK != 0
		if (dp->obj.lockid) {				/* Decrement sub-directory open counter */
			res = dec_lock(dp->obj.lockid);
		}
		if (res == FR_OK)
#endif
		{
			dp->obj.fs = 0;			/* Invalidate directory object */
		}
#if _FS_REENTRANT
		unlock_fs(fs, FR_OK);		/* Unlock volume */
#endif
	}
	return res;
}




/*-----------------------------------------------------------------------*/
/* Read Directory Entries in Sequence                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
	DIR* dp,			/* Pointer to the open directory object */
	FILINFO* fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	FATFS *fs;
	DEF_NAMBUF


	res = validate(&dp->obj, &fs);	/* Check validity of the directory object */
	if (res == FR_OK) {
		if (!fno) {
			res = dir_sdi(dp, 0);			/* Rewind the directory object */
		} else {
			INIT_NAMBUF(fs);
			res = dir_read(dp, 0);			/* Read an item */
			if (res == FR_NO_FILE) res = FR_OK;	/* Ignore end of directory */
			if (res == FR_OK) {				/* A valid entry is found */
				get_fileinfo(dp, fno);		/* Get the object information */
				res = dir_next(dp, 0);		/* Increment index for next */
				if (res == FR_NO_FILE) res = FR_OK;	/* Ignore end of directory now */
			}
			FREE_NAMBUF();
		}
	}
	LEAVE_FF(fs, res);
}



#if _USE_FIND
/*-----------------------------------------------------------------------*/
/* Find Next File                                                        */
/*-----------------------------------------------------------------------*/

FRESULT f_findnext (
	DIR* dp,		/* Pointer to the open directory object */
	FILINFO* fno	/* Pointer to the file information structure */
)
{
	FRESULT res;


	for (;;) {
		res = f_readdir(dp, fno);		/* Get a directory item */
		if (res != FR_OK || !fno || !fno->fname[0]) break;	/* Terminate if any error or end of directory */
		if (pattern_matching(dp->pat, fno->fname, 0, 0)) break;		/* Test for the file name */
#if _USE_LFN != 0 && _USE_FIND == 2
		if (pattern_matching(dp->pat, fno->altname, 0, 0)) break;	/* Test for alternative name if exist */
#endif
	}
	return res;
}



/*-----------------------------------------------------------------------*/
/* Find First File                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_findfirst (
	DIR* dp,				/* Pointer to the blank directory object */
	FILINFO* fno,			/* Pointer to the file information structure */
	const TCHAR* path,		/* Pointer to the directory to open */
	const TCHAR* pattern	/* Pointer to the matching pattern */
)
{
	FRESULT res;


	dp->pat = pattern;		/* Save pointer to pattern string */
	res = f_opendir(dp, path);		/* Open the target directory */
	if (res == FR_OK) {
		res = f_findnext(dp, fno);	/* Find the first item */
	}
	return res;
}

#endif	/* _USE_FIND */



#if _FS_MINIMIZE == 0
/*-----------------------------------------------------------------------*/
/* Get File Status                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_stat (
	const TCHAR* path,	/* Pointer to the file path */
	FILINFO* fno		/* Pointer to file information to return */
)
{
	FRESULT res;
	DIR dj;
	DEF_NAMBUF


	/* Get logical drive */
	res = find_volume(&path, &dj.obj.fs, 0);
	if (res == FR_OK) {
		INIT_NAMBUF(dj.obj.fs);
		res = follow_path(&dj, path);	/* Follow the file path */
		if (res == FR_OK) {				/* Follow completed */
			if (dj.fn[NSFLAG] & NS_NONAME) {	/* It is origin directory */
				res = FR_INVALID_NAME;
			} else {							/* Found an object */
				if (fno) get_fileinfo(&dj, fno);
			}
		}
		FREE_NAMBUF();
	}

	LEAVE_FF(dj.obj.fs, res);
}


#endif /* _FS_MINIMIZE == 0 */
#endif /* _FS_MINIMIZE <= 1 */
#endif /* _FS_MINIMIZE <= 2 */

#if _USE_LABEL
/*-----------------------------------------------------------------------*/
/* Get Volume Label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_getlabel (
	const TCHAR* path,	/* Path name of the logical drive number */
	TCHAR* label,		/* Pointer to a buffer to return the volume label */
	DWORD* vsn			/* Pointer to a variable to return the volume serial number */
)
{
	FRESULT res;
	DIR dj;
	FATFS *fs;
	UINT16 si, di;
#if _LFN_UNICODE || _FS_EXFAT
	WORD w;
#endif

	/* Get logical drive */
	res = find_volume(&path, &fs, 0);

	/* Get volume label */
	if (res == FR_OK && label) {
		dj.obj.fs = fs; dj.obj.sclust = 0;	/* Open root directory */
		res = dir_sdi(&dj, 0);
		if (res == FR_OK) {
		 	res = dir_read(&dj, 1);			/* Find a volume label entry */
		 	if (res == FR_OK) {
#if _FS_EXFAT
				if (fs->fs_type == FS_EXFAT) {
					for (si = di = 0; si < dj.dir[XDIR_NumLabel]; si++) {	/* Extract volume label from 83 entry */
						w = ld_word(dj.dir + XDIR_Label + si * 2);
#if _LFN_UNICODE
						label[di++] = w;
#else
						w = ff_convert(w, 0);	/* Unicode -> OEM */
						if (w == 0) w = '?';	/* Replace wrong character */
						if (_DF1S && w >= 0x100) label[di++] = (char)(w >> 8);
						label[di++] = (char)w;
#endif
					}
					label[di] = 0;
				} else
#endif
				{
					si = di = 0;		/* Extract volume label from AM_VOL entry with code comversion */
					do {
#if _LFN_UNICODE
						w = (si < 11) ? dj.dir[si++] : ' ';
						if (IsDBCS1(w) && si < 11 && IsDBCS2(dj.dir[si])) {
							w = w << 8 | dj.dir[si++];
						}
						label[di++] = ff_convert(w, 1);	/* OEM -> Unicode */
#else
						label[di++] = dj.dir[si++];
#endif
					} while (di < 11);
					do {				/* Truncate trailing spaces */
						label[di] = 0;
						if (di == 0) break;
					} while (label[--di] == ' ');
				}
			}
		}
		if (res == FR_NO_FILE) {	/* No label entry and return nul string */
			label[0] = 0;
			res = FR_OK;
		}
	}

	/* Get volume serial number */
	if (res == FR_OK && vsn) {
		res = move_window(fs, fs->volbase);
		if (res == FR_OK) {
			switch (fs->fs_type) {
			case FS_EXFAT: di = BPB_VolIDEx; break;
			case FS_FAT32: di = BS_VolID32; break;
			default:       di = BS_VolID;
			}
			*vsn = ld_dword(fs->win + di);
		}
	}

	LEAVE_FF(fs, res);
}

#endif /* _USE_LABEL */

#if _USE_FORWARD
/*-----------------------------------------------------------------------*/
/* Forward data to the stream directly                                   */
/*-----------------------------------------------------------------------*/

FRESULT f_forward (
	FIL* fp, 						/* Pointer to the file object */
	UINT16 (*func)(const BYTE*,UINT16),	/* Pointer to the streaming function */
	UINT16 btf,						/* Number of bytes to forward */
	UINT16* bf						/* Pointer to number of bytes forwarded */
)
{
	FRESULT res;
	FATFS *fs;
	DWORD clst, sect;
	FSIZE_t remain;
	UINT16 rcnt, csect;
	BYTE *dbuf;


	*bf = 0;	/* Clear transfer byte counter */
	res = validate(&fp->obj, &fs);		/* Check validity of the file object */
	if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK) LEAVE_FF(fs, res);
	if (!(fp->flag & FA_READ)) LEAVE_FF(fs, FR_DENIED);	/* Check access mode */

	remain = fp->obj.objsize - fp->fptr;
	if (btf > remain) btf = (UINT16)remain;			/* Truncate btf by remaining bytes */

	for ( ;  btf && (*func)(0, 0);					/* Repeat until all data transferred or stream goes busy */
		fp->fptr += rcnt, *bf += rcnt, btf -= rcnt) {
		csect = (UINT16)(fp->fptr / SS(fs) & (fs->csize - 1));	/* Sector offset in the cluster */
		if (fp->fptr % SS(fs) == 0) {				/* On the sector boundary? */
			if (csect == 0) {						/* On the cluster boundary? */
				clst = (fp->fptr == 0) ?			/* On the top of the file? */
					fp->obj.sclust : get_fat(&fp->obj, fp->clust);
				if (clst <= 1) ABORT(fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
				fp->clust = clst;					/* Update current cluster */
			}
		}
		sect = clust2sect(fs, fp->clust);			/* Get current data sector */
		if (!sect) ABORT(fs, FR_INT_ERR);
		sect += csect;
#if _FS_TINY
		if (move_window(fs, sect) != FR_OK) ABORT(fs, FR_DISK_ERR);	/* Move sector window to the file data */
		dbuf = fs->win;
#else
		if (fp->sect != sect) {		/* Fill sector cache with file data */
			if (disk_read(fs->drv, fp->buf, sect, 1) != RES_OK) ABORT(fs, FR_DISK_ERR);
		}
		dbuf = fp->buf;
#endif
		fp->sect = sect;
		rcnt = SS(fs) - (UINT16)fp->fptr % SS(fs);	/* Number of bytes left in the sector */
		if (rcnt > btf) rcnt = btf;					/* Clip it by btr if needed */
		rcnt = (*func)(dbuf + ((UINT16)fp->fptr % SS(fs)), rcnt);	/* Forward the file data */
		if (!rcnt) ABORT(fs, FR_INT_ERR);
	}

	LEAVE_FF(fs, FR_OK);
}
#endif /* _USE_FORWARD */

#if _USE_STRFUNC
/*-----------------------------------------------------------------------*/
/* Get a string from the file                                            */
/*-----------------------------------------------------------------------*/

TCHAR* f_gets (
	TCHAR* buff,	/* Pointer to the string buffer to read */
	int len,		/* Size of string buffer (characters) */
	FIL* fp			/* Pointer to the file object */
)
{
	int n = 0;
	TCHAR c, *p = buff;
	BYTE s[2];
	UINT16 rc;


	while (n < len - 1) {	/* Read characters until buffer gets filled */
#if _LFN_UNICODE
#if _STRF_ENCODE == 3		/* Read a character in UTF-8 */
		f_read(fp, s, 1, &rc);
		if (rc != 1) break;
		c = s[0];
		if (c >= 0x80) {
			if (c < 0xC0) continue;	/* Skip stray trailer */
			if (c < 0xE0) {			/* Two-byte sequence */
				f_read(fp, s, 1, &rc);
				if (rc != 1) break;
				c = (c & 0x1F) << 6 | (s[0] & 0x3F);
				if (c < 0x80) c = '?';
			} else {
				if (c < 0xF0) {		/* Three-byte sequence */
					f_read(fp, s, 2, &rc);
					if (rc != 2) break;
					c = c << 12 | (s[0] & 0x3F) << 6 | (s[1] & 0x3F);
					if (c < 0x800) c = '?';
				} else {			/* Reject four-byte sequence */
					c = '?';
				}
			}
		}
#elif _STRF_ENCODE == 2		/* Read a character in UTF-16BE */
		f_read(fp, s, 2, &rc);
		if (rc != 2) break;
		c = s[1] + (s[0] << 8);
#elif _STRF_ENCODE == 1		/* Read a character in UTF-16LE */
		f_read(fp, s, 2, &rc);
		if (rc != 2) break;
		c = s[0] + (s[1] << 8);
#else						/* Read a character in ANSI/OEM */
		f_read(fp, s, 1, &rc);
		if (rc != 1) break;
		c = s[0];
		if (IsDBCS1(c)) {
			f_read(fp, s, 1, &rc);
			if (rc != 1) break;
			c = (c << 8) + s[0];
		}
		c = ff_convert(c, 1);	/* OEM -> Unicode */
		if (!c) c = '?';
#endif
#else						/* Read a character without conversion */
		f_read(fp, s, 1, &rc);
		if (rc != 1) break;
		c = s[0];
#endif
		if (_USE_STRFUNC == 2 && c == '\r') continue;	/* Strip '\r' */
		*p++ = c;
		n++;
		if (c == '\n') break;		/* Break on EOL */
	}
	*p = 0;
	return n ? buff : 0;			/* When no data read (eof or error), return with error. */
}

#endif /* _USE_STRFUNC */

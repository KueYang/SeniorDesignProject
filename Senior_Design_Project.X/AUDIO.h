/**
 * @file AUDIO.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef AUDIO_H
#define	AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "WAVDEF.h"
#include "FILES.h"

/** @def REC_BUF_SIZE 
 * Defines the receive buffer size. */
#define REC_BUF_SIZE            512
/** @def AUDIO_BUF_SIZE 
 * Defines the audio buffer size. */
#define AUDIO_BUF_SIZE          REC_BUF_SIZE/4

void AUDIO_Init(void);
void AUDIO_Process(void);
BYTE* AUDIO_GetRecieveBuffer(void);
BOOL AUDIO_ReadFile(UINT16 bytesToRead);
void AUDIO_WriteDataToDAC(void);

/* UART related functions */
void AUDIO_ListFiles(void);
void AUDIO_setNewTone(int fret, UINT16 factor);
BOOL AUDIO_setNewFile(UINT16 selectedFile);
void AUDIO_resetFilePtr(void);

UINT32 AUDIO_getBytesRead(void);
UINT32 AUDIO_getBytesWritten(void);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */


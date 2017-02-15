/* 
 * File:   AudioNew.h
 * Author: Belinda Yang
 *
 * Created on January 28, 2017, 11:45 AM
 */

#ifndef AUDIO_H
#define	AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "WAVDEF.h"
#include "FILES.h"

/** @def MAX_NUM_OF_FILES 
 * Defines the max number of audio files to be open. */
#define MAX_NUM_OF_FILES        10
/** @def REC_BUF_SIZE 
 * Defines the receive buffer size. */
#define REC_BUF_SIZE    8192
/** @def REC_BUF_SIZE 
 * Defines the audio buffer size. */
#define AUDIO_BUF_SIZE  REC_BUF_SIZE/4

void AUDIO_Init(void);
void AUDIO_Process(void);

void AUDIO_ListFiles(void);
void AUDIO_setNewTone(int fret);
BOOL AUDIO_setNewFile(const char* fileName);
void AUDIO_resetFilePtr(void);
UINT32 AUDIO_getBytesRead(void);
UINT32 AUDIO_getBytesWritten(void);

BYTE* AUDIO_GetRecieveBuffer(void);
BOOL AUDIO_ReadFileBytes(UINT16 bytesToRead);
void AUDIO_WriteDataToDAC(void);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */


/* 
 * File:   AudioNew.h
 * Author: Belinda Yang
 *
 * Created on January 28, 2017, 11:45 AM
 */

#ifndef AUDIONEW_H
#define	AUDIONEW_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "WAVDEF.h"
#include "FILESNEW.h"

/** @def MAX_NUM_OF_FILES 
 * Defines the max number of audio files to be open. */
#define MAX_NUM_OF_FILES        10
    
/** @def REC_BUF_SIZE 
 * Defines the audio buffer size. */
#define REC_BUF_SIZE    2048
#define STACK_BUF_SIZE  REC_BUF_SIZE/4

void AUDIONEW_Init(void);
void AUDIONEW_Process(void);

void AUDIONEW_ListFiles(void);
void AUDIONEW_setNewTone(int fret);
BOOL AUDIONEW_setNewFile(const char* fileName);
void AUDIONEW_resetFilePtr(void);

void AUDIONEW_WriteDataToDAC(void);
BOOL AUDIONEW_ReadDataFromMemory(UINT16 bytesToRead);
BYTE* AUDIONEW_GetRecieveBuffer(void);

int AUDIONEW_getBytesRead(void);
int AUDIONEW_getBytesWritten(void);
BOOL AUDIONEW_GetAudioData(FILESNEW* file, UINT16 bytes);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIONEW_H */


/**
 * @file Audio.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef AUDIO_H
#define	AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "WAVDEF.h"
    
/** @def PIC1 
 * Defines the PIC that is being programmed. */
#define PIC1
/** @def MAX_NUM_OF_FILES 
 * Defines the max number of audio files to be open. */
#define MAX_NUM_OF_FILES        19
    
void AUDIO_Init(void);
void AUDIO_Process(void);

void AUDIO_getFileList(void);
void AUDIO_setNewTone(int fret);
void AUDIO_WriteDataToDAC(void);
BOOL AUDIO_ReadDataFromMemory(void);
int AUDIO_getBytesRead(void);
int AUDIO_getBytesWritten(void);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */


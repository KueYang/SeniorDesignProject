/* 
 * File:   FILESNEW.h
 * Author: Belinda Yang
 *
 * Created on January 28, 2017, 10:38 AM
 */

#ifndef FILESNEW_H
#define	FILESNEW_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "./fatfs/ff.h"

typedef struct AUDIOINFONEW
{
    /**@{*/
    UINT16  bitsPerSample;
    UINT16  numOfChannels;
    UINT16  sampleRate;
    UINT16  blockAlign;
    UINT32  dataSize;           /**< Variable used to store the size of the data. */
    char fileName[16];
    /**@}*/
}AUDIOINFONEW;
    
typedef struct FILESNEW
{
    /**@{*/
    FIL File;			/* File objects */
    FSIZE_t startPtr;
    AUDIOINFONEW audioInfo;
    /**@}*/
}FILESNEW;

FRESULT FILESNEW_ReadFile(FIL* file, BYTE* buffer, UINT16 bytes, UINT16* ptr);
FRESULT FILESNEW_FindFile(DIR* dir, FILINFO* fileInfo, const char* fileName);
BOOL FILESNEW_ListFiles(const char* selectedName);
FRESULT FILESNEW_CloseFile(FIL* file);
FRESULT FILESNEW_OpenFile(FIL* file, const char* fileName, int mode);

#ifdef	__cplusplus
}
#endif

#endif	/* FILESNEW_H */


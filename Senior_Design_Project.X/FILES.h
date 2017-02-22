/* 
 * File:   FILES.h
 * Author: Belinda Yang
 *
 * Created on January 28, 2017, 10:38 AM
 */

#ifndef FILES_H
#define	FILES_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "./fatfs/ff.h"

typedef struct AUDIOINFO
{
    /**@{*/
    UINT16  bitsPerSample;
    UINT16  numOfChannels;
    UINT16  sampleRate;
    UINT16  blockAlign;
    UINT32  dataSize;           /**< Variable used to store the size of the data. */
    char fileName[16];
    /**@}*/
}AUDIOINFO;
    
typedef struct FILES
{
    /**@{*/
    FIL File;			/* File objects */
    FSIZE_t startPtr;
    DWORD cluster;
    DWORD sector;
    AUDIOINFO audioInfo;
    /**@}*/
}FILES;

FRESULT FILES_ReadFile(FIL* file, BYTE* buffer, UINT16 bytes, UINT16* ptr);
FRESULT FILES_FindFile(DIR* dir, FILINFO* fileInfo, const char* fileName);
BOOL FILES_ListFiles(const char* selectedName);
FRESULT FILES_CloseFile(FIL* file);
FRESULT FILES_OpenFile(FIL* file, const char* fileName, int mode);

#ifdef	__cplusplus
}
#endif

#endif	/* FILES_H */


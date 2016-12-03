/**
 * @file FILES.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef FILES_H
#define	FILES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "./FIO_Library/FSIO.h"

/**@def FILENAME_LENGTH 
 * Defines the max size of a file name. */
#define FILENAME_LENGTH     32

typedef struct AUDIOINFO
{
    /**@{*/
    UINT16  bitsPerSample;
    UINT16  numOfChannels;
    UINT16  sampleRate;
    UINT16  blockAlign;
    UINT32  dataSize;           /**< Variable used to store the size of the data. */
    char*   fileName;           /**< Variable used to store the file name. */
    /**@}*/
}AUDIOINFO;

/**
 * @brief FILES data structure.
 * @details Data structure used to store information about a file. 
 * @remarks Requires Microchip's MDD File System Library.
 */
typedef struct FILES
{
    /**@{*/
    FSFILE* startPtr;           /**< Variable used to point to the beginning of the file. >*/
    FSFILE* currentPtr;         /**< Variable used to point to the current location of the open file. >*/
    SearchRec rec;              /**< Variable used to search for files. >*/
    AUDIOINFO audioInfo;
    /**@}*/
}FILES;

void FILES_Init(void);
FSFILE* FILES_OpenFile(const char* file, SearchRec* rec);
BOOL FILES_CloseFile(FSFILE* pointer);
BOOL FILES_FindFile(const char* file, SearchRec* rec);
BOOL FILES_ListFiles(SearchRec* rec);
BOOL FILES_ReadFile(BYTE* buffer, UINT8 bytes, UINT32 blocks, FSFILE* pointer);

#ifdef	__cplusplus
}
#endif

#endif	/* FILES_H */


/**
 * @file Audio.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Header for the FILES Module. 
 */

#ifndef FILES_H
#define	FILES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "./FIO_Library/FSIO.h"

#define FILENAME_LENGTH     32
    
/**@typedef FILES structure. */
typedef struct FILES
{
    FSFILE* pointer;            // Variable used to point to the current file.
    SearchRec rec;              // Variable used to search for files.
    UINT32  dataSize;           // Size of the data
    char*   fileName;           // File name
}FILES;

void FILES_Init(void);
BOOL FILES_OpenFile(const char* file, FSFILE* pointer , SearchRec* rec);
BOOL FILES_CloseFile(FSFILE* pointer);
BOOL FILES_FindFile(const char* file, SearchRec* rec);
BOOL FILES_ListFiles(SearchRec* rec);
BOOL FILES_ReadFile(BYTE* buffer, UINT8 bytes, UINT32 blocks, FSFILE* pointer);

#ifdef	__cplusplus
}
#endif

#endif	/* FILES_H */


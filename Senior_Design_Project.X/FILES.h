#ifndef FILES_H
#define	FILES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "./FIO_Library/FSIO.h"
    
#define FILENAME_LENGTH     32

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


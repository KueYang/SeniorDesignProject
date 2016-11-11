#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "./FIO_Library/FSIO.h"
#include "FILES.h"

BYTE file_attributes;
UINT8 attributes = 0x3f;

void FILES_Init(void)
{
    // Initialize the library
    while(!FSInit());
    
    // Files and Search configurations.
    file_attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;
}

BOOL FILES_OpenFile(const char* file, FSFILE* pointer , SearchRec* rec)
{
    if(FILES_FindFile(file, rec))
    {
        pointer = FSfopen(file, "r");
        if (pointer == NULL)
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL FILES_CloseFile(FSFILE* pointer)
{
   if(FSfclose(pointer))
   {
       return FALSE;
   }
   return TRUE;
}

BOOL FILES_ListFiles(SearchRec* rec)
{
    char buf[FILENAME_LENGTH];
    
    UART_sendString("\n\rShowing all WAV files in root directory:\n\r");
    if (FindFirst("*.WAV", file_attributes, rec) == 0) 
    {
        snprintf(buf, FILENAME_LENGTH, "%s\t%u KB \n\r", rec->filename, rec->filesize/1000);
        UART_sendString(&buf[0]);
        while (FindNext(rec) == 0) 
        {
            snprintf(buf, FILENAME_LENGTH, "%s\t%u KB \n\r", rec->filename, rec->filesize/1000);
            UART_sendString(&buf[0]);
        }
    }
}

BOOL FILES_FindFile(const char* file, SearchRec* rec)
{
   if(FindFirst(file, file_attributes, rec) == 0)
   {
       return TRUE;
   }
   return FALSE;
}

BOOL FILES_ReadFile(BYTE* buffer, UINT8 bytes, UINT32 blocks, FSFILE* pointer)
{
    /* Reads bytes-Byte blocks from the file and stores it in the receive buffer. */
    if(FSfread(buffer, bytes, blocks, pointer) != blocks)
    {
        return FALSE;
    }
    return TRUE;
}



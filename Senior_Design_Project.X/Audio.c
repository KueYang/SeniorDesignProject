#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "SD_Library/FSDefs.h"
#include "SD_Library/FSconfig.h"
#include "SD_Library/FSIO.h"
#include "SD_Library/SD-SPI.h"
#include "Audio.h"

BOOL AUDIO_OpenFile(char* file);
BOOL AUDIO_CloseFile(void);
BOOL AUDIO_FindFile(char* file);
BOOL AUDIO_ReadFile(UINT32 blocks);
BOOL AUDIO_ReadToEndOfFile(void);
BOOL AUDIO_WriteToDAC(void);

FSFILE* pointer;
BYTE file_attributes;
SearchRec rec;
BYTE receiveBuffer[256];

void AUDIO_Init(void)
{
//    while (!MDD_SDSPI_MediaDetect());

    // Initialize the library
    while (!FSInit());
    
    // Open the open string note file in read mode
    pointer = FSfopen ("E_S6.TXT", "r");
}

void AUDIO_Process(void)
{
    
}

BOOL AUDIO_OpenFile(char* file)
{
   pointer = FSfopen(file, "r");
   if (pointer == NULL)
   {
       return FALSE;
   }
   return TRUE;
}

BOOL AUDIO_CloseFile(void)
{
   if(FSfclose(pointer))
   {
       return FALSE;
   }
   return TRUE;
}

BOOL AUDIO_FindFile(char* file)
{
   file_attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;
   
   if(FindFirst(file, file_attributes, &rec))
   {
       return FALSE;
   }
   return TRUE;
}

BOOL AUDIO_ReadFile(UINT32 blocks)
{
    /* Reads 4-Byte blocks from the file and stores it in the receive buffer. */
    if(FSfread(receiveBuffer, 4, blocks, pointer) != 1)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL AUDIO_ReadToEndOfFile(void)
{
    while(!FSfeof(pointer))
    {
        FSfread(receiveBuffer, 4, 1, pointer);
    }
}

BOOL AUDIO_WriteToDAC(void)
{
    
}
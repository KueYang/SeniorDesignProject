#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "./FIO_Library/FSIO.h"
#include "Audio.h"

#define FILENAME_LENGTH     32

BOOL AUDIO_OpenFile(const char* file);
BOOL AUDIO_CloseFile(void);
BOOL AUDIO_FindFile(const char* file);
BOOL AUDIO_ListFiles(void);
BOOL AUDIO_ReadFile(UINT32 blocks);
BOOL AUDIO_ReadToEndOfFile(void);
BOOL AUDIO_WriteToDAC(void);

FSFILE* pointer;
BYTE file_attributes;
SearchRec rec;
BYTE receiveBuffer[256];
UINT8 attributes = 0x3f;

void AUDIO_Init(void)
{
//    while (!MDD_SDSPI_MediaDetect());

    // Initialize the library
    while (!FSInit());
    
    // Files and Search configurations.
    file_attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;
    
    // List all files in sd card
    AUDIO_ListFiles();
    
    // Open the open string note file in read mode
    if(AUDIO_OpenFile("S1ELOW.WAV"))
    {
        UART_sendString("Successfully opened the file.");
    }
    else
    {
        UART_sendString("Failed to open file.");
    }
}

void AUDIO_Process(void)
{
    
}

BOOL AUDIO_OpenFile(const char* file)
{
    // Checks if the file exists
    if(AUDIO_FindFile(file))
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

BOOL AUDIO_CloseFile(void)
{
   if(FSfclose(pointer))
   {
       return FALSE;
   }
   return TRUE;
}

BOOL AUDIO_ListFiles(void)
{
    char buf[FILENAME_LENGTH];
    
    UART_sendString("\n\rShowing all WAV files in root directory:\n\r");
    if (FindFirst("*.WAV", file_attributes, &rec) == 0) { // file found
        snprintf(buf, FILENAME_LENGTH, "%s\t%u KB \n\r", rec.filename, rec.filesize/1000);
        UART_sendString(&buf[0]);
        while (FindNext(&rec) == 0) { // more files found
            snprintf(buf, FILENAME_LENGTH, "%s\t%u KB \n\r", rec.filename, rec.filesize/1000);
            UART_sendString(&buf[0]);
        }
    }
}

BOOL AUDIO_FindFile(const char* file)
{
   if(FindFirst(file, file_attributes, &rec) == 0)
   {
       return TRUE;
   }
   return FALSE;
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
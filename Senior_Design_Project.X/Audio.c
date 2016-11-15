#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "TIMER.h"
#include "FILES.h"
#include "DAC.h"
#include "Audio.h"

/* Functions to read and handle Wav Files. */
UINT8 AUDIO_GetHeader(void);
UINT32 AUDIO_GetDataSize(void);
BOOL AUDIO_GetAudioData(void);

/* Functions to handle DAC. */
void AUDIO_InitDAC(void);
BOOL AUDIO_WriteToDAC(void);

/* Private Variable used for handling files. */
FILES files[MAX_NUM_OF_FILES];
BYTE receiveBuffer[256];        // Variable stores bytes that are read from the file.
int fileIndex;

/* File definitions*/
/* Defines file names for the pic. */
const char* fileNames[MAX_NUM_OF_FILES] = {
#ifdef PIC1
    "S1_F0.WAV",
    "S1_F1.WAV",
    "S1_F2.WAV",
    "S1_F3.WAV",
    "S1_F4.WAV",
    "S1_F5.WAV",
    "S1_F6.WAV",
    "S1_F7.WAV",
    "S1_F8.WAV",
    "S1_F9.WAV",
    "S1_F10.WAV",
    "S1_F12.WAV",
    "S1_F13.WAV",
    "S1_F14.WAV",
    "S1_F15.WAV",
    "S1_F16.WAV",
    "S1_F17.WAV",
    "S1_F18.WAV"
#endif
#ifdef PIC2
    "S2_F0.WAV",
    "S2_F1.WAV",
    "S2_F2.WAV",
    "S2_F3.WAV",
    "S2_F4.WAV",
    "S2_F5.WAV",
    "S2_F6.WAV",
    "S2_F7.WAV",
    "S2_F8.WAV",
    "S2_F9.WAV",
    "S2_F10.WAV",
    "S2_F12.WAV",
    "S2_F13.WAV",
    "S2_F14.WAV",
    "S2_F15.WAV",
    "S2_F16.WAV",
    "S2_F17.WAV",
    "S2_F18.WAV"
#endif
#ifdef PIC3
    "S3_F0.WAV",
    "S3_F1.WAV",
    "S3_F2.WAV",
    "S3_F3.WAV",
    "S3_F4.WAV",
    "S3_F5.WAV",
    "S3_F6.WAV",
    "S3_F7.WAV",
    "S3_F8.WAV",
    "S3_F9.WAV",
    "S3_F10.WAV",
    "S3_F12.WAV",
    "S3_F13.WAV",
    "S3_F14.WAV",
    "S3_F15.WAV",
    "S3_F16.WAV",
    "S3_F17.WAV",
    "S3_F18.WAV"
#endif
#ifdef PIC4
    "S4_F0.WAV",
    "S4_F1.WAV",
    "S4_F2.WAV",
    "S4_F3.WAV",
    "S4_F4.WAV",
    "S4_F5.WAV",
    "S4_F6.WAV",
    "S4_F7.WAV",
    "S4_F8.WAV",
    "S4_F9.WAV",
    "S4_F10.WAV",
    "S4_F12.WAV",
    "S4_F13.WAV",
    "S4_F14.WAV",
    "S4_F15.WAV",
    "S4_F16.WAV",
    "S4_F17.WAV",
    "S4_F18.WAV"
#endif
#ifdef PIC5
    "S5_F0.WAV",
    "S5_F1.WAV",
    "S5_F2.WAV",
    "S5_F3.WAV",
    "S5_F4.WAV",
    "S5_F5.WAV",
    "S5_F6.WAV",
    "S5_F7.WAV",
    "S5_F8.WAV",
    "S5_F9.WAV",
    "S5_F10.WAV",
    "S5_F12.WAV",
    "S5_F13.WAV",
    "S5_F14.WAV",
    "S5_F15.WAV",
    "S5_F16.WAV",
    "S5_F17.WAV",
    "S5_F18.WAV"
#endif
#ifdef PIC6
    "S6_F0.WAV",
    "S6_F1.WAV",
    "S6_F2.WAV",
    "S6_F3.WAV",
    "S6_F4.WAV",
    "S6_F5.WAV",
    "S6_F6.WAV",
    "S6_F7.WAV",
    "S6_F8.WAV",
    "S6_F9.WAV",
    "S6_F10.WAV",
    "S6_F12.WAV",
    "S6_F13.WAV",
    "S6_F14.WAV",
    "S6_F15.WAV",
    "S6_F16.WAV",
    "S6_F17.WAV",
    "S6_F18.WAV"
#endif
};

void AUDIO_Init(void)
{
    // Checks to make sure that the SD card is attached
    while (!MDD_SDSPI_MediaDetect())
    {
        // Toggles LED until SD card is detected
        PORTBbits.RB5 = 1;
        TIMER_MSecondDelay(1000);
        PORTBbits.RB5 = 0;
        TIMER_MSecondDelay(1000);
    }

    // Initializes the File Library
    FILES_Init();
    
    // Opens all 19 audio files from the sd card
    for(fileIndex = 0; fileIndex < MAX_NUM_OF_FILES; fileIndex++)
    {
        if(!FILES_OpenFile(fileNames[fileIndex], files[fileIndex].pointer, &files[fileIndex].rec) &&
                (AUDIO_GetHeader() != WAV_SUCCESS))
        {
            fileIndex--;    // Resets the file index to retry opening the file.
        }
        else 
        {
            files[fileIndex].dataSize = AUDIO_GetDataSize();
            strncpy(files[fileIndex].fileName, fileNames[fileIndex], sizeof(fileNames[fileIndex]));
        }
    }
    
    // Initializes the index to the first file.
    fileIndex = 0;
    
   // Initializes the DAC
    DAC_Init();
}

void AUDIO_Process(void)
{
    TIMER_MSecondDelay(1000);
    DAC_Init();
}

/*
 * Reads the header of the wave file. 
 */
UINT8 AUDIO_GetHeader(void)
{
    if(FILES_ReadFile(receiveBuffer, 1, WAV_HEADER_SIZE, files[fileIndex].pointer))
    {
        if((receiveBuffer[WAV_CHUNK_ID] != 'R')|
                (receiveBuffer[WAV_CHUNK_ID+1] != 'I')|
                (receiveBuffer[WAV_CHUNK_ID+2] != 'F')|
                (receiveBuffer[WAV_CHUNK_ID+3] != 'F'))
        {
            return WAV_CHUNK_ID_ERROR;
        }
        else if((receiveBuffer[WAV_FORMAT_ERROR] != 'W')|
                (receiveBuffer[WAV_FORMAT_ERROR+1] != 'A')|
                (receiveBuffer[WAV_FORMAT_ERROR+2] != 'V')|
                (receiveBuffer[WAV_FORMAT_ERROR+3] != 'E'))
        {
            return WAV_FORMAT_ERROR;
        }
        else if((receiveBuffer[WAV_CHUNK1_SUB_ID] != 'f')|
                (receiveBuffer[WAV_CHUNK1_SUB_ID+1] != 'm')|
                (receiveBuffer[WAV_CHUNK1_SUB_ID+2] != 't'))
        {
            return WAV_SUB_CHUNK1_ID_ERROR;
        }
        else if((receiveBuffer[WAV_CHUNK2_SUB_ID] != 'd')|
                (receiveBuffer[WAV_CHUNK2_SUB_ID+1] != 'a')|
                (receiveBuffer[WAV_CHUNK2_SUB_ID+2] != 't')|
                (receiveBuffer[WAV_CHUNK2_SUB_ID+2] != 'a'))
        {
            return WAV_SUB_CHUNK2_ID_ERROR;
        }
        return WAV_SUCCESS;
    }
    return WAV_HEADER_SIZE_ERROR;
}

UINT32 AUDIO_GetDataSize(void)
{
    return (receiveBuffer[WAV_DATA-1] << 24) | 
            (receiveBuffer[WAV_DATA-2] << 16) |
            (receiveBuffer[WAV_DATA-3] << 8) |
            (receiveBuffer[WAV_DATA-4]);
}

BOOL AUDIO_GetAudioData(void)
{
    if(FILES_ReadFile(receiveBuffer, 1, files[fileIndex].dataSize, files[fileIndex].pointer))
    {
        return TRUE;
    }
    return FALSE;
}


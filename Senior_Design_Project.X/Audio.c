#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "TIMER.h"
#include "FILES.h"
#include "DAC.h"
#include "Audio.h"

/* Functions to read and handle Wav Files. */
UINT8 AUDIO_GetHeader(void);

/* Functions to handle DAC. */
void AUDIO_InitDAC(void);
BOOL AUDIO_WriteToDAC(void);

FSFILE* pointer;            // Variable used to point to the current file.
SearchRec rec;              // Variable used to search for files.
BYTE receiveBuffer[256];    // Variable stores bytes that are read from the file.

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
    FILES_OpenFile("S1ELOW.WAV", pointer, &rec);
    
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
    if(FILES_ReadFile(receiveBuffer, 1, WAV_HEADER_SIZE, pointer))
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
    if(FILES_ReadFile(receiveBuffer, 1, AUDIO_GetDataSize(), pointer))
    {
        return TRUE;
    }
    return FALSE;
}


/**
 * @file Audio.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The Audio module will handle all audio processing related tasks.
 * Tasks includes: initializing the DAC and Microchip's MDD File System library, 
 * reading data from external memory and writing audio data to the DACs.
 * @remarks The Audio module requires Microchip's MDD File System library.
 */

#include <p32xxxx.h>
#include <plib.h>
#include "STDDEF.h"
#include "IO.h"
#include "TIMER.h"
#include "FILESNEW.h"
#include "UART.h"
#include "DAC.h"
#include "AudioNew.h"

UINT8 AUDIONEW_GetHeader(int index, UINT16 bytes, UINT16* readPtr);
BOOL AUDIONEW_GetAudioData(FILESNEW* file, UINT16 bytes);

/** @var files 
 * The list of audio files that are to be used. */
FILESNEW files[MAX_NUM_OF_FILES];
/** @var receiveBuffer 
 * A buffer used to store data read from the audio file. */
BYTE receiveBuffer[REC_BUF_SIZE];
UINT16 LSTACK[STACK_BUF_SIZE];
UINT16 RSTACK[STACK_BUF_SIZE];
/** @var fileIndex 
 * The index used to specify the audio file that is being read. */
UINT16 fileIndex;
UINT16 audioInPtr;
UINT16 audioOutPtr;
UINT32 bytesRead;
UINT32 bytesWritten;

const char* fileNames[MAX_NUM_OF_FILES] = {
    "OST_02.WAV",
    "S1ELOW.WAV",
    "S2_A.WAV",
    "S3_D.WAV",
    "S4_G.WAV",
    "S5_B.WAV",
    "S6_EHIGH.WAV",
    "BOST_02.WAV",
    "ETUDES~1.WAV",
    "DK64JJ.WAV"
};

/**
 * @brief Initializes the Audio module.
 * @details Initializes the SD card and Microchip MDD File library. After 
 * initialization, all the audio files that are specific to the PIC are opened.
 * The DAC is also powered on with only one of its channels on (channel A is on
 * by default). 
 * @return Void
 */
void AUDIONEW_Init(void)
{
    // Checks to make sure that the SD card is attached
    FILESNEW_Init();
    
    // Opens all related audio files.
    int i = 0;
    for(i = 0; i < MAX_NUM_OF_FILES; i++)
    {
        // Opens the given file and set a pointer to the file.
        FILESNEW_OpenFile(&files[i].File, fileNames[i],FA_READ);
        strncpy(&files[i].audioInfo.fileName[0], fileNames[i], sizeof(files[i].audioInfo.fileName));
    }

    UINT16 readPtr = 0;
    AUDIONEW_GetHeader(0, WAV_HEADER_SIZE, &readPtr);
    TIMER3_SetSampleRate(files[0].audioInfo.sampleRate);
    
    files[0].startPtr = files[0].File.fptr;
    
    // Initializes the index to the first file.
    fileIndex = 0;
    AUDIONEW_setNewTone(0);
    
    // Lists the files in memory
    FILESNEW_ListFiles(&files[fileIndex].audioInfo.fileName[0]);
}

/**
 * @brief Process audio data.
 * @details The audio data will process audio data based on data received from
 * the IO and ADC modules. The data received by the IO module will correspond to
 * the fret location. The data received by the ADC module will correspond to the
 * strumming data.
 * @remarks Requires the IO and ADC modules to be initialized. 
 * @return Void
 */
void AUDIONEW_Process(void)
{
    
}


void AUDIONEW_ListFiles(void)
{
    FILESNEW_ListFiles(&files[fileIndex].audioInfo.fileName[0]);
}

BOOL AUDIONEW_setNewFile(const char* fileName)
{
    int i = 0;
    
    // Searches the list of files for the specified file.
    for(i = 0; i < MAX_NUM_OF_FILES; i++)
    {
        if(MON_stringsMatch(&files[i].audioInfo.fileName[0], fileName))
        {
            AUDIONEW_setNewTone(i);
            return TRUE;
        }
    }
    return FALSE;
}

void AUDIONEW_setNewTone(int fret)
{
    /* Sets the index to point to the file corresponding to the fret. */
//    fileIndex = fret;
    /* Sets the bytes read to zero. */
    bytesRead = 0;
    /* Sets the bytes written to zero. */
    bytesWritten = 0;
    /* Sets the audio in pointer to zero. */
    audioInPtr = 0;
    /* Sets the audio out pointer to zero. */
    audioOutPtr = 0;
    /* Sets the file index to the specified fret. */
    fileIndex = fret;
    
    files[fileIndex].File.fptr = files[fileIndex].startPtr;
    
    DAC_ZeroOutput();
    
    MON_SendString("Setting new a tone.");
}

void AUDIONEW_resetFilePtr(void)
{
    files[fileIndex].File.fptr = files[fileIndex].startPtr;
}

FILESNEW* AUDIONEW_getFilePtr(void)
{
    return &files[fileIndex];
}

int AUDIONEW_getBytesRead(void)
{
    return bytesRead;
}

BOOL AUDIONEW_isDoneReading()
{
    if(bytesRead >= files[fileIndex].audioInfo.dataSize)
    {
        return TRUE;
    }
    return FALSE;
}

int AUDIONEW_getBytesWritten(void)
{
    return bytesWritten;
}

BOOL AUDIONEW_isDoneWriting()
{
    if(bytesWritten >= files[fileIndex].audioInfo.dataSize)
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Reads the header of a WAV file.
 * @return A code indicating if reading the file is successful or not.
 * @retval 1, Read the header successfully
 * @retval 2, Header size is invalid
 * @retval 3, Chunk ID is invalid
 * @retval 4, Header format is invalid
 * @retval 5, Chunk ID 1 is invalid
 * @retval 6, Chunk ID 1 Data is invalid
 * @retval 7, Chunk ID 2 is invalid
 */
UINT8 AUDIONEW_GetHeader(int index, UINT16 bytes, UINT16* readPtr)
{
    if(FILESNEW_ReadFile(&files[index].File, &receiveBuffer[0],bytes, readPtr) == FR_OK)
    {
        if((receiveBuffer[WAV_CHUNK_ID] != 'R')|
                (receiveBuffer[WAV_CHUNK_ID+1] != 'I')|
                (receiveBuffer[WAV_CHUNK_ID+2] != 'F')|
                (receiveBuffer[WAV_CHUNK_ID+3] != 'F'))
        {
            return WAV_CHUNK_ID_ERROR;
        }
        else if((receiveBuffer[WAV_FORMAT] != 'W')|
                (receiveBuffer[WAV_FORMAT+1] != 'A')|
                (receiveBuffer[WAV_FORMAT+2] != 'V')|
                (receiveBuffer[WAV_FORMAT+3] != 'E'))
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
                (receiveBuffer[WAV_CHUNK2_SUB_ID+3] != 'a'))
        {
            return WAV_SUB_CHUNK2_ID_ERROR;
        }
        
        files[index].audioInfo.sampleRate = receiveBuffer[WAV_SAMPLE_RATE+1] << 8 | 
                                    receiveBuffer[WAV_SAMPLE_RATE];
        files[index].audioInfo.bitsPerSample = receiveBuffer[WAV_BITS_PER_SAMPLE];
        files[index].audioInfo.blockAlign = receiveBuffer[WAV_BLOCK_ALIGN];
        files[index].audioInfo.numOfChannels = receiveBuffer[WAV_NUM_CHANNELS];
        files[index].audioInfo.dataSize = (receiveBuffer[WAV_DATA-1] << 24) | 
                                    (receiveBuffer[WAV_DATA-2] << 16) |
                                    (receiveBuffer[WAV_DATA-3] << 8) |
                                    (receiveBuffer[WAV_DATA-4]);
        return WAV_SUCCESS;
    }
    return WAV_HEADER_SIZE_ERROR;
}

/**
 * @brief Reads the data block of the audio file.
 * @return Returns a boolean indicating if the file was read successfully.
 * @retval TRUE if the file was read successfully.
 * @retval FALSE if the file was read unsuccessfully.
 */
BOOL AUDIONEW_GetAudioData(FILESNEW* file, UINT16 bytes)
{
    UINT16 blockAlign = file->audioInfo.blockAlign;
    UINT16 blocks = blockAlign*bytes;
    UINT32 bytesLeft = (file->audioInfo.dataSize - bytesRead);
    UINT16 readPtr;
    
    if(blocks >= bytesLeft)
    {
        blocks = bytesLeft;
    }
    
    if(FILESNEW_ReadFile(&file[0].File, &receiveBuffer[0], bytes, &readPtr) == FR_OK)
    {
        int i = 0;
        UINT16 audioByte;
        UINT16 unsign_audio;
        for(i = 0; i < blocks; i+=blockAlign)
        {
            audioByte = ((receiveBuffer[i+1] << 8) | (receiveBuffer[i]));
            if (audioByte & 0x8000) {
                unsign_audio = ~(audioByte - 1);
                audioByte = AC_ZERO - unsign_audio;
            }
            else {
                audioByte = AC_ZERO + audioByte;
            }
            LSTACK[audioInPtr] = audioByte;

            if(file->audioInfo.numOfChannels == 2)
            {
                audioByte = ((receiveBuffer[i+3] << 8) | (receiveBuffer[i+2]));
                if (audioByte & 0x8000) {
                    unsign_audio = ~(audioByte - 1);
                    audioByte = AC_ZERO - unsign_audio;
                }
                else {
                    audioByte = AC_ZERO + audioByte;
                }             
            }
            RSTACK[audioInPtr++] = audioByte;

            if(audioInPtr >= STACK_BUF_SIZE)
            {
                audioInPtr = 0;
            }
        }
        bytesRead += blocks;
        return TRUE;
    }
    return FALSE;
}

BOOL AUDIONEW_ReadDataFromMemory(UINT16 bytesToRead)
{
    return AUDIONEW_GetAudioData(&files[fileIndex], bytesToRead);
}

BYTE* AUDIONEW_GetRecieveBuffer(void)
{
    return &receiveBuffer[0];
}

void AUDIONEW_WriteDataToDAC(void)
{
    /* Writes 1 WORD of data to the DAC. */
    DAC_WriteToDAC(WRITE_UPDATE_CHN_A, LSTACK[audioOutPtr]);
    DAC_WriteToDAC(WRITE_UPDATE_CHN_A, RSTACK[audioOutPtr++]);
    
    if(audioOutPtr >= STACK_BUF_SIZE)
    {
        audioOutPtr = 0;
    }
    // Increments the byte written count.
    bytesWritten+=4;
}
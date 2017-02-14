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
#include "FILES.h"
#include "UART.h"
#include "FIFO.h"
#include "DAC.h"
#include "AUDIO.h"

#define TEMP_BUF_SIZE       2048

UINT8 AUDIO_GetHeader(int index, UINT16* readPtr);
BOOL AUDIO_GetAudioData(FILES* file, RECEIVE_FIFO* buffer, UINT16 bytes);
BOOL AUDIO_ReadFile(RECEIVE_FIFO* buffer, UINT16 bytesToRead);
BOOL AUDIO_ReadToTempBuffer(void);

/** @var files 
 * The list of audio files that are to be used. */
FILES files[MAX_NUM_OF_FILES];
/** @var receiveFifo.buffer 
 * A buffer used to store data read from the audio file. */
BYTE tempBuffer[TEMP_BUF_SIZE];
RECEIVE_FIFO receiveFifo;
AUDIO_FIFO LAUDIOSTACK;
AUDIO_FIFO RAUDIOSTACK;
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
void AUDIO_Init(void)
{
    // Checks to make sure that the SD card is attached
    FILES_Init();
    
    // Opens all related audio files.
    int i = 0;
    UINT16 readPtr = 0;
    for(i = 0; i < MAX_NUM_OF_FILES; i++)
    {
        // Opens the given file and set a pointer to the file.
        FILES_OpenFile(&files[i].File, fileNames[i],FA_READ);

        strncpy(&files[i].audioInfo.fileName[0], fileNames[i], sizeof(files[i].audioInfo.fileName));
        AUDIO_GetHeader(i, &readPtr);
        files[i].startPtr = files[i].File.fptr;
    }

    // Initializes the index to the first file.
    fileIndex = 9;
    AUDIO_setNewTone(fileIndex);
    TIMER3_SetSampleRate(files[fileIndex].audioInfo.sampleRate);
    
    // Lists the files in memory
    FILES_ListFiles(&files[fileIndex].audioInfo.fileName[0]);
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
void AUDIO_Process(void)
{
   
}

void AUDIO_ListFiles(void)
{
    FILES_ListFiles(&files[fileIndex].audioInfo.fileName[0]);
}

BOOL AUDIO_setNewFile(const char* fileName)
{
    int i = 0;
    
    // Searches the list of files for the specified file.
    for(i = 0; i < MAX_NUM_OF_FILES; i++)
    {
        if(MON_stringsMatch(&files[i].audioInfo.fileName[0], fileName))
        {
            AUDIO_setNewTone(i);
            return TRUE;
        }
    }
    return FALSE;
}

void AUDIO_setNewTone(int fret)
{
    /* Sets the bytes read to zero. */
    bytesRead = 0;
    /* Sets the bytes written to zero. */
    bytesWritten = 0;
    /* Sets the file index to the specified fret. */
    fileIndex = fret;
    
    files[fileIndex].File.fptr = files[fileIndex].startPtr;
    
    DAC_ZeroOutput();
    
    MON_SendString("Setting new a tone.");
}

void AUDIO_resetFilePtr(void)
{
    files[fileIndex].File.fptr = files[fileIndex].startPtr;
}

FILES* AUDIO_getFilePtr(void)
{
    return &files[fileIndex];
}

int AUDIO_getBytesRead(void)
{
    return bytesRead;
}

BOOL AUDIO_isDoneReading()
{
    if(bytesRead >= files[fileIndex].audioInfo.dataSize)
    {
        return TRUE;
    }
    return FALSE;
}

int AUDIO_getBytesWritten(void)
{
    return bytesWritten;
}

BOOL AUDIO_isDoneWriting()
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
UINT8 AUDIO_GetHeader(int index, UINT16* readPtr)
{
    if(FILES_ReadFile(&files[index].File, &receiveFifo.buffer[0], WAV_HEADER_SIZE, readPtr) == FR_OK)
    {
        if((receiveFifo.buffer[WAV_CHUNK_ID] != 'R')|
                (receiveFifo.buffer[WAV_CHUNK_ID+1] != 'I')|
                (receiveFifo.buffer[WAV_CHUNK_ID+2] != 'F')|
                (receiveFifo.buffer[WAV_CHUNK_ID+3] != 'F'))
        {
            return WAV_CHUNK_ID_ERROR;
        }
        else if((receiveFifo.buffer[WAV_FORMAT] != 'W')|
                (receiveFifo.buffer[WAV_FORMAT+1] != 'A')|
                (receiveFifo.buffer[WAV_FORMAT+2] != 'V')|
                (receiveFifo.buffer[WAV_FORMAT+3] != 'E'))
        {
            return WAV_FORMAT_ERROR;
        }
        else if((receiveFifo.buffer[WAV_CHUNK1_SUB_ID] != 'f')|
                (receiveFifo.buffer[WAV_CHUNK1_SUB_ID+1] != 'm')|
                (receiveFifo.buffer[WAV_CHUNK1_SUB_ID+2] != 't'))
        {
            return WAV_SUB_CHUNK1_ID_ERROR;
        }
        else if((receiveFifo.buffer[WAV_CHUNK2_SUB_ID] != 'd')|
                (receiveFifo.buffer[WAV_CHUNK2_SUB_ID+1] != 'a')|
                (receiveFifo.buffer[WAV_CHUNK2_SUB_ID+2] != 't')|
                (receiveFifo.buffer[WAV_CHUNK2_SUB_ID+3] != 'a'))
        {
            return WAV_SUB_CHUNK2_ID_ERROR;
        }
        
        files[index].audioInfo.sampleRate = receiveFifo.buffer[WAV_SAMPLE_RATE+1] << 8 | 
                                    receiveFifo.buffer[WAV_SAMPLE_RATE];
        files[index].audioInfo.bitsPerSample = receiveFifo.buffer[WAV_BITS_PER_SAMPLE];
        files[index].audioInfo.blockAlign = receiveFifo.buffer[WAV_BLOCK_ALIGN];
        files[index].audioInfo.numOfChannels = receiveFifo.buffer[WAV_NUM_CHANNELS];
        files[index].audioInfo.dataSize = (receiveFifo.buffer[WAV_DATA-1] << 24) | 
                                    (receiveFifo.buffer[WAV_DATA-2] << 16) |
                                    (receiveFifo.buffer[WAV_DATA-3] << 8) |
                                    (receiveFifo.buffer[WAV_DATA-4]);
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
BOOL AUDIO_GetAudioData(FILES* file, RECEIVE_FIFO* fifo, UINT16 bytes)
{
    UINT32 bytesLeft = (file->audioInfo.dataSize - bytesRead);
    UINT16 readPtr = 0;
    
    if(bytes > bytesLeft)
    {
        bytes = bytesLeft;
    }
    
    if(FILES_ReadFile(&(file->File), &(fifo->buffer[0]), bytes, &readPtr) == FR_OK)
    {   
        int i = 0;
        UINT16 audioByte;
        UINT16 unsign_audio;
        BYTE LSB, MSB;
        for(i = 0; i < bytes; i+=4)
        {
            LSB = fifo->FIFO_ReceivePop(fifo);
            MSB = fifo->FIFO_ReceivePop(fifo);
            audioByte = ((MSB << 8) | LSB);
            if (audioByte & 0x8000) 
            {
                unsign_audio = ~(audioByte) + 1;
                audioByte = AC_ZERO - unsign_audio;
            }
            else
            {
                audioByte = AC_ZERO + audioByte;
            }
            LAUDIOSTACK.FIFO_AudioPush(&LAUDIOSTACK, audioByte);

            if(file->audioInfo.numOfChannels == 2)
            {
                LSB = fifo->FIFO_ReceivePop(fifo);
                MSB = fifo->FIFO_ReceivePop(fifo);
                audioByte = ((MSB << 8) | LSB);
                if (audioByte & 0x8000) 
                {
                    unsign_audio = ~(audioByte) + 1;
                    audioByte = AC_ZERO - unsign_audio;
                }
                else 
                {
                    audioByte = AC_ZERO + audioByte;
                }             
            }
            RAUDIOSTACK.FIFO_AudioPush(&RAUDIOSTACK, audioByte);
        }
        bytesRead+=bytes;
        return TRUE;
    }
    return FALSE;
}

BOOL AUDIO_ReadFile(RECEIVE_FIFO* buffer, UINT16 bytesToRead)
{
    return AUDIO_GetAudioData(&files[fileIndex], buffer, bytesToRead);
}

BOOL AUDIO_ReadToTempBuffer(void)
{
    UINT32 bytesLeft = (files[fileIndex].audioInfo.dataSize - bytesRead);
    UINT16 bytes = TEMP_BUF_SIZE;
    UINT16 readPtr = 0;
    
    if(bytes > bytesLeft)
    {
        bytes = bytesLeft;
    }
    
    if(FILES_ReadFile(&files[fileIndex].File, &tempBuffer[0], bytes, &readPtr) == FR_OK)
    {   
        int i = 0;
        for(i = 0; i <= TEMP_BUF_SIZE; i++)
        {
            receiveFifo.FIFO_ReceivePush(&receiveFifo, tempBuffer[i]);
        }
        return TRUE;
    }
    return FALSE;
}

BOOL AUDIO_ReadFileBytes(UINT16 bytesToRead)
{
    return AUDIO_GetAudioData(&files[fileIndex], &receiveFifo, bytesToRead);
}

BYTE* AUDIO_GetRecieveBuffer(void)
{
    return &receiveFifo.buffer[0];
}

void AUDIO_WriteDataToDAC(void)
{
    if(AUDIO_isDoneReading() && AUDIO_isDoneWriting())
    {
        TIMER3_ON(FALSE);
        AUDIO_setNewTone(9);
    }
    else
    {
        if(bytesRead == bytesWritten)
        {
            AUDIO_ReadFile(&receiveFifo, REC_BUF_SIZE);
        }
        else if(bytesWritten%TEMP_BUF_SIZE == 0)
        {
            AUDIO_ReadToTempBuffer();
        }
        else if(bytesRead > bytesWritten)
        {
            /* Writes 1 WORD of data to the DAC. */
            DAC_WriteToDAC(WRITE_UPDATE_CHN_A, LAUDIOSTACK.FIFO_AudioPop());
            DAC_WriteToDAC(WRITE_UPDATE_CHN_B, RAUDIOSTACK.FIFO_AudioPop());

            // Increments the byte written count.
            bytesWritten+=8;
        }
    }
}
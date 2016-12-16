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
#include "FILES.h"
#include "UART.h"
#include "DAC.h"
#include "Audio.h"

#define REC_BUF_SIZE    256
#define READ_BYTES      16

UINT8 AUDIO_GetHeader(FILES* file, int index);
BOOL AUDIO_GetAudioData(FILES* file, int bytes);

/** @var files 
 * The list of audio files that are to be used. */
FILES files[MAX_NUM_OF_FILES];
/** @var receiveBuffer 
 * A buffer used to store data read from the audio file. */
BYTE receiveBuffer[REC_BUF_SIZE];
UINT16 audioData[REC_BUF_SIZE];
/** @var fileIndex 
 * The index used to specify the audio file that is being read. */
int fileIndex;
int audioInPtr;
int audioOutPtr;
int bytesRead;
int bytesWritten;
/** @var fileNames 
 * The file names of audio files for the specified pic. */
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
    while(MDD_SDSPI_MediaNotDetect());

    // Initializes the File Library
    FILES_Init();
    
    // Lists the files in memory
    FILES_ListFiles(&files[0].rec);
    
    // Opens the given file and set a pointer to the file.
    files[0].currentPtr = FILES_OpenFile("OST_02.WAV", &files[0].rec);
    if(files[0].currentPtr != NULL)
    {
        /* Parses the header from the files and stores them. */
        if(AUDIO_GetHeader(&files[0], 0) == WAV_SUCCESS)
        {
            /* Saves a pointer to the beginning of the data section of the file. */
            files[0].startPtr = files[0].currentPtr;
            /* Configures the Timer2 period given the sample rate. */
            TIMER_SetSampleRate(files[0].audioInfo.sampleRate);
        }
    }
    
    // Initializes the index to the first file.
    fileIndex = 0;
    
    TIMER3_ON(TRUE);
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
    if(AUDIO_isDoneReading() && AUDIO_isDoneWriting())
    {
        AUDIO_setNewTone(0);
        IO_setCurrentFret(0);
        TIMER2_ON(FALSE);
        TIMER3_ON(FALSE);
    }
}

void AUDIO_setNewTone(int fret)
{
    /* Sets the index to point to the file corresponding to the fret. */
    fileIndex = fret;
    /* Sets the bytes read to zero. */
    bytesRead = 0;
    /* Sets the bytes written to zero. */
    bytesWritten = 0;
    /* Sets the current file pointer to the beginning of the data section of the file. */
    files[fileIndex].currentPtr = files[fileIndex].startPtr;
}

BOOL AUDIO_isDoneReading()
{
    if(bytesRead >= files[fileIndex].audioInfo.dataSize)
    {
        return TRUE;
    }
    return FALSE;
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
UINT8 AUDIO_GetHeader(FILES* file, int index)
{
    if(FILES_ReadFile(receiveBuffer, 1, WAV_HEADER_SIZE, file->currentPtr))
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
        
        file[index].audioInfo.sampleRate = receiveBuffer[WAV_SAMPLE_RATE+1] << 8 | 
                                    receiveBuffer[WAV_SAMPLE_RATE];
        file[index].audioInfo.bitsPerSample = receiveBuffer[WAV_BITS_PER_SAMPLE];
        file[index].audioInfo.blockAlign = receiveBuffer[WAV_BLOCK_ALIGN];
        file[index].audioInfo.numOfChannels = receiveBuffer[WAV_NUM_CHANNELS];
        file[index].audioInfo.dataSize = (receiveBuffer[WAV_DATA-1] << 24) | 
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
BOOL AUDIO_GetAudioData(FILES* file, int bytes)
{
    UINT16 blocks = file->audioInfo.blockAlign*bytes;
    if(FILES_ReadFile(receiveBuffer, 1, blocks, file->currentPtr))
    {
        int i = 0;
        UINT16 audioByte;
        for(i = 0; i < blocks; i += file->audioInfo.blockAlign)
        {
            audioByte = receiveBuffer[i] << 8 | receiveBuffer[i+1];
            audioData[audioInPtr++] = audioByte;
            
            if(audioInPtr >= REC_BUF_SIZE)
            {
                audioInPtr = 0;
            }
        }
        bytesRead += blocks;
        return TRUE;
    }
    return FALSE;
}

int i = 0;
void Audio_ReadDataFromMemory(void)
{
    UART_sendString("Reading Data \n\r");
    /* Reads 16 bytes of data. */
    AUDIO_GetAudioData(&files[fileIndex], READ_BYTES);
    UART_sendCharacter(receiveBuffer[0]);
    UART_sendString("\n\r");
}

void Audio_WriteDataToDAC(void)
{
    /* Writes 1 byte of data to the DAC. */
    DAC_WriteToDAC(WRITE_UPDATE_CHN_A, audioData[audioOutPtr++]);
    if(audioOutPtr >= 255)
    {
        audioOutPtr = 0;
    }
    // Increments the byte written count.
    bytesWritten++;
}



/**
 * @file Audio.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The Audio module will handle all audio processing related tasks.
 * Tasks includes: initializing Fatfs File System library, 
 * reading data from external memory and writing audio data to the DACs.
 * @remarks The Audio module requires Fatfs File System library. The library
 * uses the library from the pic24 example project.
 */

#include <p32xxxx.h>
#include <stdio.h>
#include <string.h>
#include "STDDEF.h"
#include "IO.h"
#include "DAC.h"
#include "FILES.h"
#include "FILEDEF.h"
#include "AUDIO.h"

UINT8 AUDIO_GetHeader(int index);
BOOL AUDIO_GetAudioData(FILES* file, UINT16 bytes);

/** @var files 
 * The list of audio files that are to be used. */
FILES files[MAX_NUM_OF_FILES];
/** @var receiveBuffer
 * A buffer used to store data read from the audio file. */
BYTE receiveBuffer[REC_BUF_SIZE];
/** @var LAUDIOSTACK 
 * A buffer used to store left channel audio data. */
UINT16 LAUDIOSTACK[AUDIO_BUF_SIZE];
/** @var RAUDIOSTACK
 * A buffer used to store right channel audio data. */
UINT16 RAUDIOSTACK[AUDIO_BUF_SIZE];
/** @var fileIndex 
 * The index used to specify the audio file that is being read. */
UINT16 fileIndex;
/** @var audioInPtr 
 * The index used to store audio data into to audio buffer. */
UINT16 audioInPtr;
/** @var audioOutPtr 
 * The index used to write audio data out of the audio buffer. */
UINT16 audioOutPtr;
/** @var bytesRead 
 * Stores the number of bytes that have been read. */
UINT32 bytesRead;
/** @var bytesWritten 
 * Stores the number of bytes that have been written. */
UINT32 bytesWritten;
/** @var hasReadFile 
 * Stores boolean indicating the specified audio file has been read. */
BOOL hasReadFile;

UINT16 scaleFactor;

/**
 * @brief Initializes the Audio module.
 * @details Initializes the SD card and Microchip MDD File library. After 
 * initialization, all the audio files that are specific to the PIC are opened.
 * @return Void
 */
void AUDIO_Init(void)
{
    // Checks to make sure that the SD card is attached and initialized
    FILES_Init();
    
    // Opens all related audio files.
    int i = 0;
    for(i = 0; i < MAX_NUM_OF_FILES; i++)
    {
        // Opens the given file and sets a pointer to the file.
        FILES_OpenFile(&files[i].File, fileNames[i],FA_READ);
        // Copies the files name.
        strncpy(&files[i].audioInfo.fileName[0], fileNames[i], sizeof(files[i].audioInfo.fileName));
        // Reads the file header.
        AUDIO_GetHeader(i);
        // Sets the file start pointer.
        files[i].startPtr = files[i].File.fptr;
        files[i].cluster = files[i].File.clust;
        files[i].sector = files[i].File.sect;
    }

    // Initializes the index to the first file.
    fileIndex = FILE_1;
    // Sets the initial tone.
    AUDIO_setNewTone(fileIndex, 1);
    // Sets the TIMER clock period to write out audio data.
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
    if(TIMER3_IsON())
    {
        AUDIO_ReadFile(REC_BUF_SIZE);
    }
}

/**
 * @brief Displays the list of audio files.
 * @details Finds all WAV files in the root directory of the SD card and displays
 * them via UART to serial.
 * @remarks Requires the UART module and SD card to be initialized. 
 * @return Void
 */
void AUDIO_ListFiles(void)
{
    FILES_ListFiles(&files[fileIndex].audioInfo.fileName[0]);
}

/**
 * @brief Sets a new file to be read.
 * @arg selectedFile The selected file to be set.
 * @return A boolean indicating if the file has been set successfully.
 * @retval TRUE, file has been set successfully.
 * @retval FALSE, file has not been set successfully.
 */
BOOL AUDIO_setNewFile(UINT16 selectedFile)
{
    if(selectedFile <= 20 && selectedFile >= 0)
    {
        AUDIO_setNewTone(selectedFile, 1);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Sets a new tone.
 * @details Sets a new tone based on the fret that is passed into the function.
 * Resets all related variables prior to reading the new tone.
 * @arg fret The fret that is being played.
 * @return Void
 */
char buf[64];
void AUDIO_setNewTone(int fret, UINT16 factor)
{
    /* Disables the timer if it is on. */
    if(TIMER3_IsON())
    {
        TIMER3_ON(FALSE);
        MON_SendString("Turning off timer");
    }
    
    /* Clears out all the buffers. */
    memset(&receiveBuffer[0], AC_ZERO, sizeof(receiveBuffer));
    memset(&LAUDIOSTACK[0], AC_ZERO, sizeof(LAUDIOSTACK));
    memset(&RAUDIOSTACK[0], AC_ZERO, sizeof(RAUDIOSTACK));
    
    /* Sets the audio in pointer to zero. */
    audioInPtr = 0;
    /* Sets the audio out pointer to zero. */
    audioOutPtr = 0;
    /* Sets the bytes read to zero. */
    bytesRead = 0;
    /* Sets the bytes written to zero. */
    bytesWritten = 0;
    /* Resets the file pointer. */
    AUDIO_resetFilePtr();
    /* Sets the file index to the specified fret. */
    fileIndex = fret;
    /* Sets the DAC's output to zero. */
    DAC_Zero();
    /* Sets the scaling factor. */
    scaleFactor = factor;
    /* Sets the hasReadFile boolean. */
    hasReadFile = FALSE;
    
    snprintf(&buf[0] ,64 ,"Fret: %d \n\rScale Factor: %f \n\rSetting new a tone.", fret, scaleFactor/1024);
    MON_SendString(&buf[0]);
}

/**
 * @brief Resets the file pointer for selected file.
 * @return Void
 */
void AUDIO_resetFilePtr(void)
{
    files[fileIndex].File.fptr = files[fileIndex].startPtr;
    files[fileIndex].File.clust = files[fileIndex].cluster;
    files[fileIndex].File.sect = files[fileIndex].sector;
}

/**
 * @brief Returns the number of bytes read.
 * @return Returns the number of bytes read
 */
UINT32 AUDIO_getBytesRead(void)
{
    return bytesRead;
}

/**
 * @brief Checks if reading the selected file is done.
 * @return Returns a boolean indicating if done reading the selected audio file.
 * @retval TRUE, if done reading the file.
 * @retval FALSE, if not done reading the file.
 */
BOOL AUDIO_isDoneReading(void)
{
    if(bytesRead >= files[fileIndex].audioInfo.dataSize)
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Returns the number of bytes written.
 * @return Returns the number of bytes written.
 */
UINT32 AUDIO_getBytesWritten(void)
{
    return bytesWritten;
}

/**
 * @brief Checks if writing to the DAC is done.
 * @return Returns a boolean indicating if writing to the DAC is done.
 * @retval TRUE, if done writing to the DAC.
 * @retval FALSE, if not done writing to the DAC.
 */
BOOL AUDIO_isDoneWriting(void)
{
    if(bytesWritten >= files[fileIndex].audioInfo.dataSize)
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Reads the header of a WAV file.
 * @arg index The file that is being read.
 * @return A code indicating if reading the file is successful or not.
 * @retval 1, Read the header successfully
 * @retval 2, Header size is invalid
 * @retval 3, Chunk ID is invalid
 * @retval 4, Header format is invalid
 * @retval 5, Chunk ID 1 is invalid
 * @retval 6, Chunk ID 1 Data is invalid
 * @retval 7, Chunk ID 2 is invalid
 */
UINT8 AUDIO_GetHeader(int index)
{
    UINT16 readPtr = 0;
    if(FILES_ReadFile(&files[index].File, &receiveBuffer[0], WAV_HEADER_SIZE, &readPtr) == FR_OK)
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
        
        // Stores the corresponding header information
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
 * @brief Reads a number of bytes from the audio file.
 * @arg file The files to read from.
 * @arg bytes The number of bytes to read.
 * @return Returns a boolean indicating if the file was read successfully.
 * @retval TRUE if the file was read successfully.
 * @retval FALSE if the file was read unsuccessfully.
 */
BOOL AUDIO_GetAudioData(FILES* file, UINT16 bytes)
{
    UINT32 bytesLeft = (file->audioInfo.dataSize - bytesRead);
    UINT16 readPtr = 0;
    
    // Calculates the number of bytes left to read.
    if(bytes > bytesLeft)
    {
        bytes = bytesLeft;
    }
    
    // Reads the file and verifies that the file is read.
    if(FILES_ReadFile(&(file->File), &receiveBuffer[0], bytes, &readPtr) == FR_OK)
    {   
        // Converts and writes read bytes to audio buffers.
        int i = 0;
        UINT16 audioData, unsign_audio;
        for(i = 0; i < bytes; i+=4)
        {
            // Left Channel
            audioData = ((receiveBuffer[i+1] << 8) | (receiveBuffer[i]));
            if (audioData & 0x8000) {
                unsign_audio = ~(audioData - 1);
                audioData = AC_ZERO - unsign_audio;
            }
            else {
                audioData = AC_ZERO + audioData;
            }
            LAUDIOSTACK[audioInPtr] = audioData;
            
            if(file->audioInfo.numOfChannels == 2)
            {
                audioData = ((receiveBuffer[i+3] << 8) | (receiveBuffer[i+2]));
                if (audioData & 0x8000) {
                    unsign_audio = ~(audioData - 1);
                    audioData = AC_ZERO - unsign_audio;
                }
                else {
                    audioData = AC_ZERO + audioData;
                }             
            }
            RAUDIOSTACK[audioInPtr++] = audioData;
            
            if(audioInPtr >= AUDIO_BUF_SIZE)
            {
                audioInPtr = 0;
            }
        }
        bytesRead+=bytes;
        hasReadFile = TRUE;
        return TRUE;
    }
    hasReadFile = FALSE;
    return FALSE;
}

/**
 * @brief Reads a number of bytes from the audio file.
 * @arg bytesToRead The number of bytes to read.
 * @return Returns a boolean indicating if the file was read successfully.
 * @retval TRUE if the file was read successfully.
 * @retval FALSE if the file was read unsuccessfully or is already done reading.
 */
BOOL AUDIO_ReadFile(UINT16 bytesToRead)
{
    if(AUDIO_isDoneReading())
    {
        return FALSE;
    }

    return AUDIO_GetAudioData(&files[fileIndex], bytesToRead);
}

/**
 * @brief Gets buffer pointer.
 * @details Gets a pointer to the buffer that stores the bytes read from the SD 
 * Card.
 * @return Returns a pointer to the read buffer. 
 */
BYTE* AUDIO_GetRecieveBuffer(void)
{
    return &receiveBuffer[0];
}

/**
 * @brief Writes audio data out to the DAC
 * @details Writes audio data out to the DAC. Handles stopping and resetting all 
 * the selected after writing all audio data out to the DAC.
 * @return Void
 */
void AUDIO_WriteDataToDAC(void)
{
    if(AUDIO_isDoneReading() && AUDIO_isDoneWriting())
    {
        AUDIO_setNewTone(FILE_0, 1);
    }
    else
    {
        if((bytesRead > bytesWritten) && hasReadFile == TRUE)
        {
            /* Writes 1 WORD of data to the DAC Channel A, left channel. */
            DAC_WriteToDAC(WRITE_UPDATE_CHN_A, LAUDIOSTACK[audioOutPtr]);
            /* Writes 1 WORD of data to the DAC Channel B, right channel. */
            DAC_WriteToDAC(WRITE_UPDATE_CHN_B, RAUDIOSTACK[audioOutPtr++]);

            if(audioOutPtr >= AUDIO_BUF_SIZE)
            {
                audioOutPtr = 0;
            }
            
            // Increments the byte written count.
            bytesWritten+=4;
        }
    }
}
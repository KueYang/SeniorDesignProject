/**
 * @file Audio.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef AUDIO_H
#define	AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

/** @def PIC1 
 * Defines the PIC that is being programmed. */
#define PIC1
/** @def MAX_NUM_OF_FILES 
 * Defines the max number of audio files to be open. */
#define MAX_NUM_OF_FILES        19

/** @def WAV_HEADER_SIZE 
 * Defines the size of a WAV file header. */
#define WAV_HEADER_SIZE         44
/** @def WAV_CHUNK_ID 
 * Defines the index of the WAV chunk ID. */
#define WAV_CHUNK_ID            0 
/** @def WAV_CHUNK_SIZE 
 * Defines the index of the WAV chunk size. */
#define WAV_CHUNK_SIZE          4
/** @def WAV_FORMAT 
 * Defines the index of the WAV format. */
#define WAV_FORMAT              8
/** @def WAV_CHUNK1_SUB_ID 
 * Defines the index of the WAV chunk1 sub ID.*/
#define WAV_CHUNK1_SUB_ID       12
/** @def WAV_CHUNK1_SUB_SIZE 
 * Defines the index of the WAV chunk1 sub size. */
#define WAV_CHUNK1_SUB_SIZE     16
/** @def WAV_AUDIO_FORMAT 
 * Defines the index of the WAV audio format. */
#define WAV_AUDIO_FORMAT        20
/** @def WAV_NUM_CHANNELS 
 * Defines the index of the WAV number of channels. */
#define WAV_NUM_CHANNELS        22
/** @def WAV_SAMPLE_RATE 
 * Defines the index of the WAV sample rate. */
#define WAV_SAMPLE_RATE         24
/** @def WAV_BYTE_RATE 
 * Defines the index of the WAV byte rate. */
#define WAV_BYTE_RATE           28
/** @def WAV_BLOCK_ALIGN 
 * Defines the index of the WAV block alignment. */
#define WAV_BLOCK_ALIGN         32
/** @def WAV_BITS_PER_SAMPLE 
 * Defines the index of the WAV bits per sample. */
#define WAV_BITS_PER_SAMPLE     34
/** @def WAV_CHUNK2_SUB_ID 
 * Defines the index of the WAV chunk2 sub ID. */
#define WAV_CHUNK2_SUB_ID       36
/** @def WAV_CHUNK2_SUB_SIZE 
 * Defines the index of the WAV chunk2 sub size. */
#define WAV_CHUNK2_SUB_SIZE     40
/** @def WAV_DATA 
 * Defines the index of the WAV data. */
#define WAV_DATA                44
    
/** @def WAV_SUCCESS 
 * Defines the success code when accessing a WAV header. */
#define WAV_SUCCESS                 1
/** @def WAV_HEADER_SIZE_ERROR 
 * Defines the error code for the wrong header size. */
#define WAV_HEADER_SIZE_ERROR       2
/** @def WAV_CHUNK_ID_ERROR 
 * Defines the error code for invalid chunk ID. */
#define WAV_CHUNK_ID_ERROR          3
/** @def WAV_FORMAT_ERROR 
 * Defines the error code for invalid WAV format. */
#define WAV_FORMAT_ERROR            4
/** @def WAV_SUB_CHUNK1_ID_ERROR 
 * Defines the error code for invalid chunk1 ID. */
#define WAV_SUB_CHUNK1_ID_ERROR     5
/** @def WAV_SUB_CHUNK1_SIZE_ERROR 
 * Defines the error code for invalid chunk1 size. */
#define WAV_SUB_CHUNK1_SIZE_ERROR   6
/** @def WAV_SUB_CHUNK2_ID_ERROR 
 * Defines the error code for invalid chunk2 ID. */
#define WAV_SUB_CHUNK2_ID_ERROR     7

void AUDIO_Init(void);
void AUDIO_Process(void);
void Audio_WriteDataToDAC(void);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */


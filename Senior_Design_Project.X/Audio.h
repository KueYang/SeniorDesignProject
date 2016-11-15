#ifndef AUDIO_H
#define	AUDIO_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PIC1                // Defines the PIC that is being programmed
    
/* Defines the max number of audio files to be open. */
#define MAX_NUM_OF_FILES        19
    
/* WAV File Header Definitions. */
#define WAV_HEADER_SIZE         44
#define WAV_CHUNK_ID            0 
#define WAV_CHUNK_SIZE          4
#define WAV_FORMAT              8
#define WAV_CHUNK1_SUB_ID       12
#define WAV_CHUNK1_SUB_SIZE     16
#define WAV_AUDIO_FORMAT        20
#define WAV_NUM_CHANNELS        22
#define WAV_SAMPLE_RATE         24
#define WAV_BYTE_RATE           28
#define WAV_BLOCK_ALIGN         32
#define WAV_BITS_PER_SAMPLE     34
#define WAV_CHUNK2_SUB_ID       36
#define WAV_CHUNK2_SUB_SIZE     40
#define WAV_DATA                44

/* WAV File Errors */
#define WAV_SUCCESS                 1
#define WAV_HEADER_SIZE_ERROR       2
#define WAV_CHUNK_ID_ERROR          3
#define WAV_FORMAT_ERROR            4
#define WAV_SUB_CHUNK1_ID_ERROR     5
#define WAV_SUB_CHUNK1_SIZE_ERROR   6
#define WAV_SUB_CHUNK2_ID_ERROR     7

void AUDIO_Init(void);
void AUDIO_Process(void);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */


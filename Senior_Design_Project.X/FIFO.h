/**
 * @file FIFO.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef FIFO_H
#define	FIFO_H

#ifdef	__cplusplus
extern "C" {
#endif

/**@def BUFFERSIZE 
 * Defines the buffer size used for the FIFO queue. */
#define MON_BUFFERSIZE  512
/** @def REC_BUF_SIZE 
 * Defines the receive buffer size. */
#define REC_BUF_SIZE    8192
/** @def REC_BUF_SIZE 
 * Defines the audio buffer size. */
#define AUDIO_BUF_SIZE  REC_BUF_SIZE/4

/**
 * @brief FIFO data structure.
 * @details The FIFO data structure is used to create and store a FIFO queue.
 */
typedef struct MON_FIFO
{
    /**@{*/
    char buffer[MON_BUFFERSIZE];    /**< Variable used to store the FIFO data. */
    UINT16  headPtr;               /**< Variable used to point to the front of the queue. */
    UINT16  tailPtr;               /**< Variable used to point to the back of the queue. */
    UINT16  bufferSize;            /**< Variable used to stores the queue size. */
    BOOL (*FIFO_MonPush)();
    char (*FIFO_MonPop)();
    /**@}*/
}MON_FIFO;

typedef struct RECEIVE_FIFO
{
    /**@{*/
    BYTE buffer[REC_BUF_SIZE];    /**< Variable used to store the FIFO data. */
    UINT16  headPtr;               /**< Variable used to point to the front of the queue. */
    UINT16  tailPtr;               /**< Variable used to point to the back of the queue. */
    UINT16  bufferSize;            /**< Variable used to stores the queue size. */
    BOOL (*FIFO_ReceivePush)();
    char (*FIFO_ReceivePop)();
    /**@}*/
}RECEIVE_FIFO;

typedef struct AUDIO_FIFO
{
    /**@{*/
    WORD buffer[AUDIO_BUF_SIZE];    /**< Variable used to store the FIFO data. */
    UINT16  headPtr;               /**< Variable used to point to the front of the queue. */
    UINT16  tailPtr;               /**< Variable used to point to the back of the queue. */
    UINT16  bufferSize;            /**< Variable used to stores the queue size. */
    BOOL (*FIFO_AudioPush)();
    char (*FIFO_AudioPop)();
    /**@}*/
}AUDIO_FIFO;

char FIFO_MonPop(MON_FIFO* fifo);
BOOL FIFO_MonPush(MON_FIFO* fifo, char ch);

char FIFO_ReceivePop(RECEIVE_FIFO* fifo);
BOOL FIFO_ReceivePush(RECEIVE_FIFO* fifo, char ch);

char FIFO_AudioPop(AUDIO_FIFO* fifo);
BOOL FIFO_AudioPush(AUDIO_FIFO* fifo, char ch);

#ifdef	__cplusplus
}
#endif

#endif	/* FIFO_H */


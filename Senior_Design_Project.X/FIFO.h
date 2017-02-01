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
#define BUFFERSIZE  2048

/**
 * @brief FIFO data structure.
 * @details The FIFO data structure is used to create and store a FIFO queue.
 */
typedef struct FIFO
{
    /**@{*/
    char buffer[BUFFERSIZE];    /**< Variable used to store the FIFO data. */
    UINT16  headPtr;               /**< Variable used to point to the front of the queue. */
    UINT16  tailPtr;               /**< Variable used to point to the back of the queue. */
    UINT16  bufferSize;            /**< Variable used to stores the queue size. */
    /**@}*/
}FIFO;

char FIFO_Pop(FIFO* fifo);
BOOL FIFO_Push(FIFO* fifo, char ch);

#ifdef	__cplusplus
}
#endif

#endif	/* FIFO_H */


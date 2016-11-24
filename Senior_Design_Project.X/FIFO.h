/**
 * @file Audio.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Header for the FIFO Module. 
 */

#ifndef FIFO_H
#define	FIFO_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUFFERSIZE  1024

/** @typedef FIFO structure. */
typedef struct fifo{
    char buffer[BUFFERSIZE];
    int  headPtr;
    int  tailPtr;
    int  bufferSize;
}FIFO;

char FIFO_Pop(FIFO* fifo);
BOOL FIFO_Push(FIFO* fifo, char ch);

#ifdef	__cplusplus
}
#endif

#endif	/* FIFO_H */


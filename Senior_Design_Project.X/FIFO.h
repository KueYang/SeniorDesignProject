#ifndef FIFO_H
#define	FIFO_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUFFERSIZE  1024
    
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


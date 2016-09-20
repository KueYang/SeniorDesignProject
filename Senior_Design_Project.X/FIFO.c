#include "STDDEF.h"
#include "FIFO.h"
#include "UART.h"

BOOL FIFO_Push(FIFO* fifo, char ch)
{
    if(fifo->bufferSize >= BUFFERSIZE)
    {
        return FALSE;
    }
    fifo->buffer[fifo->tailPtr++] = ch;
    fifo->bufferSize++;
    
    if(fifo->tailPtr >= BUFFERSIZE)
    {
        fifo->tailPtr = 0;
    }
    return TRUE;
}

char FIFO_Pop(FIFO* fifo)
{
    fifo->bufferSize--;
    char ch = fifo->buffer[fifo->headPtr++];
    
    if(fifo->headPtr >= BUFFERSIZE)
    {
        fifo->headPtr = 0;
    }
    return ch;
}

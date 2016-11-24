/**
 * @file FIFO.c
 * @author Kue Yang
 * @date 11/22/2016
 * @brief The FIFO Module.
 */

#include "STDDEF.h"
#include "FIFO.h"
#include "UART.h"

/**
 * @brief Pushes data into the FIFO queue.
 * @return Returns a boolean to indicate whether operation is successful or not.
 * @retval TRUE if pushing data to queue is successful.
 * @retval FALSE if pushing data to queue fails.
 */
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

/**
 * @brief Pops data from the FIFO queue.
 * @return Returns a boolean to indicate whether operation is successful or not.
 * @retval TRUE if popping data from the queue is successful.
 * @retval FALSE if popping data from the queue fails.
 */
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

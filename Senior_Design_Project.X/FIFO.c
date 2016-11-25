/**
 * @file FIFO.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The FIFO module handles all FIFO related tasks. The FIFO module 
 * handles pushing and popping data into a given FIFO queue for processing.
 */

#include "STDDEF.h"
#include "FIFO.h"
#include "UART.h"

/**
 * @brief Pushes data into the FIFO queue.
 * @arg fifo The FIFO buffer that will be receiving data.
 * @arg ch The data that will be inserted into the FIFO.
 * @return Returns a boolean to indicate whether operation is successful or not.
 * @retval TRUE If pushing data to queue is successful.
 * @retval FALSE If pushing data to queue is unsuccessful.
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
 * @arg fifo The FIFO buffer that data will be read from.
 * @return Returns a boolean to indicate whether the operation is successful or not.
 * @retval TRUE If popping data from the queue is successful.
 * @retval FALSE If popping data from the queue fails.
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

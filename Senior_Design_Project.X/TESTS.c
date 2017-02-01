/* 
 * File:   TestNew.c
 * Author: Belinda Yang
 *
 * Created on January 30, 2017, 10:52 PM
 */

#include <p32xxxx.h>
#include <plib.h>
#include "TIMER.h"
#include "UART.h"
#include "DAC.h"
#include "AudioNew.h"
#include "TESTS.h"

void Test1_TimeReadWrite(void);
void Test2_TimeRead(void);
void Test3_TimeWrite(void);

UINT32 startTime;
UINT32 endTime;
UINT32 timeToRead;
char buf[128];

void Test_SelectTest(UINT16 test)
{
    switch(test)
    {
        case 0:
            Test1_TimeReadWrite();
            break;
        case 1:
            Test2_TimeRead();
            break;
        case 2:
            Test3_TimeWrite();
            break;
    }
}


void Test1_TimeReadWrite(void)
{
    startTime = TIMER_GetMSecond();
    endTime = 0;
    timeToRead = 0;
    
    int bytesWritten = 0;
    int bytesRead = 0;
    
    while(!(AUDIONEW_isDoneReading() && AUDIONEW_isDoneWriting()))
    {
        bytesWritten = AUDIONEW_getBytesWritten();
        bytesRead = AUDIONEW_getBytesRead();

        if(bytesRead == bytesWritten)
        {
            AUDIONEW_ReadDataFromMemory(REC_BUF_SIZE);
        }
        else
        {
            AUDIONEW_WriteDataToDAC();
        }
    }
    
    endTime = TIMER_GetMSecond();
    timeToRead = endTime - startTime;
    
    MON_SendString("Test 1 - Read and Write Test");
    snprintf(&buf[0] ,128 ,"startTime: %d, endTime: %d, timeToRead: %d", startTime, endTime, timeToRead);
    MON_SendString(&buf[0]);
}

void Test2_TimeRead(void)
{
    startTime = TIMER_GetMSecond();
    endTime = 0;
    timeToRead = 0;
    
    AUDIONEW_ReadDataFromMemory(REC_BUF_SIZE);
    
    endTime = TIMER_GetMSecond();
    timeToRead = endTime - startTime;
    
    MON_SendString("Test 2 - Read Test");
    snprintf(&buf[0] ,128 ,"startTime: %d, endTime: %d, timeToRead: %d", startTime, endTime, timeToRead);
    MON_SendString(&buf[0]);
}

void Test3_TimeWrite(void)
{
    startTime = 0;
    endTime = 0;
    timeToRead = 0;
    
    AUDIONEW_ReadDataFromMemory(REC_BUF_SIZE);
    
    startTime = TIMER_GetMSecond();
    
    AUDIONEW_WriteDataToDAC();
    
    endTime = TIMER_GetMSecond();
    timeToRead = endTime - startTime;
    
    MON_SendString("Test 3 - Write Test");
    snprintf(&buf[0] ,128 ,"startTime: %d, endTime: %d, timeToRead: %d", startTime, endTime, timeToRead);
    MON_SendString(&buf[0]);
}




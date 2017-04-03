/* 
 * File:   TestNew.c
 * Author: Belinda Yang
 *
 * Created on January 30, 2017, 10:52 PM
 */

#include <p32xxxx.h>
#include "plib/plib.h"
#include "STDDEF.h"
#include "TIMER.h"
#include "UART.h"
#include "DAC.h"
#include "AUDIO.h"
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
    
    while(!(AUDIO_isDoneReading() && AUDIO_isDoneWriting()))
    {
        bytesWritten = AUDIO_getBytesWritten();
        bytesRead = AUDIO_getBytesRead();

        if((bytesRead == bytesWritten) && !AUDIO_isDoneReading())
        {
            AUDIO_ReadFile(REC_BUF_SIZE);
        }
        else
        {
            AUDIO_WriteDataToDAC();
        }
    }
    
    endTime = TIMER_GetMSecond();
    timeToRead = endTime - startTime;
    
//    MON_SendString("Test 1 - Read and Write Test");
//    snprintf(&buf[0] ,128 ,"startTime: %d, endTime: %d, timeToRead: %d", startTime, endTime, timeToRead);
//    MON_SendString(&buf[0]);
    AUDIO_resetFilePtr();
}

void Test2_TimeRead(void)
{
    startTime = TIMER_GetMSecond();
    endTime = 0;
    timeToRead = 0;
    
    if(AUDIO_ReadFile(REC_BUF_SIZE))
    {
        endTime = TIMER_GetMSecond();
        timeToRead = endTime - startTime;

//        MON_SendString("Test 2 - Read Test");
//        snprintf(&buf[0] ,128 ,"startTime: %d, endTime: %d, timeToRead: %d", startTime, endTime, timeToRead);
//        MON_SendString(&buf[0]);
        AUDIO_resetFilePtr();
    }
    else
    {
//        MON_SendString("Test 2 - Failed to read file.");
    }
}

void Test3_TimeWrite(void)
{
    startTime = 0;
    endTime = 0;
    timeToRead = 0;
    
    AUDIO_ReadFile(REC_BUF_SIZE);
    
    startTime = TIMER_GetMSecond();
    
    AUDIO_WriteDataToDAC();
    
    endTime = TIMER_GetMSecond();
    timeToRead = endTime - startTime;
    
//    MON_SendString("Test 3 - Write Test");
//    snprintf(&buf[0] ,128 ,"startTime: %d, endTime: %d, timeToRead: %d", startTime, endTime, timeToRead);
//    MON_SendString(&buf[0]);
    AUDIO_resetFilePtr();
}




#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "Timer.h"
#include "FIFO.h"
#include "UART.h"

#define UART_MODULE_ID          UART1
#define DESIRED_BAUDRATE        (19200)     //The desired BaudRate

void UART_sendString(const char *string);
void UART_sendCharacter(const char *character);
BOOL UART_isBufferEmpty(FIFO* buffer);
char UART_getNextChar(FIFO* buffer);
void UART_putNextChar(FIFO* buffer, char ch);

void UART_processCommand(void);
void MON_removeWhiteSpace(const char* string);
void MON_COM_GetHelp(void);
void MON_COM_GetInputTest(void);

UINT32 actualBaudRate; 
FIFO rxBuffer, txBuffer;
UINT32 commandListSize;

COMMANDS MON_COMMANDS[] = {
    {"HELP", 0, "Display the list of commands avaliable.", MON_COM_GetHelp},
    {"TEST", 1, "Test getting commands. FORMAT: TEST arg1 arg2.", MON_COM_GetInputTest},
    {""}
};


void UART_Init(void)
{
    commandListSize = 2;
    
    // Re-mapped pins RB14 and RB15 pins to U1RX and U1TX
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(3,U1RX,RPA2);     // Assign RPA2 as input pin for U1RX
        PPSOutput(1,RPB3,U1TX);    // Set RPB3R pin as output for U1TX
        PPSLock;
    });
    
    // Configure UART1 module
    UARTConfigure(UART_MODULE_ID, UART_ENABLE_PINS_TX_RX_ONLY | UART_INVERT_RECEIVE_POLARITY |UART_INVERT_TRANSMIT_POLARITY);
    UARTSetFifoMode(UART_MODULE_ID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_MODULE_ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    actualBaudRate = UARTSetDataRate(UART_MODULE_ID, GetPeripheralClock(), DESIRED_BAUDRATE);
    UARTEnable(UART_MODULE_ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    // Configure UART RX1 and TX1 Interrupt
    INTEnable(INT_SOURCE_UART_RX(UART_MODULE_ID), INT_DISABLED);
    INTEnable(INT_SOURCE_UART_TX(UART_MODULE_ID), INT_DISABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_SUB_PRIORITY_LEVEL_1);
}

int UART_GetBaudRate(int desireBaud)
{
    int baudrate = 0;

    baudrate = ((GetPeripheralClock()/desireBaud)/16) - 1;

    return baudrate;
}

void UART_Process(void)
{
    UART_sendString("> This is a test. \n\r");
    mPORTBToggleBits(BIT_5);
//    TIMER_MSecondDelay(1000);
//    UART_processCommand();
} 

// UART Receive Functions Handlers
void UART_processCommand(void)
{
    if(!UART_isBufferEmpty(&rxBuffer))
    {
        int command = 0;
        
        switch(command)
        {
            case 0:
                MON_COM_GetHelp();
                break;
            case 1:
                
                break;
            default:
                UART_sendString(">");
                break;
        }
    }
}

// UART Transmission Functions Handlers
void UART_sendString(const char *string)
{
    while(*string != '\0')
    {
        while(!UARTTransmitterIsReady(UART_MODULE_ID));
        UARTSendDataByte(UART_MODULE_ID, *string);
        string++;
    }
    while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
}

void UART_sendCharacter(const char* character)
{
        while(!UARTTransmitterIsReady(UART_MODULE_ID));
        UARTSendDataByte(UART_MODULE_ID, *character);
        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
}

void __ISR(_UART1_VECTOR, IPL2AUTO) IntUart1Handler(void)
{
	if(INTGetFlag(INT_SOURCE_UART_RX(UART_MODULE_ID)))
	{
		BYTE data = UARTGetDataByte(UART_MODULE_ID);
        // Writes data to receive buffer.
        UART_putNextChar(&rxBuffer, data);
        // Toggle LED to indicate UART activity
        mPORTBToggleBits(BIT_5);
        // Clear the RX interrupt Flag
	    INTClearFlag(INT_SOURCE_UART_RX(UART_MODULE_ID));
	}
}

void UART_putNextChar(FIFO* buffer, char ch)
{
    FIFO_Push(buffer, ch);
}

char UART_getNextChar(FIFO* buffer)
{
    return FIFO_Pop(buffer);
}

BOOL UART_isBufferEmpty(FIFO* buffer)
{
    if(buffer->bufferSize > 0)
    {
        return FALSE;
    }
    return TRUE;
}

void MON_removeWhiteSpace(const char* string)
{
    while(string != "\0")
    {
        if(string == " ")
        {
            string = string++;
        }
        string++;
    }
}

void MON_spliceCommand(const char* string)
{
    
}

void MON_COM_GetHelp(void)
{
    int i = 0;
    char buf[1024];
    
    for(i = 0; i < commandListSize; i++)
    {
        sprintf(buf, "%s, %s \r\n", MON_COMMANDS[i].commandName, MON_COMMANDS[i].commandInfo);
        UART_sendString(buf);
    }  
}

void MON_COM_GetInputTest(void)
{
    
}
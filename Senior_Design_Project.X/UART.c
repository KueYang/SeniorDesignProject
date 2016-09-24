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
void UART_sendCharacter(const char character);
BOOL UART_isBufferEmpty(FIFO* buffer);
char UART_getNextChar(FIFO* buffer);
void UART_putNextChar(FIFO* buffer, char ch);

void UART_processCommand(void);
void MON_parseCommand(char** cmd, FIFO* buffer);
COMMANDS MON_getCommand(const char* cmd);
void MON_removeWhiteSpace(const char* string);

void MON_GetHelp(void);
void MON_GetInputTest(void);

FIFO rxBuffer, txBuffer;
UINT32 cmdListSize;
UINT16 actualBaudRate;

COMMANDS MON_COMMANDS[] = {
    {"HELP", "Display the list of commands avaliable.", MON_GetHelp},
    {"TEST", "Test getting commands. FORMAT: TEST arg1 arg2.", MON_GetInputTest},
    {"", "", NULL}
};

void UART_Init(void)
{ 
    cmdListSize = 2;
    
    // Re-mapped pins RB14 and RB15 pins to U1RX and U1TX
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(3,U1RX,RPA2);     // Assign RPA2 as input pin for U1RX
        PPSOutput(1,RPB3,U1TX);    // Set RPB3R pin as output for U1TX
        PPSLock;
    });
    
    // Configure UART1 module
    UARTConfigure(UART_MODULE_ID, UART_ENABLE_PINS_TX_RX_ONLY | UART_INVERT_RECEIVE_POLARITY | UART_INVERT_TRANSMIT_POLARITY);
    UARTSetFifoMode(UART_MODULE_ID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_MODULE_ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    actualBaudRate = UARTSetDataRate(UART_MODULE_ID, GetPeripheralClock(), DESIRED_BAUDRATE);
    UARTEnable(UART_MODULE_ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    // Configure UART RX1 and TX1 Interrupt
    INTEnable(INT_SOURCE_UART_RX(UART_MODULE_ID), INT_ENABLED);
    INTEnable(INT_SOURCE_UART_TX(UART_MODULE_ID), INT_DISABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_SUB_PRIORITY_LEVEL_1);
    
    UART_sendString("> ");
}

int UART_GetBaudRate(int desireBaud)
{
    return ((GetPeripheralClock()/desireBaud)/16) - 1;
}

void UART_Process(void)
{
//    UART_processCommand();
//    TIMER_MSecondDelay(5000);
} 

// UART Receive Functions Handlers
void UART_processCommand(void)
{
    if(!UART_isBufferEmpty(&rxBuffer))
    {
        char* command[3];
        
        /* Parses command from receive buffer. */
        MON_parseCommand(command, &rxBuffer);
        
        /* Grabs the command handler. */
        MON_getCommand(command[0]);
        
        /* Prepares for the next command. */
        UART_sendString("> ");
    }
}

void MON_parseCommand(char** cmd, FIFO* buffer)
{   
    int i = 0;
    char* str = "";
    
    /* Pops off the first character in the receive buffer. */
    char ch = UART_getNextChar(buffer);
    
    while(ch != '\r')
    {
        /* 
         * If character is a space, save the string and continue parsing other strings. 
         * Concatenate character to the current string value if next character isn't a space. 
         */
        if(ch == ' ')
        {
            cmd[i++] = str;
        }
        else
        {
            strncat(str, &ch,1);
        }
        /* Pops off the next character in the receive buffer. */
        ch = UART_getNextChar(buffer);
    }
}

COMMANDS MON_getCommand(const char* cmd)
{
    int i = 0;
    for(i = 0; i < cmdListSize; i++)
    {
        if(cmd == MON_COMMANDS[i].name)
        {
            return MON_COMMANDS[i];
        }
    }
    return MON_COMMANDS[cmdListSize - 1];
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

void UART_sendCharacter(const char character)
{
        while(!UARTTransmitterIsReady(UART_MODULE_ID));
        UARTSendDataByte(UART_MODULE_ID, character);
        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
}

void __ISR(_UART1_VECTOR, IPL4AUTO) IntUart1Handler(void)
{
	if(INTGetFlag(INT_SOURCE_UART_RX(UART_MODULE_ID)))
	{   
        /* Checks if bus collision has occurred and clears collision flag.*/
        if(IFS1bits.U1EIF)
        {
           U1STAbits.OERR = 0;
           IFS1bits.U1EIF=0;
        }
        else if(IFS1bits.U1RXIF)
        {
            /* Checks if there is data ready to read from the receive buffer. */
            if(UART_DATA_READY)
            {
                // Reads BYTEs from receive buffer.
                BYTE data = U1RXREG;

                // Writes data to receive buffer.
                UART_putNextChar(&rxBuffer, data);

                // Toggle LED to indicate UART activity
                mPORTBToggleBits(BIT_5);
            }
        }
        
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

void MON_GetHelp(void)
{
    
}

void MON_GetInputTest(void)
{
    
}
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
int MON_parseCommand(COMMANDSTR* cmd, FIFO* buffer);
COMMANDS MON_getCommand(COMMANDSTR* cmd);
void MON_removeWhiteSpace(const char* string);

void MON_GetHelp(void);
void MON_GetInputTest(void);

FIFO rxBuffer, txBuffer;
UINT32 cmdListSize;
UINT16 actualBaudRate;
BOOL cmdReady;

COMMANDS MON_COMMANDS[] = {
    {"HELP", "Display the list of commands avaliable.", MON_GetHelp},
    {"TEST", "Test getting commands. FORMAT: TEST arg1 arg2.", MON_GetInputTest},
    {"", "", NULL}
};

void UART_Init(void)
{ 
    cmdListSize = 2;
    cmdReady = FALSE;
    
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
    UART_processCommand();
} 

// UART Receive Functions Handlers
void UART_processCommand(void)
{
    if(!UART_isBufferEmpty(&rxBuffer) && cmdReady == TRUE)
    {
        COMMANDSTR cmdStr;
        COMMANDS command;
        int numOfArgs = 0;
        
        /* Parses command from receive buffer. */
        numOfArgs = MON_parseCommand(&cmdStr, &rxBuffer);
        
        /* Grabs the command handler. */
        command = MON_getCommand(&cmdStr);
        command.handler();
        
        /* Prepares for the next command. */
        UART_sendString("\n\r> ");
        
        cmdReady = FALSE;
    }
}

int MON_parseCommand(COMMANDSTR* cmd, FIFO* buffer)
{   
    int i = 0, numOfArgs = 0;
    int argIndexes[3] = {0,0,0};
    char str[24] = "";
    
    /* Pops off the first character in the receive buffer. */
    char ch = UART_getNextChar(buffer);
    /* Grabs the first command from the fifo buffer. */
    while(ch != '\r' && i < 24)
    {
        str[i++] = ch;
        ch = UART_getNextChar(buffer);
        
        if(ch == ' ' || ch == '\r')
        {
            argIndexes[numOfArgs++] = i;
        }
    }
    
    switch(numOfArgs)
    {
        case 1:
            cmd->name = &str[0];
            break;
        case 2:
            cmd->name = &str[0];
            cmd->arg1 = &str[argIndexes[1]+1];
            break;
        case 3:
            cmd->name = &str[0];
            cmd->arg1 = &str[argIndexes[0]+1];
            cmd->arg2 = &str[argIndexes[1]+1];
            break;
        default:
            
            break;
    }
    
    return numOfArgs;
}

COMMANDS MON_getCommand(COMMANDSTR* cmd)
{
    int i = 0;
    for(i = 0; i < cmdListSize; i++)
    {
        if(strcmp(cmd->name, MON_COMMANDS[i].name) == 0)
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
                
                // Sets a flag for end of receiving a command.
                if(data == '\r')
                {
                    cmdReady = TRUE;
                }
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
    int i =0;
    char buf[128];
    for(i = 0; i < cmdListSize; i++)
    {
        strncpy(&buf[0], MON_COMMANDS[i].name, sizeof(buf));
        strncat(buf, MON_COMMANDS[i].description, sizeof(MON_COMMANDS[i].description));
        UART_sendString(&buf[0]);
    }
}

void MON_GetInputTest(void)
{
    
}
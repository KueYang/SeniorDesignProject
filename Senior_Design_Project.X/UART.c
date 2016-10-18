#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "Timer.h"
#include "FIFO.h"
#include "UART.h"

#define UART_MODULE_ID          UART1
#define DESIRED_BAUDRATE        (19200)     //The desired BaudRate
#define WRITE_BUFFER_SIZE       256
#define CMD_SIZE                16
#define DESCRIPTION_SIZE        (WRITE_BUFFER_SIZE-CMD_SIZE)

BOOL UART_isBufferEmpty(FIFO* buffer);
char UART_getNextChar(FIFO* buffer);
void UART_putNextChar(FIFO* buffer, char ch);

void UART_processCommand(void);
int MON_parseCommand(COMMANDSTR* cmd, FIFO* buffer);
COMMANDS MON_getCommand(const char* cmdName);
void MON_removeWhiteSpace(const char* string);
BOOL compareStrings(const char* str1, const char* str2);

void MON_GetHelp(void);
void MON_GetInputTest(void);

COMMANDSTR cmdStr;
FIFO rxBuffer, txBuffer;
UINT16 cmdListSize;
UINT16 actualBaudRate;
BOOL cmdReady;

COMMANDS MON_COMMANDS[] = {
    {"HELP", " Display the list of commands avaliable. \n\r", MON_GetHelp},
    {"TEST", " Test getting commands. FORMAT: TEST arg1 arg2. \n\r", MON_GetInputTest},
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
    
    UART_sendString("\r\n> ");
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
        int numOfArgs = 0;
        
        /* Parses command from receive buffer. */
        numOfArgs = MON_parseCommand(&cmdStr, &rxBuffer);
        
        /* Calls the command handler if the returned command isn't empty. */
        COMMANDS command = MON_getCommand(cmdStr.name);
        if(command.name != NULL)
        {
            command.handler();
        }
        
        /* Clears command variable. */
        memset(&cmdStr.name[0], 0, sizeof(cmdStr.name));
        memset(&cmdStr.arg1[0], 0, sizeof(cmdStr.arg1));
        memset(&cmdStr.arg2[0], 0, sizeof(cmdStr.arg2));
        
        /* Prepares for the next command. */
        UART_sendString("\n\r> ");
        
        cmdReady = FALSE;
    }
}

int MON_parseCommand(COMMANDSTR* cmd, FIFO* buffer)
{   
    int i = 0, numOfArgs = 0;
    int argIndexes[3] = {0,0,0};
    char str[32] = "";
    
    /* Pops off the first character in the receive buffer. */
    char ch = UART_getNextChar(buffer);
    int count = 0;
    int numOfBytes[3] = {0,0,0};
    /* Grabs the first command from the fifo buffer. */
    while(ch != '\r' && i < 32)
    {
        str[i++] = ch;
        ch = UART_getNextChar(buffer);
        count++;
        
        if(ch == ' ' || ch == '\r')
        {
            numOfBytes[numOfArgs] = count;
            argIndexes[numOfArgs++] = i-1;
        }
    }
    
    switch(numOfArgs)
    {
        case 1:
            strncpy(&cmd->name[0], &str[0], numOfBytes[0]);
        case 2:
            strncpy(&cmd->name[0], &str[0], numOfBytes[0]);
            strncpy(&cmd->arg1[0], &str[argIndexes[1]], numOfBytes[1]);
            break;
        case 3:
            strncpy(&cmd->name[0], &str[0], numOfBytes[0]);
            strncpy(&cmd->arg1[0], &str[argIndexes[1]], numOfBytes[1]);
            strncpy(&cmd->arg2[0], &str[argIndexes[2]], numOfBytes[2]);
            break;
        default:
            
            break;
    }
    
    return numOfArgs;
}

BOOL compareStrings(const char* str1, const char* str2)
{
    while (*str1 == *str2) 
    {
        if (*str1 == '\0' || *str2 == '\0')
        {
            break;
        }
        str1++;
        str2++;
    }

    if (*str1 == '\0' && *str2 == '\0')
    {
       return TRUE;
    }
    return FALSE; 
}

COMMANDS MON_getCommand(const char* cmdName)
{
    int i = 0;
    for(i = 0; i < cmdListSize; i++)
    {
        if(compareStrings(cmdName, MON_COMMANDS[i].name))
        {
            return MON_COMMANDS[i];
        }
    }
    return MON_COMMANDS[cmdListSize]; // returns an empty null command
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
    char buf[128] = "";
    for(i = 0; i < cmdListSize; i++)
    {
        memset(&buf[0], 0, sizeof(buf)); // Clears the buffer for each command.
        strncpy(&buf[0], MON_COMMANDS[i].name, sizeof(MON_COMMANDS[i].name));
        strncat(&buf[sizeof(MON_COMMANDS[i].name)], MON_COMMANDS[i].description, (128-sizeof(MON_COMMANDS[i].name)));
        UART_sendString(&buf[0]);
    }
}

void MON_GetInputTest(void)
{
    char buf[128] = "";
    
    strncpy(&buf[0], "TESTING ", 8);
    strncat(&buf[8], cmdStr.arg1, sizeof(cmdStr.arg1));
    strncat(&buf[sizeof(cmdStr.arg1)], cmdStr.arg2, (128-8-sizeof(cmdStr.arg1)));
    UART_sendString(&buf[0]);
}
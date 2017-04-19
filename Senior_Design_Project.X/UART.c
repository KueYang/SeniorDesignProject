/**
 * @file UART.c
 * @author Kue Yang
 * @date 11/22/2016
 * @details The UART module will handle serial communication. The module will be
 * used to receive commands via serial communications and can be use as a 
 * debugging tool. The serial communication uses the rs232 serial interface. 
 */

#include <p32xxxx.h>
#include "plib/plib.h"
#include <stdio.h>
#include <string.h>
#include "HardwareProfile.h"
#include "STDDEF.h"
#include "Timer.h"
#include "FIFO.h"
#include "DAC.h"
#include "AUDIO.h"
#include "UART.h"

/** @def DESIRED_BAUDRATE 
 * The desired UART baud rate. */
#define DESIRED_BAUDRATE        (19200)     //The desired BaudRate
/** @def WRITE_BUFFER_SIZE 
 * The buffer size for writing. */
#define WRITE_BUFFER_SIZE       128
/** @def CMD_SIZE 
 * The size of the command name string. */
#define CMD_SIZE                16
/** @def DESCRIPTION_SIZE 
 * The size of command description string. */
#define DESCRIPTION_SIZE        (WRITE_BUFFER_SIZE-CMD_SIZE)

/** UART Helper Functions. */
int UART_GetBaudRate(int desireBaud);

/** FIFO helper functions. */
BOOL UART_isBufferEmpty(MON_FIFO* buffer);
char UART_getNextChar(MON_FIFO* buffer);
void UART_putNextChar(MON_FIFO* buffer, char ch);

/** Command Helper Functions. */
void UART_processCommand(void);
int MON_parseCommand(COMMANDSTR* cmd, MON_FIFO* buffer);
COMMANDS MON_getCommand(const char* cmdName);

/** Commands Handlers. */
void MON_GetHelp(void);

/* File related commands*/
void MON_GetFileList(void);
void MON_Set_File(void);
void MON_Reset_File(void);
void MON_Read_File(void);

/* DAC related commands. */
void MON_TestDAC(void);
void MON_ZeroDAC(void);
void MON_SinDAC(void);

/* Timer related commands. */
void MON_Timer_ON_OFF(void);
void MON_Timer_Get_PS(void);
void MON_Timer_Set_PS(void);

/** @var cmdStr 
 * The command string. */
COMMANDSTR cmdStr;
/** @var rxBuffer 
 * The UART receive buffer. */
MON_FIFO rxBuffer;
/** @var txBuffer 
 * The UART transmit buffer. */
MON_FIFO txBuffer;
/** @var cmdReady 
 * The UART command receive flag. */
BOOL cmdReady;
/** @var actualBaudRate 
 * The configured UART baud rate. */
UINT16 actualBaudRate;
/** @var numOfCmds 
 * The number of commands. */
UINT16 numOfCmds;
/** @var MON_COMMANDS 
 * The list of commands. */
COMMANDS MON_COMMANDS[] = {
    {"HELP", " Display the list of commands avaliable. ", MON_GetHelp},
    {"LIST", " Lists all WAV files. ", MON_GetFileList},
    {"SET", " Sets the file to read. FORMAT: SET fileName.", MON_Set_File},
    {"RESET", " Resets the file pointer to beginning of file.", MON_Reset_File},
    {"READ", " Reads numOfBytes from current file. Read from beginning of the file if reset is set to 1. FORMAT: READ reset numOfBytes.", MON_Read_File},
    {"DAC", " Sets an output value on the DAC. MIN: 0, MAX: 65535. FORMAT: DAC value. ", MON_TestDAC},
    {"ZERO", " Sets all DAC outputs to zero. ", MON_ZeroDAC},
    {"SIN", " Tests the DAC using a sin wave. ", MON_SinDAC},
    {"TONE", " Toggles on/off the Audio Timer. ", MON_Timer_ON_OFF},
    {"PDG", " Get the current period set on timer 3. FORMAT: PDG.", MON_Timer_Get_PS},
    {"PDS", " Configures the timer period. FORMAT: PDS period .", MON_Timer_Set_PS},
    {"", "", NULL}
};

/**
 * @brief Initialize the UART module.
 * @return Void
 */
void UART_Init(void)
{ 
    cmdReady = FALSE;
    numOfCmds = sizeof(MON_COMMANDS)/sizeof(MON_COMMANDS[0]);
    
    // Re-mapped pins RPC1 and RPE5 pins to U1RX and U1TX
    mSysUnlockOpLock({
        PPSUnLock;
        PPSInput(1,U1RX,RPC1);     // Assign RPC1 as input pin for U1RX
        PPSOutput(2,RPE5,U1TX);    // Set RPE5 pin as output for U1TX
        PPSLock;
    });
    
    U1MODEbits.ON = 0;          // Disables the UART module
    U1MODEbits.SIDL = 0;        // Disables sleep on idle
    U1MODEbits.IREN = 0;        // IrDA is disabled
    U1MODEbits.RTSMD = 0;       // U1RTS pin is in Flow control Mode
    U1MODEbits.UEN = 0b00;      // UTX and URX enabled, CTS controlled by PORT register
    U1MODEbits.WAKE = 0;        // Wake-up disabled
    U1MODEbits.LPBACK = 0;      // Loop back disabled
    U1MODEbits.ABAUD = 0;       // Auto Baud rate disabled
    U1MODEbits.RXINV = 1;       // URX idle low
    U1MODEbits.PDSEL = 0b00;    // 8-bit data, no parity
    U1MODEbits.STSEL = 0;       // 1 Stop Bit
    U1MODEbits.BRGH = 0;        // Standard Speed mode, 16x baud clock enabled
    
    U1BRG = UART_GetBaudRate(DESIRED_BAUDRATE);
    
    U1STAbits.ADM_EN = 0;       // Auto Address Detect disabled
    U1STAbits.ADDEN = 0;        // Address Detect disabled
    U1STAbits.UTXEN = 1;        // UTX enabled
    U1STAbits.UTXINV = 1;       // UTX idle low
    U1STAbits.UTXISEL = 0b10;   // Interrupt when TX buffer empty
    U1STAbits.UTXBRK = 0;       // Break transmission disabled
    U1STAbits.URXEN = 1;        // URX enabled
    U1STAbits.URXISEL = 0b00;   // Interrupt when receive buffer isn't empty
    
    U1MODEbits.ON = 1;          // Enables the UART module
    
    // Configure UART RX1 and TX1 Interrupt
    IFS1bits.U1RXIF = 0;        // Clears Receive Interrupt Flag
    IEC1bits.U1RXIE = 1;        // Enables U1RX Interrupt Enable
    IFS1bits.U1TXIF = 0;        // Clears Transmit Interrupt Flag 
    IEC1bits.U1TXIE = 0;        // Disables U1TX Interrupt Enable
    IPC7bits.U1IP = 2;          // Sets UART Interrupt Priority 2
    IPC7bits.U1IS = 1;          // Sets UART Interrupt Sub-Priority 2
    
//    MON_GetHelp();
    MON_SendString(">");  // Sends a prompt.
}

/**
 * @brief Calculates the baud rate configuration.
 * @arg desireBaud The desired baud rate.
 * @return Returns the baud rate for the given peripheral clock.
 */
int UART_GetBaudRate(int desireBaud)
{
    return ((GetPeripheralClock()/(16*desireBaud))) - 1;
}

/**
 * @brief Processes all UART related tasks.
 * @return Void.
 */
void UART_Process(void)
{
//    UART_processCommand();
} 

/**
 * @brief Processes commands received by the UART module.
 * @return Void.
 */
void UART_processCommand(void)
{
    if(!UART_isBufferEmpty(&rxBuffer) && cmdReady == TRUE)
    {
        int numOfArgs = 0;

        /* Parses command from receive buffer. */
        numOfArgs = MON_parseCommand(&cmdStr, &rxBuffer);
        
        if(numOfArgs != -1)
        {
            /* Calls the command handler if the returned command isn't empty. */
            COMMANDS command = MON_getCommand(cmdStr.name);
            if(command.handler != NULL)
            {
                command.handler();
            }
            else
            {
                MON_SendString("Command doesn't exist.");
            }

            /* Clears command variable. */
            memset(&cmdStr.name[0], 0, sizeof(cmdStr.name));
            memset(&cmdStr.arg1[0], 0, sizeof(cmdStr.arg1));
            memset(&cmdStr.arg2[0], 0, sizeof(cmdStr.arg2));
        }
        
        /* Prepares for the next command. */
        MON_SendString(">");
        
        /* Resets the ready flag. */
        cmdReady = FALSE;
    }
}

/**
 * @brief Parses the received commands.
 * @arg cmd The command data structure used to store the command.
 * @arg buffer The receive buffer.
 * @return Returns the number of command arguments.
 */
int MON_parseCommand(COMMANDSTR* cmd, MON_FIFO* buffer)
{   
    int i = 0, numOfArgs = 0;
    int argIndexes[3] = {0,0,0};
    char str[64] = "";
    
    /* Pops off the first character in the receive buffer. */
    char ch = UART_getNextChar(buffer);
    /* Checks for a return character and empty space. */
    if(ch == '\r' || ch == ' ')
    {
        return -1;
    }
    
    int count = 0;
    int numOfBytes[3] = {0,0,0};
    
    /* Grabs the first command from the fifo buffer. */
    while(ch != '\r' && i < 64)
    {
        str[i++] = (char)MON_lowerToUpper(&ch);
        count++;
        ch = UART_getNextChar(buffer);
        
        if(ch == ' ' || ch == '\r')
        {
            numOfBytes[numOfArgs] = count;
            argIndexes[numOfArgs++] = i-count+1;
            count = 0;
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

/**
 * @brief Transmit a character.
 * @details Add character to transmit fifo buffer.
 * @arg character The character to transmit.
 * @return Void.
 */
void MON_SendChar(const char* character)
{
    UART_putNextChar(&txBuffer, *character);
    
    IFS1bits.U1TXIF = 0;        // Clears Transmit Interrupt Flag 
    IEC1bits.U1TXIE = 1;        // Enables U1TX Interrupt Enable
}

/**
 * @brief Transmit a string.
 * @details Adds string to transmit fifo buffer.
 * @arg str The string to transmit.
 * @return Void.
 */
void MON_SendString(const char* str)
{
    if(*str == '>')
    {
        UART_putNextChar(&txBuffer, *str);
    }
    else
    {
        while(*str != '\0')
        {
            UART_putNextChar(&txBuffer, *str);
            str++;
        }

        UART_putNextChar(&txBuffer, '\n');
        UART_putNextChar(&txBuffer, '\r');
    }
    
    IFS1bits.U1TXIF = 0;        // Clears Transmit Interrupt Flag 
    IEC1bits.U1TXIE = 1;        // Enables U1TX Interrupt Enable
}

/**
 * @brief Transmit a string.
 * @details Transmit a string without a newline or return character.
 * @arg str The string to transmit.
 * @return Void.
 */
void MON_SendStringNR(const char* str)
{
    if(*str == '>')
    {
        UART_putNextChar(&txBuffer, *str);
    }
    else
    {
        while(*str != '\0')
        {
            UART_putNextChar(&txBuffer, *str);
            str++;
        }
    }
    
    IFS1bits.U1TXIF = 0;        // Clears Transmit Interrupt Flag 
    IEC1bits.U1TXIE = 1;        // Enables U1TX Interrupt Enable
}

/**
 * @brief Changes character to undercase.
 * @details Changes character to undercase.
 * @arg ch The character to undercase.
 * @return The undercase character
 */
char MON_lowerToUpper(const char* ch)
{
    char uChar = (*ch);
    if (uChar >= 'a' && uChar <= 'z') 
    {
         uChar = uChar - 32;
    }
    return uChar;
}

/**
 * @brief Checks if two strings matches.
 * @arg str1 The first string to compare.
 * @arg str2 The second string to compare.
 * @return Returns a boolean indicating if the strings matches.
 * @retval TRUE if both strings matches.
 * @retval FALSE if the strings does not match.
 */
BOOL MON_stringsMatch(const char* str1, const char* str2)
{
    /* Loops through both strings until reaching a null terminator or if neither strings matches. */
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

/**
 * @brief Removes white spaces from a string.
 * @arg string The string that is being modified.
 * @return Void.
 */
void MON_removeWhiteSpace(const char* string)
{
    while(*string != '\0')
    {
        if(*string == ' ')
        {
            string = string++;
        }
        string++;
    }
}

/**
 * @brief Gets the length of a string.
 * @arg string The string that is being modified.
 * @return Returns the string length.
 */
UINT16 MON_getStringLength(const char* string)
{
    UINT16 length = 0;
    while(*string != '\0')
    {
        string++;
        length++;
    }
    return length;
}

/**
 * @brief Gets the handler for the specified command.
 * @arg cmdName The name of the command.
 * @return Returns the command handler.
 */
COMMANDS MON_getCommand(const char* cmdName)
{
    int i = 0;
    for(i = 0; i < numOfCmds; i++)
    {
        if(MON_stringsMatch(cmdName, MON_COMMANDS[i].name))
        {
            return MON_COMMANDS[i];
        }
    }
    return MON_COMMANDS[numOfCmds]; // returns an empty null command
}

/**
 * @brief The UART1 Interrupt Service Routine.
 * @details The UART1 interrupt service routine will handle receiving data. If
 * data is received, the data is pushed into a FIFO queue for later processing. 
 * If data is received is a return key, the received data has ended and the 
 * command is ready flag is set. 
 * @return Void.
 */
void __ISR(_UART1_VECTOR, IPL2AUTO) IntUart1Handler(void)
{
    CLEAR_WATCHDOG_TIMER;
    
	if(IFS1bits.U1RXIF)
	{   
        /* Checks if bus collision has occurred and clears collision flag.*/
        if(IFS1bits.U1EIF)
        {
           U1STAbits.OERR = 0;
           IFS1bits.U1EIF=0;
        }
        else
        {
            /* Checks if there is data ready to read from the receive buffer. */
            if(U1STAbits.URXDA == 1)
            {
                // Reads BYTEs from receive buffer.
                BYTE rxData = U1RXREG;

                // Writes data to receive buffer.
                UART_putNextChar(&rxBuffer, rxData);
                
                // Sets a flag for end of receiving a command.
                if(rxData == '\r')
                {
                    cmdReady = TRUE;
                    UART_processCommand();
                }
            }
        }
        
        // Clear the RX interrupt Flag.
        IFS1bits.U1RXIF = 0;
	}
    
    if(IFS1bits.U1TXIF)
	{   
        /* Checks if bus collision has occurred and clears collision flag.*/
        if(IFS1bits.U1EIF)
        {
           U1STAbits.OERR = 0;
           
            IFS1bits.U1TXIF = 0;        // Clears Transmit Interrupt Flag 
            IEC1bits.U1TXIE = 1;        // Enables U1TX Interrupt Enable
        }
        else
        {
            /* Checks if there is data ready to read from the receive buffer. */
            if(!U1STAbits.UTXBF && !UART_isBufferEmpty(&txBuffer))
            {
                BYTE txData = UART_getNextChar(&txBuffer);
                U1TXREG = txData;
            }
            /* Checks if the transmit buffer is empty. If so, disable the TX interrupt. */
            if(UART_isBufferEmpty(&txBuffer) && U1STAbits.TRMT)
            {
                IFS1bits.U1TXIF = 0;        // Clears Transmit Interrupt Flag 
                IEC1bits.U1TXIE = 0;        // Disables U1TX Interrupt Enable
            }
        }
        
        // Clear the TX interrupt Flag.
        IFS1bits.U1TXIF = 0;
	}
    
    CLEAR_WATCHDOG_TIMER;
}

/**
 * @brief Pushes the specified character into the buffer.
 * @arg buffer The buffer used to store the character.
 * @arg ch The character to push into buffer.
 * @return Void.
 */
void UART_putNextChar(MON_FIFO* buffer, char ch)
{
    FIFO_MonPush(buffer, ch);
}

/**
 * @brief Pops a character from the buffer.
 * @arg buffer The buffer to pop the character from.
 * @return Returns the character that is popped off from the buffer.
 */
char UART_getNextChar(MON_FIFO* buffer)
{
    return FIFO_MonPop(buffer);
}

/**
 * @brief Checks if the specified buffer is empty.
 * @arg buffer The buffer that is being checked.
 * @return Returns a boolean indicating if the buffer is empty.
 * @retval TRUE if the buffer is empty.
 * @retval FALSE if the buffer is not empty.
 */
BOOL UART_isBufferEmpty(MON_FIFO* buffer)
{
    if(buffer->bufferSize > 0)
    {
        return FALSE;
    }
    return TRUE;
}

/**
 * @brief Displays the list of available commands.
 * @return Void.
 */
void MON_GetHelp(void)
{
    int i = 0;
    UINT16 strLength = 0;
    char buf[128] = "";
    for(i = 0; i < numOfCmds; i++)
    {
        memset(&buf[0], 0, sizeof(buf)); // Clears the buffer for each command.
        strLength = MON_getStringLength(MON_COMMANDS[i].name);
        strncpy(&buf[0], MON_COMMANDS[i].name, strLength);
        strncat(&buf[strLength], MON_COMMANDS[i].description, (128-strLength));
        MON_SendString(&buf[0]);
    }
}

/**
 * @brief Command used to display a list of files.
 * @return Void.
 */
void MON_GetFileList(void)
{
    AUDIO_ListFiles();
}

/**
 * @brief Command used to select a specific file.
 * @return Void.
 */
void MON_Set_File(void)
{
    UINT16 selectedFile = atoi(cmdStr.arg1);
    if(AUDIO_setNewFile(selectedFile))
    {
        MON_SendString("The new file has been set.");
    }
    else
    {
        MON_SendString("Failed to open the file. Make sure the file exists.");
    }
}

/**
 * @brief Command used to reset the selected file pointer
 * @return Void.
 */
void MON_Reset_File(void)
{
    AUDIO_resetFilePtr();
    MON_SendString("The file pointer has been reset.");
}

/**
 * @brief Command used to read the selected file.
 * @return Void.
 */
void MON_Read_File(void)
{
    UINT16 reset = atoi(cmdStr.arg1);
    UINT16 bytesToRead = atoi(cmdStr.arg2);
    BYTE* bufPtr;
    char buf[16];
    int i = 0;
    
    if(reset)
    {
        AUDIO_resetFilePtr();
    }
    
    AUDIO_ReadFile(bytesToRead);
    bufPtr = (BYTE*)AUDIO_GetRecieveBuffer();

    // Prints out the columns
    MON_SendString("       0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B"); 
    MON_SendString("     ---------------------------------------------------------------");
    // Prints out the rows
    MON_SendStringNR("0x00 | ");
    
    for(i = 1; i <= bytesToRead; i++)
    {
        snprintf(&buf[0], 16, "0x%02x ", bufPtr[i-1]);
        MON_SendStringNR(&buf[0]);
        if(i%12 == 0)
        {
            MON_SendString("");   // Adds a new line and returns
            snprintf(&buf[0], 16, "0x%02x | ", i-1);
            MON_SendStringNR(&buf[0]);
        }
    }
    MON_SendString("");   // Adds a new line and returns
}

/**
 * @brief Command used to write data to the DAC.
 * @return Void.
 */
void MON_TestDAC(void)
{
    UINT32 value = atoi(cmdStr.arg1);
    
    //Verifies that the value received is within range.
    if(value >= 65535)
    {
        value = 65535;
    }
    else if(value <= 0)
    {
        value = 0;
    }
    DAC_WriteToDAC(WRITE_UPDATE_CHN_A_B, value);
}

/**
 * @brief Command used to zero the DAC's output.
 * @return Void.
 */
void MON_ZeroDAC(void)
{
    DAC_ZeroOutput();
}

/**
 * @brief Command used to write a sin wav to the DAC.
 * @return Void.
 */
void MON_SinDAC(void)
{
    WORD testBytes[1024] = {0x400,0x406,0x40d,0x413,0x419,0x41f,0x426,0x42c,
0x432,0x439,0x43f,0x445,0x44b,0x452,0x458,0x45e,
0x464,0x46b,0x471,0x477,0x47d,0x484,0x48a,0x490,
0x496,0x49d,0x4a3,0x4a9,0x4af,0x4b5,0x4bc,0x4c2,
0x4c8,0x4ce,0x4d4,0x4da,0x4e1,0x4e7,0x4ed,0x4f3,
0x4f9,0x4ff,0x505,0x50b,0x511,0x517,0x51d,0x524,
0x52a,0x530,0x536,0x53c,0x542,0x547,0x54d,0x553,
0x559,0x55f,0x565,0x56b,0x571,0x577,0x57d,0x582,
0x588,0x58e,0x594,0x59a,0x59f,0x5a5,0x5ab,0x5b1,
0x5b6,0x5bc,0x5c2,0x5c7,0x5cd,0x5d2,0x5d8,0x5de,
0x5e3,0x5e9,0x5ee,0x5f4,0x5f9,0x5ff,0x604,0x60a,
0x60f,0x614,0x61a,0x61f,0x624,0x62a,0x62f,0x634,
0x639,0x63f,0x644,0x649,0x64e,0x653,0x658,0x65d,
0x663,0x668,0x66d,0x672,0x677,0x67b,0x680,0x685,
0x68a,0x68f,0x694,0x699,0x69d,0x6a2,0x6a7,0x6ac,
0x6b0,0x6b5,0x6b9,0x6be,0x6c3,0x6c7,0x6cc,0x6d0,
0x6d5,0x6d9,0x6dd,0x6e2,0x6e6,0x6eb,0x6ef,0x6f3,
0x6f7,0x6fc,0x700,0x704,0x708,0x70c,0x710,0x714,
0x718,0x71c,0x720,0x724,0x728,0x72c,0x72f,0x733,
0x737,0x73b,0x73e,0x742,0x746,0x749,0x74d,0x750,
0x754,0x757,0x75b,0x75e,0x762,0x765,0x768,0x76c,
0x76f,0x772,0x775,0x778,0x77b,0x77f,0x782,0x785,
0x788,0x78b,0x78d,0x790,0x793,0x796,0x799,0x79b,
0x79e,0x7a1,0x7a3,0x7a6,0x7a9,0x7ab,0x7ae,0x7b0,
0x7b3,0x7b5,0x7b7,0x7ba,0x7bc,0x7be,0x7c0,0x7c2,
0x7c5,0x7c7,0x7c9,0x7cb,0x7cd,0x7cf,0x7d1,0x7d2,
0x7d4,0x7d6,0x7d8,0x7da,0x7db,0x7dd,0x7df,0x7e0,
0x7e2,0x7e3,0x7e5,0x7e6,0x7e7,0x7e9,0x7ea,0x7eb,
0x7ed,0x7ee,0x7ef,0x7f0,0x7f1,0x7f2,0x7f3,0x7f4,
0x7f5,0x7f6,0x7f7,0x7f8,0x7f8,0x7f9,0x7fa,0x7fb,
0x7fb,0x7fc,0x7fc,0x7fd,0x7fd,0x7fe,0x7fe,0x7ff,
0x7ff,0x7ff,0x7ff,0x800,0x800,0x800,0x800,0x800,
0x800,0x800,0x800,0x800,0x800,0x7ff,0x7ff,0x7ff,
0x7ff,0x7fe,0x7fe,0x7fe,0x7fd,0x7fd,0x7fc,0x7fc,
0x7fb,0x7fa,0x7fa,0x7f9,0x7f8,0x7f7,0x7f6,0x7f6,
0x7f5,0x7f4,0x7f3,0x7f2,0x7f1,0x7f0,0x7ee,0x7ed,
0x7ec,0x7eb,0x7e9,0x7e8,0x7e7,0x7e5,0x7e4,0x7e2,
0x7e1,0x7df,0x7de,0x7dc,0x7da,0x7d9,0x7d7,0x7d5,
0x7d3,0x7d2,0x7d0,0x7ce,0x7cc,0x7ca,0x7c8,0x7c6,
0x7c3,0x7c1,0x7bf,0x7bd,0x7bb,0x7b8,0x7b6,0x7b4,
0x7b1,0x7af,0x7ac,0x7aa,0x7a7,0x7a5,0x7a2,0x7a0,
0x79d,0x79a,0x797,0x795,0x792,0x78f,0x78c,0x789,
0x786,0x783,0x780,0x77d,0x77a,0x777,0x774,0x770,
0x76d,0x76a,0x767,0x763,0x760,0x75d,0x759,0x756,
0x752,0x74f,0x74b,0x748,0x744,0x740,0x73d,0x739,
0x735,0x731,0x72e,0x72a,0x726,0x722,0x71e,0x71a,
0x716,0x712,0x70e,0x70a,0x706,0x702,0x6fe,0x6f9,
0x6f5,0x6f1,0x6ed,0x6e8,0x6e4,0x6e0,0x6db,0x6d7,
0x6d2,0x6ce,0x6c9,0x6c5,0x6c0,0x6bc,0x6b7,0x6b3,
0x6ae,0x6a9,0x6a4,0x6a0,0x69b,0x696,0x691,0x68d,
0x688,0x683,0x67e,0x679,0x674,0x66f,0x66a,0x665,
0x660,0x65b,0x656,0x651,0x64c,0x646,0x641,0x63c,
0x637,0x632,0x62c,0x627,0x622,0x61c,0x617,0x612,
0x60c,0x607,0x601,0x5fc,0x5f6,0x5f1,0x5eb,0x5e6,
0x5e0,0x5db,0x5d5,0x5d0,0x5ca,0x5c4,0x5bf,0x5b9,
0x5b3,0x5ae,0x5a8,0x5a2,0x59c,0x597,0x591,0x58b,
0x585,0x57f,0x57a,0x574,0x56e,0x568,0x562,0x55c,
0x556,0x550,0x54a,0x544,0x53f,0x539,0x533,0x52d,
0x527,0x520,0x51a,0x514,0x50e,0x508,0x502,0x4fc,
0x4f6,0x4f0,0x4ea,0x4e4,0x4de,0x4d7,0x4d1,0x4cb,
0x4c5,0x4bf,0x4b9,0x4b2,0x4ac,0x4a6,0x4a0,0x49a,
0x493,0x48d,0x487,0x481,0x47a,0x474,0x46e,0x468,
0x461,0x45b,0x455,0x44f,0x448,0x442,0x43c,0x435,
0x42f,0x429,0x423,0x41c,0x416,0x410,0x409,0x403,
0x3fd,0x3f7,0x3f0,0x3ea,0x3e4,0x3dd,0x3d7,0x3d1,
0x3cb,0x3c4,0x3be,0x3b8,0x3b1,0x3ab,0x3a5,0x39f,
0x398,0x392,0x38c,0x386,0x37f,0x379,0x373,0x36d,
0x366,0x360,0x35a,0x354,0x34e,0x347,0x341,0x33b,
0x335,0x32f,0x329,0x322,0x31c,0x316,0x310,0x30a,
0x304,0x2fe,0x2f8,0x2f2,0x2ec,0x2e6,0x2e0,0x2d9,
0x2d3,0x2cd,0x2c7,0x2c1,0x2bc,0x2b6,0x2b0,0x2aa,
0x2a4,0x29e,0x298,0x292,0x28c,0x286,0x281,0x27b,
0x275,0x26f,0x269,0x264,0x25e,0x258,0x252,0x24d,
0x247,0x241,0x23c,0x236,0x230,0x22b,0x225,0x220,
0x21a,0x215,0x20f,0x20a,0x204,0x1ff,0x1f9,0x1f4,
0x1ee,0x1e9,0x1e4,0x1de,0x1d9,0x1d4,0x1ce,0x1c9,
0x1c4,0x1bf,0x1ba,0x1b4,0x1af,0x1aa,0x1a5,0x1a0,
0x19b,0x196,0x191,0x18c,0x187,0x182,0x17d,0x178,
0x173,0x16f,0x16a,0x165,0x160,0x15c,0x157,0x152,
0x14d,0x149,0x144,0x140,0x13b,0x137,0x132,0x12e,
0x129,0x125,0x120,0x11c,0x118,0x113,0x10f,0x10b,
0x107,0x102,0xfe,0xfa,0xf6,0xf2,0xee,0xea,
0xe6,0xe2,0xde,0xda,0xd6,0xd2,0xcf,0xcb,
0xc7,0xc3,0xc0,0xbc,0xb8,0xb5,0xb1,0xae,
0xaa,0xa7,0xa3,0xa0,0x9d,0x99,0x96,0x93,
0x90,0x8c,0x89,0x86,0x83,0x80,0x7d,0x7a,
0x77,0x74,0x71,0x6e,0x6b,0x69,0x66,0x63,
0x60,0x5e,0x5b,0x59,0x56,0x54,0x51,0x4f,
0x4c,0x4a,0x48,0x45,0x43,0x41,0x3f,0x3d,
0x3a,0x38,0x36,0x34,0x32,0x30,0x2e,0x2d,
0x2b,0x29,0x27,0x26,0x24,0x22,0x21,0x1f,
0x1e,0x1c,0x1b,0x19,0x18,0x17,0x15,0x14,
0x13,0x12,0x10,0xf,0xe,0xd,0xc,0xb,
0xa,0xa,0x9,0x8,0x7,0x6,0x6,0x5,
0x4,0x4,0x3,0x3,0x2,0x2,0x2,0x1,
0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,
0x1,0x2,0x2,0x3,0x3,0x4,0x4,0x5,
0x5,0x6,0x7,0x8,0x8,0x9,0xa,0xb,
0xc,0xd,0xe,0xf,0x10,0x11,0x12,0x13,
0x15,0x16,0x17,0x19,0x1a,0x1b,0x1d,0x1e,
0x20,0x21,0x23,0x25,0x26,0x28,0x2a,0x2c,
0x2e,0x2f,0x31,0x33,0x35,0x37,0x39,0x3b,
0x3e,0x40,0x42,0x44,0x46,0x49,0x4b,0x4d,
0x50,0x52,0x55,0x57,0x5a,0x5d,0x5f,0x62,
0x65,0x67,0x6a,0x6d,0x70,0x73,0x75,0x78,
0x7b,0x7e,0x81,0x85,0x88,0x8b,0x8e,0x91,
0x94,0x98,0x9b,0x9e,0xa2,0xa5,0xa9,0xac,
0xb0,0xb3,0xb7,0xba,0xbe,0xc2,0xc5,0xc9,
0xcd,0xd1,0xd4,0xd8,0xdc,0xe0,0xe4,0xe8,
0xec,0xf0,0xf4,0xf8,0xfc,0x100,0x104,0x109,
0x10d,0x111,0x115,0x11a,0x11e,0x123,0x127,0x12b,
0x130,0x134,0x139,0x13d,0x142,0x147,0x14b,0x150,
0x154,0x159,0x15e,0x163,0x167,0x16c,0x171,0x176,
0x17b,0x180,0x185,0x189,0x18e,0x193,0x198,0x19d,
0x1a3,0x1a8,0x1ad,0x1b2,0x1b7,0x1bc,0x1c1,0x1c7,
0x1cc,0x1d1,0x1d6,0x1dc,0x1e1,0x1e6,0x1ec,0x1f1,
0x1f6,0x1fc,0x201,0x207,0x20c,0x212,0x217,0x21d,
0x222,0x228,0x22e,0x233,0x239,0x23e,0x244,0x24a,
0x24f,0x255,0x25b,0x261,0x266,0x26c,0x272,0x278,
0x27e,0x283,0x289,0x28f,0x295,0x29b,0x2a1,0x2a7,
0x2ad,0x2b3,0x2b9,0x2be,0x2c4,0x2ca,0x2d0,0x2d6,
0x2dc,0x2e3,0x2e9,0x2ef,0x2f5,0x2fb,0x301,0x307,
0x30d,0x313,0x319,0x31f,0x326,0x32c,0x332,0x338,
0x33e,0x344,0x34b,0x351,0x357,0x35d,0x363,0x36a,
0x370,0x376,0x37c,0x383,0x389,0x38f,0x395,0x39c,
0x3a2,0x3a8,0x3ae,0x3b5,0x3bb,0x3c1,0x3c7,0x3ce,
0x3d4,0x3da,0x3e1,0x3e7,0x3ed,0x3f3,0x3fa,0x400};
    
    int i = 0, j = 0;
    UINT16 audioByte;
    UINT16 unsign_audio;
    
    for(j = 0; j < 100; j++)
    {
        for(i = 0; i < 1024; i++)
        {
            audioByte = ((testBytes[i+1] << 4) | (testBytes[i]));
            if (audioByte & 0x0800) {
                unsign_audio = ~(audioByte - 1);
                audioByte = AC_ZERO - unsign_audio;
            }
            else {
                audioByte = AC_ZERO + audioByte;
            }
            DAC_WriteToDAC(WRITE_UPDATE_CHN_A_B, audioByte);
        }
    }
}

/**
 * @brief Command used to Toggle on/off the Timer 3 module.
 * @return Void.
 */
void MON_Timer_ON_OFF(void)
{
    if(!TIMER3_IsON())
    {
        TIMER3_ON(TRUE);
    }
    else
    {
        TIMER3_ON(FALSE);
        AUDIO_setNewTone(1);
    }
}

/**
 * @brief Command used to display a Timer 3 period.
 * @return Void.
 */
void MON_Timer_Get_PS(void)
{   
    char buf[32];
    snprintf(&buf[0] ,32 ,"The current period: %d", (UINT16)PR3);
    MON_SendString(&buf[0]);
}

/**
 * @brief Command used to set Timer 3 period.
 * @return Void.
 */
void MON_Timer_Set_PS(void)
{   
    char buf[32];
    UINT16 prd = atoi(cmdStr.arg1);
    T3CONbits.ON = 0;
    PR3 = prd;
    TMR3 = 0;
    
    snprintf(&buf[0] ,32 ,"The period set to: %d", prd);
    MON_SendString(&buf[0]);
}

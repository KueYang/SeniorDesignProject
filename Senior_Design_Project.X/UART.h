/**
 * @file UART.h
 * @author Kue Yang
 * @date 11/22/2016
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * @brief COMMANDS data structure.
 * @details The COMMANDS data structure is used to store a command with its 
 * corresponding command handler. The command handler is used in conjunction 
 * with the serial communication for processing a command that is received from 
 * the user.
 */
typedef struct COMMANDS
{
    /**@{*/
    const char* name;           /**< Variable used to store the command name */
    const char* description;    /**< Variable used to store a description of the command. */
    void (*handler)(void);      /**< Variable used to point to the command handler. */
    /**@}*/
}COMMANDS;

/**
 * @brief COMMANDSTR data structure.
 * @details The command string data structure is used to store a command with its 
 * corresponding arguments. 
 */
typedef struct COMMANDSTR
{
    /**@{*/
    char name[32];      /**< Variable used to store the name of the command. */
    char arg1[32];      /**< Variable used to store the first command argument. */
    char arg2[32];      /**< Variable used to store the second command argument. */
    /**@}*/
}COMMANDSTR;

void UART_Init(void);
void UART_Process(void);

/* String Helper Functions. */
void MON_removeWhiteSpace(const char* string);
UINT16 MON_getStringLength(const char* string);
BOOL MON_stringsMatch(const char* str1, const char* str2);
char MON_lowerToUpper(const char* ch);

/* UART character and string handlers. */
void MON_SendStringNR(const char* str);
void MON_SendString(const char* str);
void MON_SendChar(const char* character);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */


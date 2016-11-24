/**
 * @file UART.h
 * @author Kue Yang
 * @date 11/22/2016
 * @brief Header for the UART Module. 
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

/**@defgroup Command Typedef Structures
 * @{
 */
/** Command Structure */
typedef struct COMMANDS{
    const char* name;
    const char* description;
    void (*handler)(void);
}COMMANDS;

/** Command String Structure */
typedef struct COMMANDSTR{
    char name[32];
    char arg1[32];
    char arg2[32];
}COMMANDSTR;
/** @} */

void UART_Init(void);
void UART_Process(void);
void UART_sendString(const char *string);
void UART_sendCharacter(const char character);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */


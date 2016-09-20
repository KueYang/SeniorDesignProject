#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct COMMANDS{
    const char* commandName;
    const int   commandID;
    const char* commandInfo;
    void (*MON_COMMAND)(void);
}COMMANDS;

void UART_Init(void);
void UART_Process(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */


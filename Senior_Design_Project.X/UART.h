#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct COMMANDS{
    const char* name;
    const char* description;
    void (*handler)(void);
}COMMANDS;

typedef struct COMMANDSTR{
    char* name;
    char* arg1;
    char* arg2;
}COMMANDSTR;

void UART_Init(void);
void UART_Process(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */


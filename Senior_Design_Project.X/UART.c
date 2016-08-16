#include <p32xxxx.h>
#include "STDDEF.h"
#include "Timer.h"
#include "UART.h"

#define FCY                 8000000 //8Mhz
#define DESIRE_BAUD_RATE    9600
#define FIFO_BUFFER_SIZE    128

void UART_WriteChar(char* str);
void UART_WriteString(char* str);
BYTE UART_ReadChar();
UINT16 UART_ReadString();

BYTE RxFifo[FIFO_BUFFER_SIZE];
BYTE TxFifo[FIFO_BUFFER_SIZE];
UINT16 rxReBufPtr = 0, rxWrBufPtr = 0, txReBufPtr = 0, txWrBufPtr = 0;
UINT16 RxDataCount = 0, TxDataCount = 0;
const char MON_Header[] = "Monitor Command: \n\r";
const char MON_PROMPT[] = ">";


void UART_Init(void)
{
    U1BRG = 25;                 // Baud Rate 9600

    U1MODEbits.UARTEN = 0;                          // UART1 is Disabled
    U1MODEbits.USIDL = 0;                           // Continue operation at Idle mode
    U1MODEbits.IREN = 0;                            // IrDA En/Decoder is disabled
    U1MODEbits.RTSMD = 0;                           // Flow Control mode
    U1MODEbits.WAKE = 0;                            // Wake-up on start bit is enabled
    U1MODEbits.LPBACK = 0;                          // Loop-back is disabled
    U1MODEbits.ABAUD = 0;                           // auto baud is disabled
    U1MODEbits.RXINV = 1;                           // U1RX Idle State is '1'
    U1MODEbits.BRGH = 0;                            // Standard mode Baud rate, 16 clocks per bit period
    U1MODEbits.PDSEL = 0b00;                        // 8bit no parity
    U1MODEbits.STSEL = 0;                           // one stop bit
    U1MODEbits.UEN = 0b00;                          // UTX, RTX, U1CTS, U1RTS are enable and on use.

    U1STAbits.UTXISEL1 = 0b00;                      // Interrupt when a character is transferred to the TSR
    U1STAbits.UTXBRK = 0;                           // sync break tx is disabled
    U1STAbits.UTXINV = 0;                           // U1TX Idle state is '1'
    U1STAbits.UTXEN = 1;                            // U1TX  is enabled
    U1STAbits.URXISEL = 0b00;                       // interrupt flag bit is set when RXBUF is filled with at least 1 character
    U1STAbits.ADDEN = 0;                            // address detect mode is disabled

    IFS1bits.U1RXIF = 0;                            // clear interrupt flag of rx
    IEC1bits.U1RXIE = 1;                            // enable rx recieved data interrupt

    IFS1bits.U1TXIF = 0;                            // clear interrupt flag of tx
    IEC1bits.U1TXIE = 1;                            // enable tx recieved data interrupt
    
    U1MODEbits.UARTEN = 1;                          // UART1 is Enabled
}

void UART_Process(void)
{
    
}


void UART_WriteChar(char* str)
{
    while(U1STAbits.UTXBF != 0);
//    U1TXREG = (BYTE)str;
}

void UART_WriteStr(char* str)
{
    while(str!= 0){
        UART_WriteChar(str);
        str++; 
    }
}

BYTE UART_ReadChar()
{
    RxDataCount--;
    return RxFifo[rxWrBufPtr++];
}

UINT16 UART_ReadString()
{
    int num_char = 0;
    char strPtr = ' ';
    
    while(strPtr != '\r')
    {
        strPtr = UART_ReadChar();
        num_char++;
    };
    
    return num_char;
}

//void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void)
//{
////    if (TxDataCount >= 0)
////    {
////        U1TXREG = TxFifo[txReBufPtr++];
////        TxDataCount -= 1;
////    }
////
////    if(txReBufPtr == FIFO_BUFFER_SIZE)
////    {
////        txReBufPtr = 0;
////    }
//    U1TXREG = '>';
//
//    IFS1bits.U1TXIF = 0;		// clear interrupt flag of rx
//}
//
//void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void)
//{
////    if (RxDataCount <= FIFO_BUFFER_SIZE)
////    {
////        RxFifo[rxWrBufPtr++] = U1RXREG;
////        RxDataCount += 1;
////    }
////
////    if(rxWrBufPtr >= FIFO_BUFFER_SIZE)
////    {
////        rxWrBufPtr = 0;
////    }
//    WORD temp = U1RXREG;
//    U1TXREG = (BYTE)temp;
//
//    IFS1bits.U1RXIF = 0;		// clear interrupt flag of tx
//}



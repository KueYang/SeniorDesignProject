#include <p32xxxx.h>
#include <plib.h>
#include "CONFIG.h"
#include "STDDEF.h"
#include "Timer.h"
#include "UART.h"

#define UART_MODULE_ID          UART1 // PIM is connected to Explorer through UART1 module
#define DESIRED_BAUDRATE        (9600) //The desired BaudRate
#define	GetPeripheralClock()    (SYS_FREQ/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock()	(SYS_FREQ)

void WriteString(const char *string);
void PutCharacter(const char character);

void UART_Init(void)
{
    // Unlocks the PPS assignments
    PPSUnLock;
    // Re-mapped pins RB14 and RB15 pins to U1RX and U1TX
    PPSInput(3,U1RX,RPB13);     // Assign RPB13 as input pin for U1RX
    PPSOutput(1,RPB15,U1TX);    // Set RPB15 pin as output for U1TX
    // Locks the PPS assignments
    PPSLock;
    
    // Configure UART1 module
    UARTConfigure(UART_MODULE_ID, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART_MODULE_ID, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_MODULE_ID, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_MODULE_ID, GetPeripheralClock(), DESIRED_BAUDRATE);
    UARTEnable(UART_MODULE_ID, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    // Configure UART RX1 and TX1 Interrupt
    INTEnable(INT_SOURCE_UART_RX(UART_MODULE_ID), INT_ENABLED);
    INTEnable(INT_SOURCE_UART_TX(UART_MODULE_ID), INT_ENABLED);
    INTSetVectorPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_PRIORITY_LEVEL_2);
    INTSetVectorSubPriority(INT_VECTOR_UART(UART_MODULE_ID), INT_SUB_PRIORITY_LEVEL_0);
    
    WriteString("*** UART Interrupt-driven Application Example ***\r\n");
    WriteString("*** Type some characters and observe echo and RA7 LED toggle ***\r\n");
}

void UART_Process(void)
{
    
}

// Helper functions
void WriteString(const char *string)
{
    while(*string != '\0')
    {
        while(!UARTTransmitterIsReady(UART_MODULE_ID));
        UARTSendDataByte(UART_MODULE_ID, *string);
        string++;
        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
    }
}
void PutCharacter(const char character)
{
        while(!UARTTransmitterIsReady(UART_MODULE_ID));
        UARTSendDataByte(UART_MODULE_ID, character);
        while(!UARTTransmissionHasCompleted(UART_MODULE_ID));
}

void __ISR(_UART1_VECTOR, IPL2AUTO) IntUart1Handler(void)
{
	// Is this an RX interrupt?
	if(INTGetFlag(INT_SOURCE_UART_RX(UART_MODULE_ID)))
	{
        // Clear the RX interrupt Flag
	    INTClearFlag(INT_SOURCE_UART_RX(UART_MODULE_ID));
        // Echo what we just received.
        PutCharacter(UARTGetDataByte(UART_MODULE_ID));
        // Toggle LED to indicate UART activity
        mPORTAToggleBits(BIT_0);
	}

	// We don't care about TX interrupt
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART_MODULE_ID)) )
	{
        INTClearFlag(INT_SOURCE_UART_TX(UART_MODULE_ID));
	}
}
/********************************************************************
 * FileName:        int.h
 * Dependencies:
 * Processor:       PIC32MX
 * Hardware:        N/A
 * Assembler:       N/A
 * Linker:          N/A
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement:
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * $Id: INT.h,v 1.6 2006/11/07 23:29:45 C12923 Exp $
 * $Name:  $

 ********************************************************************/

#ifndef _PIC32INT_HEADER_FILE
#define _PIC32INT_HEADER_FILE

#ifdef __cplusplus
extern "C"
  {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************
#include <xc.h>
#include <sys/attribs.h>

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************
    
// *****************************************************************************
/* Interrupt Source

  Summary:
    Interrupt source definitions (PIC32MX1xx/2xx/5xx family).

  Description:
    These definitions can be used to set or get the state of the interrupt source flag and
    interrupt enable.

    This enumeration is used by
      void INTClearFlag(INT_SOURCE source)
      void INTSetFlag(INT_SOURCE source)
      unsigned int INTGetFlag(INT_SOURCE source)
      void INTEnable(INT_SOURCE source, INT_EN_DIS enable)
      unsigned int INTGetEnable(INT_SOURCE source)
*/
typedef enum //IRQ's from Table 5-1
{
    INT_CT = 0,             // Core Timer Interrupt

    INT_CS0,                // Core Software Interrupt 0
    INT_CS1,                // Core Software Interrupt 1

    INT_INT0,               // External Interrupt 0
    INT_INT1,               // External Interrupt 1
    INT_INT2,               // External Interrrupt 2
    INT_INT3,               // External Interrupt 3
    INT_INT4,               // External Interupt 4

    INT_T1,                 // Timer1
    INT_T2,                 // Timer2
    INT_T3,                 // Timer3
    INT_T4,                 // Timer4
    INT_T5,                 // Timer5

    INT_IC1E,               // Input Capture Error 1
    INT_IC2E,               // Input Capture Error 2
    INT_IC3E,               // Input Capture Error 3
    INT_IC4E,               // Input Capture Error 4
    INT_IC5E,               // Input Capture Error 5

    INT_IC1,                // Input Capture 1
    INT_IC2,                // Input Capture 2
    INT_IC3,                // Input Capture 3
    INT_IC4,                // Input Capture 4
    INT_IC5,                // Input Capture 5

    INT_OC1,                // Output Compare 1
    INT_OC2,                // Output Compare 2
    INT_OC3,                // Output Compare 3
    INT_OC4,                // Output Compare 4
    INT_OC5,                // Output Compare 5

    INT_CNA,                // Change Notice, Port A
    INT_CNB,                // Change Notice, Port B
    INT_CNC,                // Change Notice, Port C
    INT_CND,                // Change Notice, Port D
    INT_CNE,                // Change Notice, Port E
    INT_CNF,                // Change Notice, Port F
    INT_CNG,                // Change Notice, Port G

    INT_SPI1,
    INT_SPI2,
    INT_SPI3,
    INT_SPI4,

    INT_SPI1E,              // SPI1 Fault
    INT_SPI2E,              // SPI2 Fault
    INT_SPI3E,              // SPI3 Fault
    INT_SPI4E,              // SPI4 Fault

    INT_SPI1TX,             // SPI1 Transfer Done
    INT_SPI2TX,             // SPI2 Transfer Done
    INT_SPI3TX,             // SPI3 Transfer Done
    INT_SPI4TX,             // SPI4 Transfer Done

    INT_SPI1RX,             // SPI1 Receive Done
    INT_SPI2RX,             // SPI2 Receive Done
    INT_SPI3RX,             // SPI3 Receive Done
    INT_SPI4RX,             // SPI4 Receive Done

    INT_I2C1,
    INT_I2C2,

    INT_I2C1B,              // I2C1 Bus Colision Event
    INT_I2C2B,              // I2C2 Bus Colision Event

    INT_I2C1S,              // I2C1 Slave Event
    INT_I2C2S,              // I2C2 Slave Event

    INT_I2C1M,              // I2C1 Master Event
    INT_I2C2M,              // I2C2 Master Event

    INT_U1,
    INT_U2,
    INT_U3_DONOTUSE,        // DO NOT USE - interrupt table entry empty, use individual entries below
    INT_U4,
    INT_U5,

    INT_U1E,                // UART1 Error Event
    INT_U2E,                // UART2 Error Event
    INT_U3E,                // UART2 Error Event
    INT_U4E,                // UART2 Error Event
    INT_U5E,                // UART2 Error Event

    INT_U1RX,               // UART1 RX Event
    INT_U2RX,               // UART2 RX Event
    INT_U3RX,               // UART2 RX Event
    INT_U4RX,               // UART2 RX Event
    INT_U5RX,               // UART2 RX Event

    INT_U1TX,               // UART1 TX Event
    INT_U2TX,               // UART2 TX Event
    INT_U3TX,               // UART2 TX Event
    INT_U4TX,               // UART2 TX Event
    INT_U5TX,               // UART2 TX Event

    INT_AD1,                // ADC1 Convert Done

    INT_PMP,                // Parallel Master Port
    INT_PMPE,               // Parallel Master Port Error

    INT_CMP1,               // Comparator 1 Interrupt
    INT_CMP2,               // Comparator 2 Interrupt
    INT_CMP3,               // Comparator 3 Interrupt

    INT_FSCM,               // Fail-safe Monitor

    INT_FCE,                // Flash Control Event

    INT_RTCC,               // Real Time Clock

    INT_CTMU,               // CTMU

    INT_DMA0,               // DMA Channel 0
    INT_DMA1,               // DMA Channel 1
    INT_DMA2,               // DMA Channel 2
    INT_DMA3,               // DMA Channel 3

    INT_USB,                // USB

    INT_CAN1,               // CAN1

    INT_NUM                 // Number of available interrupt soruces and vectors

}  INT_SOURCE;


// *****************************************************************************
/* Interrupt Multi-Channel Converter

  Summary:
    Interrupt multi-channel converter definitions.

  Description:
    These definitions can be used to convert a channel into the correct INT_SOURCE value.

    These macros can be used by
      void INTClearFlag(INT_SOURCE source)
      void INTSetFlag(INT_SOURCE source)
      unsigned int INTGetFlag(INT_SOURCE source)
      void INTEnable(INT_SOURCE source, INT_EN_DIS enable)
      unsigned int INTGetEnable(INT_SOURCE source)
*/
// Core Software Interrupt Channel.
#define INT_SOURCE_CORE_SW(chn)         /*DOM-IGNORE-BEGIN*/(INT_CS0 + (chn))           /*DOM-IGNORE-END*/
// External Interrupt Channel.
#define INT_SOURCE_EX_INT(chn)          /*DOM-IGNORE-BEGIN*/(INT_INT0 + (chn))          /*DOM-IGNORE-END*/
// Timer Interrupt Channel.
#define INT_SOURCE_TIMER(chn)           /*DOM-IGNORE-BEGIN*/(INT_T1 + (chn))            /*DOM-IGNORE-END*/
// Input Capture Interrupt Channel.
#define INT_SOURCE_INPUT_CAP(chn)       /*DOM-IGNORE-BEGIN*/(INT_IC1 + (chn))           /*DOM-IGNORE-END*/
// Output Compare Interrupt Channel.
#define INT_SOURCE_OUTPUT_COM(chn)      /*DOM-IGNORE-BEGIN*/(INT_OC1 + (chn))           /*DOM-IGNORE-END*/
// SPI Interrupt Channel.
#define INT_SOURCE_SPI(chn)             /*DOM-IGNORE-BEGIN*/((INT_SPI1 - 1) + (chn))    /*DOM-IGNORE-END*/
// SPI Error Interrupt Channel.
#define INT_SOURCE_SPI_ERROR(chn)       /*DOM-IGNORE-BEGIN*/((INT_SPI1E - 1) + (chn))   /*DOM-IGNORE-END*/
// SPI Transmit Interrupt Channel.
#define INT_SOURCE_SPI_TX(chn)          /*DOM-IGNORE-BEGIN*/((INT_SPI1TX - 1) + (chn))  /*DOM-IGNORE-END*/
// SPI Receive Interrupt Channel.
#define INT_SOURCE_SPI_RX(chn)          /*DOM-IGNORE-BEGIN*/((INT_SPI1RX - 1) + (chn))  /*DOM-IGNORE-END*/
// I2C Interrupt Channel.
#define INT_SOURCE_I2C(chn)             /*DOM-IGNORE-BEGIN*/(INT_I2C1 + (chn))          /*DOM-IGNORE-END*/
// I2C Bus Collision Interrupt Channel.
#define INT_SOURCE_I2C_ERROR(chn)       /*DOM-IGNORE-BEGIN*/(INT_I2C1B + (chn))         /*DOM-IGNORE-END*/
// I2C Slave Interrupt Channel.
#define INT_SOURCE_I2C_SLAVE(chn)       /*DOM-IGNORE-BEGIN*/(INT_I2C1S + (chn))         /*DOM-IGNORE-END*/
// I2C Master Interrupt Channel.
#define INT_SOURCE_I2C_MASTER(chn)      /*DOM-IGNORE-BEGIN*/(INT_I2C1M + (chn))         /*DOM-IGNORE-END*/
// UART Interrupt Channel.
#define INT_SOURCE_UART(chn)            /*DOM-IGNORE-BEGIN*/(INT_U1 + (chn))            /*DOM-IGNORE-END*/
// UART Error Interrupt Channel.
#define INT_SOURCE_UART_ERROR(chn)      /*DOM-IGNORE-BEGIN*/(INT_U1E + (chn))           /*DOM-IGNORE-END*/
// UART Receive Interrupt Channel.
#define INT_SOURCE_UART_RX(chn)         /*DOM-IGNORE-BEGIN*/(INT_U1RX + (chn))          /*DOM-IGNORE-END*/
// UART Transmit Interrupt Channel.
#define INT_SOURCE_UART_TX(chn)         /*DOM-IGNORE-BEGIN*/(INT_U1TX + (chn))          /*DOM-IGNORE-END*/
// DMA Interrupt Channel.
#define INT_SOURCE_DMA(chn)             /*DOM-IGNORE-BEGIN*/(INT_DMA0 + (chn))          /*DOM-IGNORE-END*/
// CAN Interrupt Channel.
#define INT_SOURCE_CAN(chn)             /*DOM-IGNORE-BEGIN*/(INT_CAN1 + (chn))          /*DOM-IGNORE-END*/


// *****************************************************************************
/* Interrupt Vector

  Summary:
    Interrupt vector definitions (PIC32MX1xx/2xx family).

  Description:
    These definitions can be used to set or get the state of the interrupt vector
    priority and sub-priority.

    This enumeration is used by:
      void INTSetVectorPriority(INT_VECTOR vector, INT_PRIORITY priority)
      INT_PRIORITY INTGetVectorPriority(INT_VECTOR vector)
      void INTSetVectorSubPriority(INT_VECTOR vector, INT_SUB_PRIORITY subPriority)
      INT_SUB_PRIORITY INTGetVectorSubPriority(INT_VECTOR vector)

  WARNING:
    Do not use with the __IPL(v,ipl) macro.

*/
typedef enum //Vector Number from Table 7-1
{
    INT_CORE_TIMER_VECTOR,            // Core Timer Interrupt

    INT_CORE_SOFTWARE_0_VECTOR,       // Core Software Interrupt 0
    INT_CORE_SOFTWARE_1_VECTOR,       // Core Software Interrupt 1

    INT_EXTERNAL_0_VECTOR,            // External Interrupt 0
    INT_EXTERNAL_1_VECTOR,            // External Interrupt 1
    INT_EXTERNAL_2_VECTOR,            // External Interrrupt 2
    INT_EXTERNAL_3_VECTOR,            // External Interrupt 3
    INT_EXTERNAL_4_VECTOR,            // External Interupt 4

    INT_TIMER_1_VECTOR,               // Timer1
    INT_TIMER_2_VECTOR,               // Timer2
    INT_TIMER_3_VECTOR,               // TImer3
    INT_TIMER_4_VECTOR,               // Timer4
    INT_TIMER_5_VECTOR,               // Timer5

    INT_INPUT_CAPTURE_1_VECTOR,       // Input Capture 1
    INT_INPUT_CAPTURE_2_VECTOR,       // Input Capture 2
    INT_INPUT_CAPTURE_3_VECTOR,       // Input Capture 3
    INT_INPUT_CAPTURE_4_VECTOR,       // Input Capture 4
    INT_INPUT_CAPTURE_5_VECTOR,       // Input Capture 5

    INT_OUTPUT_COMPARE_1_VECTOR,       // Output Compare 1
    INT_OUTPUT_COMPARE_2_VECTOR,       // Output Compare 2
    INT_OUTPUT_COMPARE_3_VECTOR,       // Output Compare 3
    INT_OUTPUT_COMPARE_4_VECTOR,       // Output Compare 4
    INT_OUTPUT_COMPARE_5_VECTOR,       // Output Compare 5

    INT_SPI_1_VECTOR,                 // SPI1 Vector
    INT_SPI_2_VECTOR,                 // SPI2 Vector
    INT_SPI_3_VECTOR,                 // SPI3 Vector
    INT_SPI_4_VECTOR,                 // SPI4 Vector

    INT_UART_1_VECTOR,                // UART1 Vector
    INT_UART_2_VECTOR,                // UART2 Vector
    INT_UART_3_VECTOR,                // UART3 Vector
    INT_UART_4_VECTOR,                // UART4 Vector
    INT_UART_5_VECTOR,                // UART5 Vector

    INT_I2C_1_VECTOR,                 // I2C1 Vector
    INT_I2C_2_VECTOR,                 // I2C2 Vector

    INT_CHANGE_NOTICE_VECTOR,         // Change Notice
    INT_ADC_VECTOR,                   // ADC1 Convert Done
    INT_PMP_VECTOR,                   // Parallel Master Port

    INT_COMPARATOR_1_VECTOR,          // Comparator 1 Interrupt
    INT_COMPARATOR_2_VECTOR,          // Comparator 2 Interrupt
    INT_COMPARATOR_3_VECTOR,          // Comparator 3 Interrupt

    INT_FAIL_SAFE_MONITOR_VECTOR,     // Fail-safe Monitor
    INT_RTCC_VECTOR,                  // Real-Time Clock

    INT_CTMU_VECTOR,                  // CTMU

    INT_DMA_0_VECTOR,                 // DMA Channel 0
    INT_DMA_1_VECTOR,                 // DMA Channel 1
    INT_DMA_2_VECTOR,                 // DMA Channel 2
    INT_DMA_3_VECTOR,                 // DMA Channel 3

    INT_FCE_VECTOR,                   // Flash Control Event

    INT_USB_1_VECTOR,                 // USB

    INT_CAN_1_VECTOR,                 // CAN

}INT_VECTOR;

// *****************************************************************************
/* Interrupt Muli-Channel Converter

  Summary:
    Interrupt muli-channel converter definitions.

  Description:
    These definitions can be used to convert a channel into the correct INT_VECTOR value.

    These macros can be used by:
      void INTSetVectorPriority(INT_VECTOR vector, INT_PRIORITY priority)
      INT_PRIORITY INTGetVectorPriority(INT_VECTOR vector)
      void INTSetVectorSubPriority(INT_VECTOR vector, INT_SUB_PRIORITY subPriority)
      INT_SUB_PRIORITY INTGetVectorSubPriority(INT_VECTOR vector)

  WARNING:
    Do not use with the __IPL(v,ipl) macro.

*/
// Core Software Interrupt Channel
#define INT_VECTOR_CORE_SW(chn)         /*DOM-IGNORE-BEGIN*/(INT_CORE_SOFTWARE_0_VECTOR + (chn))    /*DOM-IGNORE-END*/
// External Interrupt Channel
#define INT_VECTOR_EX_INT(chn)          /*DOM-IGNORE-BEGIN*/(INT_EXTERNAL_0_VECTOR + (chn))         /*DOM-IGNORE-END*/
// Timer Interrupt Channel
#define INT_VECTOR_TIMER(chn)           /*DOM-IGNORE-BEGIN*/(INT_TIMER_1_VECTOR + (chn))            /*DOM-IGNORE-END*/
// Input Capture Interrupt Channel
#define INT_VECTOR_INPUT_CAP(chn)       /*DOM-IGNORE-BEGIN*/(INT_INPUT_CAPTURE_1_VECTOR + (chn))    /*DOM-IGNORE-END*/
// Output Compare Interrupt Channel
#define INT_VECTOR_OUTPUT_COM(chn)      /*DOM-IGNORE-BEGIN*/(INT_OUTPUT_COMPARE_1_VECTOR + (chn))   /*DOM-IGNORE-END*/
// SPI Interrupt Channel
#define INT_VECTOR_SPI(chn)             /*DOM-IGNORE-BEGIN*/((INT_SPI_1_VECTOR - 1) + (chn))        /*DOM-IGNORE-END*/
// I2C Interrupt Channel
#define INT_VECTOR_I2C(chn)             /*DOM-IGNORE-BEGIN*/(INT_I2C_1_VECTOR + (chn))              /*DOM-IGNORE-END*/
// UART Interrupt Channel
#define INT_VECTOR_UART(chn)            /*DOM-IGNORE-BEGIN*/(INT_UART_1_VECTOR + (chn))             /*DOM-IGNORE-END*/
// DMA Interrupt Channel
#define INT_VECTOR_DMA(chn)             /*DOM-IGNORE-BEGIN*/(INT_DMA_0_VECTOR + (chn))              /*DOM-IGNORE-END*/
// CAN Interrupt Channel
#define INT_VECTOR_CAN(chn)             /*DOM-IGNORE-BEGIN*/(INT_CAN_1_VECTOR + (chn))              /*DOM-IGNORE-END*/    
    
// *****************************************************************************
/* Interrupt Single Vector Shadow Set

  Summary:
    Single vector shadow set selector.

  Description:
    These definitions can be used enable the use of the shadow set when
    in single vector mode.
*/
typedef enum
{
    // Use the the CPU's register set when entering the handler.
    INT_REGISTOR_SET_SELECT_NONE /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    // Use the shadow set when entering the handler
    INT_REGISTOR_SET_SELECT_SS0  /*DOM-IGNORE-BEGIN*/ = (_INTCON_SS0_MASK) /*DOM-IGNORE-END*/

}INT_SV_SS;

// *****************************************************************************
/* Interrupt Vector Priority

  Summary:
    Interrupt vector priority definitions.

  Description:
    These definitions can be used to set the priority of an interrupt
    vector.
*/
typedef enum
{
    // Interrupt vector priority level of 0, interrupt is disabled.
    INT_PRIORITY_DISABLED /*DOM-IGNORE-BEGIN*/=      0 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 1.
    INT_PRIORITY_LEVEL_1  /*DOM-IGNORE-BEGIN*/=      1 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 2.
    INT_PRIORITY_LEVEL_2  /*DOM-IGNORE-BEGIN*/=      2 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 3.
    INT_PRIORITY_LEVEL_3  /*DOM-IGNORE-BEGIN*/=      3 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 4.
    INT_PRIORITY_LEVEL_4  /*DOM-IGNORE-BEGIN*/=      4 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 5.
    INT_PRIORITY_LEVEL_5  /*DOM-IGNORE-BEGIN*/=      5 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 6.
    INT_PRIORITY_LEVEL_6  /*DOM-IGNORE-BEGIN*/=      6 /*DOM-IGNORE-END*/,
    // Interrupt vector priority level of 7.
    INT_PRIORITY_LEVEL_7  /*DOM-IGNORE-BEGIN*/=      7  /*DOM-IGNORE-END*/
}INT_PRIORITY;

// *****************************************************************************
/* Interrupt Vector Sub-priority

  Summary:
    Interrupt vector sub-priority definitions.

  Description:
    These definitions can be used to set the sub-priority of an interrupt
    vector.
*/
typedef enum
{
    // Interrupt vector sub-priority level of 0.
    INT_SUB_PRIORITY_LEVEL_0 /*DOM-IGNORE-BEGIN*/=   0 /*DOM-IGNORE-END*/,
    // Interrupt vector sub-priority level of 1.
    INT_SUB_PRIORITY_LEVEL_1 /*DOM-IGNORE-BEGIN*/=   1 /*DOM-IGNORE-END*/,
    // Interrupt vector sub-priority level of 2.
    INT_SUB_PRIORITY_LEVEL_2 /*DOM-IGNORE-BEGIN*/=   2 /*DOM-IGNORE-END*/,
    // Interrupt vector sub-priority level of 3.
    INT_SUB_PRIORITY_LEVEL_3 /*DOM-IGNORE-BEGIN*/=   3 /*DOM-IGNORE-END*/
}INT_SUB_PRIORITY;

// *****************************************************************************
/* Interrupt Enable/Disable

  Summary:
    Interrupt enable/disable definitions.

  Description:
    These definitions can be used to enable or disable an interrupt.
*/
typedef enum
{
    // Disables the interrupt.
    INT_DISABLED /*DOM-IGNORE-BEGIN*/ = 0 /*DOM-IGNORE-END*/,
    // Enables the interrupt.
    INT_ENABLED /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/
}INT_EN_DIS;
// *****************************************************************************
/* Interrupt Temporal Proximity Control

  Summary:
    Interrupt Temporal Proximity Control definitions.

  Description:
    These definitions can be used to set the interrupt priority of the Interrupt Temporal Proximity Timer.
*/
typedef enum
{
    // Temporal Proximity Timer disabled.
    INT_TPC_DISABLE            /*DOM-IGNORE-BEGIN*/=      0x00000700 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 1.
    INT_TPC_GROUP_PRI_LEVEL_1  /*DOM-IGNORE-BEGIN*/=      1 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 2 and below.
    INT_TPC_GROUP_PRI_LEVEL_2  /*DOM-IGNORE-BEGIN*/=      2 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 3 and below.
    INT_TPC_GROUP_PRI_LEVEL_3  /*DOM-IGNORE-BEGIN*/=      3 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 4 and below.
    INT_TPC_GROUP_PRI_LEVEL_4  /*DOM-IGNORE-BEGIN*/=      4 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 5 and below.
    INT_TPC_GROUP_PRI_LEVEL_5  /*DOM-IGNORE-BEGIN*/=      5 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 6 and below.
    INT_TPC_GROUP_PRI_LEVEL_6  /*DOM-IGNORE-BEGIN*/=      6 /*DOM-IGNORE-END*/,
    // Temporal Proximity Timer enable for interupt priority level 7 and below.
    INT_TPC_GROUP_PRI_LEVEL_7  /*DOM-IGNORE-BEGIN*/=      7 /*DOM-IGNORE-END*/
}INT_TPC_GROUP;

// *****************************************************************************
/* Interrupt Vector Spacing

  Summary:
    Interrupt vector spacing definitions.

  Description:
    These definitions can be used to set the core's EBASE vector spacing.
*/
typedef enum
{
    // Vector spacing of 32 bytes.
    INT_VS_32  /*DOM-IGNORE-BEGIN*/= 32 /*DOM-IGNORE-END*/,
    // Vector spacing of 64 bytes.
    INT_VS_64  /*DOM-IGNORE-BEGIN*/= 64 /*DOM-IGNORE-END*/,
    // Vector spacing of 128 bytes.
    INT_VS_128  /*DOM-IGNORE-BEGIN*/= 128 /*DOM-IGNORE-END*/,
    // Vector spacing of 256 bytes.
    INT_VS_256  /*DOM-IGNORE-BEGIN*/= 256 /*DOM-IGNORE-END*/,
    // Vector spacing of 512 bytes.
    INT_VS_512  /*DOM-IGNORE-BEGIN*/= 512 /*DOM-IGNORE-END*/
}INT_VS;

// *****************************************************************************
/* Interrupt Configuration

  Summary:
    Interrupt configuration parameters.

  Description:
    These definitions can be used to configure the system interrupt controller.
*/
typedef enum
{
    // Configure the interrupt module for Mult-vector mode.
    INT_SYSTEM_CONFIG_MULT_VECTOR,
    // Configure the interrupt module for Single Vectored mode.
    INT_SYSTEM_CONFIG_SINGLE_VECTOR
}INT_SYSTEM_CONFIG;

// *****************************************************************************
// *****************************************************************************
// Section: Interrupt Peripheral Library Interface Routines and Macors
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    unsigned int __attribute__((nomips16)) INTDisableInterrupts(void)

  Summary:
    Disables the PIC32MX from handling interrupts.

  Description:
    This routine disables the core from handling any pending interrupt requests.

  Precondition:
    None

  Parameters:
    None

  Returns:
    The previous state of the CP0 register Status.IE bit.  The INTRestoreInterrupts
    function can be used in other routines to restore the system interrupt state.

  Example:
    <code>
    unsigned int intStatus;

    intStatus = INTDisableInterrupts();
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
unsigned int __attribute__((nomips16)) INTDisableInterrupts(void);

/*******************************************************************************
  Function:
    unsigned int __attribute__((nomips16))  INTEnableInterrupts(void)

  Summary:
    Enables the PIC32MX to handle interrupts.

  Description:
    This routine enables the core to handle any pending interrupt requests.

  Precondition:
    Need to configure system using INTConfigureSystem

  Parameters:
    None

  Returns:
    The previous state of the CP0 register Status.IE bit.  The INTRestoreInterrupts
    function can be used in other routines to restore the system interrupt state.

  Example:
    <code>
    unsigned int intStatus;

    intStatus = INTEnableInterrupts();
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
unsigned int __attribute__((nomips16))  INTEnableInterrupts(void);

/*******************************************************************************
  Function:
    void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status)

  Summary:
    Restores the PIC32MX interrupt state.

  Description:
    This routine restores the core to the previous interrupt handling state.

  Precondition:
    None

  Parameters:
    status      - the state of the CP0 register Status.IE

  Returns:
    None

  Example:
    <code>
    unsigned int intStatus;

    intStatus = INTDisableInterrupts();

    // ... application code

    INTRestoreInterrupts(intStatus);
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status);

/*******************************************************************************
  Function:
    void __attribute__ ((nomips16)) INTConfigureSystem(INT_SYSTEM_CONFIG config);

  Summary:
    Configures the system for  multi-vector or single vectored interrupts.

  Description:
    This routine configures the core to receive interrupt requests and configures the
    Interrupt module for Multi-vectored or Single Vectored mode.

  Precondition:
    None

  Parameters:
    config      - The interrupt configuration to set.

  Returns:
    None

  Example:
    Configure for Multi-vectored mode
    <code>

    // configure for multi-vectored mode
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);

    // enable interrupts
    INTEnableInterrupts();

    // ...
    </code>

    Configure for Single Vectored mode
    <code>

    // configure for single vectored mode
    INTConfigureSystem(INT_SYSTEM_CONFIG_SINGLE_VECTOR);

    // enable interrupts
    INTEnableInterrupts();

    // ...
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
void __attribute__ ((nomips16)) INTConfigureSystem(INT_SYSTEM_CONFIG config);

/*******************************************************************************
  Function:
    void __attribute__((nomips16)) INTSetEBASE(unsigned int ebase_address)

  Summary:
    Sets the PIC32MX exception base.

  Description:
    This routine sets the exception base of the core.

  Precondition:
    None

  Parameters:
    ebase_address   - The address of the EBASE.
                    * must be be located in KSEG0 or KSEG1
                    * must be 4KB aligned

  Returns:
    None

  Example:
    <code>

    INTSetEBASE(0xBFC01000);
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
void __attribute__((nomips16)) INTSetEBASE(unsigned int ebase_address);

/*******************************************************************************
  Function:
    void __attribute__((nomips16)) INTSetVectorSpacing(INT_VS vector_spacing)

  Summary:
    Sets the PIC32MX exception vector spacing.

  Description:
    This routine sets the exception vector spacing of the core.

  Precondition:
    None

  Parameters:
    vector_spacing  - The desired vector spacing.

  Returns:
    None

  Example:
    <code>

    INTSetVectorSpacing(INT_VS_32);
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
void __attribute__((nomips16)) INTSetVectorSpacing(INT_VS vector_spacing);

/*******************************************************************************
  Function:
    extern inline void __attribute__ ((always_inline)) INTSingleVectorRegisterSet(INT_SV_SS reg_set)

  Summary:
    Sets the single vectored interrupt handler's general purpose register set.

  Description:
    This routine sets the single vectored interrupt handler's general purpose register set.

  Precondition:
    The interrupt controller must be set in Single Vectored mode.

  Parameters:
    reg_set         - Register set for the single vectored handler.

  Returns:
    None

  Example:
    <code>

    INTSingleVectorRegisterSet(INT_REGISTOR_SET_SELECT_NONE);
    </code>

  Remarks:
    None.
  *****************************************************************************/
extern inline void __attribute__ ((always_inline)) INTSingleVectorRegisterSet(INT_SV_SS reg_set)
{
    INTCONCLR   = reg_set;
    INTCONSET    = reg_set;
}

/*******************************************************************************
  Function:
    extern inline unsigned int __attribute__ ((always_inline)) INTGetInterruptVectorNumber(void)

  Summary:
    Gets the pending interrupt vector.

  Description:
    This routine gets the pending interrupt vector number.

  Precondition:
    None

  Parameters:
    None

  Returns:
    The pending interrupt vector number.

  Example:
    <code>
    unsigned int vector_num;

    vector_num = INTGetInterruptVectorNumber();

    if(vector_num == _CORE_TIMER_VECTOR)
    {
        // ...
    }
    </code>

  Remarks:
    The user can refer to the device file to get the vector number defines.
  *****************************************************************************/
extern inline unsigned int __attribute__ ((always_inline)) INTGetInterruptVectorNumber(void)
{
    return (unsigned int)(INTSTATbits.VEC);
}
/*******************************************************************************
  Function:
    extern inline INT_PRIORITY __attribute__ ((always_inline)) INTGetInterruptVectorPriority(void)

  Summary:
    Gets the pending interrupt vector's priority.

  Description:
    This routine gets the pending interrupt vector's priority.

  Precondition:
    None

  Parameters:
    None

  Returns:
    The pending interrupt vector's priority.

  Example:
    <code>
    INT_PRIORITY vector_pri;

    vector_pri = INTGetInterruptVectorPriority();
    </code>

  Remarks:
    None.
  *****************************************************************************/
extern inline INT_PRIORITY __attribute__ ((always_inline)) INTGetInterruptVectorPriority(void)
{
    return (INT_PRIORITY)(INTSTATbits.SRIPL);
}

/*******************************************************************************
  Function:
    extern inline void __attribute__ ((always_inline)) INTGetInterruptVectorNumberAndPriority(unsigned int *number, INT_PRIORITY *priority)

  Summary:
    Gets the pending interrupt vector number and priority.

  Description:
    This routine gets the pending interrupt vector number and priority.

  Precondition:
    *number - pointer to the vector number

    *priority - pointer to the interrupt priority

  Parameters:
    None

  Returns:
    The pending interrupt vector's priority.

  Example:
    <code>
    INT_PRIORITY vector_pri;
    unsigned int vector_num;

    INTGetInterruptVectorNumberAndPriority(&vector_num, &vector_pri);

    if(vector_num == _CORE_TIMER_VECTOR)
    {
        // ...
    }
    </code>

  Remarks:
    None.
  *****************************************************************************/
extern inline void __attribute__ ((always_inline)) INTGetInterruptVectorNumberAndPriority(unsigned int *number, INT_PRIORITY *priority)
{
    *number     = INTSTAT;
    *priority   = (INT_PRIORITY)((*number >> 8) & 7);
    *number     &= 0x3F;
}

/*******************************************************************************
  Function:
    void INTClearFlag(INT_SOURCE source)

  Summary:
    Clears an interrupt request flag.

  Description:
    This routine clears an interrupt request flag.

  Precondition:
    None

  Parameters:
    source  - Interrupt source.

  Returns:
    None

  Example:
    <code>
    INTClearFlag(INT_CT);
    </code>

  Remarks:
    None
  *****************************************************************************/
void INTClearFlag(INT_SOURCE source);

/*******************************************************************************
  Function:
    void INTSetFlag(INT_SOURCE source)

  Summary:
    Sets an interrupt request flag.

  Description:
    This routine sets an interrupt request flag.

  Precondition:
    None

  Parameters:
    source  - Interrupt source.

  Returns:
    None

  Example:
    <code>
    INTSetFlag(INT_CT);
    </code>

  Remarks:
    If the corresponding interrupt enable is set, this routine will cause the
    application to vector to the interrupt's handler.
  *****************************************************************************/
void INTSetFlag(INT_SOURCE source);

/*******************************************************************************
  Function:
    unsigned int INTGetFlag(INT_SOURCE source)

  Summary:
    Gets the interrupt request flag.

  Description:
    This routine gets an interrupt request flag.

  Precondition:
    None

  Parameters:
    source  - Interrupt source.

  Returns:
    * 0 if the interrupt request flag is clear
    * 1 if the interrupt request flag is set

  Example:
    <code>
    if(INTGetFlag(INT_CT))
    {
        // ... do something
    }
    </code>

  Remarks:
    None
  *****************************************************************************/
unsigned int INTGetFlag(INT_SOURCE source);

/*******************************************************************************
  Function:
    void INTEnable(INT_SOURCE source, INT_EN_DIS enable)

  Summary:
    Enables or disables the interrupt.

  Description:
    This routine enables or disables the interrupt source.

  Precondition:
    None

  Parameters:
    source  - Interrupt source.
    enable  - Enable state to set.

  Returns:
    None

  Example:
    <code>
    INTEnable(INT_CT, INT_ENABLED);
    </code>

  Remarks:
    None
  *****************************************************************************/
void INTEnable(INT_SOURCE source, INT_EN_DIS enable);

/*******************************************************************************
  Function:
    unsigned int INTGetEnable(INT_SOURCE source)

  Summary:
    Gets the interrupt enable.

  Description:
    This routine gets an interrupt enable.

  Precondition:
    None

  Parameters:
    source  - Interrupt source.

  Returns:
    * 0 if interrupt is disabled
    * else interrupt is enabled

  Example:
    <code>
    if(INTGetEnable(INT_CT))
    {
        // ... do something
    }
    </code>

  Remarks:
    None
  *****************************************************************************/
unsigned int INTGetEnable(INT_SOURCE source);

/*******************************************************************************
  Function:
    void INTSetVectorPriority(INT_VECTOR vector, INT_PRIORITY priority)

  Summary:
    Sets the interrupt vector's priority.

  Description:
    This routine sets the interrupt vector's priority.

  Precondition:
    None

  Parameters:
    vector    - Interrupt vector.
    priority  - Interrupt vector's priority.

  Returns:
    None

  Example:
    <code>
    INTSetVectorPriority(INT_CORE_TIMER_VECTOR, INT_PRIORITY_LEVEL_4);
    </code>

  Remarks:
    None
  *****************************************************************************/
void INTSetVectorPriority(INT_VECTOR vector, INT_PRIORITY priority);

/*******************************************************************************
  Function:
    INT_PRIORITY INTGetVectorPriority(INT_VECTOR vector)

  Summary:
    Gets the interrupt vector's priority.

  Description:
    This routine gets the interrupt vector's priority.

  Precondition:
    None

  Parameters:
    vector    - Interrupt vector.

  Returns:
    interrupt vector's priority

  Example:
    <code>
    INT_PRIORITY intPriority;

    intPriority = INTGetVectorPriority(INT_CORE_TIMER_VECTOR);
    </code>

  Remarks:
    None
  *****************************************************************************/
INT_PRIORITY INTGetVectorPriority(INT_VECTOR vector);

/*******************************************************************************
  Function:
    void INTSetVectorSubPriority(INT_VECTOR vector, INT_SUB_PRIORITY subPriority)

  Summary:
    Sets the interrupt vector's sub-priority.

  Description:
    This routine sets the interrupt vector's sub-priority.

  Precondition:
    None

  Parameters:
    vector          - Interrupt vector.
    subPriority     - Interrupt vector's sub-priority.

  Returns:
    None

  Example:
    <code>
    INTSetVectorSubPriority(INT_CORE_TIMER_VECTOR, INT_SUB_PRIORITY_LEVEL_1);
    </code>

  Remarks:
    None
  *****************************************************************************/
void INTSetVectorSubPriority(INT_VECTOR vector, INT_SUB_PRIORITY subPriority);

/*******************************************************************************
  Function:
    INT_SUB_PRIORITY INTGetVectorSubPriority(INT_VECTOR vector)

  Summary:
    Gets the interrupt vector's priority.

  Description:
    This routine gets the interrupt vector's priority.

  Precondition:
    None

  Parameters:
    vector    - Interrupt vector.

  Returns:
    interrupt vector's sub-priority

  Example:
    <code>
    INT_SUB_PRIORITY intSubPriority;

    intSubPriority = INTGetVectorSubPriority(INT_CORE_TIMER_VECTOR);
    </code>

  Remarks:
    None
  *****************************************************************************/
INT_SUB_PRIORITY INTGetVectorSubPriority(INT_VECTOR vector);


/*******************************************************************************
  Function:
    extern inline void __attribute__ ((always_inline)) INTSetIntProximityTimerReload(unsigned int time)

  Summary:
    Sets or reloads the interrupt proximity timer.

  Description:
    This routine sets or reloads the interrupt proximity timer.

  Precondition:
    None

  Parameters:
    time            - 32-bit timer value.

  Returns:
    None

  Example:
    <code>
    INTSetIntProximityTimerReload(56000);
    </code>

  Remarks:
    None
  *****************************************************************************/
extern inline void __attribute__ ((always_inline)) INTSetIntProximityTimerReload(unsigned int time)
{
    IPTMR = time;
}
/*******************************************************************************
  Function:
    extern inline unsigned int __attribute__ ((always_inline)) INTGetIntProximityTimer(void)

  Summary:
    Gets the interrupt proximity timer.

  Description:
    This routine gets the interrupt proximity timer.

  Precondition:
    None

  Parameters:
    None

  Returns:
    The current tick count of the timer.

  Example:
    <code>
    unsigned int time

    time = INTGetIntProximityTimer();
    </code>

  Remarks:
    None
  *****************************************************************************/
extern inline unsigned int __attribute__ ((always_inline)) INTGetIntProximityTimer(void)
{
    return IPTMR;
}
/*******************************************************************************
  Function:
    extern inline void __attribute__ ((always_inline)) INTSetTemporalProximityControl(INT_TPC_GROUP level)

  Summary:
    Sets the Temporal Proximity Control level.

  Description:
    This routine sets the temporal Proximity Control level.

  Precondition:
    None

  Parameters:
    level   - Level of the temporal proximity control.

  Returns:
    None

  Example:
    <code>
    INTSetTemporalProximityControl(INT_TPC_GROUP_PRI_LEVEL_3);
    </code>

  Remarks:
    None
  *****************************************************************************/
extern inline void __attribute__ ((always_inline)) INTSetTemporalProximityControl(INT_TPC_GROUP level)
{
    INTCONCLR = INT_TPC_DISABLE;

    if(level != INT_TPC_DISABLE)
        INTCONSET = (level << _INTCON_TPC_POSITION);
}

/*******************************************************************************
//DOM-IGNORE-BEGIN
  *****************************************************************************/
#define CORE_SW_0           0x00000100
#define CORE_SW_1           0x00000200
/*******************************************************************************
//DOM-IGNORE-END
  *****************************************************************************/


/*******************************************************************************
  Function:
    void __attribute__ ((nomips16)) CoreSetSoftwareInterrupt0(void)

  Summary:
    Set the Core Software Interrupt 0.

  Description:
    This routine sets the core software interrupt 0.  This will produce an interrupt
    service request.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Example:
    <code>

    CoreSetSoftwareInterrupt0();
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
void __attribute__((nomips16)) CoreSetSoftwareInterrupt0(void);

/*******************************************************************************
  Function:
    void __attribute__ ((nomips16)) CoreSetSoftwareInterrupt1(void)

  Summary:
    Set the Core Software Interrupt 1.

  Description:
    This routine sets the core software interrupt 1.  This will produce an interrupt
    service request.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Example:
    <code>

    CoreSetSoftwareInterrupt1();
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.
  *****************************************************************************/
void __attribute__((nomips16))  CoreSetSoftwareInterrupt1(void);

/*******************************************************************************
  Function:
    void __attribute__ ((nomips16)) CoreClearSoftwareInterrupt0(void)

  Summary:
    Clears the Core Software Interrupt 0.

  Description:
    This routine clears the core software interrupt 0.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Example:
    <code>

    CoreClearSoftwareInterrupt0();

    INTClearFlag(INT_CS0);
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.

    You must call this routine before clearing the interrupt request flag.
  *****************************************************************************/
void __attribute__((nomips16))  CoreClearSoftwareInterrupt0(void);

/*******************************************************************************
  Function:
    void __attribute__ ((nomips16)) CoreClearSoftwareInterrupt1(void)

  Summary:
    Clears the Core Software Interrupt 1.

  Description:
    This routine clears the core software interrupt 1.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Example:
    <code>

    CoreClearSoftwareInterrupt1();

    INTClearFlag(INT_CS1);
    </code>

  Remarks:
    This function cannot be compiled using the MIPS16 instruction set.

    You must call this routine before clearing the interrupt request flag.
  *****************************************************************************/
void __attribute__((nomips16))  CoreClearSoftwareInterrupt1(void);

#ifdef __cplusplus
  }
#endif

#endif

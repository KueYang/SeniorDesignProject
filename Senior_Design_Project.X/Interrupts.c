#include <xc.h>
#include "plib/peripheral/Interrupts.h"

enum
{
    SFR_REG,
    SFR_CLR,
    SFR_SET,
    SFR_INV
};

typedef volatile unsigned int   VUINT;

typedef struct
{
    VUINT           *ifs;
    VUINT           *iec;
    unsigned int    mask;
}INT_SCR_TBL_ENTRY;

const INT_SCR_TBL_ENTRY __IntSrcTbl[] =
{
    {   &IFS0,  &IEC0,  _IFS0_CTIF_MASK        },  // Core Timer Interrupt

    {   &IFS0,  &IEC0,  _IFS0_CS0IF_MASK       },  // Core Software Interrupt 0
    {   &IFS0,  &IEC0,  _IFS0_CS1IF_MASK       },  // Core Software Interrupt 1

    {   &IFS0,  &IEC0,  _IFS0_INT0IF_MASK      },  // External Interrupt 0
    {   &IFS0,  &IEC0,  _IFS0_INT1IF_MASK      },  // External Interrupt 1
    {   &IFS0,  &IEC0,  _IFS0_INT2IF_MASK      },  // External Interrupt 2
    {   &IFS0,  &IEC0,  _IFS0_INT3IF_MASK      },  // External Interrupt 3
    {   &IFS0,  &IEC0,  _IFS0_INT4IF_MASK      },  // External Interrupt 4

    {   &IFS0,  &IEC0,  _IFS0_T1IF_MASK        },  // Timer 1
    {   &IFS0,  &IEC0,  _IFS0_T2IF_MASK        },  // Timer 2
    {   &IFS0,  &IEC0,  _IFS0_T3IF_MASK        },  // TImer 3
    {   &IFS0,  &IEC0,  _IFS0_T4IF_MASK        },  // Timer 4
    {   &IFS0,  &IEC0,  _IFS0_T5IF_MASK        },  // Timer 5

    {   &IFS1,  &IEC1,  (_IFS1_U1EIF_MASK | _IFS1_U1RXIF_MASK | _IFS1_U1TXIF_MASK)}, //UART1
    {   &IFS1,  &IEC1,  (_IFS1_U2EIF_MASK | _IFS1_U2RXIF_MASK | _IFS1_U2TXIF_MASK)}, //UART2
    {   (VUINT *)0, (VUINT *)0,   0                                               },  //UART3 - Not supported, it crosses two SFR registers
//    {   &IFS2,  &IEC2,  (_IFS2_U4EIF_MASK | _IFS2_U4RXIF_MASK | _IFS2_U4TXIF_MASK)}, //UART4

    {   &IFS1,  &IEC1,  _IFS1_U1EIF_MASK       },  // UART 1 Error
    {   &IFS1,  &IEC1,  _IFS1_U2EIF_MASK       },  // UART 2 Error
//    {   &IFS1,  &IEC1,  _IFS1_U3EIF_MASK       },  // UART 3 Error
//    {   &IFS2,  &IEC2,  _IFS2_U4EIF_MASK       },  // UART 4 Error

    {   &IFS1,  &IEC1,  _IFS1_U1RXIF_MASK      },  // UART 1 Receive
    {   &IFS1,  &IEC1,  _IFS1_U2RXIF_MASK      },  // UART 2 Receive
//    {   &IFS1,  &IEC1,  _IFS1_U3RXIF_MASK      },  // UART 3 Receive
//    {   &IFS2,  &IEC2,  _IFS2_U4RXIF_MASK      },  // UART 4 Receive

    {   &IFS1,  &IEC1,  _IFS1_U1TXIF_MASK      },  // UART 1 Transmit
    {   &IFS1,  &IEC1,  _IFS1_U2TXIF_MASK      },  // UART 2 Transmit
//    {   &IFS2,  &IEC2,  _IFS2_U3TXIF_MASK      },  // UART 3 Transmit
//    {   &IFS2,  &IEC2,  _IFS2_U4TXIF_MASK      },  // UART 4 Transmit
    
    {   &IFS0,  &IEC0,  _IFS0_AD1IF_MASK       },  // ADC 1 Convert Done
};

typedef struct
{
    VUINT           *ipc;
    unsigned int    sub_shift;
    unsigned int    pri_shift;
}INT_VECTOR_TBL_ENTRY;

const INT_VECTOR_TBL_ENTRY __IntVectorTbl[] =
{
    {   &IPC0,  _IPC0_CTIS_POSITION,    _IPC0_CTIP_POSITION     },  // Core Timer Interrupt

    {   &IPC0,  _IPC0_CS0IS_POSITION,   _IPC0_CS0IP_POSITION    },  // Core Software Interrupt 0
    {   &IPC0,  _IPC0_CS1IS_POSITION,   _IPC0_CS1IP_POSITION    },  // Core Software Interrupt 1

    {   &IPC0, _IPC0_INT0IS_POSITION,  _IPC0_INT0IP_POSITION   },  // External Interrupt 0
    {   &IPC1, _IPC1_INT1IS_POSITION,  _IPC1_INT1IP_POSITION   },  // External Interrupt 1
    {   &IPC2, _IPC2_INT2IS_POSITION,  _IPC2_INT2IP_POSITION   },  // External Interrupt 2
    {   &IPC3, _IPC3_INT3IS_POSITION,  _IPC3_INT3IP_POSITION   },  // External Interrupt 3
    {   &IPC4, _IPC4_INT4IS_POSITION,  _IPC4_INT4IP_POSITION   },  // External Interrupt 4

    {   &IPC1, _IPC1_T1IS_POSITION,    _IPC1_T1IP_POSITION     },  // Timer 1
    {   &IPC2, _IPC2_T2IS_POSITION,    _IPC2_T2IP_POSITION     },  // Timer 2
    {   &IPC3, _IPC3_T3IS_POSITION,    _IPC3_T3IP_POSITION     },  // T1mer 3
    {   &IPC4, _IPC4_T4IS_POSITION,    _IPC4_T4IP_POSITION     },  // Timer 4
    {   &IPC5, _IPC5_T5IS_POSITION,    _IPC5_T5IP_POSITION     },  // Timer 5

//    {   &IPC7,  _IPC7_U1IS_POSITION,   _IPC7_U1IP_POSITION     },  // UART 1
    {   &IPC9,  _IPC9_U2IS_POSITION,   _IPC9_U2IP_POSITION     },  // UART 2
//    {   &IPC9,  _IPC9_U3IS_POSITION,   _IPC9_U3IP_POSITION     },  // UART 3
//    {   &IPC9,  _IPC9_U4IS_POSITION,   _IPC9_U4IP_POSITION     },  // UART 4
    
    {   &IPC5,  _IPC5_AD1IS_POSITION,  _IPC5_AD1IP_POSITION    },  // ADC 1 Convert Done
    
    {   (VUINT *)0,  0,  0      },
};

/******************************************************************************
 * Function:        void ClearCoreSW0(void)
 *
 * Description:		Clears the core software interrupt
 *
 * PreCondition:    None
 *
 * Inputs:			None
 *
 * Output:          Count value.
 *
 * Example:			ClearCoreSW0()
 *
 *****************************************************************************/
void __attribute__((nomips16))  CoreClearSoftwareInterrupt0(void)
{
   unsigned int val;
   asm volatile("mfc0 %0,$13" : "=r" (val));
   val &= ~(CORE_SW_0);
   asm volatile("mtc0 %0,$13" : "+r" (val));
}

/******************************************************************************
 * Function:        void ClearCoreSW1(void)
 *
 * Description:		Clears the core software interrupt
 *
 * PreCondition:    None
 *
 * Inputs:			None
 *
 * Output:          Count value.
 *
 * Example:			ClearCoreSW1()
 *
 *****************************************************************************/
void __attribute__((nomips16))  CoreClearSoftwareInterrupt1(void)
{
   unsigned int val;
   asm volatile("mfc0 %0,$13" : "=r" (val));
   val &= ~(CORE_SW_1);
   asm volatile("mtc0 %0,$13" : "+r" (val));
}

/*******************************************************************************
  Function:
    void __attribute__ ((nomips16)) INTConfigureSystem(INT_SYSTEM_CONFIG config);

  Summary:
    Configures system for  multi-vector interrupts.

  Description:
    This routine configures the core to receive interrupt requests and configures the 
    interrupt controller for multi-vectored mode.

  Precondition:
    None

  Parameters:
    config      - the interrupt configuration to set

  Returns:
    None

  *****************************************************************************/
void __attribute__ ((nomips16)) INTConfigureSystem(INT_SYSTEM_CONFIG config)
{
    unsigned int val;

    // set the CP0 cause IV bit high
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    if(config == INT_SYSTEM_CONFIG_MULT_VECTOR)
    {
        INTCONSET = _INTCON_MVEC_MASK;
    }
    else
    {
        INTCONCLR = _INTCON_MVEC_MASK;
    }
}

/*********************************************************************
 * Function:        mINTDisableInterrupts()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Interrupts are disabled
 *
 * Overview:        Interrupts are disabled by clearing the IE bit
 *                  in the status register
 ********************************************************************/
unsigned int __attribute__((nomips16)) INTDisableInterrupts(void)
{
    unsigned int status = 0;

    asm volatile("di    %0" : "=r"(status));

    return status;
}

/*********************************************************************
 * Function:        mINTEnableInterrupts()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Interrupts are enabled
 *
 * Overview:        Interrupts are enabled by setting the IE bit
 *                  in the status register
 ********************************************************************/
unsigned int __attribute__((nomips16))  INTEnableInterrupts(void)
{
    unsigned int status = 0;

    asm volatile("ei    %0" : "=r"(status));

    return status;
}

/*********************************************************************
 * Function:        void INTEnableSystemMultiVectoredInt(void)
 *
 * PreCondition:    EBASE and IntCtl.VS set up
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    System interrupts are turned on
 *
 * Overview:        Enables system wide multi-vectored interrupts
 ********************************************************************/
void __attribute__ ((nomips16)) INTEnableSystemMultiVectoredInt(void)
{
    unsigned int val;

    // set the CP0 cause IV bit high
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    INTCONSET = _INTCON_MVEC_MASK;

    // set the CP0 status IE bit high to turn on interrupts
    INTEnableInterrupts();

}

/*********************************************************************
 * Function:       void INTEnableSystemSingleVectoredInt(void)
 *
 * PreCondition:    EBASE and IntCtrl.VS set up (VS = 0)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Single vectored interrupts are turned on
 *
 * Overview:        Enables system wide single vectored interrupts
 ********************************************************************/
void __attribute__ ((nomips16)) INTEnableSystemSingleVectoredInt(void)
{
    unsigned int val;

    // set the CP0 cause IV bit high
    asm volatile("mfc0   %0,$13" : "=r"(val));
    val |= 0x00800000;
    asm volatile("mtc0   %0,$13" : "+r"(val));

    INTCONCLR = _INTCON_MVEC_MASK;

    // set the CP0 status IE bit high to turn on interrupts
    INTEnableInterrupts();
}

/*********************************************************************
 * Function:        unsigned int INTGetPendingInterrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          The pending interrupt or invalid data
 *
 * Side Effects:    None
 *
 * Overview:        Gets the pending interrupt
 ********************************************************************/
unsigned int INTGetPendingInterrupt(void)
{
    unsigned int result;
    unsigned int i;
    unsigned int mask;
    unsigned int value;

    result = INT_NUM;

    if((value = IFS0))
    {
        for(mask = 1, i = 0; mask > 0; mask <<= 1, i++)
        {
            if(mask & value)
                return i;
        }
    }

    if((value = IFS1))
    {
        for(mask = 1, i = 32; mask > 0; mask <<= 1, i++)
        {
            if(mask & value)
                return i;
        }
    }

    return INT_NUM;
}

/*********************************************************************
 * Function:        INTRestoreInterrupts(unsigned int status)
 *
 * PreCondition:    None
 *
 * Input:           value of the status registor
 *
 * Output:
 *
 * Side Effects:    Interrupts are restored to previous state
 *
 * Overview:        Interrupts are enabled by setting the IE bit
 *                  in the status register
 ********************************************************************/
void __attribute__((nomips16))  INTRestoreInterrupts(unsigned int status)
{
    if(status & 0x00000001)
    {
        asm volatile("ei");
    }
    else
    {
        asm volatile("di");
    }
}


/*********************************************************************
 * Function:        INTSetEBASE()
 *
 * PreCondition:    None
 *
 * Input:           ebase_address - address of ebase
 *
 * Output:          None
 *
 * Side Effects:    Interrupts are disabled
 *
 * Overview:        Interrupts are disabled by clearing the IE bit
 *                  in the status register
 ********************************************************************/
void __attribute__((nomips16)) INTSetEBASE(unsigned int ebase_address)
{
    unsigned int temp, status;
    status = INTDisableInterrupts();
    temp = _CP0_GET_STATUS();                               // get the CP0 Status register
    temp |= _CP0_STATUS_BEV_MASK;                           // set the BEV bit
    _CP0_SET_STATUS(temp);                                  // Update the Status register
    _CP0_SET_EBASE(ebase_address);                          // Set the EBASE Value (Kseg0 or Kseg1)
    temp = _CP0_GET_STATUS();                               // get the CP0 Status register
    temp &= ~(_CP0_STATUS_BEV_MASK);                        // Clear EXL and BEV bits
    _CP0_SET_STATUS(temp);                                  // Update the Status register
    INTRestoreInterrupts(status);
}

/*********************************************************************
 * Function:        INTSetEBASEAndVectorSpacing()
 *
 * PreCondition:    None
 *
 * Input:           ebase_address - address of ebase

 *                  vector_spacing - address of ebase
 *
 * Output:          None
 *
 * Side Effects:    Interrupts are disabled
 *
 * Overview:        Interrupts are disabled by clearing the IE bit
 *                  in the status register
 ********************************************************************/
void __attribute__((nomips16)) INTSetEBASEAndVectorSpacing(unsigned int ebase_address, unsigned int vector_spacing)
{
    unsigned int temp;
    unsigned int status;
    status = INTDisableInterrupts();
    temp = _CP0_GET_STATUS();                               // get the CP0 Status register
    temp |= _CP0_STATUS_BEV_MASK;                           // set the BEV bit
    _CP0_SET_STATUS(temp);                                  // Update the Status register
    _CP0_SET_EBASE(ebase_address);                          // Set the EBASE Value (Kseg0 or Kseg1)
    _CP0_SET_INTCTL(vector_spacing);                      // set the vector spacing
    temp = _CP0_GET_STATUS();                               // get the CP0 Status register
    temp &= ~(_CP0_STATUS_BEV_MASK);                        // Clear EXL and BEV bits
    _CP0_SET_STATUS(temp);                                  // Update the Status register
    INTRestoreInterrupts(status);
}

/*********************************************************************
 * Function:        INTSetVectorSpacing()
 *
 * PreCondition:    None
 *
 * Input:           vector_spacing - address of ebase
 *
 * Output:          None
 *
 * Side Effects:    Interrupts are disabled
 *
 * Overview:        Interrupts are disabled by clearing the IE bit
 *                  in the status register
 ********************************************************************/
void __attribute__((nomips16)) INTSetVectorSpacing(unsigned int vector_spacing)
{
    unsigned int temp;
    unsigned int status;
    status = INTDisableInterrupts();
    temp = _CP0_GET_STATUS();                               // get the CP0 Status register
    temp |= _CP0_STATUS_BEV_MASK;                           // set the BEV bit
    _CP0_SET_STATUS(temp);                                  // Update the Status register
    _CP0_SET_INTCTL(vector_spacing);                      // set the vector spacing
    temp = _CP0_GET_STATUS();                               // get the CP0 Status register
    temp &= ~(_CP0_STATUS_BEV_MASK);                        // Clear EXL and BEV bits
    _CP0_SET_STATUS(temp);                                  // Update the Status register
    INTRestoreInterrupts(status);
}

/******************************************************************************
 * Function:        void SetCoreSW0(void)
 *
 * Description:		Sets a core software interrupt
 *
 * PreCondition:    None
 *
 * Inputs:			None
 *
 * Output:          Count value.
 *
 * Example:			SetCoreSW0()
 *
 *****************************************************************************/
void __attribute__((nomips16)) CoreSetSoftwareInterrupt0(void)
{
   unsigned int val;
   asm volatile("mfc0 %0,$13" : "=r" (val));
   val |= CORE_SW_0;
   asm volatile("mtc0 %0,$13" : "+r" (val));
}

void __attribute__((nomips16)) CoreSetSoftwareInterrupt1(void)
{
   unsigned int val;
   asm volatile("mfc0 %0,$13" : "=r" (val));
   val |= CORE_SW_1;
   asm volatile("mtc0 %0,$13" : "+r" (val));
}
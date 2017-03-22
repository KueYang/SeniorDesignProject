/********************************************************************
 * FileName:        perilib.h
 * Dependencies:
 * Processor:       PICxx
 * Hardware:        N/A
 * Assembler:       N/A
 * Linker:          N/A
 * Company:         Microchip Technology, Inc.
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
 *********************************************************************
 * File Description:
 *
 * Peripheral Library Master Include
 *
 *
 * $Id$
 ********************************************************************/

#ifndef _PERIPHERAL_LIBRARY_MASTER_HEADER_FILE
#define _PERIPHERAL_LIBRARY_MASTER_HEADER_FILE

#ifndef __C32__
#define __C32__ 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/attribs.h>
#include "peripheral/Interrupts.h"
#include "peripheral/lock.h"
#include "peripheral/pps.h"

#ifdef __cplusplus
}
#endif

#endif

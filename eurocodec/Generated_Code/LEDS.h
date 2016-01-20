/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : LEDS.h
**     Project     : eurocodec
**     Processor   : MK22FN512VLH12
**     Component   : 74HC595
**     Version     : Component 01.022, Driver 01.00, CPU db: 3.00.000
**     Repository  : Legacy User Components
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-12-16, 17:48, # CodeGen: 21
**     Abstract    :
**
Driver for the 74HC595 8bit serial shift register.
**     Settings    :
**          Component name                                 : LEDS
**          Latch (STCP)                                   : STCP
**          Data (DS)                                      : DS
**          Clock (SHCP)                                   : SHCP
**          Clock/Latch delay (ns)                         : 19
**          Wait                                           : WAIT1
**          MSB first                                      : yes
**          Enable Pin (OE)                                : Disabled
**     Contents    :
**         Disable   - void LEDS_Disable(void);
**         ShiftByte - void LEDS_ShiftByte(byte val);
**         Latch     - void LEDS_Latch(void);
**         ReadByte  - byte LEDS_ReadByte(void);
**         WriteByte - void LEDS_WriteByte(byte val);
**         Deinit    - void LEDS_Deinit(void);
**         Init      - void LEDS_Init(void);
**
**     License   :  Open Source (LGPL)
**     Copyright : (c) Copyright Erich Styger, 2013, all rights reserved.
**     Web: http://mcuoneclipse.com
**     This an open source software implementing a driver using Processor Expert.
**     This is a free software and is opened for education, research and commercial developments under license policy of following terms:
**     * This is a free software and there is NO WARRANTY.
**     * No restriction on use. You can use, modify and redistribute it for personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
**     * Redistributions of source code must retain the above copyright notice.
** ###################################################################*/
/*!
** @file LEDS.h
** @version 01.00
** @brief
**
Driver for the 74HC595 8bit serial shift register.
*/         
/*!
**  @addtogroup LEDS_module LEDS module documentation
**  @{
*/         

#ifndef __LEDS_H
#define __LEDS_H

/* MODULE LEDS. */

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited beans */
#include "STCP1.h"
#include "DS1.h"
#include "SHCP1.h"
#include "WAIT1.h"

#include "Cpu.h"




void LEDS_Deinit(void);
/*
** ===================================================================
**     Method      :  LEDS_Deinit (component 74HC595)
**     Description :
**         Deinitializes the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LEDS_Init(void);
/*
** ===================================================================
**     Method      :  LEDS_Init (component 74HC595)
**     Description :
**         Initializes the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LEDS_Latch(void);
/*
** ===================================================================
**     Method      :  LEDS_Latch (component 74HC595)
**     Description :
**         Sends a latch pulse to move the bits from the internal
**         memory to the output pins.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LEDS_ShiftByte(byte val);
/*
** ===================================================================
**     Method      :  LEDS_ShiftByte (component 74HC595)
**     Description :
**         Shift a byte to the shift register. The most significant bit
**         will be on output pin 0. Use Latch() to show the data on the
**         output pins.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - 8bit value to be shifted
**     Returns     : Nothing
** ===================================================================
*/

void LEDS_Disable(void);
/*
** ===================================================================
**     Method      :  LEDS_Disable (component 74HC595)
**     Description :
**         Disables the device
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LEDS_WriteByte(byte val);
/*
** ===================================================================
**     Method      :  LEDS_WriteByte (component 74HC595)
**     Description :
**         Shifts the given byte and latches it.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - value to be shifted into the shift
**                           register. The value will be latched and
**                           will appear on the output pins.
**     Returns     : Nothing
** ===================================================================
*/

byte LEDS_ReadByte(void);
/*
** ===================================================================
**     Method      :  LEDS_ReadByte (component 74HC595)
**     Description :
**         Returns the current value of the shift register. The current
**         value of the shift register is cached in the driver.
**     Parameters  : None
**     Returns     :
**         ---             - current shift register value (cached)
** ===================================================================
*/

/* END LEDS. */

#endif
/* ifndef __LEDS_H */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
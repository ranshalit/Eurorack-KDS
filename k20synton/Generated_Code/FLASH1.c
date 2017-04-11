/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : FLASH1.c
**     Project     : k20synton
**     Processor   : MK20DX256VLL10
**     Component   : FLASH_LDD
**     Version     : Component 01.106, Driver 01.15, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-04-09, 23:00, # CodeGen: 5
**     Abstract    :
**          This embedded component implements an access to an on-chip flash memory.
**          Using this component the flash memory could be written to, erased,
**          read from. Some other services would be provided, if they are supported
**          by hardware, e.g. changes of the flash memory protection state.
**     Settings    :
**          Component name                                 : FLASH1
**          Device                                         : FTFL
**          Use user memory areas                          : no
**          Interrupt service                              : Disabled
**            Write batch size                             : Minimal
**            Erase batch size                             : Minimal
**            Read batch size                              : Unlimited
**          Safe launch and wait                           : yes
**            Safe routine location                        : Defined statically by the component
**            Interruptable wait loop                      : no
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Auto initialization                          : no
**            Event mask                                   : 
**              OnOperationComplete                        : Enabled
**              OnError                                    : Disabled
**            CPU clock/configuration selection            : 
**              Clock configuration 0                      : This component enabled
**              Clock configuration 1                      : This component disabled
**              Clock configuration 2                      : This component disabled
**              Clock configuration 3                      : This component disabled
**              Clock configuration 4                      : This component disabled
**              Clock configuration 5                      : This component disabled
**              Clock configuration 6                      : This component disabled
**              Clock configuration 7                      : This component disabled
**     Contents    :
**         Init  - LDD_TDeviceData* FLASH1_Init(LDD_TUserData *UserDataPtr);
**         Read  - LDD_TError FLASH1_Read(LDD_TDeviceData *DeviceDataPtr, LDD_FLASH_TAddress...
**         Write - LDD_TError FLASH1_Write(LDD_TDeviceData *DeviceDataPtr, LDD_TData *FromPtr,...
**         Erase - LDD_TError FLASH1_Erase(LDD_TDeviceData *DeviceDataPtr, LDD_FLASH_TAddress...
**         Main  - void FLASH1_Main(LDD_TDeviceData *DeviceDataPtr);
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file FLASH1.c
** @version 01.15
** @brief
**          This embedded component implements an access to an on-chip flash memory.
**          Using this component the flash memory could be written to, erased,
**          read from. Some other services would be provided, if they are supported
**          by hardware, e.g. changes of the flash memory protection state.
*/         
/*!
**  @addtogroup FLASH1_module FLASH1 module documentation
**  @{
*/         

/* MODULE FLASH1. */

#include "Events.h"
#include "FLASH1.h"
#include "FreeRTOS.h" /* FreeRTOS interface */

#ifdef __cplusplus
extern "C" {
#endif

#define FTFE_FLEXNVM_START_ADDR          0x10000000U
/* bit 23 selects between pFLASH and FLEXNVM regions */
#define FTFE_FLEXNVM_CCOB_START_ADDR     0x00800000U


/* SafeRoutine() method prototype */
typedef void (* LDD_FLASH_TSafeRoutinePtr)(void); 

/* Structure used to copy SafeRoutine() code from FLASH to RAM. */
typedef struct{
  uint16_t SafeRoutine[24];            /* Safe routine code buffer */
} LDD_FLASH_TSafeRoutine;              /* Safe routine code buffer type */

/* Structure is used to allocate space for SafeRoutine() in the device data structure. */
/* Note: Code of SafeRoutine() must be placed on 4 bytes aligned address = 3 bytes of padding are added. */
typedef struct{
  uint16_t SafeRoutine[24];            /* Safe routine code buffer */
  uint8_t Padding[3];                  /* Padding */  
} LDD_FLASH_TSafeRoutineWithPadding;   /* Safe routine code buffer type */

/*
Safe launch and wait routine code. This code launch a command, waits for command
completion or command violation and then invalidates flash cache(s) and buffers.
Routine is written as array of words to be constant size and compiler independent.
C language implementation for Kinetis K could look like:

void SafeRoutine(void) {
  uint32_t CacheSetting;
  
  FTFL_PDD_LaunchCommand(FTFL_BASE_PTR);
  while ((FTFL_PDD_GetFlags(FTFL_BASE_PTR) & FTFL_PDD_COMMAND_COMPLETE) == 0U ) {};
  
  CacheSetting = FMC_PDD_GetFlashBankCacheAndBuffersSetting(FMC_BASE_PTR);
  FMC_PDD_DisableFlashBankCacheAndBuffers(FMC_BASE_PTR);
  FMC_PDD_InvalideteFlashCaheAndBuffers(FMC_BASE_PTR);
  FMC_PDD_SetFlashBankCacheAndBuffers(FMC_BASE_PTR, CacheSetting);
}

*/
static const uint16_t SafeRoutine[24] = {
  0xb407,          /*  push  {r0, r1, r2}                                    */
  0x4909,          /*  ldr   r1, FTFL_FSTAT_RegAddress                       */  
  0x2080,          /*  movs  r0, #0x80                                       */
  0x7008,          /*  strb  r0, [r1, #0]                                    */
  /* while(!FTFL_PDD_GetCmdCompleteFlag(FTFL_PDD_BASE_PTR)){}                */  
                   /* <LoadFlags>:                                           */
  0x7808,          /*  ldrb  r0, [r1, #0]                                    */
  0x0600,          /*  lsls  r0, r0, #24                                     */
  0xd5fc,          /*  bpl.n a <LoadFlags>                                   */
  /* r2 =  FMC_PFB0CR                                                        */ 
  0x4907,          /*  ldr   r1, FMC_PFB0CR_RegAddress                       */
  0x680a,          /*  ldr   r2, [r1, #0]                                    */
  /* FMC_PFB0CR =  0; - Disable cache and buffers                            */ 
  0x2000,          /*  movs  r0, #0x00                                       */
  0x6008,          /*  str   r0, [r1, #0]                                    */
  /* FMC_PFB0CR = 0xf80000; - Invalidate all cache ways                      */ 
  0xf44f, 0x0078,  /*  mov.w r0, #0xf80000                                   */
  0x6008,          /*  str   r0, [r1, #0]                                    */
  /* FMC_PFB0CR =  r2  revert cache settings                                 */ 
  0x600a,          /*  str   r2, [r1, #0]                                    */
  /* r2 =  FMC_PFB1CR                                                        */ 
  0x684a,          /*  ldr   r2, [r1, #4]                                    */
  /* FMC_PFB1CR =  0; - Disable cache and buffers                            */ 
  0x6048,          /*  str   r0, [r1, #4]                                    */
  /* FMC_PFB1CR =  r2  revert cache settings                                 */ 
  0x604a,          /*  str   r2, [r1, #4]                                    */
  0xbc07,          /*  pop   {r0, r1, r2}                                    */
  0x4770,          /*  bx    lr                                              */
  /* FTFL_FSTAT_RegAddress:                                                  */
  (uint16_t)(((uint32_t)&FTFL_FSTAT)>>0 & 0xFFFF),
  (uint16_t)(((uint32_t)&FTFL_FSTAT)>>16 & 0xFFFF),
  /* FMC_PFB0CR_RegAddress:                                                  */
  (uint16_t)(((uint32_t)&FMC_PFB0CR)>>0 & 0xFFFF),
  (uint16_t)(((uint32_t)&FMC_PFB0CR)>>16 & 0xFFFF)
};

typedef struct {
  LDD_FLASH_TOperationType          CurrentOperation; /* Current operation type */
  LDD_FLASH_TOperationStatus        CurrentOperationStatus; /* Current operation status */
  LDD_FLASH_TCommand                CurrentCommand; /* Last flash device command */
  LDD_FLASH_TErrorFlags             CurrentErrorFlags; /* Current error flags */
  LDD_FLASH_TDataSize               CurrentDataSize; /* Data size of the current operation step */
  uint32_t                          DataCounter; /* Counter of data to be proceeded in the current operation */
  uint8_t                          *CurrentDataPtr; /* Pointer to the source/destination data for the current operation step */
  LDD_FLASH_TAddress                CurrentFlashAddress; /* Address of destination/source in the flash memory for the operation step */
  LDD_FLASH_TSafeRoutineWithPadding SafeRoutine; /* Safe routine buffer */
  LDD_FLASH_TSafeRoutinePtr         SafeRoutinePtr; /* Safe routine pointer */
  LDD_TUserData                    *UserDataPtr; /* Pointer to user data */
} FLASH1_TDeviceData;

typedef FLASH1_TDeviceData *FLASH1_TDeviceDataPtr; /* Pointer to the device data structure. */
/* {FreeRTOS RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static FLASH1_TDeviceData DeviceDataPrv__DEFAULT_RTOS_ALLOC;

#define AVAILABLE_EVENTS_MASK (LDD_FLASH_ON_OPERATION_COMPLETE)

/* Internal method prototypes */
static LDD_TError RangeCheck(LDD_FLASH_TAddress Address, LDD_FLASH_TDataSize Size);

/*
** ===================================================================
**     Method      :  FLASH1_Init (component FLASH_LDD)
*/
/*!
**     @brief
**         Initializes the device. Allocates memory for the device data
**         structure, allocates interrupt vectors and sets interrupt
**         priority, sets pin routing, sets timing, etc. If the "Enable
**         in init. code" property is set to "yes" ([Enabled in init.
**         code]) value then the device is also enabled(see the
**         description of the Enable() method). In this case the
**         Enable() method is not necessary and needn't to be generated.
**         This method can be called only once. Before the second call
**         of Init() the Deinit() must be called first.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Device data structure pointer.
*/
/* ===================================================================*/
LDD_TDeviceData* FLASH1_Init(LDD_TUserData *UserDataPtr)
{
  FLASH1_TDeviceDataPtr DeviceDataPrv;

  /* Allocate LDD device structure */
  /* {FreeRTOS RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPrv = &DeviceDataPrv__DEFAULT_RTOS_ALLOC;
  DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_IDLE; /* Initialization of the Current operation status item */
  /* SafeRoutine() code must be placed on 4 bytes aligned address */  
  DeviceDataPrv->SafeRoutinePtr = (LDD_FLASH_TSafeRoutinePtr)((((uint32_t)&DeviceDataPrv->SafeRoutine) + 0x03U) & ~0x03U);
  /* Copy of the SafeRoutine() code to the buffer in the device data structure */
  *(LDD_FLASH_TSafeRoutine *)(void *)DeviceDataPrv->SafeRoutinePtr = *(LDD_FLASH_TSafeRoutine *)(void *)&SafeRoutine;
  DeviceDataPrv->UserDataPtr = UserDataPtr; /* Store a pointer to the User data structure */
  /* SIM_SCGC6: FTFL=1 */
  SIM_SCGC6 |= SIM_SCGC6_FTFL_MASK;
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_FLASH1_ID,DeviceDataPrv);
  return DeviceDataPrv;
}

/*
** ===================================================================
**     Method      :  FLASH1_Write (component FLASH_LDD)
*/
/*!
**     @brief
**         This method sets up a flash memory write operation. The
**         operation itself is performing by defined batches (property
**         [Write batch size]) by periodical calling the componentís
**         Main method in the user application (higher level OS service)
**         or by the componentís ISR, if an componentís interrupt
**         service is enabled.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         FromPtr         - Pointer to data to be written to
**                           the flash memory.
**     @param
**         ToAddress       - Address to the flash memory
**                           the input data would be written to.
**     @param
**         Size            - Size of the input data (in bytes) to be
**                           written to the flash memory.
**     @return
**                         - Error code
**                           ERR_OK - OK
**                           ERR_DISABLED - Component is disabled
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_BUSY - Some flash memory operation is
**                           already in progress 
**                           ERR_PARAM_ADDRESS - Desired flash memory
**                           area is out of allowed range
**                           ERR_NOTAVAIL - When Safe launch and wait
**                           mode is enabled (property Safe launch and
**                           wait) and safe routine location is defined
**                           in runtime (property Safe routine location)
**                           and the safe routine location has not been
**                           specified yet (the SetSafeRoutineLocation
**                           method has not been used to define the
**                           location the safe routine will be copied to).
*/
/* ===================================================================*/
LDD_TError FLASH1_Write(LDD_TDeviceData *DeviceDataPtr, LDD_TData *FromPtr, LDD_FLASH_TAddress ToAddress, LDD_FLASH_TDataSize Size)
{
  FLASH1_TDeviceDataPtr DeviceDataPrv = (FLASH1_TDeviceDataPtr)DeviceDataPtr; /* Auxiliary variable - pointer to an internal state structure */

  if (!(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_IDLE) && /* If some operation is already in progress return error */\
     !(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_STOP) && \
     !(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_FAILED)) {
    return ERR_BUSY;
  }
  if (RangeCheck(ToAddress, Size) != (LDD_TError)ERR_OK) { /* Is the given flash memory area in the allowed range? */
    return ERR_PARAM_ADDRESS;          /* If no, return error */
  }
  /* Filling of the internal state structure */
  DeviceDataPrv->CurrentOperation = LDD_FLASH_WRITE; /* Set the current operation type */
  DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_START; /* Set the current operation status to START */
  DeviceDataPrv->CurrentCommand = LDD_FLASH_WRITE_LONG_WORD; /* Set the flash command to be proceed for the operation */
  DeviceDataPrv->DataCounter = Size;   /* Reset Data counter */
  DeviceDataPrv->CurrentDataPtr = (uint8_t *)FromPtr; /* Reset the "From pointer" for the operation */
  DeviceDataPrv->CurrentFlashAddress = ToAddress; /* Reset the "To address" for the operation */
  DeviceDataPrv->CurrentDataSize = 0U; /* Reset Current data size */
  return ERR_OK;                       /* Return with no error */
}

/*
** ===================================================================
**     Method      :  FLASH1_Read (component FLASH_LDD)
*/
/*!
**     @brief
**         This method sets up a flash memory read operation. The
**         operation itself is performing by defined batches (property
**         [Read batch size]) by periodical calling the componentís
**         Main method in the user application (higher level OS service).
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         FromAddress     - Address to the flash
**                           memory the output data would be read from.
**     @param
**         ToPtr           - Pointer to a location the data would
**                           be written to.
**     @param
**         Size            - Size of the data (in bytes)  to be read
**                           from the flash memory.
**     @return
**                         - Error code
**                           ERR_OK - OK
**                           ERR_DISABLED - Component is disabled
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_BUSY - Some flash memory operation is
**                           already in progress 
**                           ERR_PARAM_ADDRESS - Desired flash memory
**                           area is out of allowed range
*/
/* ===================================================================*/
LDD_TError FLASH1_Read(LDD_TDeviceData *DeviceDataPtr, LDD_FLASH_TAddress FromAddress, LDD_TData *ToPtr, LDD_FLASH_TDataSize Size)
{
  FLASH1_TDeviceDataPtr DeviceDataPrv = (FLASH1_TDeviceDataPtr)DeviceDataPtr; /* Auxiliary variable - pointer to an internal state structure */

  if (!(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_IDLE) && /* If some operation is already in progress return error */\
     !(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_STOP) && \
     !(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_FAILED)) {
    return ERR_BUSY;
  }
  if (RangeCheck(FromAddress, Size) != (LDD_TError)ERR_OK) {
    return ERR_PARAM_ADDRESS;
  }
  /* Filling of the internal state structure */
  DeviceDataPrv->CurrentOperation = LDD_FLASH_READ; /* Set the current operation type */
  DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_START; /* Set the current operation status to START */
  DeviceDataPrv->DataCounter = Size;   /* Reset Data counter */
  DeviceDataPrv->CurrentDataPtr = (uint8_t *)ToPtr; /* Reset the "To pointer" for the operation */
  DeviceDataPrv->CurrentFlashAddress = FromAddress; /* Reset the "From address" for the operation */
  DeviceDataPrv->CurrentDataSize = 1U; /* Set the current data size */
  return ERR_OK;                       /* Return with no error */
}

/*
** ===================================================================
**     Method      :  FLASH1_Erase (component FLASH_LDD)
*/
/*!
**     @brief
**         This method sets up a flash memory erase operation. The
**         operation itself is performing by defined batches (property
**         [Erase batch size]) by periodical calling the componentís
**         Main method in the user application (higher level OS service)
**         or by the componentís ISR, if an componentís interrupt
**         service is enabled.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         FromAddress     - Address of the flash
**                           memory area (the first erase sector is the
**                           sector the given address belongs to) to be
**                           erased.
**     @param
**         Size            - Size of the flash memory area (in bytes)
**                           to be erased. The flash memory is erased by
**                           the erase sectors. The first erased sector
**                           is a sector the address specified by the
**                           input parameter Address belongs to. The
**                           last erased sector is a sector the address
**                           calculated like an addition of the address
**                           specified by the input parameter Address
**                           and the size specified by the input
**                           parameter Size belongs to.
**     @return
**                         - Error code
**                           ERR_OK - OK
**                           ERR_DISABLED - Component is disabled
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_BUSY - Some flash memory operation is
**                           already in progress 
**                           ERR_PARAM_ADDRESS - Desired flash memory
**                           area is out of allowed range or is not
**                           aligned to erasable units' bounderies
**                           ERR_NOTAVAIL - When Safe launch and wait
**                           mode is enabled (property Safe launch and
**                           wait) and safe routine location is defined
**                           in runtime (property Safe routine location)
**                           and the safe routine location has not been
**                           specified yet (the SetSafeRoutineLocation
**                           method has not been used to define the
**                           location the safe routine will be copied to).
*/
/* ===================================================================*/

LDD_TError FLASH1_Erase(LDD_TDeviceData *DeviceDataPtr, LDD_FLASH_TAddress FromAddress, LDD_FLASH_TDataSize Size)
{
  FLASH1_TDeviceDataPtr DeviceDataPrv = (FLASH1_TDeviceDataPtr)DeviceDataPtr; /* Auxiliary variable - pointer to an internal state structure */

  if (!(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_IDLE) && /* If some operation is already in progress return error. */\
     !(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_STOP) && \
     !(DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_FAILED)) {
    return ERR_BUSY;
  }
  if (RangeCheck(FromAddress, Size) != (LDD_TError)ERR_OK) { /* Is an address range of desired operation out of used flash memory areas? */
    return ERR_PARAM_ADDRESS;          /* If yes, return error. */
  }
  if (((FromAddress & FLASH1_ERASABLE_UNIT_MASK) != 0U) || ((Size & FLASH1_ERASABLE_UNIT_MASK) != 0U)){ /* Is the desired area misaligned to erasable unit borders? */
    return ERR_PARAM_ADDRESS;          /* If yes, return error. */
  }
  /* Filling of the internal state structure */
  DeviceDataPrv->CurrentOperation = LDD_FLASH_ERASE; /* Set the current operation type */
  DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_START; /* Set the current operation status to START */
  DeviceDataPrv->DataCounter = Size;   /* Reset Data counter */
  DeviceDataPrv->CurrentDataPtr = NULL; /* Reset the "Data pointer" for the operation */
  DeviceDataPrv->CurrentFlashAddress = FromAddress; /* Reset the "From address" for the operation */
  DeviceDataPrv->CurrentDataSize = FLASH1_ERASABLE_UNIT_SIZE; /* Set the current data size */
  DeviceDataPrv->CurrentCommand = LDD_FLASH_ERASE_SECTOR; /* Set the current operation flash command */
  return ERR_OK;                       /* Return with no error */
}

/*
** ===================================================================
**     Method      :  FLASH1_Main (component FLASH_LDD)
*/
/*!
**     @brief
**         This method is used to perform one batch of a flash memory
**         operation. This method is used to perform batches of all
**         flash memory operations (Write, Read, Erase, EraseBlock,
**         VerifyErasedBlock) when the component works in the polled
**         mode (interrupt service is disabled - property [Interrupt
**         service]). This method performs batches of the read flash
**         memory operation, when the component works in the interrupt
**         triggered mode (interrupt service is enabled). This method
**         is enabled only if the component works in the polled mode or
**         if the Read method is enabled.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
*/
/* ===================================================================*/
#define FLEX_MEMORY_BASE_ADDRESS    0x10000000U
#define FLEX_MEMORY_ADDRESS_SWITCH  0x00800000U
#define UNLIMITED_BATCH_SIZE 0U
#define WRITE_BATCH_SIZE 1U
#define ERASE_BATCH_SIZE 1U
#define READ_BATCH_SIZE UNLIMITED_BATCH_SIZE
#define WRITABLE_UNIT_MASK 3U

void FLASH1_Main(LDD_TDeviceData *DeviceDataPtr)
{
  FLASH1_TDeviceDataPtr DeviceDataPrv = (FLASH1_TDeviceDataPtr)DeviceDataPtr; /* Auxiliary variable - pointer to an internal state structure */
  uint32_t DataToPrg = 0xFFFFFFFFLU;   /* Auxiliary variable - current data to be proceeded */
  uint32_t DataToPrgMask = 0U;
  uint32_t CurrentFlashPrgUnitData;    /* Current flash location content */
  uint8_t PrgBytesCount;               /* Auxiliary variable - Current data counter - number of data to be proceeded (in bytes) */
  uint8_t MaxPossiblePrgBytes = 0U;
  uint8_t DstAddrOffset = 0U;          /* Offset of the desired flash location to be written from the begging of the smallest writable unit the desired location belongs to */
  uint8_t i;
  LDD_FLASH_TAddress FlashPrgUnitAddr;
  uint16_t StepsOfBatch = 0U;          /* Number of batch cycles to be proceeded */
  bool NextBatchCycle;                 /* Unlimited batch flag */
  uint8_t CurrentFlags;                /* Auxiliary variable - current hw flags */
  LDD_FLASH_TAddress CurrentFlashAddress; /* Auxiliary variable - Current flash address */

  if ((DeviceDataPrv->CurrentOperationStatus != LDD_FLASH_RUNNING) && /* If there is not an operation in progress or pending then end */\
     (DeviceDataPrv->CurrentOperationStatus != LDD_FLASH_START) && \
     (DeviceDataPrv->CurrentOperationStatus != LDD_FLASH_STOP_REQ)) {
    return;
  }
  if ((FTFL_PDD_GetFlags(FTFL_BASE_PTR) & FTFL_PDD_COMMAND_COMPLETE) == 0U){ /* If there is some flash operation in progress then end */
    return;
  }
  if (DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_START) { /* Is this run the first run of the Main method in this operation? */
    FTFL_PDD_ClearFlags(FTFL_BASE_PTR, /* If yes, clear hw flags */
                        FTFL_PDD_READ_COLLISION_ERROR | \
                        FTFL_PDD_ACCESS_ERROR | \
                        FTFL_PDD_PROTECTION_VIOLATION);
    DeviceDataPrv->CurrentErrorFlags = 0U; /* Clear Current SW error flags */
  } else {                             /* If this is not the first run of the operation, checks the error flags */
    CurrentFlags = FTFL_PDD_GetFlags(FTFL_BASE_PTR);
    DeviceDataPrv->CurrentErrorFlags |= CurrentFlags & /* Save current hw flags */\
    (FTFL_PDD_READ_COLLISION_ERROR | \
     FTFL_PDD_ACCESS_ERROR | \
     FTFL_PDD_PROTECTION_VIOLATION);
    if (DeviceDataPrv->CurrentErrorFlags != 0U) { /* Has some error occurred? */
      DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_FAILED; /* If yes, set the operation state to FAILED */
      if (DeviceDataPrv->CurrentOperation != LDD_FLASH_READ) {
      }
      return;                          /* End */
    }
    if (DeviceDataPrv->DataCounter == 0U) { /* If all the data has been successfully proceeded, finish the operation */
      DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_IDLE; /* If yes, change the operation state to IDLE */
      if (DeviceDataPrv->CurrentOperation != LDD_FLASH_READ) {
      }
      FLASH1_OnOperationComplete(DeviceDataPrv->UserDataPtr); /* Invoke the OnOperationComplete event */
      return;                          /* End */
    }
  }
  switch (DeviceDataPrv->CurrentOperation) { /* Define the batch counter's initial value according to the current operation */
    case LDD_FLASH_WRITE:              /* The current operation is Write */
      StepsOfBatch = WRITE_BATCH_SIZE; /* Initialize the batch counter */
      break;                           /* Break of the case */
    case LDD_FLASH_ERASE:              /* The current operation is Erase */
      StepsOfBatch = ERASE_BATCH_SIZE; /* Initialize the batch counter */
      break;                           /* Break of the case */
    case LDD_FLASH_READ:               /* The current operation is Read */
      StepsOfBatch = READ_BATCH_SIZE;  /* Initialize the batch counter */
      break;                           /* Break of the case */
    default:
      break;
  }
  if (StepsOfBatch == UNLIMITED_BATCH_SIZE) { /* Is the unlimited batch of size selected for the current operation? */
    NextBatchCycle = TRUE;             /* If the unlimited batch of size is used, set the NextCycle force variable to TRUE */
  } else {
    NextBatchCycle = FALSE;            /* If the unlimited batch of size is used, set the NextCycle force variable to FALSE */
  }
  while ((StepsOfBatch > 0U) || (NextBatchCycle)) { /* Should another batch of the current operation be proceeded in this Main method processing? */
    CurrentFlags = FTFL_PDD_GetFlags(FTFL_BASE_PTR); /* Read current hardware flags */
    if ((CurrentFlags & (FTFL_PDD_READ_COLLISION_ERROR /* Check the error flags */\
                     | FTFL_PDD_ACCESS_ERROR \
                     | FTFL_PDD_PROTECTION_VIOLATION)) != 0U) {
      return;                          /* If there has some error occurred then end. The error will be analyzed in the next call of the Main method */
    }
    if (DeviceDataPrv->CurrentOperationStatus == LDD_FLASH_START) { /* Is this step the first step of the operation? */
      DeviceDataPrv->CurrentOperationStatus = LDD_FLASH_RUNNING; /* If yes, change Current operation status to RUNNING */
    } else {
      DeviceDataPrv->CurrentFlashAddress += DeviceDataPrv->CurrentDataSize; /* If no, define of an address of a step of the current operation (address of the first step of the operation is defined by a operational method, which defines the operation */
      if ((DeviceDataPrv->CurrentOperation == LDD_FLASH_READ) || (DeviceDataPrv->CurrentOperation == LDD_FLASH_WRITE)) {
        DeviceDataPrv->CurrentDataPtr += DeviceDataPrv->CurrentDataSize; /* Update of the Current data pointer */
      }
    }
    switch(DeviceDataPrv->CurrentOperation) { /* Perform needed actions for the next step of the operation (for the next flash operation command) according to the current operation type */
      case LDD_FLASH_READ:             /* Read operation */
        *DeviceDataPrv->CurrentDataPtr = *(uint8_t *)DeviceDataPrv->CurrentFlashAddress; /* Read data from flash */
        DeviceDataPrv->DataCounter--;  /* Update of the Data counter */
        break;
      case LDD_FLASH_ERASE:            /* Erase operation */
        DeviceDataPrv->DataCounter -= (uint32_t)(FLASH1_ERASABLE_UNIT_SIZE); /* Update of the Data counter */
        break;
      case LDD_FLASH_WRITE:
        DstAddrOffset = (uint8_t)DeviceDataPrv->CurrentFlashAddress & WRITABLE_UNIT_MASK; /* Compute the address of the writable unit */
        MaxPossiblePrgBytes = (WRITABLE_UNIT_MASK - DstAddrOffset) + 1U; /* Compute number of bytes from the destination address to the end of the writable unit */
        if (DeviceDataPrv->DataCounter < MaxPossiblePrgBytes) {
          PrgBytesCount = (uint8_t)DeviceDataPrv->DataCounter;
        } else {
          PrgBytesCount = MaxPossiblePrgBytes;
        }
        for (i = 0U; i < PrgBytesCount; i++)  {
          ((uint8_t *)(void *)&DataToPrg)[DstAddrOffset + i] = ((uint8_t *)(void *)(DeviceDataPrv->CurrentDataPtr))[i];
          ((uint8_t *)(void *)&DataToPrgMask)[DstAddrOffset + i] = 0xFFu;
        }
        FlashPrgUnitAddr = DeviceDataPrv->CurrentFlashAddress - DstAddrOffset;
        CurrentFlashPrgUnitData = *(uint32_t *)FlashPrgUnitAddr;
        if(((DataToPrg & DataToPrgMask) & (~CurrentFlashPrgUnitData)) > 0U) {
          DeviceDataPrv->CurrentErrorFlags |= LDD_FLASH_MULTIPLE_WRITE_ERROR;
          return;
        }
        DataToPrg = DataToPrg ^ (~CurrentFlashPrgUnitData);
        DeviceDataPrv->CurrentDataSize = PrgBytesCount;
        DeviceDataPrv->DataCounter -= PrgBytesCount;
        FTFL_PDD_WriteFCCOBLongWordData(FTFL_BASE_PTR, DataToPrg);
        break;
      default:
        break;
    } /* switch(DeviceDataPrv->CurrentOperation) */
    StepsOfBatch -= 1U;
    if (DeviceDataPrv->DataCounter == 0U) {
      NextBatchCycle = FALSE;
      StepsOfBatch = 0U;
    }
    if (DeviceDataPrv->CurrentOperation != LDD_FLASH_READ) {
      FTFL_PDD_SetFCCOBCommand(FTFL_BASE_PTR, DeviceDataPrv->CurrentCommand); /* Set the desired flash operation command */
      if (DeviceDataPrv->CurrentFlashAddress >= FTFE_FLEXNVM_START_ADDR) {
        CurrentFlashAddress = (LDD_FLASH_TAddress)((DeviceDataPrv->CurrentFlashAddress ^ FTFE_FLEXNVM_START_ADDR) | FTFE_FLEXNVM_CCOB_START_ADDR);
      } else {
        CurrentFlashAddress = DeviceDataPrv->CurrentFlashAddress;
      }
      FTFL_PDD_SetFCCOBAddress(FTFL_BASE_PTR, ((uint32_t)(CurrentFlashAddress - DstAddrOffset))); /* Set an address of the flash memory location for the current flash operation command */
      /* {FreeRTOS RTOS Adapter} Critical section begin (RTOS function call is defined by FreeRTOS RTOS Adapter property) */
      taskENTER_CRITICAL();
      ((LDD_FLASH_TSafeRoutinePtr)((uint32_t)(DeviceDataPrv->SafeRoutinePtr) | 1U))(); /* Run the Safe routine */
      /* {FreeRTOS RTOS Adapter} Critical section ends (RTOS function call is defined by FreeRTOS RTOS Adapter property) */
      taskEXIT_CRITICAL();
    }
  }
}

/*
** ===================================================================
**     Method      :  RangeCheck (component FLASH_LDD)
**
**     Description :
**         Range check internal routine. This routine is used by the 
**         component's method to test the input address range.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static LDD_TError RangeCheck(LDD_FLASH_TAddress Address, LDD_FLASH_TDataSize Size)
{
  if (Size != 0U) {
    if (Address > FLASH1_PFLASH_SIZE) {
      if ((Address < FLASH1_DFLASH_ADDRESS) || \
         (Address > (FLASH1_DFLASH_ADDRESS + (FLASH1_DFLASH_SIZE - 1U))) || \
         (Size > FLASH1_DFLASH_ADDRESS) || \
         (Address > (FLASH1_DFLASH_ADDRESS + (FLASH1_DFLASH_SIZE - Size)))) {
        return ERR_PARAM_ADDRESS;
      }
    } else {
      if ((Size > FLASH1_PFLASH_SIZE) || (Address > (FLASH1_PFLASH_SIZE - Size))) {
        return ERR_PARAM_ADDRESS;
      }
    }
  } else {
    return ERR_PARAM_ADDRESS;
  }
  return ERR_OK;
}

/* END FLASH1. */

#ifdef __cplusplus
}  /* extern "C" */
#endif

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

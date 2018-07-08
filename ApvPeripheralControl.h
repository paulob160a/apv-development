/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvPeripheralControl.h                                                     */
/* 04.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/*  - initialisation and setup of the chip environment for proper peripheral  */
/*    behaviour                                                               */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_PERIPHERAL_CONTROL_H_
#define _APV_PERIPHERAL_CONTROL_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sam3x8e.h>
#include "ApvUtilities.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

// The UART minimum baud rate for this project is defined here as 9600
#define APV_UART_BAUD_RATE_9600                ((uint32_t)9600)
// The UART always divides MCK by 16
#define APV_UART_MCK_FIXED_DIVIDE_16           ((uint32_t)16)

/******************************************************************************/
/* SAM3A Interrupt Priority Levels 0 { .. } 15 :                              */
/*  - these are applied to all system interrupts ( -12 { .. } -1 ) and all    */
/*    peripheral interrupts ( 0 { .. } 44 )                                   */
/* References: Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section   */
/*             10 - 4 p50 and Section 10-19 p152                              */
/******************************************************************************/

#define SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS     (4)
#define SAM3A_INTERRUPT_PRIORITY_LEVEL_MASK     ((1 << SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS) - 1)

#define SAM3A_INTERRUPT_PRIORITY_LEVEL_MINIMUM  ((1 << SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS) - 1)
#define SAM3A_INTERRUPT_PRIORITY_LEVEL_MAXIMUM  (0)

#define APV_DEVICE_INTERRUPT_PRIORITIES         SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS // used to pack priorities

#define APV_DEVICE_INTERRUPT_PRIORITY_RANGE     ((-(APV_SYSTEM_INTERRUPT_ID_MEMORY_MANAGEMENT_FAULT)) + APV_PERIPHERAL_IDS)   // -( -12 ) + 45  == 57 : 08.07.18
#define APV_DEVICE_INTERRUPT_PRIORITIES_PACKED  ((APV_DEVICE_INTERRUPT_PRIORITY_RANGE / APV_DEVICE_INTERRUPT_PRIORITIES) + 1) // ( 57 / 4 ) + 1 == 15 : 08.07.18
 
/******************************************************************************/
/*                *** PROCESS INTERRUPT PRIORITY LEVELS ***                   */
/******************************************************************************/

#define APV_DEVICE_INTERRUPT_PRIORITY_BASE      APV_INTERRUPT_LEVEL_PRIORITY_MINIMUM

#define APV_DEVICE_INTERRUPT_PRIORITY_SYSTICK   APV_INTERRUPT_LEVEL_PRIORITY_14 // SysTick has elevated priority : 080718

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvUartControlAction_tTag
  {
  APV_UART_CONTROL_ACTION_RESET = 0,
  APV_UART_CONTROL_ACTION_ENABLE,
  APV_UART_CONTROL_ACTION_DISABLE,
  APV_UART_CONTROL_ACTION_RESET_STATUS,
  APV_UART_CONTROL_ACTIONS
  } apvUartControlAction_t;

typedef enum apvUartParity_tTag
  {
  APV_UART_PARITY_EVEN = 0,
  APV_UART_PARITY_ODD,
  APV_UART_PARITY_SPACE,
  APV_UART_PARITY_MARK,
  APV_UART_PARITY_NONE,
  APV_UART_PARITY_SET
  } apvUartParity_t;

typedef enum apvUartChannelMode_tTag
  {
  APV_UART_CHANNEL_MODE_NORMAL = 0,
  APV_UART_CHANNEL_MODE_AUTOMATIC,
  APV_UART_CHANNEL_MODE_LOCAL_LOOPBACK,
  APV_UART_CHANNEL_MODE_REMOTE_LOOPBACK,
  APV_UART_CHANNEL_MODE_SET
  } apvUartChannelMode_t;

typedef enum apvUartBaudRateSelection_tTag
  {
  APV_UART_BAUD_RATE_SELECT_9600 = 0,
  APV_UART_BAUD_RATE_SELECT_19200,
  APV_UART_BAUD_RATE_SELECT_38400,
  APV_UART_BAUD_RATE_SELECT_57600,
  APV_UART_BAUD_RATE_SELECT_76800,
  APV_UART_BAUD_RATE_SELECT_96000,
  APV_UART_BAUD_RATE_SELECT_152000,
  APV_UART_BAUD_RATE_SELECT_SET
  } apvUartBaudRateSelection_t;

typedef enum apvUartInterruptSelect_tTag
  {
  APV_UART_INTERRUPT_SELECT_TRANSMIT = 0,
  APV_UART_INTERRUPT_SELECT_RECEIVE,
  APV_UART_INTERRUPT_SELECT_DUPLEX,
  APV_UART_INTERRUPT_SELECT_SET
  } apvUartInterruptSelect_t;

typedef enum apvInterruptPriorityLevels_tTag
  {
  APV_INTERRUPT_LEVEL_PRIORITY_MAXIMUM = SAM3A_INTERRUPT_PRIORITY_LEVEL_MAXIMUM,
  APV_INTERRUPT_LEVEL_PRIORITY_0       = APV_INTERRUPT_LEVEL_PRIORITY_MAXIMUM,
  APV_INTERRUPT_LEVEL_PRIORITY_1,
  APV_INTERRUPT_LEVEL_PRIORITY_2,
  APV_INTERRUPT_LEVEL_PRIORITY_3,
  APV_INTERRUPT_LEVEL_PRIORITY_4,
  APV_INTERRUPT_LEVEL_PRIORITY_5,
  APV_INTERRUPT_LEVEL_PRIORITY_6,
  APV_INTERRUPT_LEVEL_PRIORITY_7,
  APV_INTERRUPT_LEVEL_PRIORITY_8,
  APV_INTERRUPT_LEVEL_PRIORITY_9,
  APV_INTERRUPT_LEVEL_PRIORITY_10,
  APV_INTERRUPT_LEVEL_PRIORITY_11,
  APV_INTERRUPT_LEVEL_PRIORITY_12,
  APV_INTERRUPT_LEVEL_PRIORITY_13,
  APV_INTERRUPT_LEVEL_PRIORITY_14,
  APV_INTERRUPT_LEVEL_PRIORITY_15,
  APV_INTERRUPT_LEVEL_PRIORITY_MINIMUM = SAM3A_INTERRUPT_PRIORITY_LEVEL_MINIMUM,
  APV_INTERRUPT_LEVEL_PRIORITIES       = (SAM3A_INTERRUPT_PRIORITY_LEVEL_MINIMUM + 1)
  } apvInterruptPriorityLevels_t;

/******************************************************************************/
/* To save a little memory peripheral interrupt priority levels are packed in */
/* blocks of 4-bits (priorities are 0 { .. } 15)                              */
/******************************************************************************/

typedef struct apvInterruptPriorities_tTag
  {
  uint16_t apvInterruptPriority0 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  uint16_t apvInterruptPriority1 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  uint16_t apvInterruptPriority2 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  uint16_t apvInterruptPriority3 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  } apvInterruptPriorities_t;

typedef union apvInterruptPriorityLevel_tTag
  {
  apvInterruptPriorities_t interruptPriorities;
  uint16_t                 deviceInterruptPriorities;
  } apvInterruptPriorityLevel_t;

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

extern          Pmc                         ApvPeripheralControlBlock;                                       // shadow peripheral control block
extern          Pmc                        *ApvPeripheralControlBlock_p;                                     // physical block address

extern          Uart                         ApvUartControlBlock;                                            // shadow UART control block
extern volatile Uart                        *ApvUartControlBlock_p;                                          // physical block address

extern          apvInterruptPriorityLevel_t  apvInterruptPriorities[APV_DEVICE_INTERRUPT_PRIORITIES_PACKED]; // shadow device interrupt priorities

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvSwitchPeripheralClock(apvPeripheralId_t peripheralId,
                                               bool              peripheralSwitch);
extern APV_ERROR_CODE apvSwitchPeripheralLines(apvPeripheralId_t peripheralLineId,
                                               bool              peripheralLineSwitch);
extern APV_ERROR_CODE apvSwitchNvicDeviceIrq(apvPeripheralId_t peripheralIrqId,
                                             bool              peripheralIrqSwitch);
extern APV_ERROR_CODE apvConfigureUart(apvUartParity_t            uartParity,
                                       apvUartChannelMode_t       uartChannelMode,
                                       apvUartBaudRateSelection_t uartBaudRate);
extern APV_ERROR_CODE apvControlUart(apvUartControlAction_t uartControlAction);
extern APV_ERROR_CODE apvUartCharacterTransmit(uint8_t transmitBuffer);
extern APV_ERROR_CODE apvUartCharacterReceive(uint8_t *receiveBuffer);
extern APV_ERROR_CODE apvUartSwitchInterrupt(apvUartInterruptSelect_t interruptSelect,
                                             bool                     interruptSwitch);
extern APV_ERROR_CODE apvGetInterruptPriority(int16_t  interruptSource,
                                              uint8_t *interruptPriority);
extern APV_ERROR_CODE apvSetInterruptPriority(int16_t                      interruptSourceNumber,
                                              uint8_t                      interruptPriority,
                                              apvInterruptPriorityLevel_t *deviceInterruptPriorities);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

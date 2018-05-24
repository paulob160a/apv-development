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
#define APV_UART_BAUD_RATE_9600      ((uint32_t)9600)
// The UART always divides MCK by 16
#define APV_UART_MCK_FIXED_DIVIDE_16 ((uint32_t)16)

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

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

extern          Pmc  ApvPeripheralControlBlock;                                   // shadow peripheral control block
extern          Pmc *ApvPeripheralControlBlock_p;                                 // physical block address

extern          Uart  ApvUartControlBlock;                                        // shadow UART control block
extern volatile Uart *ApvUartControlBlock_p;                                      // physical block address

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

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

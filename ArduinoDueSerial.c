/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvSerial.c                                                                */
/* 19.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - manager for the board serial communications                              */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "sam3x8e.h"
#include "ApvSerial.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/
/******************************************************************************/
/* Static Variables :                                                         */
/******************************************************************************/

static bool apvSerialCommsManagerAssigned                  = false;
static void (*apvPrimarySerialCommsInterruptHandler)(void) = NULL;

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvSerialCommsManager() :                                                  */
/*  --> apvPrimarySerialPort : serial port serving as primary                 */
/*  <-- apvErrorCode         : error codes                                    */
/*                                                                            */
/*  - responsible for assigning and setting up the serial port handlers       */
/******************************************************************************/

APV_ERROR_CODE apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort)
  {
/******************************************************************************/

  APV_ERROR_CODE apvErrorCode = APV_ERROR_CODE_NONE;

/******************************************************************************/

  // Check to see if the primary serial comms has already been assigned
  if (apvSerialCommsManagerAssigned == true)
    {
    apvErrorCode = APV_ERROR_CODE_PRIMARY_SERIAL_PORT_ALREADY_ASSIGNED;
    }
  else
    {
    apvSerialCommsManagerAssigned = true;

    switch(apvPrimarySerialPort)
      {
      case APV_PRIMARY_SERIAL_PORT_USART0 : // NOT IMPLEMENTED
                                            break;
      case APV_PRIMARY_SERIAL_PORT_USART1 : // NOT IMPLEMENTED
                                            break;
      case APV_PRIMARY_SERIAL_PORT_USART2 : // NOT IMPLEMENTED
                                            break;
      case APV_PRIMARY_SERIAL_PORT_USART3 : // NOT IMPLEMENTED
                                            break;
      case APV_PRIMARY_SERIAL_PORT_UART   : // THIS IS THE DEFAULT CHOICE
      default                             : apvPrimarySerialCommsInterruptHandler = UART_Handler;
                                            break;
      }
    }

/******************************************************************************/

  return(apvErrorCode);

/******************************************************************************/
  } /* end of apvSerialCommsManager                                           */

/******************************************************************************/
/* apvPrimarySerialCommsHandler() :                                           */
/*  --> apvPrimarySerialPort : serial port serving as primary                 */
/******************************************************************************/

void apvPrimarySerialCommsHandler(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvPrimarySerialCommsHandler                                    */

/******************************************************************************/
/* UART Handler :                                                             */
/*  - UART interrupt handler (replaces the "weak" default definition          */
/******************************************************************************/

void UART_Handler(void)
  {
/******************************************************************************/

  // What comms protocol is the UART supporting ?
  if (apvPrimarySerialCommsInterruptHandler == UART_Handler)
    {
    // This is the primary serial comms handler
    apvPrimarySerialCommsHandler(APV_PRIMARY_SERIAL_PORT_UART);
    }

/******************************************************************************/
  } /* end of UART_Handler                                                    */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
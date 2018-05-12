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
#include "ApvError.h"
#include "ApvSerial.h"
#include "ApvPeripheralControl.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/
/******************************************************************************/
/* Static Variables :                                                         */
/******************************************************************************/

static    bool  apvSerialCommsManagerAssigned                  = false;
static    void  (*apvPrimarySerialCommsInterruptHandler)(void) = NULL;
 
/******************************************************************************/
/* These variables are only intended to implement a simple foreground/back-   */
/* ground loopback                                                            */
/******************************************************************************/

volatile apvSerialTransmitBuffer_t  transmitBuffer,
                                   *transmitBuffer_p = &transmitBuffer;

volatile uint8_t                   receiveBuffer     = 0;
  
volatile bool                      transmitInterrupt = false,
                                   receiveInterrupt  = false;

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvSerialBufferInitialise() :                                              */
/*  --> serialBuffer         : pointer to a simple serial buffer structure    */
/*  --> serialBufferLength   : buffer length 0 > <length> <= MAX_LENGTH       */
/*  --> transmitPhrase       : optional buffer message preload                */
/*  --> transmitPhraseLength : optional buffer message preload length         */
/*  <-- serialError          : error codes                                    */
/*                                                                            */
/*  - initialise a simple serial buffer structure                             */
/*                                                                            */
/******************************************************************************/

APV_SERIAL_ERROR_CODE apvSerialBufferInitialise(volatile apvSerialTransmitBuffer_t *serialBuffer,
                                                         uint16_t                   serialBufferLength,
                                                const    char                      *transmitPhrase,
                                                         uint16_t                   transmitPhraseLength)
  {
/******************************************************************************/

  APV_SERIAL_ERROR_CODE serialError = APV_SERIAL_ERROR_CODE_NONE;
  uint16_t              bufferIndex = 0;

/******************************************************************************/

  if (serialBuffer == NULL)
    {
    serialError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if ((serialBufferLength == 0) || (serialBufferLength > APV_SERIAL_BUFFER_MAXIMUM_LENGTH))
      {
      serialError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      { // If an optional transmit phrase has been passed, load it into the transmit buffer
      if ((transmitPhrase != NULL) && (transmitPhraseLength > 0))
        {
        if (transmitPhraseLength > serialBufferLength)
          {
          transmitPhraseLength = serialBufferLength;
          }

         serialBuffer->serialTransmitMessageLength = transmitPhraseLength;

         do
          {
          serialBuffer->serialTransmitBuffer[transmitPhraseLength - 1] = *(transmitPhrase + (transmitPhraseLength - 1));
          transmitPhraseLength = transmitPhraseLength - 1;
          }
        while (transmitPhraseLength > 0);
        }
      else
        {
        for (bufferIndex = 0; bufferIndex < serialBufferLength; bufferIndex++)
          {
          serialBuffer->serialTransmitBuffer[bufferIndex] = 0;
          }
        }

      serialBuffer->serialTransmitBufferLength = serialBufferLength;
      serialBuffer->serialTransmitBufferIndex  = 0;
      }
    }

/******************************************************************************/

  return(serialError);

/******************************************************************************/
  } /* end of apvSerialBufferInitialise                                       */

/******************************************************************************/
/* apvSerialCommsManager() :                                                  */
/*  --> apvPrimarySerialPort : serial port serving as primary                 */
/*  <-- apvErrorCode         : error codes                                    */
/*                                                                            */
/*  - responsible for assigning and setting up the serial port handlers       */
/******************************************************************************/

APV_SERIAL_ERROR_CODE apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort)
  {
/******************************************************************************/

  APV_SERIAL_ERROR_CODE apvErrorCode = APV_SERIAL_ERROR_CODE_NONE;

/******************************************************************************/

  // Check to see if the primary serial comms has already been assigned
  if (apvSerialCommsManagerAssigned == true)
    {
    apvErrorCode = APV_SERIAL_ERROR_CODE_PRIMARY_SERIAL_PORT_ALREADY_ASSIGNED;
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
      default                             : apvPrimarySerialCommsInterruptHandler = UART_Handler; // attach the UART handler to the primary serial port
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

  uint32_t statusRegister = 0;

/******************************************************************************/

  if (apvPrimarySerialPort == APV_PRIMARY_SERIAL_PORT_UART)
    {
    statusRegister = ApvUartControlBlock_p->UART_SR;

    if ((statusRegister & UART_SR_RXRDY) == UART_SR_RXRDY)
      {
      // Read the new character
      apvUartCharacterReceive(&receiveBuffer);

      receiveInterrupt = true;
      }
    else
      {
      if ((statusRegister & UART_SR_TXEMPTY) && (statusRegister & UART_SR_TXRDY))
        {
        // Send the next character in the buffer
        ApvUartControlBlock_p->UART_THR = transmitBuffer_p->serialTransmitBuffer[transmitBuffer_p->serialTransmitBufferIndex];
        ApvUartControlBlock.UART_THR    = transmitBuffer_p->serialTransmitBuffer[transmitBuffer_p->serialTransmitBufferIndex];

        if (transmitBuffer_p->serialTransmitBufferIndex < transmitBuffer_p->serialTransmitMessageLength)
          { 
          transmitBuffer_p->serialTransmitBufferIndex = transmitBuffer_p->serialTransmitBufferIndex + 1;
          transmitInterrupt                           = true;
          }
        else
          { // No more characters, shut down the transmit interrupt
          transmitBuffer_p->serialTransmitBufferIndex = 0;
                    
          apvUartSwitchInterrupt(APV_UART_INTERRUPT_SELECT_TRANSMIT,
                                 false);

          transmitInterrupt = false;
          }
        }
      }
    }

/******************************************************************************/
  } /* end of apvPrimarySerialCommsHandler                                    */

/******************************************************************************/
/* UART Handler :                                                             */
/*  - UART interrupt handler (replaces the "weak" default definition)         */
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

  NVIC_ClearPendingIRQ(UART_IRQn);

/******************************************************************************/
  } /* end of UART_Handler                                                    */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
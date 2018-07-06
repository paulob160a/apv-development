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
#include "ApvUtilities.h"
#include "ApvError.h"
#include "ApvSerial.h"
#include "ApvPeripheralControl.h"
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/
/* These variables control the primary serial receive port buffering          */
/******************************************************************************/
/* 27.06.18 : this can be removed, better handled by a ring-buffer of         */
/*            pointers. This may nit be needed in it's entirety               */
/******************************************************************************/

// This is the ring-buffer "free-list"
apvRingBuffer_t   apvSerialPortPrimaryRingBuffer[APV_PRIMARY_SERIAL_RING_BUFFER_SET];
apvRingBuffer_t  *apvSerialPortPrimaryRingBuffer_p[APV_PRIMARY_SERIAL_RING_BUFFER_SET] = 
  {
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER,
  APV_RING_BUFFER_LIST_EMPTY_POINTER
  };

/******************************************************************************/

// This is the transmit ring-buffer of pointers to a set of ring-buffers
apvRingBuffer_t   apvUartPortTransmitBuffer,
                 *apvUartPortPrimaryTransmitRingBuffer_p = &apvUartPortTransmitBuffer;
// This is the receive ring-buffer of pointers to a set of ring-buffers
apvRingBuffer_t   apvUartPortReceiveBuffer,
                 *apvUartPortPrimaryReceiveRingBuffer_p = &apvUartPortReceiveBuffer;

apvRingBuffer_t *apvPrimarySerialCommsTransferBuffer = NULL;
apvRingBuffer_t *apvPrimarySerialCommsReceiveBuffer  = NULL;
apvRingBuffer_t *apvPrimarySerialCommsTransmitBuffer = NULL;

uint8_t          apvPrimarySerialBufferIndex         = APV_PRIMARY_SERIAL_RING_BUFFER_0;

/******************************************************************************/
/* Static Variable Definitions :                                              */
/******************************************************************************/

static    bool  apvSerialCommsManagerAssigned                  = false;
static    void  (*apvPrimarySerialCommsInterruptHandler)(void) = NULL;
 
/******************************************************************************/
/* These variables are only intended to implement a simple foreground/back-   */
/* ground loopback                                                            */
/******************************************************************************/

volatile apvSerialTransmitBuffer_t  transmitBuffer,
                                   *transmitBuffer_p = &transmitBuffer;

volatile uint8_t                   receiveBuffer     = 0,
                                   transmitterBuffer = 0;
  
volatile bool                      transmitInterrupt        = false,
                                   transmitInterruptTrigger = false,
                                   receiveInterrupt         = false;

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
/*  --> apvPrimarySerialPort                    :                             */
/*                           serial port serving as primary                   */
/*  --> apvPrimarySerialPortReceiverRingBuffers :                             */
/*                           the number of receive ring-buffers to assign and */
/*                           initialise                                       */
/*  <-- apvErrorCode                            : error codes                 */
/*                                                                            */
/*  - responsible for assigning and setting up the serial port handlers       */
/*                                                                            */
/******************************************************************************/

APV_SERIAL_ERROR_CODE apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT   apvPrimarySerialPort,
                                            apvPrimaryRingBufferSet_t apvPrimarySerialPortReceiverRingBuffers)
  {
/******************************************************************************/

  APV_SERIAL_ERROR_CODE     apvErrorCode    = APV_SERIAL_ERROR_CODE_NONE;

  apvPrimaryRingBufferSet_t apvBufferIndex = 0;

/******************************************************************************/

  // Check to see if the primary serial comms has already been assigned
  if (apvSerialCommsManagerAssigned == true)
    {
    apvErrorCode = APV_SERIAL_ERROR_CODE_PRIMARY_SERIAL_PORT_ALREADY_ASSIGNED;
    }
  else
    {
    if ((apvPrimarySerialPortReceiverRingBuffers == 0) || (apvPrimarySerialPortReceiverRingBuffers > APV_PRIMARY_SERIAL_RING_BUFFER_SET))
      {
      apvErrorCode = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      {
      apvSerialCommsManagerAssigned = true;

      // Create the "free" ring-buffer set for this port
      apvErrorCode = apvRingBufferSetInitialise(&apvSerialPortPrimaryRingBuffer_p[apvBufferIndex],
                                                &apvSerialPortPrimaryRingBuffer[apvBufferIndex],
                                                 APV_PRIMARY_SERIAL_RING_BUFFER_SET,
                                                 APV_SERIAL_BUFFER_MAXIMUM_LENGTH);

      if (apvErrorCode == APV_ERROR_CODE_NONE)
        {
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
          default                             : apvPrimarySerialCommsInterruptHandler =  UART_Handler;                                                     // attach the UART handler to the primary serial port

                                                // Get a "free" ring buffer for the receiver
                                                if (apvRingBufferSetPullBuffer(&apvSerialPortPrimaryRingBuffer_p[0],
                                                                               &apvPrimarySerialCommsReceiveBuffer,
                                                                                APV_PRIMARY_SERIAL_RING_BUFFER_SET,
                                                                                false                               ) != APV_ERROR_CODE_NONE)
                                                  {
                                                  apvErrorCode = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                                  }
                                                else
                                                  {
                                                  if (apvPrimarySerialCommsReceiveBuffer == APV_RING_BUFFER_LIST_EMPTY_POINTER)
                                                    {
                                                    apvErrorCode = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
                                                    }
                                                  else
                                                    {
                                                    // ...and a "free" ring buffer for the transmitter
                                                    if (apvRingBufferSetPullBuffer(&apvSerialPortPrimaryRingBuffer_p[0],
                                                                                   &apvPrimarySerialCommsTransmitBuffer,
                                                                                    APV_PRIMARY_SERIAL_RING_BUFFER_SET,
                                                                                    false                               ) != APV_ERROR_CODE_NONE)
                                                      {
                                                      apvErrorCode = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                                      }
                                                    else
                                                      {
                                                      if (apvPrimarySerialCommsReceiveBuffer == APV_RING_BUFFER_LIST_EMPTY_POINTER)
                                                        {
                                                        apvErrorCode = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
                                                        }
                                                      else
                                                        {
                                                        // ...and a "free" ring buffer for the transfer buffer
                                                        if (apvRingBufferSetPullBuffer(&apvSerialPortPrimaryRingBuffer_p[0],
                                                                                       &apvPrimarySerialCommsTransferBuffer,
                                                                                        APV_PRIMARY_SERIAL_RING_BUFFER_SET,
                                                                                        false                               ) != APV_ERROR_CODE_NONE)
                                                          {
                                                          apvErrorCode = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                                          }
                                                        else
                                                          {
                                                          if (apvPrimarySerialCommsReceiveBuffer == APV_RING_BUFFER_LIST_EMPTY_POINTER)
                                                            {
                                                            apvErrorCode = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
                                                            }
                                                          else
                                                            {
        
                                                            }
							                                                   }
						                                               	  }
						                                                }
                                                    }
                                                  }

                                                break;
          }
        }
      }
    }

/******************************************************************************/

  return(apvErrorCode);

/******************************************************************************/
  } /* end of apvSerialCommsManager                                           */

/******************************************************************************/
/* apvPrimarySerialCommsHandler() :                                           */
/*  --> apvPrimarySerialPort : serial port serving as primary                 */
/*                                                                            */
/* - handle the serial UART transmit and receive interrupts. Serial I/O is a  */
/*   per-character operation where the received characters are loaded onto a  */
/*   known ring-buffer for consumption by higher processing layers and those  */
/*   layers load a known ring-buffer in the opposite direction for transmit.  */
/*   Currently the ring-buffers have a native element of 32-bits to allow the */
/*   storage of 1, 2, 3 and 4-byte values. Hence all operations need to be    */
/*   four-byte aligned when pushing and pulling values                        */
/*                                                                            */
/******************************************************************************/

void apvPrimarySerialCommsHandler(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort)
  {
/******************************************************************************/

   uint32_t statusRegister = 0;
   uint32_t txRxBuffer     = 0;

/******************************************************************************/

  if (apvPrimarySerialPort == APV_PRIMARY_SERIAL_PORT_UART)
    {
    statusRegister = ApvUartControlBlock_p->UART_SR;

    if ((statusRegister & UART_SR_RXRDY) == UART_SR_RXRDY)
      {
      apvInterruptCounters[APV_RECEIVE_INTERRUPT_COUNTER] = apvInterruptCounters[APV_RECEIVE_INTERRUPT_COUNTER] + 1;

      // Read the new character
      apvUartCharacterReceive((uint8_t *)&txRxBuffer);

      // Put it into the current receiver ring buffer if there is room
      if (apvRingBufferLoad( apvPrimarySerialCommsReceiveBuffer,
                             APV_RING_BUFFER_TOKEN_TYPE_LONG_WORD,
                            (uint32_t *)&txRxBuffer,
                             sizeof(uint8_t),
                             false) == 0)
        {
        while (true)
          ;
        }
      }

    if (((statusRegister & UART_SR_TXEMPTY) == UART_SR_TXEMPTY) && 
        ((statusRegister & UART_SR_TXRDY)   == UART_SR_TXRDY)   &&
        (transmitInterrupt                  == true))
      {
      apvInterruptCounters[APV_TRANSMIT_INTERRUPT_COUNTER] = apvInterruptCounters[APV_TRANSMIT_INTERRUPT_COUNTER] + 1;

      // Send the next character if one exists
      if (apvRingBufferUnLoad( apvPrimarySerialCommsTransmitBuffer,
                               APV_RING_BUFFER_TOKEN_TYPE_LONG_WORD,
                              (uint32_t *)&txRxBuffer,
                               sizeof(uint8_t),
                               false) != 0)
        {
        ApvUartControlBlock_p->UART_THR = (uint8_t)txRxBuffer;
        ApvUartControlBlock.UART_THR    = (uint8_t)txRxBuffer;
        }
      else
        {
        transmitInterrupt = false;

        // No transmit characters left - shut down the transmit interrupt
        ApvUartControlBlock_p->UART_IDR = UART_IDR_TXRDY;
        ApvUartControlBlock.UART_IDR    = UART_IDR_TXRDY;
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

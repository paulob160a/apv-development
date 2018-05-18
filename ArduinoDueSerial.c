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
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/
/* These variables control the primary serial receive port buffering          */
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

// This is the transmit ring-buffer of pointers to a set of (received) ring-buffers
apvRingBuffer_t   apvSerialPortTransmitBuffer,
                 *apvSerialPortTransmitBuffer_p = &apvSerialPortTransmitBuffer;

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
                                                    // Create the small ring-buffer transmit queue (of pointers to ring-buffers)
                                                    apvErrorCode = apvRingBufferInitialise(apvSerialPortTransmitBuffer_p,
                                                                                           APV_PRIMARY_SERIAL_RING_BUFFER_SET);
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
/******************************************************************************/

void apvPrimarySerialCommsHandler(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort)
  {
/******************************************************************************/

  uint32_t statusRegister = 0;
  uint16_t receivedTokens = 0,
           transmitTokens = 0;

/******************************************************************************/

  if (apvPrimarySerialPort == APV_PRIMARY_SERIAL_PORT_UART)
    {
    statusRegister = ApvUartControlBlock_p->UART_SR;

    if ((statusRegister & UART_SR_RXRDY) == UART_SR_RXRDY)
      {
      // Read the new character
      apvUartCharacterReceive(&receiveBuffer);

      // Put it into the current receiver ring buffer
      receivedTokens = apvRingBufferLoad( apvPrimarySerialCommsReceiveBuffer,
                                         (uint32_t *)&receiveBuffer,
                                          sizeof(uint8_t),
                                          false);

      if (receivedTokens == 0)
        { 
        // A full buffer has been received - put the current receive buffer address on the transmit queue
        apvRingBufferLoad( apvSerialPortTransmitBuffer_p,
                          (uint32_t *)&apvPrimarySerialCommsReceiveBuffer,
                           sizeof(uint8_t),
                           false);

        // Signal the transmitter that new characters are ready
        transmitInterrupt = true;

        // Pull a new ring-buffer from the "free" list
        if (apvRingBufferSetPullBuffer(&apvSerialPortPrimaryRingBuffer_p[0],
                                     &apvPrimarySerialCommsReceiveBuffer,
                                      APV_PRIMARY_SERIAL_RING_BUFFER_SET,
                                      false) != APV_RING_BUFFER_LIST_EMPTY_POINTER)
          {
          // Now save the latest received character
          receivedTokens = apvRingBufferLoad( apvPrimarySerialCommsReceiveBuffer,
                                            (uint32_t *)&receiveBuffer,
                                              sizeof(uint8_t),
                                              false);

          receiveInterrupt = true;
          }
        else
          { // This is not a situation that lends itself to a happy ending - switch off the Rx interrupt!
          ApvUartControlBlock_p->UART_IDR = UART_IDR_RXRDY;
          ApvUartControlBlock.UART_IDR    = UART_IDR_RXRDY;

          receiveInterrupt = false;
          }
        }
      else
        {
        receiveInterrupt = true;
        }
      }

#ifdef _APV_UART_TEST_MODE_0
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
#endif
    if (((statusRegister & UART_SR_TXEMPTY) == UART_SR_TXEMPTY) && ((statusRegister & UART_SR_TXRDY) == UART_SR_TXRDY))
      {
       //NEED BETTER BUFFER HANDLING!!! USED & FREE LISTS
      transmitTokens = apvRingBufferUnLoad( apvPrimarySerialCommsTransmitBuffer,
                                           &transmitterBuffer,
                                            sizeof(uint8_t),
                                            false);

      // Send the next character if one exists, otherwise shutdown the interrupt
      if (transmitTokens != 0)
        {
        ApvUartControlBlock_p->UART_THR = transmitterBuffer;
        ApvUartControlBlock.UART_THR    = transmitterBuffer;

        transmitInterrupt = true;
        }
      else
        { 
        apvUartSwitchInterrupt(APV_UART_INTERRUPT_SELECT_TRANSMIT,
                               false);

        transmitInterrupt = false;
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
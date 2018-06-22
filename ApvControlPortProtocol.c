/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvControlPortProtocol.c                                                   */
/* 26.05.18                                                                   */
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
#include "ApvControlPortProtocol.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

uint8_t apvSignOnMessage[APV_SIGN_ON_MESSAGE_MAXIMUM_LENGTH] = APV_SIGN_ON_MESSAGE;

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvControlPortSignOn() :                                                   */
/*                                                                            */
/*  --> apvSignOMessage        : pointer to the APV sign-on message           */
/*  --> apvSignOnMessageLength : APV sign-on message length                   */
/*  <-- signOnError            : error codes                                  */
/*                                                                            */
/* - sends a sign-on message via the primary serial port control protocol.    */
/*   This should only be used before the UART interrupt is switched on!       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvControlPortSignOn(uint8_t  *apvSignOMessage,
                                    uint16_t  apvSignOnMessageLength)
  {
/******************************************************************************/

  APV_ERROR_CODE signOnError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (apvSignOMessage == NULL)
    {
    signOnError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    // Get a transmit buffer from the free list
    if (apvRingBufferSetPullBuffer(&apvSerialPortPrimaryRingBuffer_p[0],
                                   &apvPrimarySerialCommsTransmitBuffer,
                                    APV_PRIMARY_SERIAL_RING_BUFFER_SET,
                                    false ) != APV_ERROR_CODE_NONE)
      {
      signOnError = APV_ERROR_CODE_CONFIGURATION_ERROR;
      }
    else
      {
      // Load the sign-on message onto the buffer
      while (apvSignOnMessageLength > 0)
        { // If the message is too long the extra characters will just be thrown away
        apvRingBufferLoad( apvPrimarySerialCommsTransmitBuffer,
                          (uint32_t *)apvSignOMessage,
                           sizeof(uint8_t),
                           false);

        apvSignOMessage        = apvSignOMessage        + 1;
        apvSignOnMessageLength = apvSignOnMessageLength - 1;
        }

      // Push the buffer onto the transmit buffer queue
      apvRingBufferLoad( apvUartPortPrimaryTransmitRingBuffer_p,
                        (uint32_t *)&apvPrimarySerialCommsTransmitBuffer,
                         sizeof(uint8_t),
                         false);

      // Signal the background loop
      transmitInterrupt = true;
      }
    }

/******************************************************************************/

  return(signOnError);

/******************************************************************************/
  } /* end of apvControlPortSignOn                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

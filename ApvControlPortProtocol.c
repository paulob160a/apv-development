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
#include <string.h>
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

uint8_t                        apvSignOnMessage[APV_SIGN_ON_MESSAGE_MAXIMUM_LENGTH] = APV_SIGN_ON_MESSAGE;

apvCommandProtocolDefinition_t apvCommandProtocol[APV_COMMAND_PROTOCOL_MESSAGE_DEFINITIONS] = 
  {
    {
      {
        {
        APV_COMMAND_PROTOCOL_FIELD_TYPE_TEXT,
          {
          APV_COMMAND_PROTOCOL_MESSAGE_COMMAND_SIGN_ON
          }
        }
      },
    APV_COMMAND_PROTOCOL_MESSAGE_RESPONSE_SIGN_ON,
    NULL
    }
  };

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
/* apvStringCompare() :                                                       */
/*  --> *templateString       : a string containing a token to find in a      */
/*                              comparison string                             */
/*  -->  templateStringOffset : starting point of the token to find           */
/*  -->  templateStringLength : length of the token to find                   */
/*  --> *examinedString       : a string to be searched for the token         */
/*  -->  examinedStringOffset : starting point of the string to be searched   */
/*  -->  matchingTerminator   : non-matching character terminates token IF    */
/*                              "matching magic" for a blind string is 'true' */
/*  -->  matchingMagic        : only test the examined string for ANY non-    */
/*                              terminating character                         */
/*  <--  stringEquivalence    : [ false == token not found | true == token    */
/*                                                                   found ]  */
/*                                                                            */
/* - a super-funky string comparison utility that can compare a token of a    */
/*   given length in a "template" string with an "examined" string. The token */
/*   can be offset from the chosen beginning-point of either or both strings. */
/*   In addition the utility can search for a "blind" match in the "examined" */
/*   string of the given length of any "template" string, terminating the     */
/*   search on success or a given terminating character.                      */
/*                                                                            */
/******************************************************************************/

bool apvStringCompare(char     *templateString,
                      uint16_t  templateStringOffset,
                      uint16_t  templateStringLength,
                      char     *examinedString,
                      uint16_t  examinedStringOffset,
                      char      matchingTerminator,
                      bool      matchingMagic)
  {
/******************************************************************************/

  bool     stringEquivalence       = false;

  uint16_t templateStringSubLength = 0,
           examinedStringSubLength = 0,
           stringComparisonCounter = 0;

/******************************************************************************/

  if ((templateString       != NULL) && (examinedString     != NULL) &&
      (templateStringLength != 0))
    {
    if (templateStringOffset < strlen(templateString))                             // template string offset is in the template string ?
      {
      templateStringSubLength = strlen(templateString) - templateStringOffset;     // remaining template string after the offset

      if (templateStringLength <= templateStringSubLength)                         // template string length is less than or equal to the remaining template string length ?
        {
        if (examinedStringOffset < strlen(examinedString))                         // examined string offset is in the examined string ?
          {
          examinedStringSubLength = strlen(examinedString) - examinedStringOffset; // remaining examined string after the offset

          if (templateStringLength <= examinedStringSubLength)                     // template string length is less than or equal to the remaining examined string length ?
            {
            // The token has room to exist in the template string and the examined string...
            templateString = templateString + templateStringOffset;
            examinedString = examinedString + examinedStringOffset;

            for (stringComparisonCounter = 0; stringComparisonCounter < templateStringLength; stringComparisonCounter++)
              {
              if (matchingMagic == true)
                {
                if ((*(examinedString + stringComparisonCounter)) == matchingTerminator)
                  {
                  break;
                  }
                }
              else
                {
                if ((*(templateString + stringComparisonCounter)) != (*(examinedString + stringComparisonCounter)))
                  {
                  break;
                  }
                }
              } // for (stringComparisonCounter...

            if (stringComparisonCounter == templateStringLength)
              {
              stringEquivalence = true;
              }
            }
          }
        }
      }
    }

/******************************************************************************/

  return(stringEquivalence);

/******************************************************************************/
  } /* end of apvStringCompare                                                */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

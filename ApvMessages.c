/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMessages.c                                                              */
/* 04.06.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - describes the message payload format and structure for all APV           */
/*   communication                                                            */
/*                                                                            */
/*   A complete message is built from a number of "fragments" the number and  */
/*   format of which are known at the transmit-side and receive-side. To keep */
/*   message-parsing as simple as possible each fragment is constrained to be */
/*   a complete field e.g. <prefix>, or <number> or <text> or... The first    */
/*   character in a fragment payload is the fragment number which serves to   */
/*   permit out-of-order message re-assembly (if a multi-message ARQ is used) */
/*   and as part of the message-receipt error-checking. The fragment number   */
/*   is mandatory and so no space is required between it and the following    */
/*   fragment data e.g. :                                                     */
/*                                                                            */
/*    <message-fragment-0> : 0xff                            - the Hadamard-  */
/*                                                             coded '0x00'   */
/*                           'R' 'q' 'S' 'i' 'g' 'n' 'O' 'n' - sign-on prefix */
/*                           ' '                             - redundant      */
/*                                                             white-space    */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include "ApvMessages.h"
#include "ApvMessageHandling.h"

/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/
/* Any message is built from a finite number of fragments and assembled into  */
/* a group of seperate messages for transmission/reception. The message can   */
/* be a set of constant-format components e.g. naming strings and variable-   */
/* format components e.g. variable numeric values. Note that variables are    */
/* loaded into the payload variable array in associated numeric order i.e.    */
/* (say) payload structure #4 is variable it's corresponding string must be   */
/* in variable array slot #4. Unused slots must be set to the empty string "" */
/******************************************************************************/

apvMessageStructure_t messagePayloadStructures[APV_MESSAGE_MAXIMUM_NUMBER_OF_FRAGMENTS];
char                  messagePayloadVariables[APV_MESSAGE_MAXIMUM_NUMBER_OF_FRAGMENTS][APV_MESSAGE_MAXIMUM_FRAGMENT_LENGTH + 1]; // variable plus string-ending

/******************************************************************************/
/* Local Variable Declarations :                                              */
/******************************************************************************/

// Command : the terminating message server to return the device "sign-on" message
static const apvMessageFormat_t apvMessageRequestSignOn = 
  {
  APV_MESSAGE_COMMAND_RESPONSE_TYPE,
  { 
    { 
    APV_MESSAGE_REQUEST_SIGN_ON_PREFIX,
    APV_MESSAGE_FRAGMENT_TYPE_PREFIX,
    APV_MESSAGE_FRAGMENT_FIX_STATIC,
    APV_MESSAGE_FRAGMENT_INDEX_0 
    } 
  },
  APV_MESSAGE_REQUEST_SIGN_ON_FRAGMENTS,
  NULL,
  NULL
  };

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvMessagePayloadAssembly() :                                              */
/*  --> messageFormat           : defining format for this message            */
/*  --> messageStructure        : the message fragments will be built here    */
/*  --> messageFragmentContents : the VARIABLE components to build into the   */
/*                                matching fragment. These must already be in */
/*                                string format                               */
/*  <-- payloadAssemblyError    : error codes                                 */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvMessagePayloadAssembly(apvMessageFormat_t     *messageFormat,
                                         apvMessageStructure_t **messageStructure,
                                         char                  **messageFragmentContents)
  {
/******************************************************************************/

  APV_ERROR_CODE payloadAssemblyError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((messageFormat == NULL) || (messageStructure == NULL) || (*messageStructure == NULL) || (messageFragmentContents == NULL))
    {
    payloadAssemblyError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    }

/******************************************************************************/

  return(payloadAssemblyError);

/******************************************************************************/
  } /* end of apvMessagePayloadAssembly                                       */

/******************************************************************************/
/* apvUniToBiPolarConvert() :                                                 */
/*  -->  uniPolar       : 8-bit unipolar code as [ '0' | '1' ]                */
/*  <--> biPolar        : 8-bit bipolar code  as [ '1' | '-1' ]               */
/*  <--  polarErrorCode : error codes                                         */
/*                                                                            */
/* - convert an unsinged 8-bit integer representing an 8-bit Hadamard code to */
/*   a fully polar 8-bit Hadamard code                                        */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUniToBiPolarConvert(uint8_t           uniPolar,
                                      apvBiPolarCode_t *biPolar)
  {
/******************************************************************************/

  APV_ERROR_CODE polarErrorCode = APV_ERROR_CODE_NONE;
  uint8_t        polarIndex     = APV_BIPOLAR_CODE_BITS;

/******************************************************************************/

  if (biPolar == NULL)
    {
    polarErrorCode = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    do
      { // In reverse order, convert the unipolar value to a bipolar value
      polarIndex = polarIndex - 1;

      biPolar->apvBiPolarCodeBits[polarIndex] = uniPolar & (APV_BIPOLAR_CODE_MASK << polarIndex);

      biPolar->apvBiPolarCodeBits[polarIndex] = (biPolar->apvBiPolarCodeBits[polarIndex] == APV_POLAR_CODE_NEGATIVE) ? APV_BIPOLAR_CODE_NEGATIVE : APV_BIPOLAR_CODE_POSITIVE;
      }
    while (polarIndex > 0);
    }

/******************************************************************************/

  return(polarErrorCode);

/******************************************************************************/
  }

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
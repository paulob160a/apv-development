/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMessageHandling.c                                                       */
/* 21.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - the APV project uses a single messaging link format to encapsulate the   */
/*   messages to/from different application processes                         */
/*                                                                            */
/* - the message token (byte) ordering has been designed to minimise byte-    */
/*   stuffing. The start-of-message and end-of-message tokens are the same    */
/*   and their uniqueness must be preserved else the start and end of a       */
/*   message cannot be reliably found. On the other hand it is desireable to  */
/*   send fully 8-bit tokens in the body of the message. As an 8-bit token    */
/*   can represent any value 0x00 .. 0xff including the SOM/EOM, by flagging  */
/*   legitimate 0x7e tokens in the transmitted message and removing the flags */
/*   in the received message the boundaries are preserved. Hence :-           */
/*                                                                            */
/*    - the transmit message sequence pre-message {<SOM>}<SOM> means <SOM>    */
/*    - the transmit message sequence <!FLAG><SOM> means <SOM>                */
/*    - the transmit message sequence <FLAG><SOM> means data <0x7e>           */
/*    - the transmit message sequence <FLAG><FLAG><!FLAG> means <FLAG><FLAG>  */
/*      <!FLAG>                                                               */
/*    - the transmit message sequence <FLAG><FLAG><SOM> means <FLAG><0x7e>    */
/*    - the transmit message sequence <SOM><!SOM> means message start         */
/*    - the transmit message sequence <EOM> is nominal following the          */
/*      (expected) CRC                                                        */
/*                                                                            */
/* - so far, so... As the start of a message is signalled by the sequence     */
/*   <SOM><!SOM> if the message field following <SOM> is constrained to be    */
/*   anything but <SOM> the message start uniqueness is definite. Hence :-    */
/*                                                                            */
/* - the message format by token-numbering is :                               */
/*                                                                            */
/*   00 : <SOM>. It is not included in the message length                     */
/*   01 : <message-length>. The unstuffed (flagged) message length range is   */
/*        constrained to be : 1 >= <message-length> <= ((0x7e - 1) / 2) i.e.  */
/*        1 .. 62. A message length less than '1' is meaningless. A message   */
/*        length of 62 if and when fully stuffed(!) expands to 124 or 0x7C.   */
/*        It is not included in the message length                            */
/*   02 : <<comms-plane><signal-plane>>. Bits 0-3 are the comms-plane, bits   */
/*        4-7 are the signal-plane. As this follows <SOM><!SOM> it is allowed */
/*        to be a 'bare' <SOM> (0x7e) if required. It is not included in the  */
/*        message length                                                      */
/*   03 : <message>. ( 1 * [ 1 | 2 ] ) { <token> } ( 62 * [ 1 | 2 ] ) i.e.    */
/*        03 .. ( 03 + 0x7C )                                                 */
/*                                                                            */
/*   .. + 1 : high-byte of the CCITT-CRC16 checksum of <message-length> +     */
/*            <plane> + <message>. It is not included in the message length   */
/*   .. + 2 : low-byte of the CCITT-CRC16 checksum of <message-length> +      */
/*            <plane> + <message>. It is not included in the message length   */
/*   .. + 3 : <EOM>. It is not included in the message length                 */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "ApvError.h"
#include "ApvCrcGenerator.h"
#include "ApvMessageHandling.h"
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Static Variables :                                                         */
/******************************************************************************/

apvMessageStructure_t  apvMessagePool;
apvMessageStructure_t *apvMessageRoot = &apvMessagePool;

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvFrameMessage() :                                                        */
/*  <--> messageStructure : full definition for the framed message            */
/*   --> commsPlane       : message physical path                             */
/*   --> signalPlane      : message logical path                              */
/*   --> message          : message to frame                                  */
/*   --> message length   : number of tokens in the message                   */
/*                                                                            */
/* - construct a fully link-framed message                                    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvFrameMessage(apvMessageStructure_t *messageStructure,
                               apvCommsPlanes_t       commsPlane,
                               apvSignalPlanes_t      signalPlane,
                               uint8_t               *message,
                               uint16_t               messageLength)
  {
/******************************************************************************/

  APV_ERROR_CODE framingError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((messageStructure == NULL) || (message == NULL) ||
      (messageLength < APV_MESSAGING_MINIMUM_UNSTUFFED_MESSAGE_LENGTH) ||
      (messageLength > APV_MESSAGING_MAXIMUM_UNSTUFFED_MESSAGE_LENGTH))
    {
    framingError = APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR;
    }
  else
    {
    messageStructure->apvMessagingStartOfMessageToken = APV_MESSAGING_START_OF_MESSAGE;

    messageStructure->apvMessagingPlanesToken.apvMessagePlanes.apvCommsPlane  = commsPlane;
    messageStructure->apvMessagingPlanesToken.apvMessagePlanes.apvSignalPlane = signalPlane;

    messageStructure->apvMessagingLengthOfMessage = 0;

    // Insert any required byte-stuffing
    while (messageLength > 0)
      {
      messageStructure->apvMessagingLengthOfMessage = messageStructure->apvMessagingLengthOfMessage + 1;

      if (*message == APV_MESSAGING_START_OF_MESSAGE)
        {
        messageStructure->apvMessagingPayload[messageStructure->apvMessagingLengthOfMessage - 1] =  APV_MESSAGING_STUFFING_FLAG;
        messageStructure->apvMessagingPayload[messageStructure->apvMessagingLengthOfMessage]     = *message;

        messageStructure->apvMessagingLengthOfMessage = messageStructure->apvMessagingLengthOfMessage + 1;
        }
      else
        {
        messageStructure->apvMessagingPayload[messageStructure->apvMessagingLengthOfMessage - 1] = *message;
        }

      messageLength = messageLength - 1;
      message       = message       + 1;
      }

    // Compute the CRC
    apvBlockComputeCrc(&messageStructure->apvMessagingPayload[0],
                        messageStructure->apvMessagingLengthOfMessage,
                        (uint16_t *)&messageStructure->apvMessagingCrcLowToken);

    messageStructure->apvMessagingEndOfMessageToken   = APV_MESSAGING_END_OF_MESSAGE;
    }

/******************************************************************************/

  return(framingError);

/******************************************************************************/
  } /* end of apvFrameMessage                                                 */

/******************************************************************************/
/* apvDeFrameMessage() :                                                      */
/*   --> ringBuffer       : 1 { <byte> } n                                    */
/*  <--> messageStructure : full definition for the framed message            */
/*   --> ringBuffer       : 1 { <byte> } n                                    */
/*  <--> messageState     : the current state of the message de-framing       */
/*                          finite-state-machine                              */
/*                                                                            */
/* - attempt to de-frame a message one or more tokens (bytes) at a time using */
/*   the message structure-defining finite-state-machine                      */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvDeFrameMessage(apvRingBuffer_t            *ringBuffer,
                                 apvMessageStructure_t      *messageStructure,
                                 apvMessageDeFramingState_t *messageState)
  {
/******************************************************************************/

  APV_ERROR_CODE deFramingError = APV_ERROR_CODE_NONE;

/******************************************************************************/
/******************************************************************************/

  return(deFramingError);

/******************************************************************************/
  } /* end of apvDeFrameMessage                                               */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
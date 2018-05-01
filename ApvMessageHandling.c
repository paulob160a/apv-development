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
/*   01 : <<comms-plane><signal-plane>>. Bits 0-3 are the comms-plane, bits   */
/*        4-7 are the signal-plane. As this follows <SOM> it is NOT allowed   */
/*        to be a 'bare' <SOM> (0x7e). It is not included in the message      */
/*        length                                                              */
/*   02 : <message-length>. The unstuffed (flagged) message length range is   */
/*        constrained to be : 1 >= <message-length> <= ((0x7e - 1) / 2) i.e.  */
/*        1 .. 62. A message length less than '1' is meaningless. A message   */
/*        length of 62 if and when fully stuffed(!) expands to 124 or 0x7C.   */
/*        It is not included in the message length                            */
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
#include "ApvStateMachines.h"
#include "ApvCrcGenerator.h"
#include "ApvMessageHandling.h"
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

apvMessagingDeFramingState_t *apvMessageDeFramingStateMachine = &apvMessagingDeFramingStateMachine[APV_MESSAGE_FRAME_STATE_NULL];

/******************************************************************************/
/* Static Variables :                                                         */
/******************************************************************************/

static apvMessagingStateVariables_t apvMessageVariableCache;
static apvMessageStructure_t        apvMessageFrame;

/******************************************************************************/
/* The message de-framing state-machine :                                     */
/******************************************************************************/

apvMessagingDeFramingState_t apvMessagingDeFramingStateMachine[APV_MESSAGE_FRAME_STATES] = 
  {
    {
     APV_MESSAGE_FRAME_STATE_NULL,
     APV_MESSAGE_FRAME_STATE_INITIALISATION,
     apvMessageDeFramingNull,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_INITIALISATION,
     APV_MESSAGE_FRAME_STATE_START_OF_MESSAGE,
     apvMessageDeFramingInitialise,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_START_OF_MESSAGE,
     APV_MESSAGE_FRAME_STATE_SIGNAL_AND_LOGICAL_PLANES,
     apvMessageDeFramingStartOfMessage,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_SIGNAL_AND_LOGICAL_PLANES,
     APV_MESSAGE_FRAME_STATE_MESSAGE_LENGTH,
     apvMessageDeFramingSignalAndLogicalPlanes,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_MESSAGE_LENGTH,
     APV_MESSAGE_FRAME_STATE_STUFFING_BYTE,
     apvMessageDeFramingMessageLength,
    &apvMessageVariableCache,
    &apvMessageFrame
    }
  };

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
/* apvDeFrameMessageInitialisation() :                                        */
/*   --> ringBuffer          : 1 { <byte> } n                                 */
/*  <--> messageBuffer       : full definition for the framed message         */
/*  <--> messageStateMachine : the current state of the message de-framing    */
/*                             finite-state-machine                           */
/*                                                                            */
/* - set-up to de-frame a message one or more tokens (bytes) at a time using  */
/*   the message structure-defining finite-state-machine                      */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvDeFrameMessageInitialisation(apvRingBuffer_t               *ringBuffer,
                                                          apvMessageStructure_t        *messageBuffer,
                                                          apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE deFramingError = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  // Initialise the state-machine variables
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NUMBER_OF_STATES] = APV_MESSAGE_FRAME_STATES;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE]      = APV_MESSAGE_FRAME_STATE_NULL;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_STATE]       = APV_MESSAGE_FRAME_STATE_ERROR_REPORTER;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE]     = APV_MESSAGE_FRAME_STATE_NULL;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]      = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_STUFFING_FLAG]    = (apvMessageStateVariable_t)false;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN]        = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN]       = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_CRC_SUM]          = APV_CRC_GENERATOR_INITIAL_VALUE;

  if ((ringBuffer == NULL) || (messageBuffer == NULL))
    {
    deFramingError = APV_STATE_MACHINE_CODE_ERROR;
    }
  else
    {
    // KEEP THIS FOR THE FINAL READ-BACK WHEN THE MESSAGE HAS BEEN ASSEMBLED, USE THE STATE-MACHINE MESSAGE-FRAME FOR ASSEMBLY
    messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW]     = (apvMessageStateVariable_t)(((apvMessageStateEntryPointer_t)ringBuffer)     & APV_UINT64_POINTER_UINT32_MASK);
    messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_HIGH]    = (apvMessageStateVariable_t)((((apvMessageStateEntryPointer_t)ringBuffer)    & (APV_UINT64_POINTER_UINT32_MASK << APV_UINT64_POINTER_UINT32_SHIFT)) >> APV_UINT64_POINTER_UINT32_SHIFT);
    messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_MESSAGE_BUFFER_LOW]  = (apvMessageStateVariable_t)(((apvMessageStateEntryPointer_t)messageBuffer)  & APV_UINT64_POINTER_UINT32_MASK);
    messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_MESSAGE_BUFFER_HIGH] = (apvMessageStateVariable_t)((((apvMessageStateEntryPointer_t)messageBuffer) & (APV_UINT64_POINTER_UINT32_MASK << APV_UINT64_POINTER_UINT32_SHIFT)) >> APV_UINT64_POINTER_UINT32_SHIFT);
    }

/******************************************************************************/

  return(deFramingError);

/******************************************************************************/
  } /* end of apvDeFrameMessageInitialisation                                 */

/******************************************************************************/
/* apvMessageDeFrameMessage() :                                               */
/*  --> messageStateMachine : the message state machine table                 */
/* <-- apvStateError        : error codes                                     */
/*                                                                            */
/* - activate the messaging state machine                                     */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvDeFrameMessage(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE deFramingError = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  deFramingError = apvExecuteStateMachine((apvGenericState_t *)messageStateMachine);

/******************************************************************************/

  return(deFramingError);

/******************************************************************************/
  } /* end of apvDeFrameMessage                                               */

/******************************************************************************/
/* Message de-framing state machine functions :                               */
/******************************************************************************/
/* apvMessageDeFramingNull() :                                                */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - holding state for the messaging state machine                            */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingNull(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  // Save the next active state to change state
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessageDeFramingNull                                         */

/******************************************************************************/
/* apvMessageDeFramingInitialise() :                                          */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - initialisation state for the messaging state machine : re-initialise the */
/*   non-static state variables                                               */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingInitialise(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]   = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_STUFFING_FLAG] = (apvMessageStateVariable_t)false;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN]     = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN]    = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_CRC_SUM]       = APV_CRC_GENERATOR_INITIAL_VALUE;

  // Save the next active state to change state
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessageDeFramingInitialise                                   */

/******************************************************************************/
/* apvMessageDeFramingStartOfMessage() :                                      */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - start-of-message <SOM> detection state for the messaging state machine   */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingStartOfMessage(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE  apvStateError     = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW];

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint8_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] == APV_MESSAGING_START_OF_MESSAGE)
      { // if the token is <SOM> no change
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] = APV_MESSAGING_START_OF_MESSAGE;
      }
    else
      { // if the token is not <SOM> a message may be starting - keep the new token and change state
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;
      }
    }
  else
    {
    // There are no more tokens ready - wait in this state
    apvStateError = APV_STATE_MACHINE_CODE_STOP;
    }

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessageDeFramingStartOfMessage                               */

/******************************************************************************/
/* apvMessageDeFramingSignalAndLogicalPlanes() :                              */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - decode the originating signal plane and the intended logical plane :     */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingSignalAndLogicalPlanes(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW];

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint8_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] == APV_MESSAGING_START_OF_MESSAGE)
      { // <SOM> signals a false message start so go back to the start
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE];
      }
    else
      { // The token is potentially a 'planes' token, save it and change state
      messageStateMachine->apvMessageFrame->apvMessagingPlanesToken.apvMessagePlanesToken = messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN];

      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;
      }
    }
  else
    {
    // There are no more tokens ready - wait in this state
    apvStateError = APV_STATE_MACHINE_CODE_STOP;
    }

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessageDeFramingSignalAndLogicalPlanes                       */

/******************************************************************************/
/* apvMessageDeFramingMessageLength() :                                       */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - decode the originating signal plane and the intended logical plane :     */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingMessageLength(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW];

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint8_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] == APV_MESSAGING_START_OF_MESSAGE)
      { // <SOM> signals a false message start so go back to the first state
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE];
      }
    else
      { // The token is potentially a 'length-of-message' token, save it and change state
      messageStateMachine->apvMessageFrame->apvMessagingLengthOfMessage = messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN];

      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;
      }
    }

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessageDeFramingMessageLength                                */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
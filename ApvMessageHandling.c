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
/*   01 : "inbound" <<comms-plane><signal-plane>> :                           */
/*        Bits 0-3 are the comms-plane, bits 4-7 are the signal-plane. As     */
/*        this follows <SOM> it is NOT allowed to be a 'bare' <SOM> (0x7e).   */
/*        It is not included in the message length                            */
/*   02 : "outbound" <<comms-plane><signal-plane>> :                          */
/*        Bits 0-3 are the comms-plane, bits 4-7 are the signal-plane. As     */
/*        this follows <SOM> it is NOT allowed to be a 'bare' <SOM> (0x7e).   */
/*        It is not included in the message length                            */
/*   03 : <message-length>. The unstuffed (flagged) message length range is   */
/*        constrained to be : 1 >= <message-length> <= ((0x7e - 1) / 2) i.e.  */
/*        1 .. 62. A message length less than '1' is meaningless. A message   */
/*        length of 62 if and when fully stuffed(!) expands to 124 or 0x7C.   */
/*        It is not included in the message length                            */
/*   04 : <message>. ( 1 * [ 1 | 2 ] ) { <token> } ( 62 * [ 1 | 2 ] ) i.e.    */
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

apvRingBuffer_t               apvMessageFreeBufferSet;
apvMessageStructure_t         apvMessageFreeBuffers[APV_MESSAGE_FREE_BUFFER_SET_SIZE];

uint32_t                      apvMessageSuccessCounters[APV_MESSAGE_SUCCESS_COUNTERS]   = { 0, 0 };

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
     APV_MESSAGE_FRAME_STATE_INBOUND_SIGNAL_AND_LOGICAL_PLANES,
     apvMessageDeFramingInitialise,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_INBOUND_SIGNAL_AND_LOGICAL_PLANES,
     APV_MESSAGE_FRAME_STATE_OUTBOUND_SIGNAL_AND_LOGICAL_PLANES,
     apvMessageDeFramingInBoundSignalAndLogicalPlanes,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_OUTBOUND_SIGNAL_AND_LOGICAL_PLANES,
     APV_MESSAGE_FRAME_STATE_MESSAGE_LENGTH,
     apvMessageDeFramingOutBoundSignalAndLogicalPlanes,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_MESSAGE_LENGTH,
     APV_MESSAGE_FRAME_STATE_DATA_BYTE,
     apvMessageDeFramingMessageLength,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_DATA_BYTE,
     APV_MESSAGE_FRAME_STATE_CCITT_CRC16,
     apvMessageDeFramingDataByte,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_CCITT_CRC16,
     APV_MESSAGE_FRAME_STATE_CRC_CHECK,
     apvMessagingDeFramingCrcBytes,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_CRC_CHECK,
     APV_MESSAGE_FRAME_STATE_FRAME_REPORTER,
     apvMessagingDeFramingCrcCheck,
    &apvMessageVariableCache,
    &apvMessageFrame
    },
    {
     APV_MESSAGE_FRAME_STATE_FRAME_REPORTER,
     APV_MESSAGE_FRAME_STATE_INITIALISATION,
     apvMessagingDeFramingReporter,
    &apvMessageVariableCache,
    &apvMessageFrame
    }
  };

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvMessageFramerCheckCommsPlane() :                                        */
/*   --> commsPlane : message physical path                                   */
/*                                                                            */
/* - check the requested comms plane exists                                   */
/*                                                                            */
/******************************************************************************/

bool apvMessageFramerCheckCommsPlane(apvCommsPlanes_t commsPlane)
  {
/******************************************************************************/

  bool apvCommsPlaneCheck = false;

/******************************************************************************/

  switch(commsPlane)
    {
    case APV_COMMS_PLANE_SERIAL_UART    :
    case APV_COMMS_PLANE_SERIAL_USART_0 :
    case APV_COMMS_PLANE_SERIAL_USART_1 :
    case APV_COMMS_PLANE_SERIAL_USART_2 :
    case APV_COMMS_PLANE_SERIAL_USART_3 :
    case APV_COMMS_PLANE_SPI_RADIO_0    :
    case APV_COMMS_PLANE_SPI_RADIO_1    :
    case APV_COMMS_PLANE_SPI_RADIO_2    :
    case APV_COMMS_PLANE_SPI_RADIO_3    :
    case APV_COMMS_PLANE_I2C_ANON       :
                                          apvCommsPlaneCheck = true;

    default                             : break;
    }

/******************************************************************************/

  return(apvCommsPlaneCheck);

/******************************************************************************/
  } /* end of apvMessageFramerCheckCommsPlane                                 */

/******************************************************************************/
/* apvMessageFramerCheckSignalPlane() :                                       */
/*   --> signalPlane      : message logical path                              */
/*                                                                            */
/* - check the requested signal plane exists                                  */
/*                                                                            */
/******************************************************************************/

bool apvMessageFramerCheckSignalPlane(apvSignalPlanes_t signalPlane)
  {
/******************************************************************************/

  bool apvSignalPlaneCheck = false;

/******************************************************************************/

  switch(signalPlane)
    {
    case APV_SIGNAL_PLANE_CONTROL_0   :
    case APV_SIGNAL_PLANE_DATA_0      :
    case APV_SIGNAL_PLANE_CONTROL_1   :
    case APV_SIGNAL_PLANE_DATA_1      :
    case APV_SIGNAL_PLANE_CONTROL_2   :
    case APV_SIGNAL_PLANE_DATA_2      :
    case APV_SIGNAL_PLANE_CONTROL_3   :
    case APV_SIGNAL_PLANE_DATA_3      :
    case APV_SIGNAL_PLANE_CONTROL_4   :
    case APV_SIGNAL_PLANE_DATA_4      :
    case APV_SIGNAL_PLANE_CONTROL_X_0 :
    case APV_SIGNAL_PLANE_DATA_X_0    :
    case APV_SIGNAL_PLANE_CONTROL_X_1 :
    case APV_SIGNAL_PLANE_DATA_X_1    :
                                        apvSignalPlaneCheck = true;

    default                           : break;
    }

/******************************************************************************/

  return(apvSignalPlaneCheck);

/******************************************************************************/
  } /* end of apvMessageFramerCheckSignalPlane                                */

/******************************************************************************/
/* apvMessageStructureInitialisation() :                                      */
/*  <--> messageStructure            : full definition of a message           */
/*   --> messagePayloadMaximumLength : maximum size of a framed message       */
/*  <-- framingError                 : error codes                            */
/*                                                                            */
/*  - initialise the structure to contain a framed message                    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvMessageStructureInitialisation(apvMessageStructure_t *messageStructure,
                                                 uint16_t               messagePayloadMaximumLength)
  {
/******************************************************************************/

  APV_ERROR_CODE framingError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  messageStructure->apvMessagingStartOfMessageToken                       = 0;
  messageStructure->apvMessagingInBoundPlanesToken.apvMessagePlanesToken  = 0;
  messageStructure->apvMessagingOutBoundPlanesToken.apvMessagePlanesToken = 0;
  messageStructure->apvMessagingLengthOfMessage                           = 0;
  messageStructure->apvMessagingCrcLowToken                               = 0;
  messageStructure->apvMessagingCrcHighToken                              = 0;
  messageStructure->apvMessagingEndOfMessageToken                         = 0;

  messageStructure->apvMessagingPayloadMaximumLength = messagePayloadMaximumLength;

  while (messagePayloadMaximumLength > 0)
    {
    messageStructure->apvMessagingPayload[messagePayloadMaximumLength - 1] = 0;

    messagePayloadMaximumLength = messagePayloadMaximumLength - 1;
    }

/******************************************************************************/

  return(framingError);

/******************************************************************************/
  } /* end of apvMessageStructureInitialisation                               */

/******************************************************************************/
/* apvFrameMessage() :                                                        */
/*  <--> messageStructure   : full definition for the framed message          */
/*   --> commsPlane         : message physical path                           */
/*   --> signalPlane        : message logical path                            */
/*   --> message            : message to frame                                */
/*   --> message length     : number of tokens in the payload                 */
/*  <--> messageTotalLength : the final number of tokens in the message       */
/*                                                                            */
/* - construct a fully link-framed message                                    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvFrameMessage(apvMessageStructure_t *messageStructure,
                               apvCommsPlanes_t       inBoundCommsPlane,
                               apvSignalPlanes_t      inBoundSignalPlane,
                               apvCommsPlanes_t       outBoundCommsPlane,
                               apvSignalPlanes_t      outBoundSignalPlane,
                               uint8_t               *message,
                               uint16_t               messageLength,
                               uint16_t              *messageTotalLength)
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
    *messageTotalLength = 0; // tot up all the message components

    messageStructure->apvMessagingStartOfMessageToken = APV_MESSAGING_START_OF_MESSAGE;

    messageStructure->apvMessagingInBoundPlanesToken.apvMessagePlanes.apvCommsPlane  = inBoundCommsPlane;
    messageStructure->apvMessagingInBoundPlanesToken.apvMessagePlanes.apvSignalPlane = inBoundSignalPlane;

    messageStructure->apvMessagingOutBoundPlanesToken.apvMessagePlanes.apvCommsPlane  = outBoundCommsPlane;
    messageStructure->apvMessagingOutBoundPlanesToken.apvMessagePlanes.apvSignalPlane = outBoundSignalPlane;

    messageStructure->apvMessagingLengthOfMessage = 0;

    // Load the message frame precursors
    messageStructure->apvMessagingPayload[APV_COMMS_MESSAGE_PAYLOAD_SOM_FIELD_OFFSET]             = APV_MESSAGING_START_OF_MESSAGE;
    messageStructure->apvMessagingPayload[APV_COMMS_MESSAGE_PAYLOAD_INBOUND_PLANES_FIELD_OFFSET]  = messageStructure->apvMessagingInBoundPlanesToken.apvMessagePlanesToken;
    messageStructure->apvMessagingPayload[APV_COMMS_MESSAGE_PAYLOAD_OUTBOUND_PLANES_FIELD_OFFSET] = messageStructure->apvMessagingOutBoundPlanesToken.apvMessagePlanesToken;
    messageStructure->apvMessagingPayload[APV_COMMS_MESSAGE_PAYLOAD_LENGTH_FIELD_OFFSET]          = 0; // hold the place of the message length

    *messageTotalLength = *messageTotalLength + APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET;

    // Insert any required byte-stuffing
    while (messageLength > 0)
      {
       messageStructure->apvMessagingLengthOfMessage =   messageStructure->apvMessagingLengthOfMessage + 1;
      *messageTotalLength                            = (*messageTotalLength)                           + 1;

      if (*message == APV_MESSAGING_START_OF_MESSAGE)
        {
        messageStructure->apvMessagingPayload[messageStructure->apvMessagingLengthOfMessage + APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET - 1] =  APV_MESSAGING_STUFFING_FLAG;
        messageStructure->apvMessagingPayload[messageStructure->apvMessagingLengthOfMessage + APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET]     = *message;

         messageStructure->apvMessagingLengthOfMessage =   messageStructure->apvMessagingLengthOfMessage + 1;
        *messageTotalLength                            = (*messageTotalLength)                           + 1;
        }
      else
        {
        messageStructure->apvMessagingPayload[messageStructure->apvMessagingLengthOfMessage + APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET - 1] = *message;
        }

      messageLength       = messageLength         - 1;
      message             = message               + 1;
      }

    // Compute the CRC
    apvBlockComputeCrc(&messageStructure->apvMessagingPayload[APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET],
                        messageStructure->apvMessagingLengthOfMessage,
                        (uint16_t *)&messageStructure->apvMessagingCrcLowToken);

    // Insert the payload length to the controlling variable set
    messageStructure->apvMessagingPayload[APV_COMMS_MESSAGE_PAYLOAD_LENGTH_FIELD_OFFSET] = messageStructure->apvMessagingLengthOfMessage;

    // Finally add the length of the CRC field to the total message length
    *messageTotalLength = (*messageTotalLength) + APV_CRC_WORD_WIDTH;

    messageStructure->apvMessagingEndOfMessageToken = APV_MESSAGING_END_OF_MESSAGE;
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

APV_MESSAGING_STATE_CODE apvDeFrameMessageInitialisation(apvRingBuffer_t              *ringBuffer,
                                                         apvRingBuffer_t              *messageFreeBuffers,
                                                         apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE  deFramingError   = APV_STATE_MACHINE_CODE_NONE;

  uint32_t                  *messageBuffer   =  NULL,
                           **messageBuffer_p = &messageBuffer;

/******************************************************************************/

  // Initialise the state-machine variables
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NUMBER_OF_STATES] = APV_MESSAGE_FRAME_STATES;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE]      = APV_MESSAGE_FRAME_STATE_NULL;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_STATE]       = APV_MESSAGE_FRAME_STATE_FRAME_REPORTER;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE]     = APV_MESSAGE_FRAME_STATE_NULL;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]      = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_STUFFING_FLAG]    = (apvMessageStateVariable_t)false;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN]        = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN]       = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_CRC_SUM]          = APV_CRC_GENERATOR_INITIAL_VALUE; // marks as an unfinished frame decode

  if ((ringBuffer == NULL) || (messageFreeBuffers == NULL))
    {
    deFramingError = APV_STATE_MACHINE_CODE_ERROR;
    }
  else
    {
    // Get a message buffer from the "free" list
    if (apvRingBufferUnLoad( messageFreeBuffers,
                            (uint32_t *)messageBuffer_p,
                             1,
                             true) != 0)
      {
      // KEEP THIS FOR THE FINAL READ-BACK WHEN THE MESSAGE HAS BEEN ASSEMBLED, USE THE STATE-MACHINE MESSAGE-FRAME FOR ASSEMBLY
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW]     = (apvMessageStateVariable_t)(((apvMessageStateEntryPointer_t)ringBuffer)     & APV_UINT64_POINTER_UINT32_MASK);
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_HIGH]    = (apvMessageStateVariable_t)0; // pointers are only 32-bits wide on Cortex-M
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_MESSAGE_BUFFER_LOW]  = (apvMessageStateVariable_t)(((apvMessageStateEntryPointer_t)messageBuffer)  & APV_UINT64_POINTER_UINT32_MASK);
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_MESSAGE_BUFFER_HIGH] = (apvMessageStateVariable_t)0; // pointers are only 32-bits wide on Cortex-M
      }
    else
      {
      deFramingError = APV_STATE_MACHINE_CODE_ERROR;
      }
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
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH]         = 0;
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_FRAME_CHECK]            = APV_MESSAGE_FRAME_STATE_FRAME_REPORTER;

  *((uint16_t *)&(messageStateMachine->apvMessageFrame->apvMessagingCrcLowToken))                                         = APV_CRC_GENERATOR_INITIAL_VALUE;

  // Save the next active state to change state
  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessageDeFramingInitialise                                   */

/******************************************************************************/
/* apvMessageDeFramingInBoundSignalAndLogicalPlanes() :                       */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - decode the originating signal plane and the intended logical plane :     */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingInBoundSignalAndLogicalPlanes(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW];

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint32_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    if ((messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK) == APV_MESSAGING_START_OF_MESSAGE)
      { // <SOM> signals a false message start so go back to the start
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE];
      }
    else
      { // The token is potentially a 'planes' token, save it and change state
      messageStateMachine->apvMessageFrame->apvMessagingInBoundPlanesToken.apvMessagePlanesToken = (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK);

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
  } /* end of apvMessageDeFramingInBoundSignalAndLogicalPlanes                */

/******************************************************************************/
/* apvMessageDeFramingOutBoundSignalAndLogicalPlanes() :                      */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - decode the targetted signal plane and the intended logical plane :       */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingOutBoundSignalAndLogicalPlanes(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW];

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint32_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    if ((messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK) == APV_MESSAGING_START_OF_MESSAGE)
      { // <SOM> signals a false message start so go back to the start
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE];
      }
    else
      { // The token is potentially a 'planes' token, save it and change state
      messageStateMachine->apvMessageFrame->apvMessagingOutBoundPlanesToken.apvMessagePlanesToken = (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK);

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
  } /* end of apvMessageDeFramingOutBoundSignalAndLogicalPlanes               */

/******************************************************************************/
/* apvMessageDeFramingMessageLength() :                                       */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - decode the expected message length :                                     */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingMessageLength(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)((*(apvMessageStateEntryPointer_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW])));

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint32_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    if ((messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] == APV_MESSAGING_START_OF_MESSAGE) & APV_MESSAGE_PAYLOAD_CHARACTER_MASK)
      { // <SOM> signals a false message start so go back to the first state
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_FRAME_CHECK];
      }
    else
      { // The token is potentially a 'length-of-message' token, save it and change state
      messageStateMachine->apvMessageFrame->apvMessagingLengthOfMessage                                                      = ((uint8_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK);
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH]        = (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK);

      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN]   = 0; // no tokens yet so the last one cannot be a "stuffing" byte
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]  = 0;

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
  } /* end of apvMessageDeFramingMessageLength                                */

/******************************************************************************/
/* apvMessageDeFramingDataByte() :                                            */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - get a new 'payload data' token :                                         */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessageDeFramingDataByte(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)((*(apvMessageStateEntryPointer_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW])));

  // Get the next token off the ring-buffer if it exists
  if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                          ((uint32_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                          1,
                          true) != 0)
    {
    // Compute the running CRC
    apvComputeCrc((uint8_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN], 
                                                                   ((uint16_t *)&(messageStateMachine->apvMessageFrame->apvMessagingCrcLowToken)));

    // The expected remaining payload length is...
    messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] = 
                messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] - 1;

    // If the payload length is negative, something has gone very wrong
    if (((uint16_t)((int16_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH])) > APV_MESSAGING_MAXIMUM_PAYLOAD_LENGTH)
      {
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_FRAME_CHECK];

      apvStateError = APV_STATE_MACHINE_CODE_ERROR;
      }
    else
      { // If <SOM> has been found the preceding byte should have been a stuffing flag
      if ((messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK) == APV_MESSAGING_START_OF_MESSAGE)
        {
        if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] == APV_MESSAGING_STUFFING_FLAG)
          { // Correct - reduce the overall message length by 1 and store the new data on the payload
          messageStateMachine->apvMessageFrame->apvMessagingLengthOfMessage = messageStateMachine->apvMessageFrame->apvMessagingLengthOfMessage - 1;

          messageStateMachine->apvMessageFrame->apvMessagingPayload[messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]] = APV_MESSAGING_START_OF_MESSAGE;

          // Increment the payload count by 1
          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] = 
                          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] + 1;

          // Cancel the last stuffing flag
          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] = 0;

           // On the last byte of the payload change state
          if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] == 0)
            {
            messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;
            }
          }
        else
          { // Wrong - the payload has failed, back to the start
          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = 
                      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_FRAME_CHECK];
          }
        }
      else
        {
        if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] == 0)
          { // Last byte of the payload - check for a hanging stuffing flag, load the remaining byte(s) and change state
          if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] == APV_MESSAGING_STUFFING_FLAG)
            {
            messageStateMachine->apvMessageFrame->apvMessagingPayload[messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]] = APV_MESSAGING_STUFFING_FLAG;

            messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] = 
                        messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] + 1;
            }

          messageStateMachine->apvMessageFrame->apvMessagingPayload[messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]] = 
                                                                                                         (uint8_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN];

          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] = 
                        messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] + 1;

          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;
          }
        else
          { // A stuffing byte is a holding byte
          if ((messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN] & APV_MESSAGE_PAYLOAD_CHARACTER_MASK) == APV_MESSAGING_STUFFING_FLAG)
            { // Only one stuffing flag can be outstanding so load the last one on the payload
            if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] == APV_MESSAGING_STUFFING_FLAG)
              {
              messageStateMachine->apvMessageFrame->apvMessagingPayload[messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]] = APV_MESSAGING_STUFFING_FLAG;
         
              messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] = 
                          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] + 1;
              }
            else
              { // DO NOT INCREMENT THE PAYLOAD COUNT YET : will be '1' for <ESC><SOM>, '1' + '2' for <ESC><random>
              messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] = APV_MESSAGING_STUFFING_FLAG;
              }
            }
          else
            { // Load the next byte on the payload
            if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] == APV_MESSAGING_STUFFING_FLAG)
              {
              messageStateMachine->apvMessageFrame->apvMessagingPayload[messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]] = APV_MESSAGING_STUFFING_FLAG;
              
              messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] = 
                        messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] + 1;

              messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN] = 0;
              }

            messageStateMachine->apvMessageFrame->apvMessagingPayload[messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT]] =
                                                                                                         (uint8_t)messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN];

            messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] = 
                          messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT] + 1;
            }
          }
        }
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
  } /* end of apvMessageDeFramingDataByte                                     */

/******************************************************************************/
/* apvMessagingDeFramingCrcBytes() :                                          */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - get the payload CRC tokens :                                             */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessagingDeFramingCrcBytes(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

  apvPointerConversion_t   ringBufferPointer;

/******************************************************************************/

  // Compute the ring-buffer structure address
  ringBufferPointer.apvPointerConversion = (apvMessageStateEntryPointer_t)((*(apvMessageStateEntryPointer_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW])));

  if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] < APV_CRC_WORD_WIDTH)
    {
     // Get the next token off the ring-buffer if it exists
    if (apvRingBufferUnLoad((apvRingBuffer_t *)(ringBufferPointer.apvPointerConversion),
                            ((uint32_t *)(&messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN])),
                            1,
                            true) != 0)
      {
      // Divide the running CRC by the pre-computed message CRC
      apvComputeCrc(messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN], 
                                                                     ((uint16_t *)&(messageStateMachine->apvMessageFrame->apvMessagingCrcLowToken)));

      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] = 
                  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH] + 1;
      }
    else
      {
      // There are no more tokens ready - wait in this state
      apvStateError = APV_STATE_MACHINE_CODE_STOP;
      }
    }
  else
    { // CRC computation over - change state
    messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;
    }

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessagingDeFramingCrcBytes                                   */

/******************************************************************************/
/* apvMessagingDeFramingCrcCheck() :                                          */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - do the final checks on the message integrity :                           */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessagingDeFramingCrcCheck(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  // Test if the CRC check has passed
  if (*((uint16_t *)&(messageStateMachine->apvMessageFrame->apvMessagingCrcLowToken)) == APV_CRC_GENERATOR_FINAL_VALUE)
    { // If it has, compare the expected message length to the actual message length
    if (messageStateMachine->apvMessageFrame->apvMessagingLengthOfMessage == messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT])
      { // Change state to flag the message as OK for higher layers
      messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_CRC_SUM] = APV_CRC_GENERATOR_FINAL_VALUE;
      }
    }

  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessagingDeFramingCrcCheck                                   */

/******************************************************************************/
/* apvMessagingDeFramingReporter() :                                          */
/* <--> messageStateMachine : the message state machine table                 */
/* <--  apvStateError       : error codes                                     */
/*                                                                            */
/* - deliver an error-free message to a higher layer :                        */
/*                                                                            */
/******************************************************************************/

APV_MESSAGING_STATE_CODE apvMessagingDeFramingReporter(apvMessagingDeFramingState_t *messageStateMachine)
  {
/******************************************************************************/

  APV_MESSAGING_STATE_CODE apvStateError      = APV_STATE_MACHINE_CODE_NONE;

/******************************************************************************/

  // All done, start looking for another message
#if (1)
  if (messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_CRC_SUM] == APV_CRC_GENERATOR_FINAL_VALUE)
    {
    //printf("\n Message Test Passed...");

    apvMessageSuccessCounters[APV_MESSAGE_SUCCESS_COUNTER] = apvMessageSuccessCounters[APV_MESSAGE_SUCCESS_COUNTER] + 1;
    }
  else
    {
    //printf("\n Message Test Failed...");

    apvMessageSuccessCounters[APV_MESSAGE_FAILURE_COUNTER] = apvMessageSuccessCounters[APV_MESSAGE_FAILURE_COUNTER] + 1;
    }
#endif

  messageStateMachine->apvMessageStateVariables->apvMessageStateVariables[APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE] = (apvMessageStateVariable_t)messageStateMachine->apvMessageNextState;

/******************************************************************************/

  return(apvStateError);

/******************************************************************************/
  } /* end of apvMessagingDeFramingReporter                                   */

/******************************************************************************/
/* apvCreateMessageBuffers() :                                                */
/*  --> apvMessageBufferSet     : "free" set of message buffers               */
/*  --> apvMessageBuffers       : set of message buffers to be managed in the */
/*                                "free" set                                  */
/*  --> apvMessageBufferSetSize : number of buffers to be managed             */
/*  <-- messageError            : error codes                                 */
/*                                                                            */
/* - instantiate a managed list of message buffers/structures                 */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvCreateMessageBuffers(apvRingBuffer_t       *apvMessageBufferSet,
                                       apvMessageStructure_t *apvMessageBuffers,
                                       uint32_t               apvMessageBufferSetSize)
  {
/******************************************************************************/

  APV_ERROR_CODE  messageError  = APV_ERROR_CODE_NONE;

  uint32_t        messageBuffer =  0;

/******************************************************************************/

  if ((apvMessageBufferSet     == NULL) || (apvMessageBuffers == NULL) || 
      (apvMessageBufferSetSize == 0))
    {
    messageError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    // Initialise the holding ring-buffer
    if (apvRingBufferInitialise(apvMessageBufferSet,
                                apvMessageBufferSetSize) == APV_ERROR_CODE_NONE)
      {
      // In reverse-order initialise the message buffer structures
      do 
        {
        apvMessageBufferSetSize = apvMessageBufferSetSize - 1;

        if (apvMessageStructureInitialisation((apvMessageBuffers + apvMessageBufferSetSize),
                                              APV_MESSAGING_MAXIMUM_PAYLOAD_LENGTH) != APV_ERROR_CODE_NONE)
          {
          messageError = APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR;
          }
        else
          { // Assign the address of the ring buffer to the controlling ring-buffer
          messageBuffer = (uint32_t)(apvMessageBuffers + apvMessageBufferSetSize);

          if (apvRingBufferLoad(apvMessageBufferSet,
                                (uint32_t *)&messageBuffer,
                                1,
                                false) == 0)
            {
            messageError = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
            break;
            }
          }
        }
      while (apvMessageBufferSetSize != 0);
      }
    else
      {
      messageError = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
      }
    }

/******************************************************************************/

  return(messageError);

/******************************************************************************/
  } /* end of apvCreateMessageBuffers                                         */

/******************************************************************************/
/* apvMessageStructurePrint() :                                               */
/*  --> messageStructure : pointer to a message structure                     */
/*                                                                            */
/* - non-destructively print the contents of a message structure              */
/*                                                                            */
/******************************************************************************/

void apvMessageStructurePrint(apvMessagingDeFramingState_t *stateMachine)
  {
/******************************************************************************/

  int i = 0;

/******************************************************************************/

  printf("\n");
  printf("\n Message Structure :");
  printf("\n -------------------");
  printf("\n");

  for (i = 0; i < stateMachine->apvMessageFrame->apvMessagingLengthOfMessage; i++)
    {
    printf("[%02x]", (uint8_t)stateMachine->apvMessageFrame ->apvMessagingPayload[i]);
    }

  printf("\n -------------------");
  printf("\n Inbound Planes Token   = %08x", stateMachine->apvMessageFrame->apvMessagingInBoundPlanesToken.apvMessagePlanesToken);
  printf("\n Outbound Planes Token  = %08x", stateMachine->apvMessageFrame->apvMessagingOutBoundPlanesToken.apvMessagePlanesToken);
  printf("\n Message Length         = %08x", stateMachine->apvMessageFrame->apvMessagingLengthOfMessage);
  printf("\n CRC High               = %08x", stateMachine->apvMessageFrame->apvMessagingCrcHighToken);
  printf("\n CRC Low                = %08x", stateMachine->apvMessageFrame->apvMessagingCrcLowToken);
  printf("\n -------------------");
  printf("\n");

/******************************************************************************/
  } /* end of apvMessageStructurePrint                                        */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

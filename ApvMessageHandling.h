/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMessageHandling.h                                                       */
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

#ifndef _APV_MESSAGE_HANDLING_H_
#define _APV_MESSAGE_HANDLING_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>
#include "ApvError.h"
#include "ApvUtilities.h"
#include "ApvStateMachines.h"
#include "ApvCrcGenerator.h"
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

// Communication physical and logical planes are encapsulated in one token (byte)
#define APV_COMMS_PLANE_FIELD_BITS                     (4)    // apvCommsPlanes_t
#define APV_SIGNAL_PLANE_FIELD_BITS                    (4)    // apvSignalPlanes_t
#define APV_MESSAGE_PLANE_MASK                         (0x0f) // one nybble
#define APV_MESSAGE_PLANE_SHIFT                        APV_SIGNAL_PLANE_FIELD_BITS

#define APV_MESSAGE_PAYLOAD_CHARACTER                  uint8_t
#define APV_MESSAGE_PAYLOAD_CHARACTER_WIDTH            (sizeof(uint8_t))
#define APV_MESSAGE_PAYLOAD_CHARACTER_BIT_WIDTH        8
#define APV_MESSAGE_PAYLOAD_CHARACTER_MASK             ((1 << APV_MESSAGE_PAYLOAD_CHARACTER_BIT_WIDTH) - 1)

#define APV_COMMS_MESSAGE_PLANE_FIELD_WIDTH            (sizeof(uint8_t))
#define APV_COMMS_MESSAGE_PAYLOAD_LENGTH_FIELD_WIDTH   (sizeof(uint8_t))

#define APV_COMMS_MESSAGE_PAYLOAD_SOM_FIELD_OFFSET             0
#define APV_COMMS_MESSAGE_PAYLOAD_INBOUND_PLANES_FIELD_OFFSET  (APV_COMMS_MESSAGE_PAYLOAD_SOM_FIELD_OFFSET             + 1)
#define APV_COMMS_MESSAGE_PAYLOAD_OUTBOUND_PLANES_FIELD_OFFSET (APV_COMMS_MESSAGE_PAYLOAD_INBOUND_PLANES_FIELD_OFFSET  + 1)
#define APV_COMMS_MESSAGE_PAYLOAD_LENGTH_FIELD_OFFSET          (APV_COMMS_MESSAGE_PAYLOAD_OUTBOUND_PLANES_FIELD_OFFSET + 1)
#define APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET         (APV_COMMS_MESSAGE_PAYLOAD_LENGTH_FIELD_OFFSET          + 1)

#define APV_MESSAGING_MINIMUM_UNSTUFFED_MESSAGE_LENGTH (1) // 1
#define APV_MESSAGING_MAXIMUM_UNSTUFFED_MESSAGE_LENGTH ((APV_MESSAGING_START_OF_MESSAGE - 1) >> 1) // 62
#define APV_MESSAGING_MAXIMUM_STUFFED_MESSAGE_LENGTH   (APV_MESSAGING_MAXIMUM_UNSTUFFED_MESSAGE_LENGTH << 1)

#define APV_MESSAGING_MAXIMUM_PAYLOAD_LENGTH           (APV_COMMS_MESSAGE_PAYLOAD_MESSAGE_FIELD_OFFSET + \
                                                        APV_MESSAGING_MAXIMUM_STUFFED_MESSAGE_LENGTH   + \
                                                        APV_CRC_WORD_WIDTH)

#define APV_MESSAGE_STATE_VARIABLE_CACHE_LENGTH        APV_GENERIC_STATE_VARIABLE_CACHE_LENGTH

#define APV_UINT64_POINTER_UINT32_MASK                ((uint32_t)0xffffffff) // 64-bit pointer packing/unpacking
#define APV_UINT64_POINTER_UINT32_SHIFT               ((uint32_t)32)

#define APV_MESSAGE_FREE_BUFFER_SET_SIZE              16 // the message buffers available to pass between comms layers

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/
/* This is the complete definition of an APV project message link frame. The  */
/* token ordering is non-arbitrary!                                           */
/******************************************************************************/

// Definitions of physical communication "planes"
typedef enum apvCommsPlanes_tTag
  {
  APV_COMMS_PLANE_SERIAL_UART = 0, /*  0 */
  APV_COMMS_PLANE_SERIAL_USART_0,  /*  1 */
  APV_COMMS_PLANE_SERIAL_USART_1,  /*  2 */
  APV_COMMS_PLANE_SERIAL_USART_2,  /*  3 */
  APV_COMMS_PLANE_SERIAL_USART_3,  /*  4 */
  APV_COMMS_PLANE_SPI_RADIO_0,     /*  5 */
  APV_COMMS_PLANE_SPI_RADIO_1,     /*  6 */
  APV_COMMS_PLANE_SPI_RADIO_2,     /*  7 */
  APV_COMMS_PLANE_SPI_RADIO_3,     /*  8 */
  APV_COMMS_PLANE_I2C_ANON,        /*  9 */
  APV_COMMS_PLANE_UNUSED_0,        /* 10 */
  APV_COMMS_PLANE_UNUSED_1,        /* 11 */
  APV_COMMS_PLANE_UNUSED_2,        /* 12 */
  APV_COMMS_PLANE_UNUSED_3,        /* 13 */
  APV_COMMS_PLANE_UNUSED_4,        /* 14 */
  APV_COMMS_PLANE_UNUSED_5,        /* 15 */
  APV_COMMS_PLANES
  } apvCommsPlanes_t;

// Definitions of "logical" communication "planes"
typedef enum apvSignalPlanes_tTag
  {
  APV_SIGNAL_PLANE_CONTROL_0 = 0, /* 0 */
  APV_SIGNAL_PLANE_DATA_0,        /* 1 */
  APV_SIGNAL_PLANE_CONTROL_1,     /* 2 */
  APV_SIGNAL_PLANE_DATA_1,        /* 3 */
  APV_SIGNAL_PLANE_CONTROL_2,     /* 4 */
  APV_SIGNAL_PLANE_DATA_2,        /* 5 */
  APV_SIGNAL_PLANE_CONTROL_3,     /* 6 */
  APV_SIGNAL_PLANE_UNUSED_0,      /* 7  - the pattern '0x7' is not allowed so 0x7e <SOM> is precluded (in the absence of bit-errors) */ 
  APV_SIGNAL_PLANE_UNUSED_1,      /* 8  - this is just the matching non-existent 'data' code */
  APV_SIGNAL_PLANE_DATA_3,        /* 9 */
  APV_SIGNAL_PLANE_CONTROL_4,    /* 10 */
  APV_SIGNAL_PLANE_DATA_4,       /* 11 */
  APV_SIGNAL_PLANE_CONTROL_X_0,  /* 12 */
  APV_SIGNAL_PLANE_DATA_X_0,     /* 13 */
  APV_SIGNAL_PLANE_CONTROL_X_1,  /* 14 */
  APV_SIGNAL_PLANE_DATA_X_1,     /* 15 */
  APV_SIGNAL_PLANES
  } apvSignalPlanes_t;

// Definition of the communication "planes" frame token. Note VC++ 2012 warning C4214 indicates using a bit-field type other than 'int'
typedef struct apvMessagePlanes_tTag
  {
  uint8_t apvCommsPlane  : APV_COMMS_PLANE_FIELD_BITS;  // bits 0 - 3
  uint8_t apvSignalPlane : APV_SIGNAL_PLANE_FIELD_BITS; // bits 4 - 7
  } apvMessagePlanes_t;

typedef union apvMessagePlanesToken_tTag
  {
  apvMessagePlanes_t apvMessagePlanes;
  uint8_t            apvMessagePlanesToken;
  } apvMessagePlanesToken_t;

typedef struct apvMessageStructure_tTag
  {
  uint8_t                       apvMessagingStartOfMessageToken;
  apvMessagePlanesToken_t       apvMessagingInBoundPlanesToken;
  apvMessagePlanesToken_t       apvMessagingOutBoundPlanesToken;
  uint8_t                       apvMessagingLengthOfMessage;                               // this does not include the appended CRC
  APV_MESSAGE_PAYLOAD_CHARACTER apvMessagingPayload[APV_MESSAGING_MAXIMUM_PAYLOAD_LENGTH]; // the block CRC generator adds the CRC to the end of the message
  uint8_t                       apvMessagingCrcLowToken;
  uint8_t                       apvMessagingCrcHighToken;
  uint8_t                       apvMessagingEndOfMessageToken;
  uint16_t                      apvMessagingPayloadMaximumLength;
  } apvMessageStructure_t;

// For convenience in message handling alias to an array
typedef union apvMessageFrame_tTag
  {
  apvMessageStructure_t apvMessageStructure;
  uint8_t               apvMessageFrame[sizeof(apvMessageStructure_t)];
  } apvMessageFrame_t;

typedef enum apvMessagingFrameStates_tTag
  {
  APV_MESSAGE_FRAME_STATE_NULL = 0,
  APV_MESSAGE_FRAME_STATE_INITIALISATION,
  APV_MESSAGE_FRAME_STATE_INBOUND_SIGNAL_AND_LOGICAL_PLANES,
  APV_MESSAGE_FRAME_STATE_OUTBOUND_SIGNAL_AND_LOGICAL_PLANES,
  APV_MESSAGE_FRAME_STATE_MESSAGE_LENGTH,
  APV_MESSAGE_FRAME_STATE_DATA_BYTE,
  APV_MESSAGE_FRAME_STATE_CCITT_CRC16,
  APV_MESSAGE_FRAME_STATE_CRC_CHECK,
  APV_MESSAGE_FRAME_STATE_FRAME_REPORTER,
  APV_MESSAGE_FRAME_STATES
  } apvMessagingFrameStates_t;

typedef apvMessagingFrameStates_t APV_MESSAGING_FRAME_STATES;

// This enumeration maps to the generic state-variable definitions
typedef enum apvMessagingFrameStateVariable_tTag
  {
  APV_MESSAGE_FRAME_STATE_VARIABLE_NUMBER_OF_STATES = APV_GENERIC_STATE_VARIABLE_NUMBER_OF_STATES,
  APV_MESSAGE_FRAME_STATE_VARIABLE_FIRST_STATE      = APV_GENERIC_STATE_VARIABLE_FIRST_STATE,
  APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_STATE       = APV_GENERIC_STATE_VARIABLE_LAST_STATE,
  APV_MESSAGE_FRAME_STATE_VARIABLE_ACTIVE_STATE     = APV_GENERIC_STATE_VARIABLE_ACTIVE_STATE,
  APV_MESSAGE_FRAME_STATE_VARIABLE_TOKEN_COUNT      = APV_GENERIC_STATE_VARIABLE_0, // index into the number of tokens loaded
  APV_MESSAGE_FRAME_STATE_VARIABLE_STUFFING_FLAG    = APV_GENERIC_STATE_VARIABLE_1, // [ false == no stuffing character | true == stuffing character ]
  APV_MESSAGE_FRAME_STATE_VARIABLE_NEW_TOKEN        = APV_GENERIC_STATE_VARIABLE_2, 
  APV_MESSAGE_FRAME_STATE_VARIABLE_LAST_TOKEN       = APV_GENERIC_STATE_VARIABLE_3,
  APV_MESSAGE_FRAME_STATE_VARIABLE_CRC_SUM          = APV_GENERIC_STATE_VARIABLE_4,
  APV_MESSAGE_FRAME_STATE_RING_BUFFER_LOW           = APV_GENERIC_STATE_VARIABLE_5,
  APV_MESSAGE_FRAME_STATE_RING_BUFFER_HIGH          = APV_GENERIC_STATE_VARIABLE_6,
  APV_MESSAGE_FRAME_STATE_MESSAGE_BUFFER_LOW        = APV_GENERIC_STATE_VARIABLE_7,
  APV_MESSAGE_FRAME_STATE_MESSAGE_BUFFER_HIGH       = APV_GENERIC_STATE_VARIABLE_8,
  APV_MESSAGE_FRAME_STATE_PAYLOAD_LENGTH            = APV_GENERIC_STATE_VARIABLE_9,
  APV_MESSAGE_FRAME_STATE_FRAME_CHECK               = APV_GENERIC_STATE_VARIABLE_10,
  APV_MESSAGE_FRAME_STATE_FREE_MESSAGE_BUFFERS_LOW  = APV_GENERIC_STATE_VARIABLE_11,
  APV_MESSAGE_FRAME_STATE_FREE_MESSAGE_BUFFERS_HIGH = APV_GENERIC_STATE_VARIABLE_12,
  APV_MESSAGE_FRAME_STATE_VARIABLE_CACHE_LENGTH     = APV_GENERIC_STATE_VARIABLE_CACHE_LENGTH
  } apvMessagingFrameStateVariable_t;

typedef APV_GENERIC_STATE_CODE APV_MESSAGING_STATE_CODE;

typedef uint32_t apvMessageStateVariable_t;     // state-variable type
typedef uint32_t apvMessageStateEntryPointer_t; // state-entry pointer type

// A set of variables to carry information between states
typedef struct apvMessagingStateVariables_tTag
  {
  apvMessageStateVariable_t apvMessageStateVariables[APV_MESSAGE_STATE_VARIABLE_CACHE_LENGTH];
  } apvMessagingStateVariables_t;

// Holds the state of an in-progress attempt to de-frame a low-level message
typedef struct apvMessagingDeFramingState_tTag
  {
  apvMessagingFrameStates_t      apvMessageCurrentState;
  apvMessagingFrameStates_t      apvMessageNextState;
  APV_MESSAGING_STATE_CODE     (*apvMessageStateAction)(struct apvMessagingDeFramingState_tTag *apvMessagingStateMachine);
  apvMessagingStateVariables_t  *apvMessageStateVariables;
  } apvMessagingDeFramingState_t;

typedef        apvMessagingDeFramingState_t     APV_MESSAGING_DEFRAMING_STATE;
typedef struct apvMessagingDeFramingState_tTag *APV_MESSAGING_DEFRAMING_STATE_S;

typedef enum apvMessageSuccessCounters_tTag
  {
  APV_MESSAGE_SUCCESS_COUNTER = 0,
  APV_MESSAGE_FAILURE_COUNTER,
  APV_MESSAGE_SUCCESS_COUNTERS
  } apvMessageSuccessCounters_t;

/******************************************************************************/
/* Local Variables :                                                          */
/******************************************************************************/

extern apvMessagingDeFramingState_t apvMessagingDeFramingStateMachine[APV_MESSAGE_FRAME_STATES];

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

// The message-handling state-machine
extern apvMessagingDeFramingState_t *apvMessageDeFramingStateMachine;

// The "free" list ofserial UART message buffers
extern apvRingBuffer_t               apvMessageFreeBufferSet;
extern apvMessageStructure_t         apvMessageFreeBuffers[APV_MESSAGE_FREE_BUFFER_SET_SIZE];

extern uint32_t                      apvMessageSuccessCounters[APV_MESSAGE_SUCCESS_COUNTERS];

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern bool           apvMessageFramerCheckCommsPlane(apvCommsPlanes_t commsPlane);
extern bool           apvMessageFramerCheckSignalPlane(apvSignalPlanes_t signalPlane);
extern APV_ERROR_CODE apvMessageStructureInitialisation(apvMessageStructure_t *messageStructure,
                                                        uint16_t               messagePayloadMaximumLength);

extern APV_ERROR_CODE apvCreateMessageBuffers(apvRingBuffer_t       *apvMessageBufferSet,
                                              apvMessageStructure_t *apvMessageBuffers,
                                              uint32_t               apvMessageBufferSetSize);

extern APV_ERROR_CODE apvFrameMessage(apvMessageStructure_t *messageStructure,
                                      apvCommsPlanes_t       inBoundCommsPlane,
                                      apvSignalPlanes_t      inBoundSignalPlane,
                                      apvCommsPlanes_t       outBoundCommsPlane,
                                      apvSignalPlanes_t      outBoundSignalPlane,
                                      uint8_t               *message,
                                      uint16_t               messageLength,
                                      uint16_t              *messageTotalLength);
extern APV_MESSAGING_STATE_CODE apvDeFrameMessageInitialisation(apvRingBuffer_t              *ringBuffer,
                                                                apvRingBuffer_t              *messageFreeBuffers,
                                                                apvMessagingDeFramingState_t *messageState);
extern APV_MESSAGING_STATE_CODE apvDeFrameMessage(apvMessagingDeFramingState_t *messageStateMachine);

extern void                     apvMessageStructurePrint(apvMessagingDeFramingState_t *stateMachine);

/******************************************************************************/
/* Message de-framing state machine functions :                               */
/******************************************************************************/

extern APV_MESSAGING_STATE_CODE apvMessageDeFramingNull(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessageDeFramingInitialise(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessageDeFramingInBoundSignalAndLogicalPlanes(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessageDeFramingOutBoundSignalAndLogicalPlanes(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessageDeFramingMessageLength(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessageDeFramingDataByte(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessagingDeFramingCrcBytes(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessagingDeFramingCrcCheck(apvMessagingDeFramingState_t *messageStateMachine);
extern APV_MESSAGING_STATE_CODE apvMessagingDeFramingReporter(apvMessagingDeFramingState_t *messageStateMachine);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

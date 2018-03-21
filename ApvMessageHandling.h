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

#ifndef _APV_MESSAGE_HANDLING_H_
#define _APV_MESSAGE_HANDLING_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_MESSAGING_START_OF_MESSAGE                 ((uint8_t)0x7e)                // SOM
#define APV_MESSAGING_END_OF_MESSAGE                   APV_MESSAGING_START_OF_MESSAGE // EOM

// Communication physical and logical planes are encapsulated in one token (byte)
#define APV_COMMS_PLANE_FIELD_BITS                     (4) // apvCommsPlanes_t
#define APV_SIGNAL_PLANE_FIELD_BITS                    (4) // apvSignalPlanes_t

#define APV_MESSAGING_MINIMUM_UNSTUFFED_MESSAGE_LENGTH (1) // 1
#define APV_MESSAGING_MAXIMUM_UNSTUFFED_MESSAGE_LENGTH ((APV_MESSAGING_START_OF_MESSAGE - 1) >> 1) // 62
#define APV_MESSAGING_MAXIMUM_STUFFED_MESSAGE_LENGTH   (APV_MESSAGING_MAXIMUM_UNSTUFFED_MESSAGE_LENGTH << 1)
#define APV_MESSAGING_STUFFING_FLAG                    (0x1b) // using the ASCII 'ESCAPE' character for token (byte)-stuffing

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/
/* This is the complete definition of an APV project message link frame. The  */
/* token ordering is non-arbitrary!                                           */
/******************************************************************************/

// Definitions of physical communication "planes"
typedef enum apvCommsPlanes_tTag
  {
  APV_COMMS_PLANE_SERIAL_UART = 0, /* 0 */
  APV_COMMS_PLANE_SERIAL_USART_0,  /* 1 */
  APV_COMMS_PLANE_SERIAL_USART_1,  /* 2 */
  APV_COMMS_PLANE_SERIAL_USART_2,  /* 3 */
  APV_COMMS_PLANE_SERIAL_USART_3,  /* 4 */
  APV_COMMS_PLANE_SPI_RADIO_0,     /* 5 */
  APV_COMMS_PLANE_SPI_RADIO_1,     /* 6 */
  APV_COMMS_PLANE_SPI_RADIO_2,     /* 7 */
  APV_COMMS_PLANE_SPI_RADIO_3,     /* 8 */
  APV_COMMS_PLANE_I2C_ANON,        /* 9 */
  APV_COMMS_PLANE_ANON_0,         /* 10 */
  APV_COMMS_PLANE_ANON_1,         /* 11 */
  APV_COMMS_PLANE_ANON_2,         /* 12 */
  APV_COMMS_PLANE_ANON_3,         /* 13 */
  APV_COMMS_PLANE_ANON_4,         /* 14 */
  APV_COMMS_PLANE_ANON_5,         /* 15 */
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
  APV_SIGNAL_PLANE_DATA_3,        /* 7 */
  APV_SIGNAL_PLANE_CONTROL_4,     /* 8 */
  APV_SIGNAL_PLANE_DATA_4,        /* 9 */
  APV_SIGNAL_PLANE_CONTROL_5,    /* 10 */
  APV_SIGNAL_PLANE_DATA_5,       /* 11 */
  APV_SIGNAL_PLANE_CONTROL_X_0,  /* 12 */
  APV_SIGNAL_PLANE_DATA_X_0,     /* 13 */
  APV_SIGNAL_PLANE_CONTROL_X_1,  /* 14 */
  APV_SIGNAL_PLANE_DATA_X_1,     /* 15 */
  APV_SIGNAL_PLANES
  } apvSignalPlanes_t;

// Definition of the communication "planes" frame token
typedef struct apvMessagePlanes_tTag
  {
  uint8_t apvCommsPlane : APV_COMMS_PLANE_FIELD_BITS;  // bits 0 - 3
  uint8_t apvDataPlane  : APV_SIGNAL_PLANE_FIELD_BITS; // bits 4 - 7
  } apvMessagePlanes_t;

typedef union apvMessagePlanesToken_tTag
  {
  apvMessagePlanes_t apvMessagePlanes;
  uint8_t            apvMessagePlanesToken;
  } apvMessagePlanesToken_t;

typedef struct apvMessageStructure_tTag
  {
  uint8_t                 apvMessagingStartOfMessageToken;
  uint8_t                 apvMessagingLengthOfMessage;
  apvMessagePlanesToken_t apvMessagingPlanesToken;
  uint8_t                 apvMessagingPayload[APV_MESSAGING_MAXIMUM_STUFFED_MESSAGE_LENGTH];
  uint8_t                 apvMessagingCrcHighToken;
  uint8_t                 apvMessagingCrcLowToken;
  uint8_t                 apvMessagingEndOfMessageToken;
  } apvMessageStructure_t;

// For convenience in message handling alias to an array
typedef union apvMessageFrame_tTag
  {
  apvMessageStructure_t apvMessageStructure;
  uint8_t               apvMessageFrame[sizeof(apvMessageStructure_t)];
  } apvMessageFrame_t;

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
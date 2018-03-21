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

#include "ApvMessageHandling.h"

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/

/******************************************************************************/
/* Static Variables :                                                         */
/******************************************************************************/

apvMessageStructure_t  apvMessagePool;
apvMessageStructure_t *apvMessageRoot = &apvMessagePool;

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMessages.h                                                              */
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

#ifndef _APV_MESSAGES_H_
#define _APV_MESSAGES_H_

/******************************************************************************/
/* Includes :                                                                 */
/******************************************************************************/

#include <stdint.h>
#include "ApvError.h"
#include "scl.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_MESSAGE_MAXIMUM_NUMBER_OF_FRAGMENTS APV_MESSAGE_FRAGMENT_INDICES
#define APV_MESSAGE_MAXIMUM_FRAGMENT_LENGTH     (FLOAT64_MAXIMUM_NUMBER_SIZE + 1) // maximum size is a 64-bit number with a string ending

#define APV_BIPOLAR_CODE_BITS                                8 // representational of [ '1' == '0' | '-1' == '1' ]
#define APV_BIPOLAR_CODE_MASK                   ((uint8_t)0x01)

/******************************************************************************/
/* Command Message Prefixes :                                                 */
/******************************************************************************/

#define APV_MESSAGE_REQUEST_SIGN_ON_PREFIX    "RqSignOn"
#define APV_MESSAGE_REQUEST_SIGN_ON_FRAGMENTS 1

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

/******************************************************************************/
/* Message fragments are part of the message payload and hence errors are     */
/* !detected! by the CRC. To try to !prevent! fragment code errors the codes  */
/* are in a code-redundant 'Hadamard' format                                  */
/******************************************************************************/

typedef enum apvMessageFragmentIndices_tTag
  {
  APV_MESSAGE_FRAGMENT_INDEX_0 = ((uint8_t)0xff),
  APV_MESSAGE_FRAGMENT_INDEX_1 = ((uint8_t)0xaa),
  APV_MESSAGE_FRAGMENT_INDEX_2 = ((uint8_t)0xcc),
  APV_MESSAGE_FRAGMENT_INDEX_3 = ((uint8_t)0x99),
  APV_MESSAGE_FRAGMENT_INDEX_4 = ((uint8_t)0xf0),
  APV_MESSAGE_FRAGMENT_INDEX_5 = ((uint8_t)0xa5),
  APV_MESSAGE_FRAGMENT_INDEX_6 = ((uint8_t)0xc3),
  APV_MESSAGE_FRAGMENT_INDEX_7 = ((uint8_t)0x95),
  APV_MESSAGE_FRAGMENT_INDICES
  } apvMessageFragmentIndices_t;

typedef enum apvMessageCommandResponseType_tTag
  {
  APV_MESSAGE_COMMAND_ACK_TYPE = 0,
  APV_MESSAGE_COMMAND_RESPONSE_TYPE = 0,
  APV_MESSAGE_RESPONSE_TYPE,
  APV_MESSAGE_TYPES
  } apvMessageCommandResponseType_t;

typedef enum apvMessageFragmentType_tTag
  {
  APV_MESSAGE_FRAGMENT_TYPE_PREFIX = 0,      // execute-instruction
  APV_MESSAGE_FRAGMENT_TYPE_SYMBOL,          // indirection-address
  APV_MESSAGE_FRAGMENT_TYPE_STATIC_NUMBER,   // common-numeric-format-constant-number
  APV_MESSAGE_FRAGMENT_TYPE_VARIABLE_NUMBER, // common-numeric-format-variable-number
  APV_MESSAGE_FRAGMENT_TYPE_TEXT,            // message-seperators
  APV_MESSAGE_FRAGMENT_TYPES
  } apvMessageFragmentType_t;

typedef enum apvMessageFragmentFix_tTag
  {
  APV_MESSAGE_FRAGMENT_FIX_STATIC = 0,
  APV_MESSAGE_FRAGMENT_FIX_VARIABLE,
  APV_MESSAGE_FRAGMENT_FIXES
  } apvMessageFragmentFix_t;

typedef struct apvMessageFragment_tTag
  {
  char                        apvMessageFragment[APV_MESSAGE_MAXIMUM_FRAGMENT_LENGTH]; // a fragment is one field of the total message
  apvMessageFragmentType_t    apvMessageFragmentType;
  apvMessageFragmentFix_t     apvMessageFragmentFix;
  apvMessageFragmentIndices_t apvMessageFragmentCode;
  } apvMessageFragment_t;

typedef struct apvMessageFormat_tTag
  {
  apvMessageCommandResponseType_t apvMessageCommandResponseType;
  apvMessageFragment_t            apvMessageFragment[APV_MESSAGE_MAXIMUM_NUMBER_OF_FRAGMENTS]; // the number of message fragments is bounded by this message definition
  uint8_t                         numberOfMessageFragments;                                    // the number of fragments to build this message from
  APV_ERROR_CODE                  (*apvMessageInBoundAction)(void  *messageFragments);
  APV_ERROR_CODE                  (*apvMessageOutBoundAction)(void *messageFragments);
  } apvMessageFormat_t;

typedef enum apvUniPolarCodes_tTag
  {
  APV_POLAR_CODE_NEGATIVE = 0, // unipolar '0'
  APV_POLAR_CODE_POSITIVE,     // unipolar '1'
  APV_POLAR_CODES
  } apvUniPolarCodes_t;

typedef enum apvBiPolarCodes_tTag
  {
  APV_BIPOLAR_CODE_POSITIVE = -1, // unipolar '1' == '-1'
  APV_BIPOLAR_CODE_NEGATIVE =  1, // unipolar '0' == '1'
  APV_BIPOLAR_CODES
  } apvBiPolarCodes_t;

typedef struct apvBiPolarCode_tTag
  {
  int8_t apvBiPolarCodeBits[APV_BIPOLAR_CODE_BITS];
  } apvBiPolarCode_t;

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
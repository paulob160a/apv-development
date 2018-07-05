/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvControlPortProtocol.h                                                   */
/* 26.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - manager for the board serial communications                              */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_CONTROL_PORT_PROTOCOL_H_
#define _APV_CONTROL_PORT_PROTOCOL_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include "ApvSerial.h"
#include "stdint.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_SIGN_ON_MESSAGE_MAXIMUM_LENGTH APV_SERIAL_BUFFER_MAXIMUM_LENGTH

#define APV_SIGN_ON_MESSAGE                \
"APV Primary Control Protocol \r\
----------------------------\r\n"

#define APV_COMMAND_PROTOCOL_MESSAGE_COMMAND_SIGN_ON           "APV_SIGN_ON"
#define APV_COMMAND_PROTOCOL_MESSAGE_RESPONSE_SIGN_ON          "APV Primary Control Protocol"

#define APV_COMMAND_PROTOCOL_MESSAGE_DEFINITIONS                1 // keep this in sync with the defined messages

#define APV_COMMAND_PROTOCOL_MESSAGE_IDENTIFIER_MAXIMUM_LENGTH 32 // not quite arbritrary
#define APV_COMMAND_PROTOCOL_MESSAGE_MAXIMUM_FIELDS             4 // wholly arbitrary!

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvCommandProtocolFieldType_tTag
  {
  APV_COMMAND_PROTOCOL_FIELD_TYPE_TEXT = 0,
  APV_COMMAND_PROTOCOL_FIELD_TYPE_FLOAT64,
  APV_COMMAND_PROTOCOL_FIELD_TYPES
  } apvCommandProtocolFieldType_t;

typedef enum apvCommandProtocolMessageFieldValues_tTag
  {
  APV_COMMAND_PROTOCOL_MESSAGE_FIELD_NOMINAL = 0,
  APV_COMMAND_PROTOCOL_MESSAGE_FIELD_MINIMUM,
  APV_COMMAND_PROTOCOL_MESSAGE_FIELD_MAXIMUM,
  APV_COMMAND_PROTOCOL_MESSAGE_FIELD_VALUES
  } apvCommandProtocolMessageFieldValues_t;

typedef union apvCommandProtocolMessageField_tTag
  {
  char     apvCommandProtocolText[APV_COMMAND_PROTOCOL_MESSAGE_IDENTIFIER_MAXIMUM_LENGTH];
  uint64_t apvCommandProtocolValues[APV_COMMAND_PROTOCOL_MESSAGE_FIELD_VALUES];
  } apvCommandProtocolMessageField_t;

typedef struct apvCommandProtocolField_tTag
  {
  apvCommandProtocolFieldType_t    apvCommandProtocolFieldType;
  apvCommandProtocolMessageField_t apvCommandProtocolField;
  } apvCommandProtocolField_t;

typedef struct apvCommandProtocolDefinition_tTag
  {
  apvCommandProtocolField_t    apvCommandProtocolFields[APV_COMMAND_PROTOCOL_MESSAGE_MAXIMUM_FIELDS];
  char                         commandProtocolMessageResponse[APV_COMMAND_PROTOCOL_MESSAGE_IDENTIFIER_MAXIMUM_LENGTH];   // the response text IF ANY
  void                      *(*commandProtocolMessageAction)(void *messageAction);                                       // the action - using "void *" allows any manner of input to operate on 
                                                                                                                         // and output to generate - IF ANY
  } apvCommandProtocolDefinition_t; 

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern uint8_t apvSignOnMessage[APV_SIGN_ON_MESSAGE_MAXIMUM_LENGTH];

extern apvCommandProtocolDefinition_t apvCommandProtocol[APV_COMMAND_PROTOCOL_MESSAGE_DEFINITIONS];

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvControlPortSignOn(uint8_t  *apvSignOMessage,
                                           uint16_t  apvSignOnMessageLength);

extern bool           apvStringCompare(char     *templateString,
                                       uint16_t  templateStringOffset,
                                       uint16_t  templateStringLength,
                                       char     *examinedString,
                                       uint16_t  examinedStringOffset,
                                       char      matchingTerminator,
                                       bool      matchingMagic);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
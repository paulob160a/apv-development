/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvError.h                                                                 */
/* 21.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - error codes and handlers for the whole application                       */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_ERROR_H_
#define _APV_ERROR_H_

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_MESSAGE_BUFFER_EMPTY       ((uint16_t)0)

// Error code group offsets
#define APV_SERIAL_ERROR_CODE_TRANCHE 256

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvGlobalErrorFlags_tTag
  {
  APV_GLOBAL_ERROR_FLAG_NONE = 0,
  APV_GLOBAL_ERROR_FLAG_FUNCTION_INTERRUPT_NESTING,
  APV_GLOBAL_ERROR_FLAGS
  } apvGlobalErrorFlags_t;

typedef apvGlobalErrorFlags_t APV_GLOBAL_ERROR_FLAG;

typedef enum apvErrorCodes_tTag
  {
  APV_ERROR_CODE_NONE = 0,
  APV_SERIAL_ERROR_CODE_NONE = APV_ERROR_CODE_NONE,
  APV_ERROR_CODE_MESSAGE_BUFFER_FAULTY,
  APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR,
  APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR,
  APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE,
  APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR,
  APV_ERROR_CODE_NULL_PARAMETER,
  APV_ERROR_CODE_CONFIGURATION_ERROR,
  APV_SERIAL_ERROR_CODE_PRIMARY_SERIAL_PORT_ALREADY_ASSIGNED = APV_SERIAL_ERROR_CODE_TRANCHE,
  APV_SERIAL_ERROR_CODE_TRANSMITTER_NOT_READY,
  APV_ERROR_CODES
  } apvErrorCodes_t;

typedef apvErrorCodes_t APV_ERROR_CODE;
typedef apvErrorCodes_t APV_SERIAL_ERROR_CODE;

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
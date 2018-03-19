/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvSerial.h                                                                */
/* 19.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - manager for the board serial communications                              */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_SERIAL_H_
#define _APV_SERIAL_H_

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvErrorCodes_tTag
  {
  APV_ERROR_CODE_NONE = 0,
  APV_ERROR_CODE_PRIMARY_SERIAL_PORT_ALREADY_ASSIGNED,
  APV_ERROR_CODES
  } apvErrorCodes_t;

typedef apvErrorCodes_t APV_ERROR_CODE;

typedef enum apvPrimarySerialPort_tTag
  {
  APV_PRIMARY_SERIAL_PORT_USART0 = 0,
  APV_PRIMARY_SERIAL_PORT_USART1,
  APV_PRIMARY_SERIAL_PORT_USART2,
  APV_PRIMARY_SERIAL_PORT_USART3,
  APV_PRIMARY_SERIAL_PORT_UART,
  APV_PRIMARY_SERIAL_PORTS
  } apvPrimarySerialPort_t;

typedef apvPrimarySerialPort_t APV_PRIMARY_SERIAL_PORT;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort);
extern void           apvPrimarySerialCommsHandler(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
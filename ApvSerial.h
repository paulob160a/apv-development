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
/* Include Files :                                                            */
/******************************************************************************/

#include <stdbool.h>
#include "ApvError.h"

/******************************************************************************/
/* Constant Definitions :                                                     */
/******************************************************************************/

#define APV_SERIAL_BUFFER_MAXIMUM_LENGTH 256 // a simple buffer for 256 x uint8_t

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

/* typedef enum apvSerialErrorCodes_tTag
  {
  APV_SERIAL_ERROR_CODE_NONE = 0,
  APV_SERIAL_ERROR_CODE_PRIMARY_SERIAL_PORT_ALREADY_ASSIGNED = APV_SERIAL_ERROR_CODE_TRANCHE,
  APV_SERIAL_ERROR_CODE_TRANSMITTER_NOT_READY,
  APV_SERIAL_ERROR_CODES
  } apvSerialErrorCodes_t; */

typedef apvErrorCodes_t apvSerialErrorCodes_t;

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

// This is a simplified serial transmit structuer for basic transmission and 
// testing
typedef struct apvSerialTransmitBuffer_tTag
  {
  uint8_t  serialTransmitBuffer[APV_SERIAL_BUFFER_MAXIMUM_LENGTH];
  uint16_t serialTransmitBufferLength;
  uint16_t serialTransmitMessageLength;
  uint16_t serialTransmitBufferIndex;
  } apvSerialTransmitBuffer_t;

/******************************************************************************/
/* These variables are only intended to implement a simple foreground/back-   */
/* ground loopback                                                            */
/******************************************************************************/

extern volatile apvSerialTransmitBuffer_t  transmitBuffer,
                                          *transmitBuffer_p;
extern volatile uint8_t                    receiveBuffer;
  
extern volatile bool                       transmitInterrupt;
extern volatile bool                       receiveInterrupt;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_SERIAL_ERROR_CODE apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort);
extern void                  apvPrimarySerialCommsHandler(APV_PRIMARY_SERIAL_PORT apvPrimarySerialPort);
extern APV_SERIAL_ERROR_CODE apvSerialBufferInitialise(volatile apvSerialTransmitBuffer_t *serialBuffer,
                                                                uint16_t                   serialBufferLength,
                                                       const    char                      *transmitPhrase,
                                                                uint16_t                   transmitPhraseLength);
extern APV_ERROR_CODE        apvUartCharacterTransmitPrime(Uart                      *uartControlBlock,
                                                           apvSerialTransmitBuffer_t *transmitBuffer);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
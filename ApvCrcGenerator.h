/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvCrcGenerator.c                                                          */
/* 20.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - the parts of a CCITT-specification CRC16 generator. Thanks to Jonas      */
/*   Eriksson for his table generator code at :                               */
/*    "http://github.com/zqad/crc16-ccitt"                                    */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_CRC_GENERATOR_
#define _APV_CRC_GENERATOR_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include "ApvError.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

// APV uses the CCITT-CRC16 error detection specification
#define APV_CRC_GENERATOR_POLYNOMIAL    ((uint16_t)0x1021)
#define APV_CRC_GENERATOR_INITIAL_VALUE ((uint16_t)0xFFFF)
#define APV_CRC_GENERATOR_FINAL_VALUE   ((uint16_t)0x0000)
#define APV_CRC_GENERATOR_CHECKSUM      ((uint16_t)0x2189)

#define APV_CRC_BYTE_WIDTH              8
#define APV_CRC_WORD_WIDTH              2
#define APV_CRC_BYTE_SIZE               (sizeof(uint8_t)  * APV_CRC_BYTE_WIDTH)
#define APV_CRC_WORD_SIZE               (sizeof(uint16_t) * APV_CRC_BYTE_WIDTH)

#define APV_CRC_HIGH_BYTE_SHIFT         (APV_CRC_WORD_SIZE - APV_CRC_BYTE_SIZE)
#define APV_CRC_LOW_BYTE_SHIFT          APV_CRC_BYTE_SIZE

#define APV_CRC_BYTE_MASK               ((uint16_t)0x000000FF)
#define APV_CRC_MASK_SHIFT              8

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern void           apvComputeCrc(uint8_t newByte, uint16_t *crc);
extern APV_ERROR_CODE apvBlockComputeCrc(uint8_t *messageBuffer, uint16_t messageBufferLength, uint16_t *crc);

/******************************************************************************/

#endif

/******************************************************************************/

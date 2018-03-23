/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvCommsUtilities.h                                                        */
/* 22.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - a set of functions to add higher-level communications capabilities       */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_COMMS_UTILITIES_
#define _APV_COMMS_UTILITIES_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>
#include "ApvError.h"
#ifdef WIN32
#include "stdbool.h"
#else
#include <stdbool.h>
#endif

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH  (64) // for now, but can be up to 65536
#define APV_COMMS_RING_BUFFER_MINIMUM_LENGTH   (2)

#define APV_COMMS_LSB_LEADING_BIT_MASK        (0x1) // little-endian
#define APV_COMMS_BYTE_WIDTH                    (8)
#define APV_COMMS_MSB_LEADING_BIT_MASK(iType) ((iType)(1 << ((sizeof(iType) * APV_COMMS_BYTE_WIDTH) - 1)))

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef struct apvRingBuffer_tTag
  {
  uint16_t  apvCommsRingBuffer[APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH];
  uint16_t  apvCommsRingBufferLength; // the actual ring-buffer length required
  uint16_t *apvCommsRingBufferHead;   // pointer to the "next" free buffer slot
  uint16_t *apvCommsRingBufferTail;   // pointer to the "last" full buffer slot
  uint16_t *apvCommsRingBufferStart;  // pointer to the start of the ring-buffer
  uint16_t *apvCommsRingBufferEnd;    // pointer to the end of the ring-buffer
  uint16_t  apvCommsRingBufferLoad;   // the number of "full" buffer slots
  } apvRingBuffer_t;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvRingBufferInitialise(apvRingBuffer_t *ringBuffer,
                                              uint16_t         ringBufferLength);
extern uint16_t apvRingBufferLoad(apvRingBuffer_t *ringBuffer,
                                  uint8_t         *tokens,
                                  uint16_t         numberOfTokensToLoad,
                                  bool             interruptControl);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
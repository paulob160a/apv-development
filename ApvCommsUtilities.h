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

#define APV_RING_BUFFER_INITIALISATION_TOKEN  '*'
#define APV_RING_BUFFER_INITIALISATION_FLAG   ((apvRingBufferSlotWidth)((APV_RING_BUFFER_INITIALISATION_TOKEN <  0) + \
                                                                        (APV_RING_BUFFER_INITIALISATION_TOKEN <  8) + \
                                                                        (APV_RING_BUFFER_INITIALISATION_TOKEN < 16) + \
                                                                        (APV_RING_BUFFER_INITIALISATION_TOKEN < 24)))

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef uint32_t apvRingBufferSlotWidth; // the ring-buffer slots are generic i.e. 
                                         // 32-bits wide so they can hold full-
                                         // range memory pointers if need be

typedef struct apvRingBuffer_tTag
  {
  apvRingBufferSlotWidth  apvCommsRingBuffer[APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH];
  uint16_t                apvCommsRingBufferLength; // the actual ring-buffer length required
  apvRingBufferSlotWidth *apvCommsRingBufferHead;   // pointer to the "next" free buffer slot
  apvRingBufferSlotWidth *apvCommsRingBufferTail;   // pointer to the "last" full buffer slot
  apvRingBufferSlotWidth *apvCommsRingBufferStart;  // pointer to the start of the ring-buffer
  apvRingBufferSlotWidth *apvCommsRingBufferEnd;    // pointer to the end of the ring-buffer
  uint16_t                apvCommsRingBufferLoad;   // the number of "full" buffer slots
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
extern uint16_t apvRingBufferUnLoad(apvRingBuffer_t *ringBuffer,
                                    uint8_t         *tokens,
                                    uint16_t         numberOfTokensToUnLoad,
                                    bool             interruptControl);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
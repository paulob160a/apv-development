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

#define APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH  (256) // for now, but can be up to 65536
#define APV_COMMS_RING_BUFFER_MINIMUM_LENGTH    (2)

#define APV_COMMS_LSB_LEADING_BIT_MASK        (0x1) // little-endian
#define APV_COMMS_BYTE_WIDTH                    (8)
#define APV_COMMS_MSB_LEADING_BIT_MASK(iType) ((iType)(1 << ((sizeof(iType) * APV_COMMS_BYTE_WIDTH) - 1)))

#define APV_COMMS_RANDOM_TRIGGER_MASK         ((uint8_t)0x13) // this sequence triggers a random <SOM> insertion

#define APV_RING_BUFFER_INITIALISATION_TOKEN  '*'
#define APV_RING_BUFFER_INITIALISATION_FLAG   ((apvRingBufferSlotWidth_t)((APV_RING_BUFFER_INITIALISATION_TOKEN <  0) + \
                                                                          (APV_RING_BUFFER_INITIALISATION_TOKEN <  8) + \
                                                                          (APV_RING_BUFFER_INITIALISATION_TOKEN < 16) + \
                                                                          (APV_RING_BUFFER_INITIALISATION_TOKEN < 24)))

#define APV_MESSAGING_START_OF_MESSAGE                 ((uint8_t)0x7e)                // SOM
#define APV_MESSAGING_END_OF_MESSAGE                   APV_MESSAGING_START_OF_MESSAGE // EOM
#define APV_MESSAGING_STUFFING_FLAG                    (0x1b) // using the ASCII 'ESCAPE' character for token (byte)-stuffing

// Marks a ring-buffer as currently in use in the nominal "free" list
#define APV_RING_BUFFER_LIST_EMPTY_POINTER_CODE        ((uint32_t)0xffffffff)
#define APV_RING_BUFFER_LIST_EMPTY_POINTER             ((apvRingBuffer_t *)(APV_RING_BUFFER_LIST_EMPTY_POINTER_CODE))

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef uint32_t apvRingBufferSlotWidth_t; // the ring-buffer slots are generic i.e. 
                                           // 32-bits wide so they can hold full-
                                           // range memory pointers if need be

typedef struct apvRingBuffer_tTag
  {
  apvRingBufferSlotWidth_t  apvCommsRingBuffer[APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH];
  uint16_t                  apvCommsRingBufferLength; // the actual ring-buffer length required
  apvRingBufferSlotWidth_t *apvCommsRingBufferHead;   // pointer to the "next" free buffer slot
  apvRingBufferSlotWidth_t *apvCommsRingBufferTail;   // pointer to the "last" full buffer slot
  apvRingBufferSlotWidth_t *apvCommsRingBufferStart;  // pointer to the start of the ring-buffer
  apvRingBufferSlotWidth_t *apvCommsRingBufferEnd;    // pointer to the end of the ring-buffer
  uint16_t                  apvCommsRingBufferLoad;   // the number of "full" buffer slots
  } apvRingBuffer_t;

typedef uint64_t ringBufferEntryPointer_t;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvRingBufferSetInitialise(apvRingBuffer_t **ringBufferIndirectSet,
                                                 apvRingBuffer_t  *ringBufferSet,
                                                 uint16_t          ringBufferSetElements,
                                                 uint16_t          ringBufferLength);
extern APV_ERROR_CODE apvRingBufferSetPullBuffer(apvRingBuffer_t **ringBufferIndirectSet,
                                                 apvRingBuffer_t **ringBuffer,
                                                 uint16_t          ringBufferSetElements,
                                                 bool              interruptControl);
extern APV_ERROR_CODE apvRingBufferSetPushBuffer(apvRingBuffer_t **ringBufferIndirectSet,
                                                 apvRingBuffer_t  *ringBuffer,
                                                 uint16_t          ringBufferLength,
                                                 uint16_t          ringBufferSetElements,
                                                 bool              interruptControl);
extern APV_ERROR_CODE apvRingBufferInitialise(apvRingBuffer_t *ringBuffer,
                                              uint16_t         ringBufferLength);
extern uint16_t       apvRingBufferLoad(apvRingBuffer_t *ringBuffer,
                                        uint32_t        *tokens,
                                        uint16_t         numberOfTokensToLoad,
                                        bool             interruptControl);
extern uint16_t       apvRingBufferUnLoad(apvRingBuffer_t *ringBuffer,
                                          uint32_t        *tokens,
                                          uint16_t         numberOfTokensToUnLoad,
                                          bool             interruptControl);
extern APV_ERROR_CODE apvCreateTestMessage(uint8_t  *testMessage,
                                           uint16_t  testMessageSomLength,
                                           uint16_t *testMessagePayLoadLength,
                                           uint16_t *testMessageLength,
                                           bool      testMessageInsertSom,
                                           uint16_t  testMessageSoms,
                                           bool      testMessageFault,
                                           bool      testMessageSomFault);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvUtilities.h                                                             */
/* 22.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - a set of functions to add some helpers                                   */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_UTILITIES_
#define _APV_UTILITIES_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>
#ifndef WIN32
#include <__armlib.h>
#endif
#include "ApvError.h"
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_INITIAL_INTERRUPT_NESTING_COUNT ((int16_t)0)

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvPointerConversionWords_tTag
  {
  APV_POINTER_CONVERSION_WORD_LOW = 0,
  APV_POINTER_CONVERSION_WORD_HIGH,
  APV_POINTER_CONVERSION_WORDS
  } apvPointerConversionWords_t;

typedef union apvPointerConversion_tTag
  {
  uint32_t apvPointerConversionWords[APV_POINTER_CONVERSION_WORDS]; // map the low-word to [0], high-word to [1]
  uint64_t apvPointerConversion;
  } apvPointerConversion_t;

typedef enum apvInterruptCounters_tTag
  {
  APV_TRANSMIT_INTERRUPT_COUNTER = 0,
  APV_RECEIVE_INTERRUPT_COUNTER,
  APV_INTERRUPT_COUNTERS
  } apvInterruptCounters_t;

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

extern int16_t                apvInterruptNestingCount;
extern APV_GLOBAL_ERROR_FLAG  apvGlobalErrorFlags;
extern uint32_t               apvInterruptCounters[APV_INTERRUPT_COUNTERS];

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern void APV_CRITICAL_REGION_ENTRY(void);
extern void APV_CRITICAL_REGION_EXIT(void);
extern void apvRingBufferPrint(apvRingBuffer_t *ringBuffer);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
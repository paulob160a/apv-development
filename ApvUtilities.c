/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvUtilities.c                                                             */
/* 22.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - a set of functions to add some helpers                                   */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#ifdef WIN32
#define libarm_disable_irq() //printf("\n DISABLE INTERRUPTS : NESTING LEVEL %05d", apvInterruptNestingCount);
#define libarm_enable_irq()  //printf("\n ENABLE INTERRUPTS  : NESTING LEVEL %05d", apvInterruptNestingCount);
#else
#include <sam3x8e.h>
#include <core_cm3.h>
#endif
#include "ApvUtilities.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

int16_t                apvInterruptNestingCount                     = APV_INITIAL_INTERRUPT_NESTING_COUNT;
APV_GLOBAL_ERROR_FLAG  apvGlobalErrorFlags                          = APV_GLOBAL_ERROR_FLAG_NONE;
uint32_t               apvInterruptCounters[APV_INTERRUPT_COUNTERS] = { 0x00000000, 0x00000000 };

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/
/* APV_CRITICAL_REGION_ENTRY() :                                              */
/*  - lock a region of code where one or more writers and readers have access */
/*    to the same memory region. Maintains a function-nesting count to        */
/*    determine when to safely release the lock                               */
/*                                                                            */
/******************************************************************************/

void APV_CRITICAL_REGION_ENTRY(void)
  {
/******************************************************************************/

  if (apvInterruptNestingCount <  APV_INITIAL_INTERRUPT_NESTING_COUNT)
    {
    apvGlobalErrorFlags = (APV_GLOBAL_ERROR_FLAG)(apvGlobalErrorFlags | APV_GLOBAL_ERROR_FLAG_FUNCTION_INTERRUPT_NESTING);
    }
  else
    {
    if (apvInterruptNestingCount >= APV_INITIAL_INTERRUPT_NESTING_COUNT)
      {
      __disable_irq(); // the locking mechanism
      }

    apvInterruptNestingCount = apvInterruptNestingCount + 1;
    }

/******************************************************************************/
  } /* end of APV_CRITICAL_REGION_ENTRY                                       */

/******************************************************************************/
/* APV_CRITICAL_REGION_EXIT() :                                               */
/*  - lock a region of code where one or more writers and readers have access */
/*    to the same memory region. Maintains a function-nesting count to        */
/*    determine when to safely release the lock                               */
/*                                                                            */
/******************************************************************************/

void APV_CRITICAL_REGION_EXIT(void)
  {
/******************************************************************************/

  if (apvInterruptNestingCount == (APV_INITIAL_INTERRUPT_NESTING_COUNT + 1))
    {
    __enable_irq(); // the unlocking mechanism
    }

  apvInterruptNestingCount = apvInterruptNestingCount - 1;

  if (apvInterruptNestingCount < APV_INITIAL_INTERRUPT_NESTING_COUNT)
    {
    apvGlobalErrorFlags = (APV_GLOBAL_ERROR_FLAG)(apvGlobalErrorFlags | APV_GLOBAL_ERROR_FLAG_FUNCTION_INTERRUPT_NESTING);
    }

/******************************************************************************/
  } /* end of APV_CRITICAL_REGION_EXIT                                        */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
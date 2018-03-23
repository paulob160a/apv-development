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
#define __ARMLIB_disableIRQ() printf("\n DISABLE INTERRUPTS : NESTING LEVEL %05d", apvInterruptNestingCount);
#define __ARMLIB_enableIRQ()  printf("\n ENABLE INTERRUPTS  : NESTING LEVEL %05d", apvInterruptNestingCount);
#endif
#include "ApvUtilities.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

int16_t               apvInterruptNestingCount = APV_INITIAL_INTERRUPT_NESTING_COUNT;
APV_GLOBAL_ERROR_FLAG apvGlobalErrorFlags      = APV_GLOBAL_ERROR_FLAG_NONE;

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
       __ARMLIB_disableIRQ(); // the locking mechanism
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
     __ARMLIB_enableIRQ(); // the unlocking mechanism
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
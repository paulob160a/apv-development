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

// This definition avoids extravagant casting effort from the Atmel constants
Pio  ApvPeripheralLineControlBlock[APV_PERIPHERAL_LINE_GROUPS];    // shadow PIO control blocks
Pio *ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUPS] = // physical block addresses
  {
  APV_PIO_BLOCK_A,
  APV_PIO_BLOCK_B,
  APV_PIO_BLOCK_C,
  APV_PIO_BLOCK_D
  };

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/
/* apvSwitchPeripheralLines() :                                               */
/*  --> resourceLineId       : resource ID                                    */
/*  --> resourceLineSwitch   : [ false == resource lines disabled |           */
/*                               true  == resource lines enabled ]            */
/*                                                                            */
/*  <-- resourceControlError : error codes                                    */
/*                                                                            */
/*  - enable/disable the PIO lines associated with a resource                 */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p40,     */
/*             p643 - 4                                                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSwitchResourceLines(apvResourceId_t resourceLineId,
                                      bool            resourceLineSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE resourceControlError = APV_ERROR_CODE_NONE;
  uint32_t       resourceStatus       = 0;

/******************************************************************************/

  if ((resourceLineId < APV_RESOURCE_ID_BASE) || (resourceLineId >= APV_RESOURCE_IDS))
    {
    resourceControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (resourceLineSwitch == true)
      {
      switch(resourceLineId)
        {
        /******************************************************************************/
        /* Project-specific I/O allocations :                                         */
        /******************************************************************************/

        case APV_RESOURCE_ID_STROBE_0 : // The first debug strobe is at (MCU PIN34) PIO C PIN 3 (PC3) : enable the line
                                        ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PER  = PIO_PER_P3;

                                        // Enable the pull-ups on Tx and RX
                                        resourceStatus = ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PUSR;
                                        resourceStatus = resourceStatus | PIO_PUER_P3;
                                        ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PUER = resourceStatus;

                                        //ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PUER = PIO_PUER_P3;

                                        // Enable the line output
                                        ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_OER  = PIO_OER_P3;

                                        // Start with the line low
                                        ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_CODR = PIO_CODR_P3;

                                        // Check the allocation status - parallel I/O enabled, peripheral disabled
                                        if ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PSR & PIO_PDR_P3) != PIO_PDR_P3)
                                          {
                                          resourceControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                          }
                                        else
                                          {
                                          if ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_OSR & PIO_PDR_P3) != PIO_PDR_P3)
                                            {
                                            resourceControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                            }
                                          else
                                            {
                                            if ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PUSR & PIO_PDR_P3) == PIO_PDR_P3)
                                              {
                                              resourceControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                              }
                                            }
                                          }

                                        break;

        default                      :
                                        break;
        }
      }
    else
      {
      switch(resourceLineId)
        {
        /******************************************************************************/
        /* Project-specific I/O allocations :                                         */
        /******************************************************************************/

        case APV_RESOURCE_ID_STROBE_0 : // The first debug strobe is at (MCU PIN34) PIO C PIN 3 (PC3)
                                        ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PDR = PIO_PDR_P3;

                                        // Disable the line output
                                        ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_ODR = PIO_ODR_P3;

                                        // Check the allocation status
                                        if ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_C]->PIO_PSR & PIO_PSR_P3) != PIO_PER_P3)
                                          {
                                          resourceControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                          }

                                        break;

         default                      :
                                        break;
        }
      }
    }

/******************************************************************************/

  return(resourceControlError);

/******************************************************************************/
  } /* end of apvSwitchResourceLines                                          */

/******************************************************************************/
/* apvDriveResourceIoLine() :                                                 */
/*                                                                            */
/*  --> resourceLineId    : resource id - not really used here, just a check- */
/*                          reference                                         */
/*  --> resourceLineGroup : [ APV_PERIPHERAL_LINE_GROUP_A = 0 |               */
/*                            APV_PERIPHERAL_LINE_GROUP_B |                   */
/*                            APV_PERIPHERAL_LINE_GROUP_C |                   */
/*                            APV_PERIPHERAL_LINE_GROUP_D ]                   */
/*  --> resourceLine      : [ PIO_CODR_Pxy | PIO_SODR_Pxy ] - not checked!    */
/*  --> resourceLineSense : [ false == CODR | true == SODR ]                  */
/*                                                                            */
/*  <-- resourceControlError : error codes                                    */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p40,     */
/*             p643 - 4                                                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvDriveResourceIoLine(apvResourceId_t          resourceLineId,
                                      apvPeripheralLineGroup_t resourceLineGroup,
                                      uint32_t                 resourceLine,
                                      bool                     resourceLineSense)
  {
/******************************************************************************/

  APV_ERROR_CODE resourceControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((resourceLineId    <  APV_RESOURCE_ID_BASE)       || (resourceLineId >= APV_RESOURCE_IDS) ||
      (resourceLineGroup >= APV_PERIPHERAL_LINE_GROUPS) || (resourceLine   >  ((uint32_t)PIO_CODR_P31)))
    {
    resourceControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (resourceLineSense == true)
      {
      ApvPeripheralLineControlBlock_p[resourceLineGroup]->PIO_SODR = resourceLine;
      }
    else
      {
      ApvPeripheralLineControlBlock_p[resourceLineGroup]->PIO_CODR = resourceLine;
      }
    }

/******************************************************************************/

  return(resourceControlError);

/******************************************************************************/
  } /* end of apvDriveResourceIoLine                                          */

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
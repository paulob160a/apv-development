/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvPeripheralControl.c                                                     */
/* 04.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/*  - initialisation and setup of the chip environment for proper peripheral  */
/*    behaviour                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sam3x8e.h>
#include "ApvError.h"
#include "ApvSystemTime.h"
#include "ApvEventTimers.h"
#include "ApvPeripheralControl.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

Pmc  ApvPeripheralControlBlock;         // shadow peripheral control block
Pmc *ApvPeripheralControlBlock_p = PMC; // physical block address

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
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvSwitchPeripheralClock() :                                               */
/*  --> peripheralId     : 0 { <peripheral> } 44 clock enable/disable         */
/*                         switches                                           */
/*  --> peripheralSwitch : [ false == switch the peripheral clock on |        */
/*                           true  == switch the peripheral clock off ]       */
/*                                                                            */
/* - switches the peripheral block clocks on or off. Note in this project     */
/*   using the Rowley CrossWorks toolset many of the other clocks are handled */
/*   by startup code i.e. "SAM3XA_Startup.s", "system_sam3xa.c/.h" and        */
/*   "sam3x8e.h" among others. The peripheral clock registers are "write-     */
/*   only" and are shadowed by "ApvPeripheralControlBlock". Periherals are    */
/*   consistently indentified by ID "position" in either PCxR0 or PCxR1       */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15",          */
/*             p542 - 3, p563 - 4                                             */
/******************************************************************************/

APV_ERROR_CODE apvSwitchPeripheralClock(apvPeripheralId_t peripheralId,
                                        bool              peripheralSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE peripheralControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (peripheralId >= APV_PERIPHERAL_IDS)
    {
    peripheralControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    { // Peripheral clock control is spread across two control registers
    if ((peripheralId >= APV_PERIPHERAL_ID_TC0) && (peripheralId <= APV_PERIPHERAL_ID_TC4)) // TC0 - 4 --> ID 27 - 31
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCER0 = (1 << peripheralId);
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCER1 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }

    if ((peripheralId >= APV_PERIPHERAL_ID_TC5) && (peripheralId <= APV_PERIPHERAL_ID_TC8)) // TC5 - 8 --> ID 32 - 35
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER1    = ApvPeripheralControlBlock.PMC_PCER1 | (1 << (ID_TC5 - peripheralId));
        ApvPeripheralControlBlock_p->PMC_PCER1 = (1 << (ID_TC5 - peripheralId));
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR1    = ApvPeripheralControlBlock.PMC_PCDR1 | (1 << (ID_TC5 - peripheralId));
        ApvPeripheralControlBlock_p->PMC_PCDR1 = (1 << (ID_TC5 - peripheralId));
        }
      }

    if (peripheralId == APV_PERIPHERAL_ID_UART) // UART --> ID 8
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCER0 = (1 << peripheralId);
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }
    }

/******************************************************************************/

  return(peripheralControlError);

/******************************************************************************/
  } /* end of aqpvSwitchPeripheralClock                                       */

/******************************************************************************/
/* apvSwitchNvicDeviceIrq() :                                                 */
/*  --> peripheralIrqId     : peripheral ID. By some miracle this maps to the */
/*                            IRQ identifier as well                          */
/*  --> peripheralIrqSwitch : [ false == interrupt disabled |                 */
/*                               true == interrupt enabled ]                  */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15",          */
/*             p154 - 5                                                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSwitchNvicDeviceIrq(apvPeripheralId_t peripheralIrqId,
                                      bool              peripheralIrqSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE peripheralControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (peripheralIrqId >= APV_PERIPHERAL_IDS)
    {
    peripheralControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (peripheralIrqSwitch == true)
      {
      NVIC_EnableIRQ((IRQn_Type)peripheralIrqId);
      }
    else
      {
      NVIC_DisableIRQ((IRQn_Type)peripheralIrqId);
      }
    }

/******************************************************************************/

  return(peripheralControlError);

/******************************************************************************/
  } /* end of apvSwitchNvicDeviceIrq                                          */

/******************************************************************************/
/* apvSwitchPeripheralLines() :                                               */
/*  --> peripheralIrqId      : peripheral ID. By some miracle this maps to    */
/*                             the IRQ identifier as well                     */
/*  --> peripheralLineSwitch : [ false == peripheral lines disabled |         */
/*                               true == peripheral lines enabled ]           */
/*                                                                            */
/*  - enable/disable the PIO lines associated with a peripheral. NOTE the     */
/*    Atmel way is to DISABLE the PIO on a line i.e ENABLE the peripheral and */
/*    vice-versa                                                              */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15",          */
/*             p633 - 4                                                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSwitchPeripheralLines(apvPeripheralId_t peripheralLineId,
                                        bool              peripheralLineSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE peripheralControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/


  if (peripheralLineId >= APV_PERIPHERAL_IDS)
    {
    peripheralControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (peripheralLineSwitch == true)
      {
      switch(peripheralLineId)
        {
       case APV_PERIPHERAL_ID_UART : // the UART is enabled on pins PA8 and PA9
                                     ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR = PIO_PDR_P8 | PIO_PDR_P9;
                                     break;

        default                    :
                                     break;
        }
      }
    else
      {
      switch(peripheralLineId)
        {
       case APV_PERIPHERAL_ID_UART :  // the UART is disabled on pins PA8 and PA9
                                     ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PER = PIO_PDR_P8 | PIO_PDR_P9;
                                     break;

        default                    :
                                     break;
        }
      }
    }

/******************************************************************************/

  return(peripheralControlError);

/******************************************************************************/
  } /* end of apvSwitchPeripheralLines                                        */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
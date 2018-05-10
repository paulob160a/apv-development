/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvEventtimers.c                                                           */
/* 30.04.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sam3x8e.h>
#include "ApvError.h"
#include "ApvEventTimers.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

apvEventTimersBlock_t apvEventTimerBlock[APV_EVENT_TIMERS];

uint32_t apvEventTimerGeneralPurpose = 0;

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvInitialiseEventTimerBlocks() :                                          */
/*  --> apvEventTimerBlock  : address of the first event timer block          */
/*  --> numberOfTimerBlocks : the number of memory-contiguous event-timer     */
/*                            blocks to intialise                             */
/*                                                                            */
/*  - initialise a number of memory-contiguous event timer blocks             */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvInitialiseEventTimerBlocks(apvEventTimersBlock_t *apvEventTimerBlock,
                                             uint32_t               numberOfTimerBlocks)
  {
/******************************************************************************/

  APV_ERROR_CODE apvEventTimerError = APV_ERROR_CODE_NONE;

  uint32_t       channels           = 0;

/******************************************************************************/

  if (apvEventTimerBlock == NULL)
    {
    apvEventTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (numberOfTimerBlocks == 0)
      {
      apvEventTimerError = APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR;
      }
    else
      {
      do
        {
        numberOfTimerBlocks = numberOfTimerBlocks - 1; // initialise in reverse order

        (apvEventTimerBlock + numberOfTimerBlocks)->apvEventTimerBlock = NULL; // null block (BCR) address

        for (channels = 0; channels < TCCHANNEL_NUMBER; channels++)
          {
          (apvEventTimerBlock + numberOfTimerBlocks)->apvEventTimerChannels[channels]        = NULL;  // null timer channel addresses
          (apvEventTimerBlock + numberOfTimerBlocks)->apvEventTimerChannelCallBack[channels] = NULL;  // null timer callback addresses
          (apvEventTimerBlock + numberOfTimerBlocks)->apvEventTimerChannelInUse[channels]    = false; // mark all channels as free
          }

        }
      while (numberOfTimerBlocks > 0);
      }
    }

/******************************************************************************/

  return(apvEventTimerError);

/******************************************************************************/
  } /* end of apvInitialiseEventTimerBlocks                                   */

/******************************************************************************/
/* apvAssignEventTimer() :                                                    */
/*  --> timerChannel : timer instance and channel : [ 27 .. 35 ] ==           */
/*                                                    0 { instance } 2 +      */
/*                                                    0 { channel  } 2        */
/*  --> apvEventTimerBlockBaseAddress : BASE (low) address of the event timer */
/*                                      blocks                                */
/*  --> apvEventTimerChannelCallBack  : address of the timer channel callback */
/*                                      function                              */
/*                                                                            */
/*  <-- apvEventTimerError            : error codes                           */
/*                                                                            */
/* - get an event timer block BCR address, timer channel addresses and assign */
/*   an interrupt service routine callback function address                   */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvAssignEventTimer(uint16_t                timerChannel,
                                   apvEventTimersBlock_t  *apvEventTimerBlockBaseAddress,
                                   void                  (*apvEventTimerChannelCallBack)(uint32_t apvEventTimerIndex))
  {
/******************************************************************************/

  APV_ERROR_CODE apvEventTimerError       = APV_ERROR_CODE_NONE;

  uint16_t       eventTimerBlock          = 0,
                 eventTimerChannel        = 0;

  TcChannel     *eventTimerChannelAddress = NULL;

/******************************************************************************/

  if ((apvEventTimerChannelCallBack == NULL) || (apvEventTimerBlockBaseAddress == NULL))
    {
    apvEventTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    eventTimerBlock   = (timerChannel - ID_TC0) / TCCHANNEL_NUMBER;
    eventTimerChannel = (timerChannel - ID_TC0) % TCCHANNEL_NUMBER;

    if ((eventTimerBlock >= TCCHANNEL_NUMBER) || (eventTimerChannel >= TCCHANNEL_NUMBER))
      {
      apvEventTimerError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      {
      switch(eventTimerBlock)
        {
        case APV_EVENT_TIMER_2 : (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerBlock                              = (uint32_t *)(&(TC2->TC_BCR));           // address of the BCR for this block
                                 eventTimerChannelAddress                                                                           = ((TcChannel *)TC2) + eventTimerChannel; // address of this timer channel in this block
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel]        = eventTimerChannelAddress;
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelCallBack[eventTimerChannel] = apvEventTimerChannelCallBack;
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelInUse[eventTimerChannel]    = true;
                                 break;

        case APV_EVENT_TIMER_1 : (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerBlock                              = (uint32_t *)(&(TC1->TC_BCR));           // address of the BCR for this block
                                 eventTimerChannelAddress                                                                           = ((TcChannel *)TC1) + eventTimerChannel; // address of this timer channel in this block
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel]        = eventTimerChannelAddress;
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelCallBack[eventTimerChannel] = apvEventTimerChannelCallBack;
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelInUse[eventTimerChannel]    = true;
                                 break;

        case APV_EVENT_TIMER_0 : (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerBlock                              = (uint32_t *)(&(TC0->TC_BCR));           // address of the BCR for this block
                                 eventTimerChannelAddress                                                                           = ((TcChannel *)TC0) + eventTimerChannel; // address of this timer channel in this block
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel]        = eventTimerChannelAddress;
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelCallBack[eventTimerChannel] = apvEventTimerChannelCallBack;
                                 (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelInUse[eventTimerChannel]    = true;

        default                : break;
        }
      }
    }

/******************************************************************************/

  return(apvEventTimerError);

/******************************************************************************/
  } /* end of apvAssignEventTimer                                             */

/******************************************************************************/
/* apvEventTimerChannel0CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/******************************************************************************/

void apvEventTimerChannel0CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel0CallBack                                   */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvEventTimers.c                                                           */
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
#include "ApvSystemTime.h"
#include "ApvEventTimers.h"
#include "ApvUtilities.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

         apvCoreTimerBlock_t apvCoreTimeBaseBlock;
volatile apvCoreTimerFlag_t  apvCoreTimerFlag            = APV_CORE_TIMER_FLAG_LOW,
                             apvCoreTimerBackgroundFlag  = APV_CORE_TIMER_FLAG_LOW;

// As a very fine timer the system timer may have other calls on it's use. This 
// flag may help to detect overloading as development continues
bool                         apvSystemTimerInUseFlag     = false;

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvInitialiseSystemTimer() :                                               */
/*                                                                            */
/*  --> systemTimerInterval : requested interval in nanoseconds. The system   */
/*                            timer can be used for the derived "process"     */
/*                            duration timers instead of "RTT". The system    */
/*                            timer has finer resolution BUT may have other   */
/*                            calls on its use as development continues       */
/*                                                                            */
/*  <-- systemTimerError    : error codes                                     */
/*                                                                            */
/* - set the system tick periodic interval. The clock is running on "MCLK"    */
/*   with a possible maximum division of ( 2 ^ 24 ). At 84MHz the maximum-    */
/*   possible period is 0.199729 seconds, the minimum ( 1.19 x 10 ^ - 8 )     */
/*   seconds. Rounding up to 12nsecs per tick gives an error of ~0.84% in the */
/*   timing resolution                                                        */
/*                                                                            */
/* Reference : SAM3X8E Datasheet 23.03.15 "System Timer, SysTick", p192       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvInitialiseSystemTimer(apvCoreTimerBlock_t *coreTimerBlock,
                                        uint64_t             systemTimerInterval)
  {
/******************************************************************************/

  APV_ERROR_CODE systemTimerError = APV_ERROR_CODE_NONE;

  uint32_t       timerIndex      = 0;
  uint64_t       timeBaseDivider = 0;

/******************************************************************************/

  if (coreTimerBlock == NULL)
    {
    systemTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (apvSystemTimerInUseFlag == true)
      {
      systemTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
      }
    else
      {
      // Limit the miunimum and maximum possible delays to reality!
      if (systemTimerInterval > APV_SYSTEM_TIMER_CLOCK_MAXIMUM_PERIOD)
        {
        systemTimerInterval = APV_SYSTEM_TIMER_CLOCK_MAXIMUM_PERIOD;
        }
      else
        {
        if (systemTimerInterval < APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL)
          {
          systemTimerInterval = APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL;
          }
        }

      // Compute the number of ticks required
      timeBaseDivider = systemTimerInterval / APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL;

      coreTimerBlock->timeBaseDivider = timeBaseDivider;

      // Initialise the timer set
      for (timerIndex = 0; timerIndex < APV_CORE_TIMER_DURATION_TIMERS; timerIndex++)
        {
        coreTimerBlock->durationTimer[timerIndex].durationTimerCallBack              = NULL;
        coreTimerBlock->durationTimer[timerIndex].durationTimerDownCounter           = APV_DURATION_TIMER_EXPIRED;
        coreTimerBlock->durationTimer[timerIndex].durationTimerRequestedTicks        = APV_DURATION_TIMER_EXPIRED;
        coreTimerBlock->durationTimer[timerIndex].durationTimerIndex                 = APV_DURATION_TIMER_NULL_INDEX;
        coreTimerBlock->durationTimer[timerIndex].durationTimerRequestedMicroSeconds = 0;
        coreTimerBlock->durationTimer[timerIndex].durationTimerType                  = APV_DURATION_TIMER_TYPE_NONE;
        }
      }
    }

/******************************************************************************/

  return(systemTimerError);

/******************************************************************************/
  } /* end of apvInitialiseSystemTimer                                        */

/******************************************************************************/
/* apvStartSystemTimer() :                                                    */
/*                                                                            */
/*  --> coreTimerBlock : the single core-timer block is an interrupt          */
/*                       "monitor" controlling a common timebase to give a    */
/*                       fine-(ish) timing increment for applications to      */
/*                       derive their own timing requirements. An application */
/*                       grabs a time-management slot which subdivides the    */
/*                       common timebase and automatically flags either       */
/*                       periodic or one-shot time expiry.                    */
/*                       The resolution of the common timebase is limited by  */
/*                       the chip functionality and design decisions(!) The   */
/*                       aim is NOT to cripple the processor by too short a   */
/*                       common timebase interval                             */
/*                                                                            */
/*  <-- systemTimerError : error codes                                        */
/*                                                                            */
/* - start the system tick interrupt                                          */
/*                                                                            */
/* Reference : SAM3X8E Datasheet 23.03.15 "System Timer, SysTick", p192       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvStartSystemTimer(apvCoreTimerBlock_t *coreTimerBlock)
  {
/******************************************************************************/

  APV_ERROR_CODE systemTimerError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (coreTimerBlock == NULL)
    {
    systemTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    { // Cannot start if the system timer is already in use
    if (apvSystemTimerInUseFlag == true)
      {
      systemTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
      }
    else
      { // Exit if the interrupt down-counter is not set
      if (coreTimerBlock->timeBaseDivider == 0)
        {
        systemTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
        }
      else
        {
        __disable_irq();

        apvSystemTimerInUseFlag = true;

        __enable_irq();
        }
      }

    // Limit the timebase divider to the system clock counter width
    coreTimerBlock->timeBaseDivider = coreTimerBlock->timeBaseDivider & (APV_SYSTEM_TIMER_MAXIMUM_TICKS - 1);

    // This call sets the counter registers, selects the clock rate, switches on the interrupt and enables the counter!
    SysTick_Config(coreTimerBlock->timeBaseDivider);
    }

/******************************************************************************/

  return(systemTimerError);

/******************************************************************************/
  } /* end of apvStartSystemTimer                                             */

/******************************************************************************/
/* apvInitialiseCoreTimer() :                                                 */
/*                                                                            */
/*  --> coreTimerBlock : the single core-timer block is an interrupt          */
/*                       "monitor" controlling a common timebase to give a    */
/*                       fine-(ish) timing increment for applications to      */
/*                       derive their own timing requirements. An application */
/*                       grabs a time-management slot which subdivides the    */
/*                       common timebase and automatically flags either       */
/*                       periodic or one-shot time expiry.                    */
/*                       The resolution of the common timebase is limited by  */
/*                       the chip functionality and design decisions(!) The   */
/*                       aim is NOT to cripple the processor by too short a   */
/*                       common timebase interval                             */
/*  --> coreTimerInterval : periodicity of the core timer in nanoseconds      */
/*                          this cannot be less than ( 3 * ( 1 / 32768 ) ) ~= */
/*                          91553                                             */
/*  <-- coreTimerError    : error codes                                       */
/*                                                                            */
/* - set up the Cortex-M3 core timer as a fine-timer for critical timing      */
/*                                                                            */
/* Reference : SAM3X8E Datasheet 23.03.15 "Real-time Timer (RTT)", p234       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvInitialiseCoreTimer(apvCoreTimerBlock_t *coreTimerBlock,
                                      uint64_t             coreTimerInterval)
  {
/******************************************************************************/

  APV_ERROR_CODE coreTimerError  = APV_ERROR_CODE_NONE;

  uint32_t       timerIndex      = 0;
  uint64_t       timeBaseDivider = 0;

/******************************************************************************/

  if (coreTimerBlock == NULL)
    {
    coreTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (coreTimerInterval < APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL)
      {
      coreTimerInterval = APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL;
      }

    // Compute the timebase divider
    timeBaseDivider = (coreTimerInterval * APV_CORE_TIMER_CLOCK_RATE) / APV_CORE_TIMER_CLOCK_RATE_SCALER;

    coreTimerBlock->timeBaseDivider = timeBaseDivider;

    // Initialise the timer set
    for (timerIndex = 0; timerIndex < APV_CORE_TIMER_DURATION_TIMERS; timerIndex++)
      {
      coreTimerBlock->durationTimer[timerIndex].durationTimerCallBack              = NULL;
      coreTimerBlock->durationTimer[timerIndex].durationTimerDownCounter           = APV_DURATION_TIMER_EXPIRED;
      coreTimerBlock->durationTimer[timerIndex].durationTimerRequestedTicks        = APV_DURATION_TIMER_EXPIRED;
      coreTimerBlock->durationTimer[timerIndex].durationTimerIndex                 = APV_DURATION_TIMER_NULL_INDEX;
      coreTimerBlock->durationTimer[timerIndex].durationTimerRequestedMicroSeconds = 0;
      coreTimerBlock->durationTimer[timerIndex].durationTimerType                  = APV_DURATION_TIMER_TYPE_NONE;
      }
    }

    // Load the alarm register with one period per interrupt
    RTT->RTT_AR = APV_CORE_TIMER_SINGLE_PERIOD;

    // Load the timebase counter, enable the alarm interrupt and restart the timer
    RTT->RTT_MR = (((uint32_t)timeBaseDivider) & APV_CORE_TIMER_CLOCK_DIVIDER_MASK) | RTT_MR_ALMIEN | RTT_MR_RTTRST;

/******************************************************************************/

  return(coreTimerError);

/******************************************************************************/
  } /* end of apvInitialiseCoreTimer                                          */

/******************************************************************************/
/* apvAssignDurationTimer() :                                                 */
/*  --> coreTimerBlock        : the single core-timer block                   */
/*  --> durationTimerCallBack : function to execute on timer expiry. This is  */
/*                              the communications conduit from the generic   */
/*                              core timers to the requesting process!        */
/*  --> durationTimerType     : [ APV_DURATION_TIMER_TYPE_NONE = 0 |          */
/*                                APV_DURATION_TIMER_TYPE_ONE_SHOT |          */
/*                                APV_DURATION_TIMER_TYPE_PERIODIC ]          */
/*  --> durationTimerInterval : duration of the timer in nanoseconds          */
/*  --> timerIndex            : the allocated process timer index (if any)    */
/*                                                                            */
/*  <-- durationTimerError    : error codes                                   */
/*                                                                            */
/* - allocate a process timer. The communications conduit from the core timer */
/*   set to a requesting timer is the callback. When a duration timer expires */
/*   this callback will run...if all the duration timers expire at the same   */
/*   time ALL the callbacks will run. As a good design principle any and all  */
/*   callbacks should do very little other than set a flag! THIS MECHANISM IS */
/*   NOT INTENDED FOR FINE-TIMED INTERRUPTS - USE A DEDICATED INTERRUPT TIMER */
/*   FOR THAT - IMPLEMENTED SIMILAR TO THE BACKGROUND LOOP TICK               */
/*                                                                            */
/* Reference : SAM3X8E Datasheet 23.03.15 "Real-time Timer (RTT)", p234       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvAssignDurationTimer(apvCoreTimerBlock_t      *coreTimerBlock,
                                      void                    (*durationTimerCallBack)(void *durationEventMessage),
                                      apvDurationTimerType_t    durationTimerType,
                                      uint64_t                  durationTimerInterval,
                                      apvDurationTimerSource_t  durationTimerSource,
                                      uint32_t                 *timerIndex
                                      )
  {
/******************************************************************************/

   APV_ERROR_CODE durationTimerError  = APV_ERROR_CODE_NONE;
   uint32_t       durationTimerTicks  = 0;

/******************************************************************************/

  if ((coreTimerBlock == NULL) || (durationTimerCallBack == NULL))
    {
    durationTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (durationTimerType >= APV_DURATION_TIMER_TYPES)
      {
      durationTimerError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      { 
      // There is a minimum timing interval due to the chip architecture
      if (durationTimerSource == APV_DURATION_TIMER_SOURCE_RTT)
        {
        if (durationTimerInterval < APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL)
          {
          durationTimerInterval = APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL;
          }

        // The duration timer counts down a number of ticks
        durationTimerTicks = durationTimerInterval / APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL;
        }
      else
        {
        if (durationTimerInterval < APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL)
          {
          durationTimerInterval = APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL;
          }

        durationTimerTicks = durationTimerInterval / APV_SYSTEM_TIMER_CLOCK_MINIMUM_PERIOD;
        }

      /******************************************************************************/
      /* The set of available process timers is small, just do a linear search for  */
      /* a free one - the mechanism below will only work if only one process at a   */
      /* uses this code. IF THAT IS NOT THE CASE PROTECT THE WHOLE BLOCK!           */
      /******************************************************************************/

      *timerIndex = 0;

      while ((coreTimerBlock->durationTimer[*timerIndex].durationTimerIndex != APV_DURATION_TIMER_NULL_INDEX) && 
             (*timerIndex                                                   <  APV_CORE_TIMER_DURATION_TIMERS))
        {
        *timerIndex = *timerIndex + 1;
        }

      if (*timerIndex != APV_CORE_TIMER_DURATION_TIMERS)
        { // Initialise the requested process timer block
        coreTimerBlock->durationTimer[*timerIndex].durationTimerDownCounter           =  durationTimerTicks;
        coreTimerBlock->durationTimer[*timerIndex].durationTimerRequestedMicroSeconds =  durationTimerInterval;
        coreTimerBlock->durationTimer[*timerIndex].durationTimerRequestedTicks        =  durationTimerTicks;
        coreTimerBlock->durationTimer[*timerIndex].durationTimerCallBack              =  durationTimerCallBack;

        /******************************************************************************/
        /* At this point the interrupt service routine can execute the timer if these */
        /* fields are populated. Ensure coherency!                                    */
        /******************************************************************************/

        APV_CRITICAL_REGION_ENTRY();

        coreTimerBlock->durationTimer[*timerIndex].durationTimerIndex                 = *timerIndex;
        coreTimerBlock->durationTimer[*timerIndex].durationTimerType                  =  durationTimerType;

        APV_CRITICAL_REGION_EXIT();

        /******************************************************************************/
        }
      else
        {
        durationTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
        }
      }
    }

/******************************************************************************/

  return(durationTimerError);

/******************************************************************************/
  } /* end of apvAssignDurationTimer                                          */

/******************************************************************************/
/* apvDeAssignDurationTimer() :                                               */
/*  --> coreTimerBlock        : the single core-timer block                   */
/*  --> timerIndex            : the allocated process timer index (if any)    */
/*                                                                            */
/*  - remove a duration timer from the list                                   */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvDeAssignDurationTimer(apvCoreTimerBlock_t *coreTimerBlock,
                                        uint32_t            *timerIndex)
  {
/******************************************************************************/

  APV_ERROR_CODE durationTimerError = APV_ERROR_CODE_NONE;
  uint32_t       durationTimerIndex = 0;

/******************************************************************************/

  if ((coreTimerBlock != NULL) && (*timerIndex < APV_CORE_TIMER_DURATION_TIMERS))
    {
    // Search for this index
    while (durationTimerIndex < APV_CORE_TIMER_DURATION_TIMERS)
      {
      // If the index is found deallocate it's slot
      if (durationTimerIndex == *timerIndex)
        {
        APV_CRITICAL_REGION_ENTRY();

         coreTimerBlock->durationTimer[*timerIndex].durationTimerIndex = APV_DURATION_TIMER_NULL_INDEX;
         coreTimerBlock->durationTimer[*timerIndex].durationTimerType  = APV_DURATION_TIMER_TYPE_NONE;

        APV_CRITICAL_REGION_EXIT();

        *timerIndex = APV_DURATION_TIMER_NULL_INDEX;

        break;
        }

      durationTimerIndex = durationTimerIndex + 1;
      }

    // If the loop fell out without finding the index - error!
    if (durationTimerIndex == APV_CORE_TIMER_DURATION_TIMERS)
      {
      durationTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;     
      }
    }
  else
    {
    durationTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
    }

/******************************************************************************/

  return(durationTimerError);

/******************************************************************************/
  } /* end of apvDeAssignDurationTimer                                        */

/******************************************************************************/
/* apvReTriggerDurationTimer() :                                              */
/*  --> coreTimerBlock        : the single core-timer block                   */
/*  --> timerIndex            : the allocated process timer index (if any)    */
/*  --> durationTimerInterval : duration of the timer in nanoseconds          */
/*                                                                            */
/* - retrigger a duration timer with the same or a different period           */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvReTriggerDurationTimer(apvCoreTimerBlock_t *coreTimerBlock,
                                         uint32_t             timerIndex,
                                         uint64_t             durationTimerInterval)
  {
/******************************************************************************/

  APV_ERROR_CODE durationTimerError = APV_ERROR_CODE_NONE;
  uint32_t       durationTimerIndex = 0,
                 durationTimerTicks = 0;

/******************************************************************************/

  if ((coreTimerBlock != NULL) && (timerIndex < APV_CORE_TIMER_DURATION_TIMERS))
    {
    // Search for this index
    while (durationTimerIndex < APV_CORE_TIMER_DURATION_TIMERS)
      {
      // If the index is found reset the timer interval
      if (durationTimerIndex == timerIndex)
        {
        if (durationTimerInterval < APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL)
          {
          durationTimerInterval = APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL;
          }

        durationTimerTicks = durationTimerInterval / APV_SYSTEM_TIMER_CLOCK_MINIMUM_PERIOD;

        APV_CRITICAL_REGION_ENTRY();

        coreTimerBlock->durationTimer[timerIndex].durationTimerDownCounter           =  durationTimerTicks;
        coreTimerBlock->durationTimer[timerIndex].durationTimerRequestedMicroSeconds =  durationTimerInterval;
        coreTimerBlock->durationTimer[timerIndex].durationTimerRequestedTicks        =  durationTimerTicks;

        APV_CRITICAL_REGION_EXIT();

        break;
        }

      durationTimerIndex = durationTimerIndex + 1;
      }

    // If the loop fell out without finding the index - error!
    if (durationTimerIndex == APV_CORE_TIMER_DURATION_TIMERS)
      {
      durationTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;     
      }
    }
  else
    {
    durationTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }

/******************************************************************************/

  return(durationTimerError);

/******************************************************************************/
  } /* end of apvReTriggerDurationTimer                                       */

/******************************************************************************/
/* apvExecuteDurationTimers() :                                               */
/*                                                                            */
/*  --> coreTimerBlock        : the single core-timer block                   */
/*                                                                            */
/*  <-- durationTimerError    : error codes                                   */
/*                                                                            */
/* - decrement any assigned core-timer process timers. At count zero reload   */
/*   the down-counter and execute the callback function for that process      */
/*   timer                                                                    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvExecuteDurationTimers(apvCoreTimerBlock_t *coreTimerBlock)
  {
/******************************************************************************/

  APV_ERROR_CODE      durationTimerError = APV_ERROR_CODE_NONE;
  uint32_t            timerIndex         = 0;

  apvDurationTimer_t *durationTimer = NULL;

/******************************************************************************/

  if (coreTimerBlock == NULL)
    {
    durationTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    while (timerIndex < APV_CORE_TIMER_DURATION_TIMERS)
      {
      // Dereference the process timer (in case the optimiser...doesn't!)
      durationTimer = &coreTimerBlock->durationTimer[timerIndex];

      if ((durationTimer->durationTimerIndex != APV_DURATION_TIMER_NULL_INDEX) &&
          (durationTimer->durationTimerType  != APV_DURATION_TIMER_TYPE_NONE))
        { // This process timer is populated, decrement the counter
        durationTimer->durationTimerDownCounter = 
            durationTimer->durationTimerDownCounter - 1;

        if (durationTimer->durationTimerDownCounter == 0)
          { // The timer has reached zero. If it is periodic restart it
          if (durationTimer->durationTimerType == APV_DURATION_TIMER_TYPE_PERIODIC)
            {
            durationTimer->durationTimerDownCounter = 
                durationTimer->durationTimerRequestedTicks;
            }

          // Execute the callback
          durationTimer->durationTimerCallBack((void *)&timerIndex);
          }
        }

      timerIndex = timerIndex + 1;
      }
    }

/******************************************************************************/

  return(durationTimerError);

/******************************************************************************/
  } /* end of apvExecuteDurationTimers                                        */

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
/* apvConfigureWaveformEventTimer() :                                         */
/*  --> timerChannel : timer instance and channel : [ 27 .. 35 ] ==           */
/*                                                    0 { instance } 2 +      */
/*                                                    0 { channel  } 2        */
/*  --> apvEventTimerBlockBaseAddress : BASE (low) address of the event timer */
/*                                      blocks                                */
/*  --> channelClock                  : channel clock source                  */
/*  --> timeBaseTarget                : timebase tick interval in nanoseconds */
/*  --> interruptEnable               : [ false == disable tick interrupt |   */
/*                                        true  == enable  tick interrupt ]   */
/*                                                                            */
/*  --> externalClock0                : timer block #1 external clock source  */
/*  --> externalClock1                : timer block #2 external clock source  */
/*  --> externalClock2                : timer block #3 external clock source  */
/*  <-- apvEventTimerError            : event timer error codes               */
/*                                                                            */
/*  - setup the clock and operational mode of an event timer channel          */
/*    Using "waveform" mode the number of ticks required for an interrupt     */
/*    duration are computed and loaded into channel->RC3. The clock source    */
/*    and timer type [ capture | waveform ] are selected and loaded in        */
/*    channel->CMR. The "wave select" mode is RC_UP                           */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvConfigureWaveformEventTimer(uint16_t                           timerChannel,
                                              apvEventTimersBlock_t             *apvEventTimerBlockBaseAddress,
                                              apvEventTimerChannelClockSource_t  channelClock,
                                              uint32_t                           timeBaseTarget, // nanoseconds
                                              bool                               interruptEnable,
                                              apvEventTimerChannelClockExtC0_t   externalClock0,
                                              apvEventTimerChannelClockExtC1_t   externalClock1,   
                                              apvEventTimerChannelClockExtC2_t   externalClock2)
  {
/******************************************************************************/

  APV_ERROR_CODE apvEventTimerError       = APV_ERROR_CODE_NONE;

  uint16_t       eventTimerBlock          = 0,
                 eventTimerChannel        = 0;

  uint32_t      *eventTimerBlockRegisters = NULL;

/******************************************************************************/

  if (apvEventTimerBlockBaseAddress == NULL)
    {
    apvEventTimerError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    eventTimerBlock   = (timerChannel - ID_TC0) / TCCHANNEL_NUMBER;
    eventTimerChannel = (timerChannel - ID_TC0) % TCCHANNEL_NUMBER;

    if ((eventTimerBlock >= TCCHANNEL_NUMBER) || (eventTimerChannel >= TCCHANNEL_NUMBER) || (timeBaseTarget < APV_EVENT_TIMER_TIMEBASE_MINIMUM) || (timeBaseTarget>  APV_EVENT_TIMER_TIMEBASE_MAXIMUM))
      {
      apvEventTimerError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      {
      if ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelInUse[eventTimerChannel] == false)
        {
        apvEventTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
        }
      else
        {
        uint64_t timerDivisor = 1,
                 timerTarget  = (uint64_t)timeBaseTarget;

        // Guard against division by zero
        if (timerTarget == 0)
          {
          timerTarget = APV_EVENT_TIMER_INVERSE_NANOSECONDS;
          }

        timerTarget = timerTarget << APV_EVENT_TIMER_TIMEBASE_SCALER;     // numerical scaling for the divisions required
        timerTarget = timerTarget /  APV_EVENT_TIMER_INVERSE_NANOSECONDS; // this many nanoseconds (timerTarget * (1 x 10 ^ -9))
        timerTarget = timerTarget *  APV_EVENT_TIMER_TIMEBASE_BASECLOCK;  // this fraction of the base CPU clock

        // Dereference the general event timer block registers
        eventTimerBlockRegisters = (apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerBlock;

        // Compute the timebase divisor for the requested channel (only supporting /2, /8, /32 and /128)
        switch(channelClock)
          {
          case APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_3 : timerDivisor = timerDivisor * APV_EVENT_TIMER_DIVISOR_x4; // 4 x  1  =  4
          case APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_2 : timerDivisor = timerDivisor * APV_EVENT_TIMER_DIVISOR_x4; // 4 x  4  = 16, 4 x  1 =  4 
          case APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_1 : timerDivisor = timerDivisor * APV_EVENT_TIMER_DIVISOR_x4; // 4 x 16 =  64, 4 x  4 = 16, 4 x 1 = 4
          case APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_0 : timerDivisor = timerDivisor * APV_EVENT_TIMER_DIVISOR_x2; // 2 x 64 = 128, 2 x 16 = 32, 2 x 4 = 8, 2 x 1 = 2,

          default                                    :
                                                       break;
          }

        timerTarget = timerTarget /  timerDivisor;                    // pre-scale the CPU clock
        timerTarget = timerTarget >> APV_EVENT_TIMER_TIMEBASE_SCALER; // remove the numerical scaling

        switch(eventTimerBlock)
          {
          case APV_EVENT_TIMER_2 : ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_CMR = channelClock | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
                                   ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_RC  = timerTarget;

                                   if (interruptEnable == true)
                                     {
                                     ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_IER = 
                                                 ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_IER | TC_IER_CPCS;
                                     }

                                   if (externalClock2 != APV_EVENT_TIMER_CHANNEL_TIMER_XC2_NONE)
                                     {
                                     // Load the TC2XC2S field of the BMR
                                     *(eventTimerBlockRegisters + APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET) = 
                                          *(eventTimerBlockRegisters + APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET) | externalClock2;
                                     }

                                   break;

          case APV_EVENT_TIMER_1 : ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_CMR = channelClock | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
                                   ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_RC  = timerTarget;

                                   if (interruptEnable == true)
                                     {
                                     ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_IER = 
                                                 ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_IER | TC_IER_CPCS;
                                     }

                                   if (externalClock1 != APV_EVENT_TIMER_CHANNEL_TIMER_XC1_NONE)
                                     {
                                     // Load the TC1XC1S field of the BMR
                                     *(eventTimerBlockRegisters + APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET) = 
                                          *(eventTimerBlockRegisters + APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET) | externalClock1;
                                     }

                                   break;

          case APV_EVENT_TIMER_0 : ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_CMR = channelClock | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
                                   ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_RC  = timerTarget;

                                   if (interruptEnable == true)
                                     {
                                     ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_IER = 
                                                 ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_IER | TC_IER_CPCS;
                                     }

                                   if (externalClock0 != APV_EVENT_TIMER_CHANNEL_TIMER_XC0_NONE)
                                     {
                                     // Load the TC0XC0S field of the BMR
                                     *(eventTimerBlockRegisters + APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET) = 
                                          *(eventTimerBlockRegisters + APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET) | externalClock1;
                                     }

          default                : break;
          }
        }
      }
    }

/******************************************************************************/

  return(apvEventTimerError);

/******************************************************************************/
  } /* end of apvConfigureWaveformEventTimer                                  */

/******************************************************************************/
/* apvSwitchWaveformEventTimer() :                                            */
/*  --> timerChannel : timer instance and channel : [ 27 .. 35 ] ==           */
/*                                                    0 { instance } 2 +      */
/*                                                    0 { channel  } 2        */
/*  --> apvEventTimerBlockBaseAddress : BASE (low) address of the event timer */
/*                                      blocks                                */
/*  --> apvEventTimerSwitch           : start/stop the timer                  */
/*                                         [ false == STOP | true  == START ] */
/*  <-- apvEventTimerError            : evet timer error codes                */
/*                                                                            */
/* - start/stop an event timer block channel timer                            */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSwitchWaveformEventTimer(uint16_t                           timerChannel,
                                           apvEventTimersBlock_t             *apvEventTimerBlockBaseAddress,
                                           bool                               apvEventTimerSwitch)
   {
/******************************************************************************/

          APV_ERROR_CODE apvEventTimerError       = APV_ERROR_CODE_NONE;

          uint16_t       eventTimerBlock          = 0,
                         eventTimerChannel        = 0;

 /******************************************************************************/
 
  if (apvEventTimerBlockBaseAddress == NULL)
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
      if ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannelInUse[eventTimerChannel] == false)
        {
        apvEventTimerError = APV_ERROR_CODE_EVENT_TIMER_INITIALISATION_ERROR;
        }
      else
        {
        if (apvEventTimerSwitch == false)
          {
          ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_CCR = TC_CCR_CLKDIS; // 0b1 | 0b0
          }
        else
          {
          ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_CCR = (~((uint32_t)TC_CCR_CLKDIS)); // EXPLICITLY clear CLKDIS first

          ((apvEventTimerBlockBaseAddress + eventTimerBlock)->apvEventTimerChannels[eventTimerChannel])->TC_CCR = TC_CCR_CLKEN; // 0b0 | 0b1
          }
        }
      }
    }

 /******************************************************************************/

  return(apvEventTimerError);

/******************************************************************************/
   } /* end of apvSwitchWaveformEventTimer                                    */

/******************************************************************************/
/* apvEventTimerChannel0CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel0CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/

  apvEventTimerHotShot.Flags.APV_EVENT_TIMER_CHANNEL_0_FLAG = APV_EVENT_TIMER_FLAG_SET;

/******************************************************************************/
  } /* end of apvEventTimerChannel0CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel1CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel1CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel1CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel2CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel2CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel2CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel3CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel3CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel3CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel4CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel4CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel4CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel5CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel5CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel5CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel6CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel6CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel6CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel7CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel7CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel7CallBack                                   */

/******************************************************************************/
/* apvEventTimerChannel8CallBack() :                                          */
/*  --> apvEventTimerIndex : can be used to signal an event on this timer     */
/*                           channel                                          */
/*                                                                            */
/*  - callback handler for intra interrupt service routine use on channel 0   */
/*                                                                            */
/******************************************************************************/

void apvEventTimerChannel8CallBack(uint32_t apvEventTimerIndex)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvEventTimerChannel8CallBack                                   */

/******************************************************************************/
/* apvDurationStateTimer() :                                                  */
/*   --> stateTimerIndex : currently a dummy message to this duration timer   */
/*                         callback function                                  */
/* - duration timer callback function                                         */
/*                                                                            */
/******************************************************************************/

void apvDurationStateTimer(void *stateTimerIndex)
  {
/******************************************************************************/

  static bool durationStateTimerStrobe = false;

/******************************************************************************/

  if (durationStateTimerStrobe == false)
    {
    apvDriveResourceIoLine(APV_RESOURCE_ID_STROBE_0,
                           APV_PERIPHERAL_LINE_GROUP_C,
                           PIO_PER_P3,
                           true);

    durationStateTimerStrobe = true;
    }
  else
    {
    apvDriveResourceIoLine(APV_RESOURCE_ID_STROBE_0,
                           APV_PERIPHERAL_LINE_GROUP_C,
                           PIO_PER_P3,
                           false);

    durationStateTimerStrobe = false;
    }

/******************************************************************************/
  } /* end of apvDurationStateTimer                                           */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

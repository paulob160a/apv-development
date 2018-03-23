/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvCommsUtilities.c                                                        */
/* 22.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - a set of functions to add higher-level communications capabilities       */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>

#include "ApvUtilities.h"
#include "ApvCommsUtilities.h"

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvRingBufferInitialise() :                                                */
/*  <--> ringBuffer       : pointer to a ring-buffer structure                */
/*   --> ringBufferLength : nominal ring-buffer length                        */
/*                                                                            */
/* - initialise a ring-buffer structure. The maximum length of the ring-      */
/*   buffer is fixed but it can be set to be smaller. For speed the length is */
/*   forced always to be a "round" binary number                              */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvRingBufferInitialise(apvRingBuffer_t *ringBuffer,
                                       uint16_t         ringBufferLength)
  {
/******************************************************************************/

  APV_ERROR_CODE ringBufferError = APV_ERROR_CODE_NONE;

  uint16_t       leadingBit      = APV_COMMS_MSB_LEADING_BIT_MASK(uint16_t);

/******************************************************************************/

  if ((ringBuffer == NULL) || 
      (ringBufferLength < APV_COMMS_RING_BUFFER_MINIMUM_LENGTH) ||
      (ringBufferLength > APV_COMMS_RING_BUFFER_MAXIMUM_LENGTH))
    {
    ringBufferError = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
    }
  else
    {
/******************************************************************************/
/* Constrain the ring-buffer length to a "round" binary number                */
/******************************************************************************/

    // Find the position of the leading bit - it's existence is proven above
    while (!(leadingBit & ringBufferLength))
      {
      leadingBit = ((int16_t)leadingBit) >> 1; // fill-in the leading bits as we go - this forms the lower-bit-removal mask
      }

    // Clear out all but the highest bit
    ringBuffer->apvCommsRingBufferLength = ringBufferLength & leadingBit;

    ringBuffer->apvCommsRingBufferHead   = &ringBuffer->apvCommsRingBuffer[0];
    ringBuffer->apvCommsRingBufferTail   = &ringBuffer->apvCommsRingBuffer[0];

    ringBuffer->apvCommsRingBufferStart  = &ringBuffer->apvCommsRingBuffer[0];
    ringBuffer->apvCommsRingBufferEnd    =  ringBuffer->apvCommsRingBufferStart + (ringBuffer->apvCommsRingBufferLength - 1);

    ringBuffer->apvCommsRingBufferLoad   = 0;
    }

/******************************************************************************/

  return(ringBufferError);

/******************************************************************************/
  } /* end of apvRingBufferInitialise                                         */

/******************************************************************************/
/* apvRingBufferLoad() :                                                      */
/*  <--> ringBuffer             : pointer to a ring-buffer structure          */
/*   --> tokens                 : pointer to 1 { <token> } n                  */
/*   --> numberOfTokensToLoad   : number of tokens to load                    */
/*   --> interruptControl       : optional interrupt-enable/disable switch    */
/*   <-- numberOfTokensLoaded : the actual number of tokens loaded            */
/*                                                                            */
/* - load one or more tokens onto a ring-buffer. The option to (try to) load  */
/*   more than one token at a time reduces the number of calls to this        */
/*   function. There is no error-checking done here as the function could be  */
/*   used in high-level code or interrupt-service-routines and speed of ring- */
/*   buffer servicing may be required.                                        */
/*   There is an option to disable/enable interrupts around the critical code */
/*   so again the function can be used in low- and high-level code.           */
/*   The actual number of requested tokens loaded is returned to the calling  */
/*   function to make any decision on the fate of overflowing pipes. As a     */
/*   general principle the ring-buffer should never have to throw incoming    */
/*   tokens away and should be sized as such                                  */
/*                                                                            */
/******************************************************************************/

uint16_t apvRingBufferLoad(apvRingBuffer_t *ringBuffer,
                           uint8_t         *tokens,
                           uint16_t         numberOfTokensToLoad,
                           bool             interruptControl)
  {
/******************************************************************************/

  uint16_t numberOfTokensLoaded = 0;

/******************************************************************************/

  if (numberOfTokensToLoad > 0)
    {
    // The existence of the ring-buffer and token array pointers are taken as read
    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_ENTRY();
      }

    // Check if there is any space left in the ring-buffer
    if (ringBuffer->apvCommsRingBufferLoad != ringBuffer->apvCommsRingBufferLength)
      {
      // Compute how many of the requested tokens can be loaded
      if (numberOfTokensToLoad > (ringBuffer->apvCommsRingBufferLength - ringBuffer->apvCommsRingBufferLoad))
        {
        numberOfTokensToLoad = ringBuffer->apvCommsRingBufferLength - ringBuffer->apvCommsRingBufferLoad;
        }

      ringBuffer->apvCommsRingBufferLoad = ringBuffer->apvCommsRingBufferLoad + numberOfTokensToLoad;
      numberOfTokensLoaded               = numberOfTokensToLoad;

      while (numberOfTokensToLoad > 0)
        {
        *(ringBuffer->apvCommsRingBufferHead) = *tokens;

        // Move the "head" pointer on to the next ring-buffer slot
        if (ringBuffer->apvCommsRingBufferHead == ringBuffer->apvCommsRingBufferEnd)
          {
          // If the "head" pointer is at the end of the ring-buffer wrap-around to the start
          ringBuffer->apvCommsRingBufferHead = ringBuffer->apvCommsRingBufferStart;
          }
        else
          {
          ringBuffer->apvCommsRingBufferHead = ringBuffer->apvCommsRingBufferHead + 1;
          }

        tokens               = tokens               + 1;
        numberOfTokensToLoad = numberOfTokensToLoad - 1;
        }
      }

    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_EXIT();
      }
    }

/******************************************************************************/

  return(numberOfTokensLoaded);

/******************************************************************************/
  } /* end of apvRingBufferLoad                                               */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
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

// #include "ar19937.h"
#include "ApvUtilities.h"
#include "ApvCommsUtilities.h"
#include "ApvMessageHandling.h"
#include "ApvCrcGenerator.h"

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvRingBufferSetInitialise() :                                             */
/*  --> ringBufferIndirect    : points to an array of ring buffer pointers    */
/*  --> ringBufferSet         : points to an array of ring buffers            */
/*  --> ringBufferSetElements : the number of ring buffers in the set         */
/*  --> ringBufferLength      : nominal ring-buffers' length                  */
/*  <-- ringBufferSetError    : error codes                                   */
/*                                                                            */
/* - create a controlled set of ring buffers. The ring buffer set must exist  */
/*   as (i) a set of fully-instantiated objects (ii) a set of pointers to the */
/*   fully-instantiated objects. The assumption is for any process initially  */
/*   a set of "free" ring buffers is created and initialised from which any   */
/*   number of buffers can be pulled and later returned                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvRingBufferSetInitialise(apvRingBuffer_t **ringBufferIndirectSet,
                                          apvRingBuffer_t  *ringBufferSet,
                                          uint16_t          ringBufferSetElements,
                                          uint16_t          ringBufferLength)
  {
/******************************************************************************/

  APV_ERROR_CODE ringBufferSetError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((ringBufferIndirectSet == NULL) || (ringBufferSet == NULL) || (ringBufferSetElements == 0))
    {
    ringBufferSetError = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
    }
  else
    {
    do
      {
      ringBufferSetElements = ringBufferSetElements - 1;

      // In reverse-order initialise the actual ring-buffers
      if ((ringBufferSetError = apvRingBufferInitialise((ringBufferSet + ringBufferSetElements), ringBufferLength)) != APV_ERROR_CODE_NONE)
        {
        break;
        }
      else
        {
        // Assign the address of the ring buffer to the controlling list
        *(ringBufferIndirectSet + ringBufferSetElements) = ringBufferSet + ringBufferSetElements;
        }
      }
    while (ringBufferSetElements > 0);
    }

/******************************************************************************/

  return(ringBufferSetError);

/******************************************************************************/
  } /* end of apvRingBufferSetInitialise                                      */

/******************************************************************************/
/* apvRingBufferSetPullBuffer() :                                             */
/*  --> ringBufferIndirect    : points to an array of ring buffer pointers    */
/*  --> ringBuffer            : points to a free ring buffer                  */
/*  --> ringBufferSetElements : the number of ring buffers in the set         */
/*  --> interruptControl      : optional interrupt-enable/disable switch      */
/*                                                                            */
/* - get a ring-buffer from the "free" set. The assumption is any buffer set  */
/*   will be "small" enough that a straightforward linear search is fast      */
/*   enough when finding an unused buffer                                     */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvRingBufferSetPullBuffer(apvRingBuffer_t **ringBufferIndirectSet,
                                          apvRingBuffer_t **ringBuffer,
                                          uint16_t          ringBufferSetElements,
                                          bool              interruptControl)
  {
/******************************************************************************/

  APV_ERROR_CODE ringBufferSetError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((ringBufferIndirectSet == NULL) || (ringBuffer == NULL) || (ringBufferSetElements == 0))
    {
    ringBufferSetError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (interruptControl == true)
     {
     APV_CRITICAL_REGION_ENTRY();
     }

    do
      {
      if (*(ringBufferIndirectSet + (ringBufferSetElements - 1)) != APV_RING_BUFFER_LIST_EMPTY_POINTER)
        {
        *ringBuffer                                            = *(ringBufferIndirectSet + (ringBufferSetElements - 1));
        *(ringBufferIndirectSet + (ringBufferSetElements - 1)) =  APV_RING_BUFFER_LIST_EMPTY_POINTER;

        break;
        }

      ringBufferSetElements = ringBufferSetElements - 1;
      }
    while (ringBufferSetElements > 0);

    // If there are no free ring-buffers left signal a dearth...
    if (ringBufferSetElements == 0)
      {
      *ringBuffer = APV_RING_BUFFER_LIST_EMPTY_POINTER;
      }

    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_EXIT();
      }
    }

/******************************************************************************/

  return(ringBufferSetError);

/******************************************************************************/
  } /* end of apvRingBufferSetPullBuffer                                      */

/******************************************************************************/
/* apvRingBufferSetPushBuffer() :                                             */
/*  --> ringBufferIndirect    : points to an array of ring buffer pointers    */
/*  --> ringBuffer            : points to a used ring buffer                  */
/*  --> ringBufferLength      : the lenght of the ring-bufer character array  */
/*  --> ringBufferSetElements : the number of ring buffers in the set         */
/*  --> interruptControl      : optional interrupt-enable/disable switch      */
/*                                                                            */
/* - this is the reverse of "apvRingBufferSetPullBuffer()". It should always  */
/*   be possible to return a used ring-buffer to the list                     */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvRingBufferSetPushBuffer(apvRingBuffer_t **ringBufferIndirectSet,
                                          apvRingBuffer_t  *ringBuffer,
                                          uint16_t          ringBufferLength,
                                          uint16_t          ringBufferSetElements,
                                          bool              interruptControl)
  {
/******************************************************************************/

  APV_ERROR_CODE ringBufferSetError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((ringBufferIndirectSet == NULL) || (ringBuffer == NULL) || (ringBufferSetElements == 0))
    {
    ringBufferSetError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (interruptControl == true)
     {
     APV_CRITICAL_REGION_ENTRY();
     }

    do
      {
      if (*(ringBufferIndirectSet + (ringBufferSetElements - 1)) == APV_RING_BUFFER_LIST_EMPTY_POINTER)
        {
        *(ringBufferIndirectSet + (ringBufferSetElements - 1)) = ringBuffer;

        break;
        }

      ringBufferSetElements = ringBufferSetElements - 1;
      }
    while (ringBufferSetElements > 0);

    // No unused slots are left - this is catastrophic
    if (ringBufferSetElements == 0)
      {
      ringBufferSetError = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
      }
    else
      {
      ringBufferSetError =  apvRingBufferInitialise(ringBuffer,
                                                    ringBufferLength);
      }

    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_EXIT();
      }
    }

/******************************************************************************/

  return(ringBufferSetError);

/******************************************************************************/
  } /* end of apvRingBufferSetPushBuffer                                      */

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
/* apvRingBufferReportFillState() :                                           */
/*                                                                            */
/*   --> ringBuffer       : pointer to a ring-buffer structure                */
/*  <--> numberOfTokens   : returns the number of filled ring-buffer slots    */
/*   --> interruptControl : optional interrupt-enable/disable switch          */
/*                                                                            */
/* - returns the current fill state of a ring-buffer without removing any of  */
/*   the tokens. This fucntion will be used to detect data-driven input to    */
/*   processes so any way to make it faster...                                */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvRingBufferReportFillState(apvRingBuffer_t *ringBuffer,
                                            uint16_t        *numberOfTokens,
                                            bool             interruptControl)
  {
/******************************************************************************/

  APV_ERROR_CODE ringBufferError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (ringBuffer == NULL)
    {
    ringBufferError = APV_ERROR_CODE_RING_BUFFER_DEFINITION_ERROR;
    }
  else
    {
    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_ENTRY();
      }

    *numberOfTokens = ringBuffer->apvCommsRingBufferLoad;

    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_EXIT();
      }
    }

/******************************************************************************/

  return(ringBufferError);

/******************************************************************************/
  } /* end of apvRingBufferReportFillState                                    */

/******************************************************************************/
/* apvRingBufferLoad() :                                                      */
/*  <--> ringBuffer             : pointer to a ring-buffer structure          */
/*   --> tokens                 : pointer to a transmitting buffer of         */
/*                                1 { <token> } n                             */
/*   --> numberOfTokensToLoad   : number of tokens to load                    */
/*   --> interruptControl       : optional interrupt-enable/disable switch    */
/*   <-- numberOfTokensLoaded   : the actual number of tokens loaded          */
/*                                                                            */
/* - load one or more tokens onto a ring-buffer. The option to (try to) load  */
/*   more than one token at a time reduces the number of calls to this        */
/*   function. There is no error-checking done here as the function could be  */
/*   used in high-level code or interrupt-service-routines and speed of ring- */
/*   buffer servicing may be required.                                        */
/*   There is an option to disable/enable interrupts around the critical code */
/*   so again the function can be used in low- and high-level code.           */
/*   The actual number of requested tokens loaded is returned to the calling  */
/*   function to make any decision on the fate of overloaded pipes. As a      */
/*   general principle the ring-buffer should never have to throw outgoing    */
/*   tokens away and should be sized as such                                  */
/*                                                                            */
/******************************************************************************/

uint16_t apvRingBufferLoad(apvRingBuffer_t *ringBuffer,
                           uint32_t        *tokens,
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
/* apvRingBufferUnLoad() :                                                    */
/*  <--> ringBuffer             : pointer to a ring-buffer structure          */
/*   --> tokens                 : pointer to a receiving buffer of            */
/*                                1 { <token> } n                             */
/*   --> numberOfTokensToUnLoad : number of tokens to try to unload           */
/*   --> interruptControl       : optional interrupt-enable/disable switch    */
/*   <-- numberOfTokensUnLoaded : the actual number of tokens unloaded        */
/*                                                                            */
/* - unload one or more tokens from a ring-buffer. The option to (try to)     */
/*   unload more than one token at a time reduces the number of calls to this */
/*   function. There is no error-checking done here as the function could be  */
/*   used in high-level code or interrupt-service-routines and speed of ring- */
/*   buffer servicing may be required.                                        */
/*   The actual number of requested tokens unloaded is returned to the        */
/*   calling function to make any decision on the fate of underloaded pipes.  */
/*   As a general principle the ring-buffer should never have to throw        */
/*   incoming tokens away and should be sized as such                         */
/*                                                                            */
/******************************************************************************/

uint16_t apvRingBufferUnLoad(apvRingBuffer_t *ringBuffer,
                             uint32_t        *tokens,
                             uint16_t         numberOfTokensToUnLoad,
                             bool             interruptControl)
  {
/******************************************************************************/

  uint16_t numberOfTokensUnLoaded = 0;

/******************************************************************************/

  if (numberOfTokensToUnLoad > 0)
    {
    // The existence of the ring-buffer and token array pointers are taken as read
    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_ENTRY();
      }

    // Check if there are any tokens in the ring-buffer to unload
    if (ringBuffer->apvCommsRingBufferLoad != 0)
      {
      // Compute how many tokens will actually be unloaded
      if (numberOfTokensToUnLoad > ringBuffer->apvCommsRingBufferLoad)
        {
        numberOfTokensToUnLoad = ringBuffer->apvCommsRingBufferLoad;
        }

      ringBuffer->apvCommsRingBufferLoad = ringBuffer->apvCommsRingBufferLoad - numberOfTokensToUnLoad;
      numberOfTokensUnLoaded             = numberOfTokensToUnLoad;

      while (numberOfTokensToUnLoad > 0)
        {
        *tokens = *(ringBuffer->apvCommsRingBufferTail); // EXPLICIT cast from (uint32_t) to (uint8_t)
  
#ifdef _APV_DEBUG_RING_BUFFERS_ 
        *(ringBuffer->apvCommsRingBufferTail) = APV_RING_BUFFER_INITIALISATION_FLAG; 
#endif
  
        // Move the "tail" pointer on to the next ring-buffer slot
        if (ringBuffer->apvCommsRingBufferTail == ringBuffer->apvCommsRingBufferEnd)
          {
          // If the "tail" pointer is at the end of the ring-buffer wrap-around to the start
          ringBuffer->apvCommsRingBufferTail = ringBuffer->apvCommsRingBufferStart;
          }
        else
          {
          ringBuffer->apvCommsRingBufferTail = ringBuffer->apvCommsRingBufferTail + 1;
          }
  
        tokens                 = tokens                 + 1;
        numberOfTokensToUnLoad = numberOfTokensToUnLoad - 1;
        }
      }

    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_EXIT();
      }
    }

/******************************************************************************/

  return(numberOfTokensUnLoaded);

/******************************************************************************/
  } /* end of apvRingBufferUnLoad                                             */

/******************************************************************************/
/* apvCreateTestMessage() :                                                   */
/*  <--> testMessage          : pointer to the receiving message buffer       */
/*  -->  testMessageSomLength : the number of <SOM>s to prefix the message    */
/*  <--> testMessagePayLoadLength : number of tokens on the payload i.e. non- */
/*                                  framing tokens. Returns the actual        */
/*                                  payload length including <SOM>s and       */
/*                                  stuffing flags                            */
/*   <-- testMessageLength    : the final fully-framed message length         */
/*   --> testMessageInsertSom : add flagged <SOM>s to the payload             */
/*   --> testMessageSoms      : the MAXIMUM number of flagged <SOM>s to add   */
/*   --> testMessageFault     : insert a fault in message after the CRC has   */
/*                              been calculated                               */
/*   --> testSomFault         : insert a random <SOM> into the message        */
/*   <-- testError            : error codes                                   */
/*                                                                            */
/* - construct correct or faulty messages to test the messaging state-machine */
/*                                                                            */
/******************************************************************************/
#if (0)
APV_ERROR_CODE apvCreateTestMessage(uint8_t  *testMessage,
                                    uint16_t  testMessageSomLength,
                                    uint16_t *testMessagePayLoadLength,
                                    uint16_t *testMessageLength,
                                    bool      testMessageInsertSom,
                                    uint16_t  testMessageSoms,
                                    bool      testMessageFault,
                                    bool      testMessageSomFault)
  {
/******************************************************************************/

  uint16_t       payLoadCrc           = 0,
                 payLoadLengthPointer = 0,
                 payLoadLength        = 0;

  APV_ERROR_CODE testError  = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (( testMessage              != NULL) && (testMessageLength    != NULL) &&
      (*testMessagePayLoadLength != 0)    && (testMessageSomLength != 0))
    {
    *testMessageLength = 0;

    // Load the <SOM> tokens
    while (testMessageSomLength > 0)
      {
      *(testMessage + *testMessageLength) = APV_MESSAGING_START_OF_MESSAGE;

       testMessageSomLength =  testMessageSomLength - 1;
      *testMessageLength    = *testMessageLength    + 1;
      }

    *(testMessage + *testMessageLength) = APV_MESSAGING_START_OF_MESSAGE;

    // Load a random (non 0x7e) <SOM> 'planes' token
    while (*(testMessage + *testMessageLength) == APV_MESSAGING_START_OF_MESSAGE)
      {
      *(testMessage + *testMessageLength) = (uint8_t)genrand_int32();
      }

    *testMessageLength = *testMessageLength + 1;

    // Mark the payload length pointer
    payLoadLengthPointer = *testMessageLength;

    *testMessageLength   = *testMessageLength + 1;

    // Initialise the CRC register
    payLoadCrc = APV_CRC_GENERATOR_INITIAL_VALUE;

    // Generate and load a payload - if any tokens are 0x7e <SOM> precede by the <ESCAPE> token
    payLoadLength = *testMessagePayLoadLength;

    while (*testMessagePayLoadLength > 0)
      {
      *(testMessage + *testMessageLength) = (uint8_t)genrand_int32();

       if (*(testMessage + *testMessageLength) == APV_MESSAGING_START_OF_MESSAGE)
         {
         *(testMessage + *testMessageLength) = APV_MESSAGING_STUFFING_FLAG;

         apvComputeCrc(*(testMessage + *testMessageLength), &payLoadCrc);

         *testMessageLength       = *testMessageLength        + 1;
          payLoadLength           =  payLoadLength            + 1;

         *(testMessage + *testMessageLength) = APV_MESSAGING_START_OF_MESSAGE;
         }

       apvComputeCrc(*(testMessage + *testMessageLength), &payLoadCrc);

      *testMessageLength        = *testMessageLength        + 1;

       if (testMessageInsertSom != false)
         {
         if (testMessageSoms != 0)
           {
           if ((((uint8_t)genrand_int32()) & APV_COMMS_RANDOM_TRIGGER_MASK) == APV_COMMS_RANDOM_TRIGGER_MASK)
             {
             testMessageSoms = testMessageSoms - 1;

             *(testMessage + *testMessageLength) = APV_MESSAGING_STUFFING_FLAG;

              apvComputeCrc(*(testMessage + *testMessageLength), &payLoadCrc);

             *testMessageLength = *testMessageLength + 1;

              payLoadLength     = payLoadLength      + 1;

             *(testMessage + *testMessageLength) = APV_MESSAGING_START_OF_MESSAGE;

              apvComputeCrc(*(testMessage + *testMessageLength), &payLoadCrc);

             *testMessageLength = *testMessageLength + 1;

              payLoadLength     = payLoadLength      + 1;
             }
           }
         }

      *testMessagePayLoadLength =  *testMessagePayLoadLength - 1;
      }

    // Load the payload CRC
    *(testMessage + *testMessageLength) = (uint8_t)((payLoadCrc & (APV_CRC_BYTE_MASK << APV_CRC_MASK_SHIFT)) >> APV_CRC_MASK_SHIFT);

    *testMessageLength = *testMessageLength + 1;

    *(testMessage + *testMessageLength) = (uint8_t)(payLoadCrc & APV_CRC_BYTE_MASK);

    *testMessageLength = *testMessageLength + 1;

    // Finally load the actual payload length and the <EOM> token and return the actual payload length
    *(testMessage + payLoadLengthPointer) = (uint8_t)payLoadLength;

    *(testMessage + *testMessageLength) = APV_MESSAGING_END_OF_MESSAGE;

    *testMessagePayLoadLength = payLoadLength;
    }
  else
    {
    testError = APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR;
    }

/******************************************************************************/

  return(testError);

/******************************************************************************/
  } /* end of apvCreateTestMessage                                            */
#endif
/******************************************************************************/
/* apvRingBufferPrint() :                                                     */
/*  --> ringBuffer : pointer to a ring-buffer structure                       */
/*                                                                            */
/* - non-destructively prints the LINEAR contents of a ring-buffer from       */
/*   <START> to <END>. The <HEAD>-pointer is marked as 'H->' and the <TAIL>-  */
/*   pointer as 'T->'                                                         */
/*                                                                            */
/******************************************************************************/

void apvRingBufferPrint(apvRingBuffer_t *ringBuffer)
  {
/******************************************************************************/

  int  i       = 0;
  bool printOn = false;

/******************************************************************************/

  printf("\n");
  printf("\n Ring Buffer :");
  printf("\n -------------------");
  printf("\n");

  for (i = 0; i < ringBuffer->apvCommsRingBufferLength; i++)
    {
    if (&ringBuffer->apvCommsRingBuffer[i] == ringBuffer->apvCommsRingBufferHead)
      {
      printf("<-H");

      printOn = false;
      }

    if (&ringBuffer->apvCommsRingBuffer[i] == ringBuffer->apvCommsRingBufferTail)
      {
      printf("T->");

      printOn = true;
      }

    if (printOn == true)
      {
      printf("[%02x]", (uint8_t)ringBuffer->apvCommsRingBuffer[i]);
      }
    }

  printf("\n Load = %08x", ringBuffer->apvCommsRingBufferLoad);
  printf("\n Head = %08x", (uint32_t)ringBuffer->apvCommsRingBufferHead);
  printf("\n Tail = %08x", (uint32_t)ringBuffer->apvCommsRingBufferTail);
  printf("\n -------------------");
  printf("\n Length = %08x", ringBuffer->apvCommsRingBufferLength);
  printf("\n Start  = %08x", ringBuffer->apvCommsRingBufferStart);
  printf("\n End    = %08x", ringBuffer->apvCommsRingBufferEnd);
  printf("\n -------------------");
  printf("\n");

/******************************************************************************/
  } /* end of apvRingBufferPrint                                              */

/******************************************************************************/
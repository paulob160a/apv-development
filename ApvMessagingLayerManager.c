/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMessagingLayerManager.c                                                 */
/* 30.06.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - the messaging layer manager handling functions are simple two-port       */
/*   mechanisms. One port is designated "input" and one "output". Depending   */
/*   on the inter-layer "wiring" any layer-to-layer routing is possible but   */
/*   may not make any sense - if a function decides it is being fed nonsense  */
/*   it will terminate the message flow at that point. All source/sink ring   */
/*   and message buffers should be returned to their respective sources at    */
/*   that time.                                                               */
/*                                                                            */
/*   Handler functions' input/output options are :                            */
/*                                                                            */
/*     (i) input <-- hardware   : output --> peer layer                       */
/*    (ii) input <-- peer layer : output --> hardware                         */
/*   (iii) input <-- peer layer : output --> peer layer                       */
/*                                                                            */
/*   ***********************************************************************  */
/*   * NO SINGLE HANDLER FUNCTION MAY TERMINATE TWO HARDWARE SOURCES/SINKS *  */
/*   ***********************************************************************  */
/*                                                                            */
/*   Typically a layer handler will be TWO functions, one serving a hardware  */
/*   port input and one a related hardware port output e.g. serial UART tx    */
/*   rx. So if the message path is (say) :                                    */
/*     hardware port rx -> message interpreter -> message response ->         */
/*     hardware port tx                                                       */
/*   the hardware port rx will firstly verify the incoming message. The       */
/*   message is then passed via a buffer to the message interpreter. The      */
/*   message interpreter returns the message buffer. The response is passed   */
/*   using an inter-layer message buffer to the response handler. The inter-  */
/*   layer message buffer is returned. The response handler gets a hardware   */
/*   port tx buffer (or handles tx itself if appropriate) and passes the      */
/*   message to the hardware tx port low-level functions.                     */
/*                                                                            */
/*   Using this mechanism it should be possible to "stack" message layer      */
/*   handlers e.g. serial UART rx --> message interpreter --> wireless tx     */
/*   In principle a (re-entrant) message flow should be possible but in       */
/*   practise it may be necessary to cut the inter-function message buffers   */
/*   to ping-pairs only                                                       */
/*                                                                            */
/*   NOTE : hardware redirection is possible but only after the first serving */
/*          messaging handler since this is intimately tied to the hardware   */
/*          ports' message-buffer-holding ring-buffer. Message handling       */
/*          inter-layer buffers are hardware-agnostic and only need to be     */
/*          returned to a single holding ring for ALL inter-layer message     */
/*          buffers                                                           */
/*                                                                            */
/*  In addition, other functions not involved in primary I/O can inject       */
/*  messages into the stack. Message handler functions can serve both a       */
/*  hardware channel and a logical channel superimposed on that. So both      */
/*  control messaging and data messaging can have their own originating/      */
/*  terminating handlers receiving and transmitting on a hardware port. The   */
/*  messaging streams are intermingled but the contents of messages are not - */
/*  all transmit/receive message buffers are loaded/unloaded onto the tx/rx   */
/*  ring-buffers serving a port as atomic operations. There is no facility to */
/*  "dribble" characters to/from a port other than as one or more messages    */
/*  each containing a single character                                        */
/*                                                                            */
/******************************************************************************/
/* Includes :                                                                 */
/******************************************************************************/

#include <stdio.h>
#include "apvError.h"
#include "ApvCommsUtilities.h"
#include "ApvMessageHandling.h"
#include "ApvMessagingLayerManager.h"

/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/
/* Definition of the messaging layer handlers and their interconnects         */
/******************************************************************************/

apvMessagingLayerComponent_t apvMessagingLayerComponents[APV_PLANE_SERIAL_UART_CHANNELS];

/******************************************************************************/
/* Definition of the messaging layer message buffers and the holding ring-    */
/* buffer                                                                     */
/******************************************************************************/

apvRingBuffer_t       apvMessagingLayerFreeBufferSet;
apvMessageStructure_t apvMessagingLayerFreeBuffers[APV_MESSAGING_LAYER_FREE_MESSAGE_BUFFER_SET_SIZE];

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvMessagingLayerComponentInitialise() :                                   */
/*  --> messagingLayerComponents       : array of message layer handler       */
/*                                       definitions                          */
/*  --> messagingLayerComponentEntries : size of the message layer handler    */
/*                                       array                                */
/*                                                                            */
/* - clear out all of the entries in the message layer handler definition     */
/*   array                                                                    */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvMessagingLayerComponentInitialise(apvMessagingLayerComponent_t *messagingLayerComponents,
                                                    uint16_t                      messagingLayerComponentEntries)
  {
/******************************************************************************/

  APV_ERROR_CODE layerComponentError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((messagingLayerComponents == NULL) || (messagingLayerComponentEntries == 0))
    {
    layerComponentError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    do
      {
      messagingLayerComponentEntries = messagingLayerComponentEntries - 1;

      (messagingLayerComponents + messagingLayerComponentEntries)->messagingLayerComponentLoaded  = false;
      (messagingLayerComponents + messagingLayerComponentEntries)->messagingLayerServerBufferPool = NULL;
      (messagingLayerComponents + messagingLayerComponentEntries)->messagingLayerBufferPool       = NULL;
      (messagingLayerComponents + messagingLayerComponentEntries)->messagingLayerCommsPlane       = APV_COMMS_PLANE_UNUSED_0;
      (messagingLayerComponents + messagingLayerComponentEntries)->messagingLayerSignalPlane      = APV_SIGNAL_PLANE_UNUSED_0;
      (messagingLayerComponents + messagingLayerComponentEntries)->messagingLayerServiceManager   = NULL;
      }
    while (messagingLayerComponentEntries > 0);
    }

/******************************************************************************/

  return(layerComponentError);

/******************************************************************************/
  } /* end of apvMessagingLayerComponentInitialise                            */

/******************************************************************************/
/* apvMessagingLayerComponentLoad() :                                         */
/*  --> messagingLayerComponentIndex   : identify the message layer handler   */
/*                                       component to be set up               */
/*  --> messagingLayerComponents       : array of message layer handler       */
/*                                       definitions                          */
/*  --> messagingLayerComponentEntries : size of the message layer handler    */
/*                                       array                                */
/*  --> messagingServerBuffers         : holding ring-buffer for one port of  */
/*                                       message buffers (input or output,    */
/*                                       hardware or firmware)                */
/*  --> messagingLayerBuffers          : holding ring-buffer for inter-layer  */
/*                                       message buffers                      */
/*  --> messagingLayerCommsPlane       : comms plane identifier               */
/*  --> messagingLayerSignalPlane      : signalling plane identifier          */
/*  --> messagingLayerServiceManager   : message layer component handling     */
/*                                       function                             */
/* - initialise a component of the messaging layer handlers, defining its'    */
/*   comms and signal planes, input port and output port message buffer       */
/*   sources/sinks and the component handling function                        */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvMessagingLayerComponentLoad(apvMessagingLayerPlaneHandlers_t  messagingLayerComponentIndex,
                                              apvMessagingLayerComponent_t     *messagingLayerComponents,
                                              uint16_t                          messagingLayerComponentEntries,
                                              apvRingBuffer_t                  *messagingServerBuffers,
                                              apvRingBuffer_t                  *messagingLayerBuffers,
                                              apvCommsPlanes_t                  messagingLayerCommsPlane,
                                              apvSignalPlanes_t                 messagingLayerSignalPlane,
                                              void                            (*messagingLayerServiceManager)(void))
  {
/******************************************************************************/

  APV_ERROR_CODE layerComponentError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if ((messagingLayerComponents       == NULL) || (messagingServerBuffers       == NULL) ||
      (messagingLayerBuffers          == NULL) || (messagingLayerServiceManager == NULL) || 
      (messagingLayerComponentEntries == 0)    || (messagingLayerComponentIndex > messagingLayerComponentEntries))
    {
    layerComponentError = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    // Match the compnent index to it's entry in the messaging layer definition array
    (messagingLayerComponents + messagingLayerComponentIndex)->messagingLayerComponentLoaded  = true;
    (messagingLayerComponents + messagingLayerComponentIndex)->messagingLayerServerBufferPool = messagingServerBuffers;
    (messagingLayerComponents + messagingLayerComponentIndex)->messagingLayerBufferPool       = messagingLayerBuffers;
    (messagingLayerComponents + messagingLayerComponentIndex)->messagingLayerCommsPlane       = messagingLayerCommsPlane;
    (messagingLayerComponents + messagingLayerComponentIndex)->messagingLayerSignalPlane      = messagingLayerSignalPlane;
    (messagingLayerComponents + messagingLayerComponentIndex)->messagingLayerServiceManager   = messagingLayerServiceManager;
    }

/******************************************************************************/

  return(layerComponentError);

/******************************************************************************/
  } /* apvMessagingLayerComponentLoad                                         */

/******************************************************************************/
/* Messaging layer handling functions :                                       */
/******************************************************************************/
/* apvMessagingLayerSerialUARTInputHandler() :                                */
/******************************************************************************/

void apvMessagingLayerSerialUARTInputHandler(void)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvMessagingLayerSerialUARTInputHandler                         */

/******************************************************************************/
/* apvMessagingLayerSerialUARTOutputHandler() :                               */
/******************************************************************************/

void apvMessagingLayerSerialUARTOutputHandler(void)
  {
/******************************************************************************/
/******************************************************************************/
  } /* end of apvMessagingLayerSerialUARTOutputHandler                        */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

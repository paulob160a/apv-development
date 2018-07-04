/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMessagingLayerManager.h                                                 */
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
/*   Typically a layer handler ("channel") will be serviced by TWO functions  */
/*   or "components", one serving a hardware port input and one a related     */
/*   hardware port output e.g. serial UART tx and rx. So if the message path  */
/*   is (say) :                                                               */
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

#ifndef _APV_MESSAGING_LAYER_MANAGER_H_
#define _APV_MESSAGING_LAYER_MANAGER_H_

/******************************************************************************/
/* Includes :                                                                 */
/******************************************************************************/

#include <stdint.h>
#include "ApvCommsUtilities.h"
#include "ApvMessageHandling.h"

/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/
/* The first 16 entries in the messaging layer definition array are serial    */
/* UART channels defined in pairs :                                           */
/*  - APV_SIGNAL_PLANE_CONTROL_0 == 0 : defined as input MATCHED TO           */
/*  - APV_SIGNAL_PLANE_CONTROL_1 == 2 : defined as ouput                      */
/*                                                                            */
/*  - APV_SIGNAL_PLANE_DATA_0 == 1 : defined as input MATCHED TO              */
/*  - APV_SIGNAL_PLANE_DATA_1 == 3 : defined as ouput                         */
/*                                                                            */
/******************************************************************************/

#define APV_PLANE_SERIAL_UART_CONTROL_nn(signalPlaneIndex) \
                 APV_PLANE_SERIAL_UART_CONTROL_##signalPlaneIndex = (((uint8_t)APV_COMMS_PLANE_SERIAL_UART) + ((uint8_t)APV_SIGNAL_PLANE_CONTROL_##signalPlaneIndex))

#define APV_PLANE_SERIAL_UART_DATA_nn(signalPlaneIndex) \
                 APV_PLANE_SERIAL_UART_DATA_##signalPlaneIndex = (((uint8_t)APV_COMMS_PLANE_SERIAL_UART) + ((uint8_t)APV_SIGNAL_PLANE_DATA_##signalPlaneIndex))

/******************************************************************************/

#define APV_MESSAGING_LAYER_COMPONENT_ENTRIES_SIZE        (APV_PLANE_SERIAL_UART_CHANNELS)
#define APV_MESSAGING_LAYER_FREE_MESSAGE_BUFFER_SET_SIZE  16 // the pool of inter-messaging layer message buffers

#define APV_MESSAGINIG_COMPONENT_MESSAGE_RING_BUFFER_SIZE 16 // a messaging layer components' message buffer holding ring

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvMessagingLayerPlaneHandlers_tTag
  {
  APV_PLANE_SERIAL_UART_CONTROL_nn(0),
  APV_PLANE_SERIAL_UART_CONTROL_nn(1),
  APV_PLANE_SERIAL_UART_DATA_nn(0),
  APV_PLANE_SERIAL_UART_DATA_nn(1),
  APV_PLANE_SERIAL_UART_CHANNELS
  } apvMessagingLayerPlaneHandlers_t;


typedef struct apvMessagingLayerComponent_tTag
  {
  bool               messagingLayerComponentLoaded;        // mark occupied components
  apvCommsPlanes_t   messagingLayerCommsPlane;             // the comms plane this manager handles
  apvSignalPlanes_t  messagingLayerSignalPlane;            // the signal plane this manager handles
  apvRingBuffer_t   *messagingLayerInputBufferPool;        // points to the ring-buffer serving the pool of message buffers for the layers' input messaging server
                                                           // - the messaging layer component uses this to return exhauted nessage buffers to their (port) source
                                                           // - this ring-buffer is owned by the server
  apvRingBuffer_t   *messagingLayerOutputBufferPool;       // points to the ring-buffer serving the pool of message buffers for the layers' output messaging server
                                                           // - the messaging layer conponent uses this to produce and consume inter-messaging layer message buffers
                                                           // - this ring-buffer is owned by the server
  apvRingBuffer_t   *messagingLayerInputBuffers;           // points to the ring-buffer of incoming message buffers that will be RETURNED to the servers' buffer pool
                                                           // this ring-buffer is owned by the component which does NOT own any message buffers
  void             (*messagingLayerServiceManager)(struct apvMessagingLayerComponent_tTag *thisComponent,
                                                   struct apvMessagingLayerComponent_tTag *allComponents);  // the instance of a manager function
  } apvMessagingLayerComponent_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern apvMessagingLayerComponent_t apvMessagingLayerComponents[APV_PLANE_SERIAL_UART_CHANNELS];
extern bool                         apvMessagingLayerComponentReady[APV_MESSAGING_LAYER_COMPONENT_ENTRIES_SIZE];

extern apvRingBuffer_t              apvMessagingLayerFreeBufferSet;
extern apvMessageStructure_t        apvMessagingLayerFreeBuffers[APV_MESSAGING_LAYER_FREE_MESSAGE_BUFFER_SET_SIZE];

extern apvRingBuffer_t              apvMessagingLayerComponentSerialUartTxBuffer;
extern apvRingBuffer_t              apvMessagingLayerComponentSerialUartRxBuffer;

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvMessagingLayerComponentInitialise(apvMessagingLayerComponent_t *messagingLayerComponents,
                                                           uint16_t                      messagingLayerComponentEntries);
extern APV_ERROR_CODE apvMessagingLayerComponentLoad(apvMessagingLayerPlaneHandlers_t  messagingLayerComponentIndex,
                                                     apvMessagingLayerComponent_t     *messagingLayerComponents,
                                                     uint16_t                          messagingLayerComponentEntries,
                                                     apvRingBuffer_t                  *messagingInputBufferPool,     // input is nominally from a server - but can be to a layer
                                                     apvRingBuffer_t                  *messagingOutputBufferPool,    // output is nominally to a layer - but can be to a server
                                                     apvRingBuffer_t                  *messagingLayerMessageBuffers, // the components' holding ring of borrowed message buffers
                                                     apvCommsPlanes_t                  messagingLayerCommsPlane,
                                                     apvSignalPlanes_t                 messagingLayerSignalPlane,
                                                     void                            (*messagingLayerServiceManager)(struct apvMessagingLayerComponent_tTag *thisComponent,
                                                                                                                     struct apvMessagingLayerComponent_tTag *allComponents));

extern bool           apvMessagingLayerGetComponentInputPort(apvCommsPlanes_t               componentCommsPlane, 
                                                             apvSignalPlanes_t              componentSignalPlane,
                                                             apvMessagingLayerComponent_t  *messagingLayerComponents,
                                                             apvRingBuffer_t              **componentInpuMessageBuffers);

extern void           apvMessagingLayerSerialUARTInputHandler(struct apvMessagingLayerComponent_tTag *thisComponent,
                                                              struct apvMessagingLayerComponent_tTag *allComponents);
extern void           apvMessagingLayerSerialUARTOutputHandler(struct apvMessagingLayerComponent_tTag *thisComponent,
                                                               struct apvMessagingLayerComponent_tTag *allComponents);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

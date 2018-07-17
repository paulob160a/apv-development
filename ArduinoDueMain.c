/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvMain.c                                                                  */
/* 15.03.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sam3x8e.h>
#include <core_cm3.h>
#include "ApvSerial.h"
#include "ApvEventTimers.h"
#include "ApvSystemTime.h"
#include "ApvPeripheralControl.h"
#include "ApvControlPortProtocol.h"
#include "ApvMessageHandling.h"
#include "ApvMessagingLayerManager.h"

/******************************************************************************/
/* Constant Definitions :                                                     */
/******************************************************************************/

#define APV_RUN_TIME_TX_MODULUS ((uint64_t)1024)

/******************************************************************************/
/* Local Variable Definitions :                                               */
/******************************************************************************/

static uint32_t spiTimerIndex = 0;

/******************************************************************************/
/* Local Function Declarations :                                              */
/******************************************************************************/

int main(void);

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/

int main(void)
  {
/******************************************************************************/

           APV_SERIAL_ERROR_CODE  apvSerialErrorCode        = APV_SERIAL_ERROR_CODE_NONE;
  volatile uint64_t               apvRunTimeCounter         = 0,
                                  apvRunTimeCounterOld      = 0;

           bool                   apvPrimarySerialPortStart = false;

           uint16_t components                              = 0,
                    messageCount                            = 0;

           int16_t  interruptSource                         = 0;
           uint8_t  interruptPriority                       = 0;

/******************************************************************************/

  // Create the serial UART inter-messaging layer message buffers
  apvSerialErrorCode = apvCreateMessageBuffers(&apvMessageSerialUartFreeBufferSet,
                                               &apvMessageSerialUartFreeBuffers[0],
                                                APV_MESSAGE_FREE_BUFFER_SET_SIZE);

  // Create messaging layer handler function inter-layer free message buffers
  apvSerialErrorCode = apvCreateMessageBuffers(&apvMessagingLayerFreeBufferSet,
                                               &apvMessagingLayerFreeBuffers[0],
                                                APV_MESSAGING_LAYER_FREE_MESSAGE_BUFFER_SET_SIZE);

  // Initialise the array of message layer handling components
  apvSerialErrorCode = apvMessagingLayerComponentInitialise(&apvMessagingLayerComponents[0],
                                                             APV_PLANE_SERIAL_UART_CHANNELS);

  // Load the serial UART messaging layer received message interpreter
  apvSerialErrorCode = apvMessagingLayerComponentLoad( APV_PLANE_SERIAL_UART_CONTROL_0,
                                                      &apvMessagingLayerComponents[0],
                                                       APV_PLANE_SERIAL_UART_CHANNELS,
                                                      &apvMessageSerialUartFreeBufferSet,            // serial UART free buffer set/pool
                                                      &apvMessagingLayerFreeBufferSet,               // messaging layer free buffer set/pool
                                                      &apvMessagingLayerComponentSerialUartRxBuffer, // serial UART component input ring
                                                       APV_COMMS_PLANE_SERIAL_UART,
                                                       APV_SIGNAL_PLANE_CONTROL_0,
                                                      &apvMessagingLayerSerialUARTInputHandler);

  // Load the serial UART messaging layer transmit message handler
  apvSerialErrorCode = apvMessagingLayerComponentLoad( APV_PLANE_SERIAL_UART_CONTROL_1,
                                                      &apvMessagingLayerComponents[0],
                                                       APV_PLANE_SERIAL_UART_CHANNELS,
                                                      &apvMessagingLayerFreeBufferSet,               // messaging layer free buffer set/pool
                                                      &apvMessageSerialUartFreeBufferSet,            // serial UART free buffer set/pool
                                                      &apvMessagingLayerComponentSerialUartTxBuffer, // serial UART component output ring
                                                       APV_COMMS_PLANE_SERIAL_UART,
                                                       APV_SIGNAL_PLANE_CONTROL_1,
                                                      &apvMessagingLayerSerialUARTOutputHandler);

  /******************************************************************************/
  /* Set all the interrupt source priorities to the lowest possible, for all    */
  /* configurable priorities                                                    */
  /******************************************************************************/

  for (interruptSource = APV_SYSTEM_INTERRUPT_ID_MEMORY_MANAGEMENT_FAULT; interruptSource < APV_PERIPHERAL_IDS; interruptSource++)
    {
    if (apvGetInterruptPriority( interruptSource,
                                &interruptPriority) == APV_ERROR_CODE_NONE)
      {
      apvSetInterruptPriority( interruptSource,
                               APV_DEVICE_INTERRUPT_PRIORITY_BASE,
                              &apvInterruptPriorities[0]);
      }
    }

  /******************************************************************************/
  /* Elevate the priority of the elapsed timer block core timer "SysTick"       */
  /******************************************************************************/

  apvSetInterruptPriority( APV_SYSTEM_INTERRUPT_ID_SYSTEM_TICK,
                           APV_DEVICE_INTERRUPT_PRIORITY_SYSTICK,
                          &apvInterruptPriorities[0]);

  /******************************************************************************/

  apvSerialErrorCode = apvInitialiseEventTimerBlocks(&apvEventTimerBlock[APV_EVENT_TIMER_0],
                                                      TCCHANNEL_NUMBER);

  apvSerialErrorCode = apvAssignEventTimer( APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                           &apvEventTimerBlock[APV_EVENT_TIMER_0], // BASE ADDRESS
                                            apvEventTimerChannel0CallBack);

  // Set the general-purpose (system tick) timebase to 1 millisecond (1000000 nanoseconds)
  apvEventTimerGeneralPurposeTimeBaseTarget = APV_EVENT_TIMER_GENERAL_PURPOSE_TIME_BASE;

  apvSerialErrorCode = apvConfigureWaveformEventTimer( APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                                      &apvEventTimerBlock[APV_EVENT_TIMER_0],
                                                       APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_0,
                                                       apvEventTimerGeneralPurposeTimeBaseTarget, // nanoseconds,
                                                       true,                                      // enable the RC3 interrupt
                                                       APV_EVENT_TIMER_CHANNEL_TIMER_XC0_NONE,
                                                       APV_EVENT_TIMER_CHANNEL_TIMER_XC1_NONE,   
                                                       APV_EVENT_TIMER_CHANNEL_TIMER_XC2_NONE);

  // SWITCH ON THE NVIC/TIMER IRQ
  apvSerialErrorCode = apvSwitchNvicDeviceIrq(APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                              true);

  apvSerialErrorCode = apvSwitchPeripheralClock(ID_TC0, // switch on the timebase peripheral clock
                                                true);

  apvSerialErrorCode = apvSwitchWaveformEventTimer( APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                                   &apvEventTimerBlock[APV_EVENT_TIMER_0],
                                                    true);

  // Default to using the UART for primary serial comms
  apvSerialErrorCode = apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT_UART,
                                             APV_PRIMARY_SERIAL_RING_BUFFER_SET);

  apvSerialErrorCode = apvSwitchPeripheralLines(ID_UART, // switch on the primary serial port peripheral
                                                true);

  apvSerialErrorCode = apvSwitchPeripheralClock(ID_UART, // switch on the primary serial port peripheral clock
                                                true);

  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_RESET);
  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_RESET_STATUS);
  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_ENABLE);

  apvSerialErrorCode = apvConfigureUart(APV_UART_PARITY_NONE,
                                        APV_UART_CHANNEL_MODE_NORMAL,
                                        APV_UART_BAUD_RATE_SELECT_19200);

  // SWITCH ON THE NVIC/UART IRQ
  apvSerialErrorCode = apvUartSwitchInterrupt(APV_UART_INTERRUPT_SELECT_RECEIVE,
                                              true);

  apvSerialErrorCode = apvSwitchNvicDeviceIrq(APV_PERIPHERAL_ID_UART,
                                              true);

  /******************************************************************************/
  /* SPI Setup :                                                                */
  /******************************************************************************/

  apvSerialErrorCode = apvSwitchPeripheralClock(APV_PERIPHERAL_ID_SPI0, // switch on SPI0
                                                true);

  // Operatimg mode is :
  //  - MASTER
  //  - chip-select set by registers SPI0_CSR[0 .. 3]
  //  - chip-selects are connected to one device each
  //  - fault detection mode is OFF
  //  - data receiver must be read before the next transmit event
  //  - loopback is ON
  //  - chip-select delay is 6 * MCK
  apvSerialErrorCode = apvSPISetOperatingMode(ApvSpi0ControlBlock_p,
                                              APV_SPI_MASTER_MODE,
                                              APV_SPI_PERIPHERAL_SELECT_VARIABLE,
                                              APV_SPI_CHIP_SELECT_DECODE_DIRECT,
                                              APV_SPI_MODE_FAULT_DETECTION_DISABLED,
                                              APV_SPI_WAIT_ON_DATA_READ_DISABLED,
                                              APV_SPI_LOOPBACK_DISABLED,
                                              APV_SPI_CHIP_SELECT_DELAY_MINIMUM_nS);

  // Chip-select mode is :
  //  - SPI0_CSR[0]
  //  - chip-select "inactive" is logic '1'
  //  - data CHANGES on the LEADING edge of "SPCK" and is sampled on the edge "FOLLOWING"
  //  - chip-select ALWAYS rises after each data transfer ON THE SAME SLAVE
  //  - chip-select ALWAYS rises on ANY slave
  //  - data bit width   == 16
  //  - serial baud rate == 1.25 Mbps
  //  - pre-SPCK delay   == 4 * MCK
  //  - inter-transfer delay BEFORE chip-select de-assert == 0
  apvSerialErrorCode = apvSetChipSelectCharacteristics(ApvSpi0ControlBlock_p,
                                                       APV_SPI_CHIP_SELECT_REGISTER_0,
                                                       APV_SPI_SERIAL_CLOCK_POLARITY_ONE,
                                                       APV_SPI_SERIAL_CLOCK_PHASE_DATA_CHANGE_LEADING,
                                                       APV_SPI_CHIP_SELECT_SINGLE_SLAVE_RISE,
                                                       APV_SPI_CHIP_SELECT_CHANGE_SLAVE_RISE,
                                                       APV_SPI_MAXIMUM_BIT_TRANSFER_WIDTH,
                                                       APV_SPI_BAUD_RATE_SELECT_39K215,
                                                       APV_SPI_FIRST_SPCK_TRANSITION_DELAY_nS,
                                                       APV_SPI_INTER_TRANSFER_DELAY);

  apvSerialErrorCode = apvSwitchPeripheralLines(ID_SPI0,
                                                true);

  apvSerialErrorCode = apvSPIEnable(ApvSpi0ControlBlock_p,
                                    true);

 while (true)
    {
    volatile uint8_t  spiTransmitPrefix    = 0x55;
    volatile uint8_t  spiTransmitTraffic   = 0xaa;
    volatile uint8_t  spiChipSelectOut     = 0x00;
    volatile uint16_t spiReceivedCharacter = 0;
    volatile uint8_t  spiChipSelectIn      = 0;

    apvSerialErrorCode = apvSPITransmitCharacter(ApvSpi0ControlBlock_p,
                                                 spiTransmitPrefix,
                                                 spiTransmitTraffic,
                                                 spiChipSelectOut,
                                                 APV_SPI_LAST_TRANSFER_ACTIVE);

    apvSerialErrorCode = apvSpiReceiveCharacter( ApvSpi0ControlBlock_p,
                                                &spiReceivedCharacter,
                                                &spiChipSelectIn);
    }

  /******************************************************************************/

  apvSerialErrorCode = apvSwitchPeripheralClock(ID_RTT, // switch on the core timer
                                                true);

  apvSerialErrorCode = apvInitialiseSystemTimer(&apvCoreTimeBaseBlock,
                                                 APV_SYSTEM_TIMER_CLOCK_MINIMUM_PERIOD);

  /* apvSerialErrorCode = apvInitialiseCoreTimer(&apvCoreTimeBaseBlock,
                                               APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL); */

  // Assign a dummy SPI timer for this test
  /* apvSerialErrorCode = apvAssignDurationTimer(&apvCoreTimeBaseBlock,
                                               apvSpiStateTimer,
                                               APV_DURATION_TIMER_TYPE_PERIODIC,
                                               APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL,
                                               APV_DURATION_TIMER_SOURCE_RTT,
                                              &spiTimerIndex); */

  apvSerialErrorCode = apvAssignDurationTimer(&apvCoreTimeBaseBlock,
                                               apvSpiStateTimer,
                                               APV_DURATION_TIMER_TYPE_PERIODIC,
                                               APV_SYSTEM_TIMER_CLOCK_MINIMUM_PERIOD,
                                               APV_DURATION_TIMER_SOURCE_SYSTICK,
                                              &spiTimerIndex);

  // Switch on the peripheral I/O "C" channel clock
  apvSerialErrorCode = apvSwitchPeripheralClock(ID_PIOC,
                                                true);

  // Switch on the timer strobe
  apvSerialErrorCode = apvSwitchResourceLines(APV_RESOURCE_ID_STROBE_0,
                                              true);

  // Switch on the core timer interrupt
  /* apvSerialErrorCode = apvSwitchNvicDeviceIrq(APV_CORE_TIMER_ID,
                                              true); */

  apvSerialErrorCode = apvStartSystemTimer(&apvCoreTimeBaseBlock);

/******************************************************************************/

  if (apvSerialErrorCode == APV_SERIAL_ERROR_CODE_NONE)
    {
    while (true)
      {
        __disable_irq();

       /******************************************************************************/
       /* This is the "slow" loop timer event detector                               */
       /******************************************************************************/

       if (apvEventTimerHotShot.Flags.APV_EVENT_TIMER_CHANNEL_0_FLAG == APV_EVENT_TIMER_FLAG_SET)
         {
         apvEventTimerHotShot.Flags.APV_EVENT_TIMER_CHANNEL_0_FLAG = APV_EVENT_TIMER_FLAG_CLEAR;

         apvRunTimeCounterOld = apvRunTimeCounter;

         apvRunTimeCounter    = apvRunTimeCounter + 1;
         }

       /******************************************************************************/
       /* This is the "fast" loop timer core-timer flag detector. The core-timer is  */
       /* used to derive a number of slower "process" timers                         */
       /******************************************************************************/

       if (apvCoreTimerFlag == APV_CORE_TIMER_FLAG_HIGH)
         {
         apvCoreTimerBackgroundFlag = apvCoreTimerFlag;
         apvCoreTimerFlag           = APV_CORE_TIMER_FLAG_LOW;
         }

       __enable_irq();

       /******************************************************************************/
       /* FOR NOW, AT EACH MILLISECOND...                                            */
       /******************************************************************************/

       if (apvRunTimeCounterOld != apvRunTimeCounter)
         {
         apvRunTimeCounterOld = apvRunTimeCounter;

         /******************************************************************************/
         /* Low-level message input de-framing is handled here :                       */
         /*                                                                            */
         /*        *** MESSAGE DEFRAMING : LOW-LEVEL MESSAGE STATE-MACHINES ***        */
         /*                                                                            */
         /******************************************************************************/
         /* The first level of wired (serial port) I/O is a framed message defined by  */
         /* a finite-state-machine.                                                    */
         /******************************************************************************/

         if (apvPrimarySerialPortStart == false)
           {
           apvPrimarySerialPortStart = true;

           // Initialise the lowest-level serial comms frame receiver state machine
           apvSerialErrorCode = apvDeFrameMessageInitialisation( apvPrimarySerialCommsReceiveBuffer,
                                                                &apvMessageSerialUartFreeBufferSet,
                                                                &apvMessagingDeFramingStateMachine[0]);
           }

         /******************************************************************************/
         /* BEWARE THE DEBUGGER! With the optimisation level set to 0 the debugger can */
         /* enable the virtual printf channel. THIS KILLS THE PROCESSOR! The result is */
         /* an apparent inability to keep up with even the slowest messaging. Setting  */
         /* the optimisation level to '1' disables the virtual printf channel. Using a */
         /* couple of "message successful/unsuccessful" counters demonstrates that all */
         /* is well and the processor has no problems with continuous 19.2Kbps message */
         /* arrival                                                                    */
         /******************************************************************************/

         // Run the serial UART comms message state-machine
         apvSerialErrorCode = apvDeFrameMessage(&apvMessagingDeFramingStateMachine[0]);

         /******************************************************************************/
         /* The second level of any message activity is handled here :                 */
         /*                                                                            */
         /*               *** MESSAGING LAYER : COMPONENT HANDLERS ***                 */
         /*                                                                            */
         /******************************************************************************/
         /* All messaging-layer component handling is data-driven. Each component      */
         /* handler has in input port and an output port - activity is triggered by    */
         /* messages arriving at the input ports :                                     */
         /******************************************************************************/
         /* A communications channel is usually two components, one input and one      */
         /* output. To remove the effect of the linear search on a channel i.e. if the */
         /* input component is earlier than the output component the channel gets two  */
         /* bites of the cherry because the input component sends a message to the     */
         /* output component, the "ready" state is read first for the whole list       */
         /* before acting on it                                                        */
         /******************************************************************************/

         for (components = 0; components < APV_MESSAGING_LAYER_COMPONENT_ENTRIES_SIZE; components++)
           { // Visit each CHANNEL once at each pass
           if (apvMessagingLayerComponents[components].messagingLayerComponentLoaded == true)
             { // Check if the component is loaded
             if (apvRingBufferReportFillState( apvMessagingLayerComponents[components].messagingLayerInputBuffers,
                                              &messageCount,
                                               true) == APV_ERROR_CODE_NONE)
               { // Only service components that have messages waiting at their input ports
               if (messageCount != 0)
                 {
                 apvMessagingLayerComponentReady[components] = true;
                 }
               else
                 {
                 apvMessagingLayerComponentReady[components] = false;
                 }
               }
             }
           }

         for (components = 0; components < APV_MESSAGING_LAYER_COMPONENT_ENTRIES_SIZE; components++)
           {
           if (apvMessagingLayerComponentReady[components] == true)
             {
             apvMessagingLayerComponents[components].messagingLayerServiceManager(&apvMessagingLayerComponents[components],
                                                                                  (apvMessagingLayerComponent_t *)&apvMessagingLayerComponents);
             }
           }

         /******************************************************************************/

         }

#if (0)
       if (receiveInterrupt == true)
         {
         // Try and get a new receive ring-buffer from the queue
         if (apvRingBufferUnLoad( apvUartPortPrimaryReceiveRingBuffer_p,
                                 (uint32_t *)&apvReceiveBuffer_p,
                                  sizeof(uint8_t),
                                  true) != 0)
           {
           // Put the buffer on the transmit queue
           apvRingBufferLoad( apvUartPortPrimaryTransmitRingBuffer_p,
                             (uint32_t *)&apvReceiveBuffer_p,
                              sizeof(uint8_t),
                              true);

           // Flag the transmitter new characters are available
           transmitInterrupt = true;
           }

         receiveInterrupt = false;
         }

       if (transmitInterrupt == true)
         {
         if (transmitInterruptTrigger == false)
           {
           transmitInterruptTrigger = true;

           if (apvUartBufferTransmitPrime( ApvUartControlBlock_p,
                                           apvUartPortPrimaryTransmitRingBuffer_p,
                                          &apvPrimarySerialCommsTransmitBuffer) != APV_ERROR_CODE_NONE)
             {
             // That didn't go well!
             transmitInterrupt        = false;
             transmitInterruptTrigger = false;
             }
           }
         }
       else
         {
         transmitInterruptTrigger = false;
         }

       __enable_irq();
#endif

      /******************************************************************************/
      /* Run scheduled tasks in the background                                      */
      /******************************************************************************/
      /* TASK 1 : run the core-timer to derive process-based timers                 */
      /******************************************************************************/

 /*     if (apvCoreTimerBackgroundFlag == APV_CORE_TIMER_FLAG_HIGH)
        {
        apvCoreTimerBackgroundFlag = APV_CORE_TIMER_FLAG_LOW;

        // Execute any assigned process timers
        apvSerialErrorCode = apvExecuteDurationTimers(&apvCoreTimeBaseBlock);
        } */

      /******************************************************************************/
      }
    }

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main                                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

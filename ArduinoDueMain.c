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

           APV_SERIAL_ERROR_CODE apvSerialErrorCode  = APV_SERIAL_ERROR_CODE_NONE;
  volatile uint64_t              apvRunTimeCounter   = 0;

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

   // Load the sign-on before the transmit/receive interrupt is started
  apvSerialErrorCode = apvControlPortSignOn(&apvSignOnMessage[0],
                                             strlen(apvSignOnMessage));

  // SWITCH ON THE NVIC/UART IRQ
  apvSerialErrorCode = apvUartSwitchInterrupt(APV_UART_INTERRUPT_SELECT_RECEIVE,
                                              true);

  apvSerialErrorCode = apvSwitchNvicDeviceIrq(APV_PERIPHERAL_ID_UART,
                                              true);

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

         apvRunTimeCounter = apvRunTimeCounter + 1;
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

       /******************************************************************************/
       /* The current software configuration is all to test the transmit and receive */
       /* UART interrupts and the ring-buffer lists and "free" buffer list that      */
       /* support them. The whole shebang is driven round by the receiver i.e. when  */
       /* characters turn up in the receive buffers they shold wend their way to the */
       /* transmit buffers and back out in a loop-back                               */
       /******************************************************************************/

       if (receiveInterrupt == true)
         {
         // Try and get a new receive ring-buffer from the queue
         if (apvRingBufferUnLoad( apvUartPortPrimaryReceiveRingBuffer_p,
                                 (uint32_t *)&apvPrimarySerialCommsReceiveBuffer,
                                  sizeof(uint8_t),
                                  false) != 0)
           {
           // Put the buffer on the transmit queue
           apvRingBufferLoad( apvUartPortPrimaryTransmitRingBuffer_p,
                             (uint32_t *)&apvPrimarySerialCommsReceiveBuffer,
                              sizeof(uint8_t),
                              false);

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

           if (apvUartCharacterTransmitPrime( ApvUartControlBlock_p,
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

      /******************************************************************************/
      /* Run scheduled tasks in the background                                      */
      /******************************************************************************/
      /* TASK 1 : run the core-timer to derive process-based timers                 */
      /******************************************************************************/

      if (apvCoreTimerBackgroundFlag == APV_CORE_TIMER_FLAG_HIGH)
        {
        apvCoreTimerBackgroundFlag = APV_CORE_TIMER_FLAG_LOW;

        // Execute any assigned process timers
        apvSerialErrorCode = apvExecuteDurationTimers(&apvCoreTimeBaseBlock);
        }

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

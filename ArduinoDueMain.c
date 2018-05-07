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

#include <stdint.h>
#include <stdbool.h>
#include <sam3x8e.h>
#include <core_cm3.h>
#include "ApvSerial.h"
#include "ApvEventTimers.h"
#include "ApvSystemTime.h"
#include "ApvPeripheralControl.h"

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

           APV_SERIAL_ERROR_CODE apvSerialErrorCode = APV_SERIAL_ERROR_CODE_NONE;
  volatile uint64_t              apvRunTimeCounter  = 0;

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

  // SWITCH ON THE NVC/TIMER IRQ
  apvSerialErrorCode = apvSwitchNvicDeviceIrq(APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                              true);

  apvSerialErrorCode = apvSwitchPeripheralClock(ID_TC0, // switch on the timebase peripheral clock
                                                true);

  apvSerialErrorCode = apvSwitchWaveformEventTimer( APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                                   &apvEventTimerBlock[APV_EVENT_TIMER_0],
                                                    true);

  // Default to using the UART for primary serial comms
  apvSerialErrorCode = apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT_UART);

  apvSerialErrorCode = apvSwitchPeripheralLines(ID_UART, // switch on the primary serial port peripheral
                                                true);

  apvSerialErrorCode = apvSwitchPeripheralClock(ID_UART, // switch on the primary serial port peripheral clock
                                                true);

/******************************************************************************/

  if (apvSerialErrorCode == APV_SERIAL_ERROR_CODE_NONE)
    {
    while (true)
      {
       __disable_irq();

       if (apvEventTimerHotShot.Flags.APV_EVENT_TIMER_CHANNEL_0_FLAG == APV_EVENT_TIMER_FLAG_SET)
         {
         apvEventTimerHotShot.Flags.APV_EVENT_TIMER_CHANNEL_0_FLAG = APV_EVENT_TIMER_FLAG_CLEAR;

         apvRunTimeCounter = apvRunTimeCounter + 1;
         }

       __enable_irq();
      }
    }

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main                                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
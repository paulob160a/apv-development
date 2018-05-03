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
#include "ApvSerial.h"
#include "ApvEventTimers.h"
#include "ApvSystemTime.h"

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

  // Default to using the UART for primary serial comms
  apvSerialErrorCode = apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT_UART);

  apvSerialErrorCode = apvInitialiseEventTimerBlocks(&apvEventTimerBlock[APV_EVENT_TIMER_0],
                                                      TCCHANNEL_NUMBER);

  apvSerialErrorCode = apvAssignEventTimer( APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                           &apvEventTimerBlock[APV_EVENT_TIMER_0], // BASE ADDRESS
                                            apvEventTimerChannel0CallBack);

  // Set the general-purpose (system tick) timebase to 1 millisecond (1000000 nanoseconds)
  apvEventTimerGeneralPurposeTimeBaseTarget = APV_EVENT_TIMER_GENERAL_PURPOSE_TIME_BASE;

  apvSerialErrorCode =  apvConfigureWaveformEventTimer( APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
                                                       &apvEventTimerBlock[APV_EVENT_TIMER_0],
                                                        APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_3,
                                                        apvEventTimerGeneralPurposeTimeBaseTarget, // nanoseconds,
                                                        APV_EVENT_TIMER_CHANNEL_TIMER_XC0_NONE,
                                                        APV_EVENT_TIMER_CHANNEL_TIMER_XC1_NONE,   
                                                        APV_EVENT_TIMER_CHANNEL_TIMER_XC2_NONE);

/******************************************************************************/

  while (true)
    {
    apvRunTimeCounter = apvRunTimeCounter + 1;
    }

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main                                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
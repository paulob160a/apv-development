/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvEventTimersIsrs.c                                                       */
/* 01.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/*  - the event timer ISRS are preliminarily declared as "weak" by Atmel so   */
/*    are fully instantiated here. Each timer channel is redirected through   */
/*    the callback function assigned at system setup                          */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sam3x8e.h>
#include "ApvError.h"
#include "ApvEventTimers.h"
#include "ApvSystemTime.h"
#include "ApvEventTimersIsrs.h"

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* TC0_Handler() :                                                            */
/******************************************************************************/

void TC0_Handler(void)
  {
/******************************************************************************/

           uint32_t apvTimerBlock   = (ID_TC0 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
           uint32_t apvTimerChannel = (ID_TC0 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;
  volatile uint32_t timerStatus    = 0;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC0);

  timerStatus = apvEventTimerBlock[apvTimerBlock].apvEventTimerChannels[apvTimerChannel]->TC_SR;

/******************************************************************************/
  } /* end of TC0_Handler                                                     */

/******************************************************************************/
/* TC1_Handler() :                                                            */
/******************************************************************************/

void TC1_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC1 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC1 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC1);

/******************************************************************************/
  } /* end of TC1_Handler                                                     */

/******************************************************************************/
/* TC2_Handler() :                                                            */
/******************************************************************************/

void TC2_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC2 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC2 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC2);

/******************************************************************************/
  } /* end of TC2_Handler                                                     */

/******************************************************************************/
/* TC3_Handler() :                                                            */
/******************************************************************************/

void TC3_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC3 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC3 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC3);

/******************************************************************************/
  } /* end of TC3_Handler                                                     */

/******************************************************************************/
/* TC4_Handler() :                                                            */
/******************************************************************************/

void TC4_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC4 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC4 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC4);

/******************************************************************************/
  } /* end of TC4_Handler                                                     */

/******************************************************************************/
/* TC5_Handler() :                                                            */
/******************************************************************************/

void TC5_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC5 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC5 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC5);

/******************************************************************************/
  } /* end of TC5_Handler                                                     */

/******************************************************************************/
/* TC6_Handler() :                                                            */
/******************************************************************************/

void TC6_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC6 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC6 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC6);

/******************************************************************************/
  } /* end of TC6_Handler                                                     */

/******************************************************************************/
/* TC7_Handler() :                                                            */
/******************************************************************************/

void TC7_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC7 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC7 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC7);

/******************************************************************************/
  } /* end of TC7_Handler                                                     */

/******************************************************************************/
/* TC8_Handler() :                                                            */
/******************************************************************************/

void TC8_Handler(void)
  {
/******************************************************************************/

  uint32_t apvTimerBlock   = (ID_TC8 - APV_EVENT_TIMER_BASE_ID) / TCCHANNEL_NUMBER;
  uint32_t apvTimerChannel = (ID_TC8 - APV_EVENT_TIMER_BASE_ID) % TCCHANNEL_NUMBER;

/******************************************************************************/

  apvEventTimerBlock[apvTimerBlock].apvEventTimerChannelCallBack[apvTimerChannel](ID_TC8);

/******************************************************************************/
  } /* end of TC8_Handler                                                     */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
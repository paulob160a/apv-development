/******************************************************************************/
/*                                                                            */
/* ApvSystemTime.h                                                            */
/* 03.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/* - holding file for the various system timer definitions                    */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_SYSTEM_TIME_H_
#define _APV_SYSTEM_TIME_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdint.h>
#include "ApvEventTimers.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_EVENT_TIMER_GENERAL_PURPOSE_TIME_BASE (APV_EVENT_TIMER_TIMEBASE_MINIMUM * ((uint32_t)100000)) // 1000000 nanoseconds (1 millisecond)

#define APV_EVENT_TIMER_CHANNEL_TIME_BASE_MAXIMUM ((uint32_t)((int32_t)-1))

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvEventTimerFlag_tTag
  {
  APV_EVENT_TIMER_FLAG_CLEAR = 0,
  APV_EVENT_TIMER_FLAG_SET,
  APV_EVENT_TIMER_FLAG_TYPES
  } apvEventTimerFlag_t;

typedef struct apvEventTimerHotOnes_tTag
  {
  uint32_t APV_EVENT_TIMER_CHANNEL_0_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_1_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_2_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_3_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_4_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_5_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_6_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_7_FLAG : 1;
  uint32_t APV_EVENT_TIMER_CHANNEL_8_FLAG : 1;
  } apvEventTimerHotOnes_t;

typedef union apvEventTimersHotOneList_tTag
  {
  apvEventTimerHotOnes_t Flags;
  uint32_t               FlagPole;
  } apvEventTimersHotOneList_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern apvEventTimersBlock_t      apvEventTimerBlock[APV_EVENT_TIMERS];

extern uint32_t                   apvEventTimerGeneralPurposeTimeBaseTarget;

extern apvEventTimersHotOneList_t apvEventTimerHotShot;

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
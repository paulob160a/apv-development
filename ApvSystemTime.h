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

#include "ApvEventTimers.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_EVENT_TIMER_GENERAL_PURPOSE_TIME_BASE (APV_EVENT_TIMER_TIMEBASE_MINIMUM * ((uint32_t)100000)) // 1000000 nanoseconds (1 millisecond)

#define APV_EVENT_TIMER_CHANNEL_TIME_BASE_MAXIMUM ((uint32_t)((int32_t)-1))

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern apvEventTimersBlock_t apvEventTimerBlock[APV_EVENT_TIMERS];

extern uint32_t apvEventTimerGeneralPurposeTimeBaseTarget;

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
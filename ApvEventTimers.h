/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvEventTimers.h                                                           */
/* 30.04.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_EVENT_TIMERS_H_
#define _APV_EVENT_TIMERS_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <sam3x8e.h>
#include <stdbool.h>
#include "ApvError.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

#define APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET (sizeof(uint32_t))

#define APV_EVENT_TIMER_TIMEBASE_BASECLOCK    ((uint64_t)84000000)   // SAM3X8E/A CPU CLOCK MHz
#define APV_EVENT_TIMER_TIMEBASE_SCALER       (32)                   // scale by 2 ^ 32
#define APV_EVENT_TIMER_INVERSE_NANOSECONDS   ((uint64_t)1000000000) // one-second in nanoseconds

#define APV_EVENT_TIMER_TIMEBASE_MINIMUM      ((uint32_t)10)         // nanoseconds
#define APV_EVENT_TIMER_TIMEBASE_MAXIMUM      ((uint32_t)1000000000) // nanoseconds (100 milliseconds)

#define APV_EVENT_TIMER_DIVISOR_x2            ((uint64_t)2)
#define APV_EVENT_TIMER_DIVISOR_x4            ((uint64_t)4)

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

// 3X8A/E timer channels are numbered ( 0, 0 .. 2 ), ( 1, 0 .. 2 ) and ( 1, 0 .. 2 )
// based on the Atmel TCx ids 27 - 35
typedef enum apvEventTimerUse_tTag
  {
  APV_EVENT_TIMER_BASE_ID            = ID_TC0,
  APV_EVENT_TIMER_GENERAL_PURPOSE_ID = APV_EVENT_TIMER_BASE_ID,
  APV_EVENT_TIMER_RESERVED_1,
  APV_EVENT_TIMER_RESERVED_2,
  APV_EVENT_TIMER_RESERVED_3,
  APV_EVENT_TIMER_RESERVED_4,
  APV_EVENT_TIMER_RESERVED_5,
  APV_EVENT_TIMER_RESERVED_6,
  APV_EVENT_TIMER_RESERVED_7,
  APV_EVENT_TIMER_RESERVED_8,
  APV_EVENT_TIMER_USES
  } apvEventTimerUse_t;

typedef enum apvEventTimers_tTag
  {
  APV_EVENT_TIMER_0 = 0, // (int)(( <id> - 27 ) / 3)
  APV_EVENT_TIMER_1,
  APV_EVENT_TIMER_2,
  APV_EVENT_TIMERS
  } apvEventTimers_t;

typedef enum apvEventTimerChannelClockSource_tTag
  {
  APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_0 = TC_CMR_TCCLKS_TIMER_CLOCK1, // ( 84MHz /   2 ) = 23.8nsecs
  APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_1 = TC_CMR_TCCLKS_TIMER_CLOCK2, // ( 84MHz /   8 ) = 95.24nsecs
  APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_2 = TC_CMR_TCCLKS_TIMER_CLOCK3, // ( 84MHz /  32 ) = 381nsecs
  APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_3 = TC_CMR_TCCLKS_TIMER_CLOCK4, // ( 84MHz / 128 ) = 1.524usecs
  APV_EVENT_TIMER_CHANNEL_TIMER_CLOCK_4 = TC_CMR_TCCLKS_TIMER_CLOCK5, // internal SCLK
  APV_EVENT_TIMER_CHANNEL_TIMER_XC0     = TC_CMR_TCCLKS_XC0,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC1     = TC_CMR_TCCLKS_XC2,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC2     = TC_CMR_TCCLKS_XC0,
  APV_EVENT_TIMER_CHANNEL_TIMERS        = 8
  } apvEventTimerChannelClockSource_t;

typedef enum apvEventTimerChannelClockExtC0_tTag
  {
  APV_EVENT_TIMER_CHANNEL_TIMER_XC0_TCLK0 = TC_BMR_TC0XC0S_TCLK0,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC0_TIOA1 = TC_BMR_TC0XC0S_TIOA1,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC0_TIOA2 = TC_BMR_TC0XC0S_TIOA2,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC0_NONE  = -1
  } apvEventTimerChannelClockExtC0_t;

typedef enum apvEventTimerChannelClockExtC1_tTag
  {
  APV_EVENT_TIMER_CHANNEL_TIMER_XC1_TCLK1 = TC_BMR_TC1XC1S_TCLK1,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC1_TIOA0 = TC_BMR_TC1XC1S_TIOA0,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC1_TIOA2 = TC_BMR_TC1XC1S_TIOA2,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC1_NONE  = -1
  } apvEventTimerChannelClockExtC1_t;

typedef enum apvEventTimerChannelClockExtC2_tTag
  {
  APV_EVENT_TIMER_CHANNEL_TIMER_XC2_TCLK2 = TC_BMR_TC2XC2S_TCLK2,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC2_TIOA0 = TC_BMR_TC2XC2S_TIOA2,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC2_TIOA1 = TC_BMR_TC2XC2S_TIOA1,
  APV_EVENT_TIMER_CHANNEL_TIMER_XC2_NONE  = -1
  } apvEventTimerChannelClockExtC2_t;

typedef struct apvEventTimersBlock_tTag
  {
  uint32_t  *apvEventTimerBlock; // address of the block control registers etc.,.
  TcChannel *apvEventTimerChannels[TCCHANNEL_NUMBER];
  void      (*apvEventTimerChannelCallBack[TCCHANNEL_NUMBER])(uint32_t apvEventTimerIndex);
  bool       apvEventTimerChannelInUse[TCCHANNEL_NUMBER];
  } apvEventTimersBlock_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/
/******************************************************************************/
/* function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvInitialiseEventTimerBlocks(apvEventTimersBlock_t *apvEventTimerBlock,
                                                    uint32_t               numberOfTimerBlocks);
extern APV_ERROR_CODE apvAssignEventTimer(uint16_t                timerChannel,
                                          apvEventTimersBlock_t  *apvEventTimerBlockBaseAddress,
                                          void                  (*apvEventTimerChannelCallBack)(uint32_t apvEventTimerIndex));
extern APV_ERROR_CODE apvConfigureWaveformEventTimer(uint16_t                           timerChannel,
                                                     apvEventTimersBlock_t             *apvEventTimerBlockBaseAddress,
                                                     apvEventTimerChannelClockSource_t  channelClock,
                                                     uint32_t                           timeBaseTarget, // nanoseconds
                                                     bool                               interruptEnable,
                                                     apvEventTimerChannelClockExtC0_t   externalClock0,
                                                     apvEventTimerChannelClockExtC1_t   externalClock1,   
                                                     apvEventTimerChannelClockExtC2_t   externalClock2);
extern APV_ERROR_CODE apvSwitchWaveformEventTimer(uint16_t                           timerChannel,
                                                  apvEventTimersBlock_t             *apvEventTimerBlockBaseAddress,
                                                  bool                               apvEventTimerSwitch);

extern void           apvEventTimerChannel0CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel1CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel2CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel3CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel4CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel5CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel6CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel7CallBack(uint32_t apvEventTimerIndex);
extern void           apvEventTimerChannel8CallBack(uint32_t apvEventTimerIndex);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

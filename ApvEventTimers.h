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

#define APV_SYSTEM_TIMER_TIMEBASE_BASECLOCK     ((uint64_t)84000000)   // SAM3X8E/A CPU CLOCK MHz
// The minimum system timer interval in nanoseconds, crudely rounded UP to 12nsecs per tick
#define APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL ((APV_EVENT_TIMER_INVERSE_NANOSECONDS / APV_SYSTEM_TIMER_TIMEBASE_BASECLOCK) + APV_CORE_TIMER_CLOCK_RATE_ROUNDUP)
#define APV_SYSTEM_TIMER_CLOCK_MINIMUM_PERIOD   ((uint64_t)150000)    // this limits the interrupt rate to 150usecs, useable and manageable!

#define APV_SYSTEM_TIMER_COUNTER_WIDTH          (24)
#define APV_SYSTEM_TIMER_MAXIMUM_TICKS          ((uint64_t)(1 << APV_SYSTEM_TIMER_COUNTER_WIDTH))

#define APV_SYSTEM_TIMER_CLOCK_MAXIMUM_INTERVAL (APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL * APV_SYSTEM_TIMER_MAXIMUM_TICKS)
#define APV_SYSTEM_TIMER_CLOCK_MAXIMUM_PERIOD   APV_SYSTEM_TIMER_CLOCK_MAXIMUM_INTERVAL

#define APV_EVENT_TIMER_BLOCK_REGISTER_OFFSET   (sizeof(uint32_t))

#define APV_EVENT_TIMER_TIMEBASE_BASECLOCK      ((uint64_t)84000000)   // SAM3X8E/A CPU CLOCK MHz
#define APV_EVENT_TIMER_TIMEBASE_SCALER         (32)                   // scale by 2 ^ 32
#define APV_EVENT_TIMER_INVERSE_NANOSECONDS     ((uint64_t)1000000000) // one-second in nanoseconds

#define APV_EVENT_TIMER_TIMEBASE_MINIMUM        ((uint32_t)10)         // nanoseconds
#define APV_EVENT_TIMER_TIMEBASE_MAXIMUM        ((uint32_t)1000000000) // nanoseconds (100 milliseconds)

#define APV_EVENT_TIMER_DIVISOR_x2              ((uint64_t)2)
#define APV_EVENT_TIMER_DIVISOR_x4              ((uint64_t)4)

#define APV_CORE_TIMER_CLOCK_RATE               ((uint64_t)32768)            // the main RTT clock rate
#define APV_CORE_TIMER_CLOCK_MINIMUM_DIVIDER    ((uint64_t)3)                // less than 3 results in unstable interrupt operation
#define APV_CORE_TIMER_CLOCK_RATE_SCALER        ((uint64_t)1000000000)       // translate nanoseconds to a useable integer
#define APV_CORE_TIMER_CLOCK_RATE_ROUNDUP       ((uint64_t)1)                // crude round-up of integer divisions

#define APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL   (((APV_CORE_TIMER_CLOCK_MINIMUM_DIVIDER * APV_CORE_TIMER_CLOCK_RATE_SCALER) / APV_CORE_TIMER_CLOCK_RATE) + APV_CORE_TIMER_CLOCK_RATE_ROUNDUP)

#define APV_CORE_TIMER_CLOCK_DIVIDER_MASK       ((uint32_t)0x0000ffff)       // RTT->RTT_MR : mode register RTPRES mask
#define APV_CORE_TIMER_SINGLE_PERIOD            ((uint32_t)1)                // interrupt after one (SCLK * prescaler) tick

#define APV_CORE_TIMER_DURATION_TIMERS          16                           // this is enough ? Do not want to use "malloc()"!

#define APV_DURATION_TIMER_EXPIRED               0
#define APV_DURATION_TIMER_NULL_INDEX           ((uint32_t)~0)

#define APV_CORE_TIMER_ID                       ID_RTT                       // core timer interrupt ID (Atmel id 3)

#define APV_SPI_STATE_TIMER                     APV_CORE_TIMER_CLOCK_MINIMUM_INTERVAL // currently a dummy timer duration

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

typedef enum apvDurationTimerType_tTag
  {
  APV_DURATION_TIMER_TYPE_NONE = 0,
  APV_DURATION_TIMER_TYPE_ONE_SHOT,
  APV_DURATION_TIMER_TYPE_PERIODIC,
  APV_DURATION_TIMER_TYPES
  } apvDurationTimerType_t;

typedef enum apvCoreTimerFlag_tTag 
  {
  APV_CORE_TIMER_FLAG_LOW = 0,
  APV_CORE_TIMER_FLAG_HIGH,
  APV_CORE_TIMER_FLAGS
  } apvCoreTimerFlag_t;

typedef enum apvDurationTimerSource_tTag
  {
  APV_DURATION_TIMER_SOURCE_RTT = 0,
  APV_DURATION_TIMER_SOURCE_SYSTICK,
  APV_DURATION_TIMER_SOURCES
  } apvDurationTimerSource_t;

/******************************************************************************/
/* Holding structure for the core timer-derived process timer set             */
/******************************************************************************/

typedef struct apvDurationTimer_tTag
  {
  uint32_t                durationTimerIndex;                                  // 0 { durationTimer[index] } 31
  uint32_t                durationTimerRequestedMicroSeconds;                  // lots of microseconds
  uint32_t                durationTimerRequestedTicks;                         // the reload value for the down counter
  uint32_t                durationTimerDownCounter;                            // just test for zero
  void                   (*durationTimerCallBack)(void *durationEventMessage); // called when the timer expires
  apvDurationTimerType_t  durationTimerType;                                   // a number of timer types can be supported
  } apvDurationTimer_t;

typedef struct apvCoreTimerBlock_tTag
  {
  uint32_t           timeBaseDivider;                               // the value loaded into RTT->RTT_MR:RTPRES from which all 
                                                                    // duration timers are derived
  apvDurationTimer_t durationTimer[APV_CORE_TIMER_DURATION_TIMERS]; // a fixed number of duration timers is allocated
  } apvCoreTimerBlock_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern          apvCoreTimerBlock_t apvCoreTimeBaseBlock;
extern volatile apvCoreTimerFlag_t  apvCoreTimerFlag,
                                    apvCoreTimerBackgroundFlag;

extern bool                         apvSystemTimerInUseFlag;

/******************************************************************************/
/* function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvInitialiseSystemTimer(apvCoreTimerBlock_t *coreTimerBlock,
                                               uint64_t             systemTimerInterval);
extern APV_ERROR_CODE apvStartSystemTimer(apvCoreTimerBlock_t *coreTimerBlock);
extern APV_ERROR_CODE apvInitialiseCoreTimer(apvCoreTimerBlock_t *coreTimerBlock,
                                             uint64_t             coreTimerInterval);
extern APV_ERROR_CODE apvAssignDurationTimer(apvCoreTimerBlock_t      *coreTimerBlock,
                                             void                    (*durationTimerCallBack)(void *durationEventMessage),
                                             apvDurationTimerType_t    durationTimerType,
                                             uint64_t                  durationTimerInterval,
                                             apvDurationTimerSource_t  durationTimerSource,
                                             uint32_t                 *timerIndex);
extern APV_ERROR_CODE apvExecuteDurationTimers(apvCoreTimerBlock_t *coreTimerBlock);
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
extern void           apvSpiStateTimer(void *stateTimerIndex);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

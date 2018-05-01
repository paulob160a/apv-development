/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvEventtimers.h                                                           */
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

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

// 3X8A/E timer channels are numbered ( 0, 0 .. 2 ), ( 1, 0 .. 2 ) and ( 1, 0 .. 2 )
// based on the Atmel TCx ids 27 - 35
typedef enum apvEventTimerUse_tTag
  {
  APV_EVENT_TIMER_BASE_ID            = ID_TC0,
  APV_EVENT_TIMER_RESERVED_0         = APV_EVENT_TIMER_BASE_ID,
  APV_EVENT_TIMER_RESERVED_1,
  APV_EVENT_TIMER_RESERVED_2,
  APV_EVENT_TIMER_RESERVED_3,
  APV_EVENT_TIMER_GENERAL_PURPOSE_ID,
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

extern uint32_t              apvEventTimerGeneralPurpose;
extern apvEventTimersBlock_t apvEventTimerBlock[TCCHANNEL_NUMBER];

/******************************************************************************/
/* function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvInitialiseEventTimerBlocks(apvEventTimersBlock_t *apvEventTimerBlock,
                                                    uint32_t               numberOfTimerBlocks);
extern APV_ERROR_CODE apvAssignEventTimer(uint16_t                timerChannel,
                                          apvEventTimersBlock_t  *apvEventTimerBlockBaseAddress,
                                          void                  (*apvEventTimerChannelCallBack)(uint32_t apvEventTimerIndex));

extern void           apvEventTimerChannel0CallBack(uint32_t apvEventTimerIndex);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
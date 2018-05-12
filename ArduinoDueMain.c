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

/******************************************************************************/
/* Constant Definitions :                                                     */
/******************************************************************************/

const char UartTestPhrase[] = "We all live in a Yellow Submarine";

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
//           uint8_t               txCounter           = 0;

           uint16_t              transmitBufferIndex = 0;

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
  apvSerialErrorCode = apvSerialCommsManager(APV_PRIMARY_SERIAL_PORT_UART);

  apvSerialErrorCode = apvSwitchPeripheralLines(ID_UART, // switch on the primary serial port peripheral
                                                true);

  apvSerialErrorCode = apvSwitchPeripheralClock(ID_UART, // switch on the primary serial port peripheral clock
                                                true);

  apvSerialErrorCode = apvSerialBufferInitialise(&transmitBuffer,
                                                  (uint16_t)strlen(UartTestPhrase),
                                                 &UartTestPhrase[0],
                                                  (uint16_t)strlen(UartTestPhrase));

#ifdef _APV_UART_TEST_MODE_
  {
#define APV_RUN_TIME_TX_MODULUS ((uint64_t)1024)

#define ASCII_CAP_A        ((uint8_t)'A')
#define APV_MAX_TX_COUNTER 15

/* #define ASCII_CAP_B        ((uint8_t)'B')

  uint8_t transmitBuffer = 0; */

/*  apvSerialErrorCode = apvConfigureUart(APV_UART_PARITY_NONE,
                                        APV_UART_CHANNEL_MODE_LOCAL_LOOPBACK,
                                        APV_UART_BAUD_RATE_SELECT_19200);

  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_RESET);
  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_RESET_STATUS);
  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_ENABLE);

  // SWITCH ON THE NVIC/UART IRQ
  apvSerialErrorCode = apvUartSwitchInterrupt(APV_UART_INTERRUPT_SELECT_RECEIVE,
                                              true);

  apvSerialErrorCode = apvSwitchNvicDeviceIrq(APV_PERIPHERAL_ID_UART,
                                              true);

  transmitBuffer = ASCII_CAP_A;

  apvSerialErrorCode = apvUartCharacterTransmit(transmitBuffer); */

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

/*
  transmitBuffer = ASCII_CAP_B;

  for (txCounter = 0; txCounter < APV_MAX_TX_COUNTER; txCounter++)
    {
    if (apvUartCharacterTransmit(transmitBuffer + txCounter) != APV_ERROR_CODE_NONE)
      {
      txCounter = txCounter - 1;
      }
    } */
  }

#else
  apvSerialErrorCode = apvConfigureUart(APV_UART_PARITY_NONE,
                                        APV_UART_CHANNEL_MODE_NORMAL,
                                        APV_UART_BAUD_RATE_SELECT_19200);

  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_RESET);
  apvSerialErrorCode = apvControlUart(APV_UART_CONTROL_ACTION_ENABLE);
#endif

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

       if (receiveInterrupt == true)
         {
         if (apvUartCharacterTransmit(receiveBuffer) == APV_ERROR_CODE_NONE)
           {
           receiveInterrupt = false;
           }
         }

#ifdef _APV_UART_TEST_MODE_
        if (!(apvRunTimeCounter % APV_RUN_TIME_TX_MODULUS))
           {
           __disable_irq();

           transmitBufferIndex = transmitBuffer_p->serialTransmitBufferIndex;

           // Start/reinstate the transmit interrupt if the transmit buffer is exhausted
           if (transmitBufferIndex == 0)
             {
             apvUartCharacterTransmitPrime(ApvUartControlBlock_p,
                                           transmitBuffer_p);
             }

           __enable_irq();

 /*          if (apvUartCharacterTransmit(UartTestPhrase[txCounter]) == APV_ERROR_CODE_NONE)
             {
             txCounter = txCounter + 1;

             if (txCounter > strlen(UartTestPhrase))
               {
               txCounter = 0;
               }
             } */
           }
#endif
      }
    }

/******************************************************************************/

  return(0);

/******************************************************************************/
  } /* end of main                                                            */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
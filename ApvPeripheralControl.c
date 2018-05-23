/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvPeripheralControl.c                                                     */
/* 04.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/*  - initialisation and setup of the chip environment for proper peripheral  */
/*    behaviour                                                               */
/*                                                                            */
/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sam3x8e.h>
#include "ApvError.h"
#include "ApvSystemTime.h"
#include "ApvEventTimers.h"
#include "ApvCommsUtilities.h"
#include "ApvPeripheralControl.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

Pmc  ApvPeripheralControlBlock;         // shadow peripheral control block
Pmc *ApvPeripheralControlBlock_p = PMC; // physical block address

// This definition avoids extravagant casting effort from the Atmel constants
Pio  ApvPeripheralLineControlBlock[APV_PERIPHERAL_LINE_GROUPS];    // shadow PIO control blocks
Pio *ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUPS] = // physical block addresses
  {
  APV_PIO_BLOCK_A,
  APV_PIO_BLOCK_B,
  APV_PIO_BLOCK_C,
  APV_PIO_BLOCK_D
  };

         Uart  ApvUartControlBlock;              // shadow UART control block
volatile Uart *ApvUartControlBlock_p = UART;     // physical block address

/******************************************************************************/
/* Function Definitions :                                                     */
/******************************************************************************/
/* apvSwitchPeripheralClock() :                                               */
/*  --> peripheralId     : 0 { <peripheral> } 44 clock enable/disable         */
/*                         switches                                           */
/*  --> peripheralSwitch : [ false == switch the peripheral clock on |        */
/*                           true  == switch the peripheral clock off ]       */
/*                                                                            */
/* - switches the peripheral block clocks on or off. Note in this project     */
/*   using the Rowley CrossWorks toolset many of the other clocks are handled */
/*   by startup code i.e. "SAM3XA_Startup.s", "system_sam3xa.c/.h" and        */
/*   "sam3x8e.h" among others. The peripheral clock registers are "write-     */
/*   only" and are shadowed by "ApvPeripheralControlBlock". Periherals are    */
/*   consistently indentified by ID "position" in either PCxR0 or PCxR1       */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15",          */
/*             p542 - 3, p563 - 4                                             */
/******************************************************************************/

APV_ERROR_CODE apvSwitchPeripheralClock(apvPeripheralId_t peripheralId,
                                        bool              peripheralSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE peripheralControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (peripheralId >= APV_PERIPHERAL_IDS)
    {
    peripheralControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    { // Peripheral clock control is spread across two control registers
    if ((peripheralId >= APV_PERIPHERAL_ID_TC0) && (peripheralId <= APV_PERIPHERAL_ID_TC4)) // TC0 - 4 --> ID 27 - 31
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCER0 = (1 << peripheralId);
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCER1 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }

    if ((peripheralId >= APV_PERIPHERAL_ID_TC5) && (peripheralId <= APV_PERIPHERAL_ID_TC8)) // TC5 - 8 --> ID 32 - 35
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER1    = ApvPeripheralControlBlock.PMC_PCER1 | (1 << (ID_TC5 - peripheralId));
        ApvPeripheralControlBlock_p->PMC_PCER1 = (1 << (ID_TC5 - peripheralId));
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR1    = ApvPeripheralControlBlock.PMC_PCDR1 | (1 << (ID_TC5 - peripheralId));
        ApvPeripheralControlBlock_p->PMC_PCDR1 = (1 << (ID_TC5 - peripheralId));
        }
      }

    if ((peripheralId == APV_PERIPHERAL_ID_UART) || // UART --> ID 8
        (peripheralId == APV_PERIPHERAL_ID_RTT))    // RTT  --> ID 3
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCER0 = (1 << peripheralId);
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }
    }

/******************************************************************************/

  return(peripheralControlError);

/******************************************************************************/
  } /* end of aqpvSwitchPeripheralClock                                       */

/******************************************************************************/
/* apvSwitchNvicDeviceIrq() :                                                 */
/*  --> peripheralIrqId     : peripheral ID. By some miracle this maps to the */
/*                            IRQ identifier as well                          */
/*  --> peripheralIrqSwitch : [ false == interrupt disabled |                 */
/*                               true == interrupt enabled ]                  */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15",          */
/*             p154 - 5                                                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSwitchNvicDeviceIrq(apvPeripheralId_t peripheralIrqId,
                                      bool              peripheralIrqSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE peripheralControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (peripheralIrqId >= APV_PERIPHERAL_IDS)
    {
    peripheralControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (peripheralIrqSwitch == true)
      {
      NVIC_EnableIRQ((IRQn_Type)peripheralIrqId);
      }
    else
      {
      NVIC_DisableIRQ((IRQn_Type)peripheralIrqId);
      }
    }

/******************************************************************************/

  return(peripheralControlError);

/******************************************************************************/
  } /* end of apvSwitchNvicDeviceIrq                                          */

/******************************************************************************/
/* apvSwitchPeripheralLines() :                                               */
/*  --> peripheralIrqId      : peripheral ID. By some miracle this maps to    */
/*                             the IRQ identifier as well                     */
/*  --> peripheralLineSwitch : [ false == peripheral lines disabled |         */
/*                               true  == peripheral lines enabled ]          */
/*                                                                            */
/*  - enable/disable the PIO lines associated with a peripheral. NOTE the     */
/*    Atmel way is to DISABLE the PIO on a line i.e ENABLE the peripheral and */
/*    vice-versa                                                              */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p40,     */
/*             p633 - 4                                                       */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSwitchPeripheralLines(apvPeripheralId_t peripheralLineId,
                                        bool              peripheralLineSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE peripheralControlError = APV_ERROR_CODE_NONE;

/******************************************************************************/


  if (peripheralLineId >= APV_PERIPHERAL_IDS)
    {
    peripheralControlError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (peripheralLineSwitch == true)
      {
      switch(peripheralLineId)
        {
       case APV_PERIPHERAL_ID_UART : // The UART is enabled on PIO A pins PA8 and PA9 as peripheral A. THIS IS THE DEFAULT PIO STATE!
                                     //ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR = PIO_ABSR_P8 | PIO_ABSR_P9;

                                     ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P8  | PIO_PDR_P9;

                                     // Enable the pull-ups on Tx and RX
                                     ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PUER = PIO_PUER_P8 | PIO_PUER_P9;

                                     // Check the status of the attempted configuration; 0 == peripheral selected
                                     if (((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR | PIO_PDR_P8) == PIO_PDR_P8) || 
                                         ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR | PIO_PDR_P9) == PIO_PDR_P9))
                                       {
                                       peripheralControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                       }

                                     break;

        default                    :
                                     break;
        }
      }
    else
      {
      switch(peripheralLineId)
        {
       case APV_PERIPHERAL_ID_UART :  // the UART is disabled on pins PA8 and PA9
                                     ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PER = PIO_PDR_P8 | PIO_PDR_P9;

                                     // Check the status of the attempted configuration; !0 == peripheral deselected
                                     if (((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR | PIO_PDR_P8) != PIO_PDR_P8) || 
                                         ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR | PIO_PDR_P9) != PIO_PDR_P9))
                                       {
                                       peripheralControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                       }

                                     break;

        default                    :
                                     break;
        }
      }
    }

/******************************************************************************/

  return(peripheralControlError);

/******************************************************************************/
  } /* end of apvSwitchPeripheralLines                                        */

/******************************************************************************/
/* apvConfigureUart() :                                                       */
/*  --> uartParity :      [ APV_UART_PARITY_EVEN  = 0 |                       */
/*                          APV_UART_PARITY_ODD   = 1 |                       */
/*                          APV_UART_PARITY_SPACE = 2 |                       */
/*                          APV_UART_PARITY_MARK  = 3 |                       */
/*                          APV_UART_PARITY_NONE  = 4 ]                       */
/*  --> uartChannelMode : [ APV_UART_CHANNEL_MODE_NORMAL          = 0 |       */
/*                          APV_UART_CHANNEL_MODE_AUTOMATIC       = 1 |       */
/*                          APV_UART_CHANNEL_MODE_LOCAL_LOOPBACK  = 2 |       */
/*                          APV_UART_CHANNEL_MODE_REMOTE_LOOPBACK = 3 ]       */
/*  --> uartBaudRate    : [ APV_UART_BAUD_RATE_SELECT_9600   = 0 |            */
/*                          APV_UART_BAUD_RATE_SELECT_19200  = 1 |            */
/*                          APV_UART_BAUD_RATE_SELECT_38400  = 2 |            */
/*                          APV_UART_BAUD_RATE_SELECT_57600  = 3 |            */
/*                          APV_UART_BAUD_RATE_SELECT_76800  = 4 |            */
/*                          APV_UART_BAUD_RATE_SELECT_96000  = 5 |            */
/*                          APV_UART_BAUD_RATE_SELECT_152000 = 6 ]            */
/*  <-- uartErrorCode   : error codes                                         */
/*                                                                            */
/*  - prepare the UART for duplex serial comms                                */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p750     */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvConfigureUart(apvUartParity_t            uartParity,
                                apvUartChannelMode_t       uartChannelMode,
                                apvUartBaudRateSelection_t uartBaudRate)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode   = APV_ERROR_CODE_NONE;

  uint32_t       targetRegister = 0;

/******************************************************************************/

  if (uartParity >= APV_UART_PARITY_SET)
    {
    uartErrorCode = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    if (uartChannelMode >= APV_UART_CHANNEL_MODE_SET)
      {
      uartErrorCode = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    else
      {
      if (uartBaudRate >= APV_UART_BAUD_RATE_SELECT_SET)
        {
        uartErrorCode = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
        }
      else
        {
        targetRegister = (uartParity << UART_MR_PAR_Pos) | (uartChannelMode << UART_MR_CHMODE_Pos); // build the mode register setting
        ApvUartControlBlock.UART_MR    = targetRegister;                                            // shadow-assign the setting

        ApvUartControlBlock_p->UART_MR = targetRegister;                                            // assign the actual setting

        // Compute the baud-rate generator register setting
        targetRegister = APV_EVENT_TIMER_TIMEBASE_BASECLOCK / APV_UART_MCK_FIXED_DIVIDE_16; // MCK / 16 ALWAYS!

        targetRegister = targetRegister / (APV_UART_BAUD_RATE_9600 * ((uint32_t)(uartBaudRate + APV_UART_BAUD_RATE_SELECT_19200)));

        ApvUartControlBlock.UART_BRGR    = targetRegister;
        ApvUartControlBlock_p->UART_BRGR = targetRegister;
        }
      }
    }

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  } /* end of apvConfigureUart                                                */

/******************************************************************************/
/* apvControlUart() :                                                         */
/*  --> uartControlAction : [ APV_UART_CONTROL_ACTION_RESET   |               */
/*                            APV_UART_CONTROL_ACTION_ENABLE  |               */
/*                            APV_UART_CONTROL_ACTION_DISABLE |               */
/*                            APV_UART_CONTROL_ACTION_RESET_STATUS ]          */
/*  <-- uartErrorCode     : error codes                                       */
/*                                                                            */
/*  - control the UART as a duplex serial comms device                        */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p750     */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvControlUart(apvUartControlAction_t uartControlAction)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode   = APV_ERROR_CODE_NONE;

  uint32_t       targetRegister = 0;

/******************************************************************************/

  if (uartControlAction >= APV_UART_CONTROL_ACTIONS)
    {
    uartErrorCode = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    switch(uartControlAction)
      {
      case APV_UART_CONTROL_ACTION_RESET :        targetRegister                 = UART_CR_RSTRX | UART_CR_RSTTX; // duplex RESET
                                                  ApvUartControlBlock.UART_CR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_CR = targetRegister;
                                                  break;

      case APV_UART_CONTROL_ACTION_ENABLE :       targetRegister                 = UART_CR_RXEN | UART_CR_TXEN;   // duplex ENABLE
                                                  ApvUartControlBlock.UART_CR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_CR = targetRegister;
                                                  break;

      case APV_UART_CONTROL_ACTION_DISABLE :      targetRegister                 = UART_CR_RXDIS | UART_CR_TXDIS; // duplex DISABLE
                                                  ApvUartControlBlock.UART_CR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_CR = targetRegister;
                                                  break;

      case APV_UART_CONTROL_ACTION_RESET_STATUS : targetRegister                 = UART_CR_RSTSTA;                // duplex RESET STATUS
                                                  ApvUartControlBlock.UART_CR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_CR = targetRegister;
                                                  break;

      default                                   : uartErrorCode = APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR;
                                                  break;
      }
    }

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  } /* end of apvControlUart                                                  */

/******************************************************************************/
/* apvUartCharacterTransmit() :                                               */
/*  --> transmitBuffer : holding cell for an 8-bit character code             */
/*  <-- uartErrorCode  : error codes                                          */
/*                                                                            */
/*  - load a 'transmit-side' (Arduino Tx) character into the UART transmit    */
/*    holding register                                                        */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p750     */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUartCharacterTransmit(uint8_t transmitBuffer)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode   = APV_ERROR_CODE_NONE;
  uint32_t       statusRegister  = 0;

/******************************************************************************/

   __disable_irq();

  statusRegister = ApvUartControlBlock_p->UART_SR;

  __enable_irq();

  if ((statusRegister & UART_SR_TXEMPTY) && (statusRegister & UART_SR_TXRDY))
    {
    ApvUartControlBlock_p->UART_THR = transmitBuffer;
    ApvUartControlBlock.UART_THR    = transmitBuffer;
    }
  else
    {
    uartErrorCode = APV_SERIAL_ERROR_CODE_TRANSMITTER_NOT_READY;
    }

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  } /* end of apvUartCharacterTransmit                                        */

/******************************************************************************/
/* apvUartCharacterTransmitPrime() :                                          */
/*  --> uartControlBloc    k   : physical address of the UART peripheral      */
/*  --> uartTransmitBufferList : pointer to the ready "filled" transmit ring- */
/*                               buffer structure                             */
/*  --> uartTransmitBuffer     : poibter to the active transmit ring-buffer   */
/*  <-- uartErrorCode      : error codes                                      */
/*                                                                            */
/*  - initiate interrupt-driven UART transmission                             */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUartCharacterTransmitPrime(Uart             *uartControlBlock,
                                             apvRingBuffer_t  *uartTransmitBufferList,
                                             apvRingBuffer_t **uartTransmitBuffer)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode   = APV_ERROR_CODE_NONE;
  uint32_t       transmitBuffer  = 0,
                 statusRegister  = 0;

/******************************************************************************/
  
  if ((uartTransmitBufferList == NULL) || (uartControlBlock == NULL))
    {
    uartErrorCode = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    __disable_irq();

    // Does the tranmit buffer liat have a buffer in it and does this buffer have characters in it ?
    if (apvRingBufferUnLoad( uartTransmitBufferList,
                            (uint32_t *)uartTransmitBuffer,
                             sizeof(uint8_t),
                             false) != 0)
      {
      if (apvRingBufferUnLoad(*uartTransmitBuffer,
                              &transmitBuffer,
                               sizeof(uint8_t),
                               false) != 0)
        {
        statusRegister = uartControlBlock->UART_SR;

        // If all Tx transmit operations have stopped, load and go
        if ((statusRegister & UART_SR_TXEMPTY) && (statusRegister & UART_SR_TXRDY))
          {
          // Switch on the Tx interrupt
          uartControlBlock->UART_IER    = UART_IER_TXRDY;

          // Load the first character and leave the rest to the ISR
          uartControlBlock->UART_THR    = transmitBuffer;
          ApvUartControlBlock.UART_THR  = transmitBuffer; // shadow
          }
        else
          {
          uartErrorCode = APV_SERIAL_ERROR_CODE_TRANSMITTER_NOT_READY;
          }
        }
      else
        {
        uartErrorCode = APV_ERROR_CODE_RING_BUFFER_EMPTY;
        }
      }
    else
      {
      uartErrorCode = APV_ERROR_CODE_RING_BUFFER_LIST_EMPTY;
      }


    __enable_irq();
    }

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  }

/******************************************************************************/
/* apvUartCharacterTransmit() :                                               */
/*  --> receiveBuffer : holding cell for an 8-bit character code              */
/*  <-- uartErrorCode : error codes                                           */
/*                                                                            */
/*  - load a 'receive-side' (Arduino Rx) character into the UART receiver     */
/*    holding register                                                        */
/*                                                                            */
/* Reference : "Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", p750     */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUartCharacterReceive(uint8_t *receiveBuffer)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode = APV_ERROR_CODE_NONE;

/******************************************************************************/

  *receiveBuffer = ApvUartControlBlock_p->UART_RHR;

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  } /* end of apvUartCharacterReceive                                         */

/******************************************************************************/
/* apvUartEnableInterrupt() :                                                 */
/*  --> apvUartInterruptSelect_t : [ APV_UART_INTERRUPT_SELECT_TRANSMIT |     */
/*                                   APV_UART_INTERRUPT_SELECT_RECEIVE  |     */
/*                                   APV_UART_INTERRUPT_SELECT_DUPLEX ]       */
/*  --> interruptSwitch          : [ false == DISABLE INTERRUPT |             */
/*                                   true  == ENABLE  INTERRUPT ]             */
/*  <-- uartErrorCode            : error codes                                */
/*                                                                            */
/*  - switch the UART transmit and/or receive interrupts on or off            */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUartSwitchInterrupt(apvUartInterruptSelect_t interruptSelect,
                                      bool                     interruptSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode  = APV_ERROR_CODE_NONE;

  uint32_t       targetRegister = 0;

/******************************************************************************/

  if (interruptSelect >= APV_UART_INTERRUPT_SELECT_SET)
    {
    uartErrorCode = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }
  else
    {
    // First disable ALL UART interrupt sources
    targetRegister = UART_IDR_RXRDY | UART_IDR_TXRDY | UART_IDR_ENDRX   | UART_IDR_ENDTX  | UART_IDR_OVRE | 
                     UART_IDR_FRAME | UART_IDR_PARE  | UART_IDR_TXEMPTY | UART_IDR_TXBUFE | UART_IDR_RXBUFF;

    ApvUartControlBlock_p->UART_IDR = targetRegister;

    // Disable all PDC UART transfers
    targetRegister = UART_PTCR_TXTDIS | UART_PTCR_RXTDIS;

    ApvUartControlBlock_p->UART_PTCR = targetRegister;

    switch(interruptSelect)
      {
      case APV_UART_INTERRUPT_SELECT_TRANSMIT : targetRegister                    = UART_IER_TXRDY;

                                                if (interruptSwitch == true)
                                                  {
                                                  ApvUartControlBlock.UART_IER    = targetRegister;
                                                  ApvUartControlBlock_p->UART_IER = targetRegister;
                                                  }
                                                else
                                                  {
                                                  ApvUartControlBlock.UART_IDR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_IDR = targetRegister;
                                                  }

                                                break;

      case APV_UART_INTERRUPT_SELECT_RECEIVE  : targetRegister                    = UART_IER_RXRDY;

                                                if (interruptSwitch == true)
                                                  {
                                                  ApvUartControlBlock.UART_IER    = targetRegister;
                                                  ApvUartControlBlock_p->UART_IER = targetRegister;
                                                  }
                                                else
                                                  {
                                                  ApvUartControlBlock.UART_IDR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_IDR = targetRegister;
                                                  }

                                                break;

      case APV_UART_INTERRUPT_SELECT_DUPLEX   : targetRegister                    = UART_IER_TXRDY | UART_IER_RXRDY;

                                                if (interruptSwitch == true)
                                                  {
                                                  ApvUartControlBlock.UART_IER    = targetRegister;
                                                  ApvUartControlBlock_p->UART_IER = targetRegister;
                                                  }
                                                else
                                                  {
                                                  ApvUartControlBlock.UART_IDR    = targetRegister;
                                                  ApvUartControlBlock_p->UART_IDR = targetRegister;
                                                  }

                                                break;

      default                                 : uartErrorCode = APV_ERROR_CODE_MESSAGE_DEFINITION_ERROR;
                                                break;
      }
    }

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  } /* end of apvUartSwitchInterrupt                                          */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
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
#include "ApvUtilities.h"
#include "ApvSystemTime.h"
#include "ApvEventTimers.h"
#include "ApvCommsUtilities.h"
#include "ApvPeripheralControl.h"

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

         Pmc                          ApvPeripheralControlBlock;                                      // shadow peripheral control block
         Pmc                         *ApvPeripheralControlBlock_p = PMC;                              // physical block address

         Uart                         ApvUartControlBlock;                                            // shadow UART control block
volatile Uart                        *ApvUartControlBlock_p       = UART;                             // physical block address

         Spi                          ApvSpi0ControlBlock,                                            // Shadow SPI control block
                                     *ApvSpi0ControlBlock_p       = SPI0;                             // physical block address

         apvInterruptPriorityLevel_t  apvInterruptPriorities[APV_DEVICE_INTERRUPT_PRIORITIES_PACKED]; // "packed" array of SAM3A device interrupt priorities

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
    {
    /******************************************************************************/
    /* Peripheral clock control is spread across two control registers            */
    /******************************************************************************/    
     
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

    /******************************************************************************/
    /* UART and RTT Peripherals :                                                 */
    /******************************************************************************/

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
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCDR0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }

    /******************************************************************************/
    /* I/O Port Peripherals :                                                     */
    /******************************************************************************/

    if ((peripheralId >= APV_PERIPHERAL_ID_PIOA) && (peripheralId <= APV_PERIPHERAL_ID_PIOD))
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER0 = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCER0 = (1 << peripheralId);
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCDR0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }

    /******************************************************************************/
    /* SPI Peripherals :                                                          */
    /******************************************************************************/

    if (peripheralId == APV_PERIPHERAL_ID_SPI0)
      {
      if (peripheralSwitch == true)
        {
        ApvPeripheralControlBlock.PMC_PCER0    = ApvPeripheralControlBlock.PMC_PCER0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCER0 = (1 << peripheralId);
        }
      else
        {
        ApvPeripheralControlBlock.PMC_PCDR0    = ApvPeripheralControlBlock.PMC_PCDR0 | (1 << peripheralId);
        ApvPeripheralControlBlock_p->PMC_PCDR0 = (1 << peripheralId);
        }
      }

    /******************************************************************************/
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
/*  --> peripheralLineId      : peripheral ID. By some miracle this maps to   */
/*                              the IRQ identifier as well                    */
/*  --> peripheralLineSwitch : [ false == peripheral lines disabled |         */
/*                               true  == peripheral lines enabled ]          */
/*                                                                            */
/*  - enable/disable the PIO lines associated with a peripheral. NOTE the     */
/*    Atmel way is to DISABLE the PIO on a line i.e ENABLE the peripheral and */
/*    vice-versa. The nomenclature is a bit confusing : for instance "PER"    */
/*    means "enable the parallel I/O line" NOT(!) "enable the peripheral"     */
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
  uint32_t       peripheralABSelect     = 0; // temporary for read-modify-write

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
        /******************************************************************************/
        /* Peripheral I/O Allocations :                                               */
        /******************************************************************************/

        case APV_PERIPHERAL_ID_UART : // The UART is enabled on PIO A pins PA8 and PA9 as peripheral A. THIS IS THE DEFAULT PIO STATE!
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P8  | PIO_PDR_P9;

                                      // Enable the pull-ups on Tx and RX
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PUER = PIO_PUER_P8 | PIO_PUER_P9;

                                      // Check the status of the attempted configuration; 0 == peripheral selected
                                      if (((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR & PIO_PDR_P8) == PIO_PDR_P8) || 
                                          ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR & PIO_PDR_P9) == PIO_PDR_P9))
                                        {
                                        peripheralControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                        }

                                      break;

        /******************************************************************************/
        /* SPI0 : Arduino usage :                                                     */
        /* -------------------------------------------------------------------------- */
        /*  Arduino   |   I/O   | Peripheral |   Atmel    |          Notes            */
        /*    Name    |   Pin   |   Select   |    Name    |                           */
        /* -------------------------------------------------------------------------- */
        /*    MISO    | PIOA/25 |     'A'    | SPIO_MISO  |                           */
        /*    MOSI    | PIOA/26 |     'A'    | SPIO_MOSI  |                           */
        /*    SPCK    | PIOA/27 |     'A'    | SPIO_SPCK  |                           */
        /*     SS0    | PIOA/28 |     'A'    | SPIO_NPCS0 |                           */
        /*     SS1    | PIOA/29 |     'A'    | SPIO_NPCS1 |                           */
        /*     SS2    | PIOB/21 |     'B'    | SPIO_NPCS2 | A/D14 - of 12-bit ADC ??? */
        /*     SS3    | POIB/23 |     'B'    | SPIO_NPCS3 | not brought out to header */
        /******************************************************************************/

        case APV_PERIPHERAL_ID_SPI0 : // Program PIOA/25 - SPI0_MISO
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P25;                 // enable SPI0 MISO
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR = (uint32_t)(~PIO_ABSR_P25);   // select as peripheral 'A'

                                      // Program PIOA/26 - SPIO_MOSI
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P26;                 // enable SPI0 MOSI
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR = (uint32_t)(~PIO_ABSR_P26);   // select as peripheral 'A'

                                      // Program PIOA/27 - SPI0_SPCK
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P27;                 // enable SPI0 SPCK
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR = (uint32_t)(~PIO_ABSR_P27);   // select as peripheral 'A'

                                      // Program PIOA/28 - SPI0_NPCS0
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P28;                 // enable SPI0 NPCS0
                                      peripheralABSelect = ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR;
                                      peripheralABSelect = peripheralABSelect & (uint32_t)(~PIO_ABSR_P28);
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR = peripheralABSelect;          // select as peripheral 'A'

                                      // Program PIOA/29 - SPIO_NPCS1
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PDR  = PIO_PDR_P29;                 // enable SPI0 NPCS1
                                      peripheralABSelect = ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR;
                                      peripheralABSelect = peripheralABSelect & (uint32_t)(~PIO_ABSR_P29);
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_ABSR = peripheralABSelect;          // select as peripheral 'A'

                                      //Program PIOB/21 - SPI0_NPCS2
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_B]->PIO_PDR  = PIO_PDR_P21;                 // enable SPI0 NPCS2
                                      peripheralABSelect = ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_B]->PIO_ABSR;
                                      peripheralABSelect = peripheralABSelect | PIO_ABSR_P21;
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_B]->PIO_ABSR = peripheralABSelect;          // select as peripheral 'A'

                                      break;

        default                     :
                                      break;
        }
      }
    else
      {
      switch(peripheralLineId)
        {
        /******************************************************************************/
        /* Peripheral I/O Allocations :                                               */
        /******************************************************************************/

        case APV_PERIPHERAL_ID_UART : // The UART is disabled on pins PA8 and PA9
                                      ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PER = PIO_PDR_P8 | PIO_PDR_P9;

                                      // Check the status of the attempted configuration; !0 == peripheral deselected
                                      if (((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR & PIO_PDR_P8) != PIO_PDR_P8) || 
                                          ((ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUP_A]->PIO_PSR & PIO_PDR_P9) != PIO_PDR_P9))
                                        {
                                        peripheralControlError = APV_ERROR_CODE_CONFIGURATION_ERROR;
                                        }

                                      break;

        case APV_PERIPHERAL_ID_SPI0 :
                                      break;

        default                     :
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
/*  --> uartControlBlock : address of the serial UART hardware definition     */
/*  --> transmitBuffer   : a single-character transmit buffer                 */
/*  <-- uartErrorCode    : error codes                                        */
/*                                                                            */
/* - put a single character onto the output port and trigger interrupts to    */
/*   stream out further characters in an external ring-buffer                 */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUartCharacterTransmitPrime(Uart     *uartControlBlock,
                                             uint32_t  transmitBuffer,
                                             bool      interruptControl)
  {
/******************************************************************************/

  APV_ERROR_CODE uartErrorCode  = APV_ERROR_CODE_NONE;

  uint32_t       statusRegister = 0;

/******************************************************************************/

  if (uartControlBlock == NULL)
    {
    uartErrorCode = APV_ERROR_CODE_NULL_PARAMETER;
    }
  else
    {
    if (interruptControl == true)
     {
     APV_CRITICAL_REGION_ENTRY();
     }

    statusRegister = uartControlBlock->UART_SR;

    // If all Tx transmit operations have stopped, load and go
    if ((statusRegister & UART_SR_TXEMPTY) && (statusRegister & UART_SR_TXRDY))
      {
      // Switch on the Tx interrupt
      uartControlBlock->UART_IER = UART_IER_TXRDY;

      // Load the character and leave the rest to the ISR
      uartControlBlock->UART_THR    = transmitBuffer;
      ApvUartControlBlock.UART_THR  = transmitBuffer; // shadow
      }
    else
      {
      uartErrorCode = APV_SERIAL_ERROR_CODE_TRANSMITTER_NOT_READY;
      }

    if (interruptControl == true)
      {
      APV_CRITICAL_REGION_EXIT();
      }
    }

/******************************************************************************/

  return(uartErrorCode);

/******************************************************************************/
  } /* end of apvUartCharacterTransmitPrime                                   */

/******************************************************************************/
/* apvUartBufferTransmitPrime() :                                             */
/*  --> uartControlBlock       : physical address of the UART peripheral      */
/*  --> uartTransmitBufferList : pointer to the ready "filled" transmit ring- */
/*                               buffer structure                             */
/*  --> uartTransmitBuffer     : pointer to the active transmit ring-buffer   */
/*  <-- uartErrorCode          : error codes                                  */
/*                                                                            */
/*  - initiate interrupt-driven UART transmission                             */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvUartBufferTransmitPrime(Uart             *uartControlBlock,
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

    // Does the transmit buffer liat have a buffer in it and does this buffer have characters in it ?
    if (apvRingBufferUnLoad( uartTransmitBufferList,
                             APV_RING_BUFFER_TOKEN_TYPE_LONG_WORD,
                            (uint32_t *)uartTransmitBuffer,
                             sizeof(uint8_t),
                             false) != 0)
      {
      if (apvRingBufferUnLoad(*uartTransmitBuffer,
                               APV_RING_BUFFER_TOKEN_TYPE_LONG_WORD,
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
  } /* end of apvUartBufferTransmitPrime                                      */

/******************************************************************************/
/* apvUartCharacterReceive() :                                                */
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
/* apvGetInterruptPriority() :                                                */
/*  --> interruptSourceNumber : APV_SYSTEM_INTERRUPT_ID_NMI { interrupt }     */
/*                              APV_PERIPHERAL_IDS                            */
/*  <-- interruptPriority     : [ 0 .. 15 ]                                   */
/*  <-- priorityError         : error codes                                   */
/*                                                                            */
/* - get the priority level for an interrupt source, including the CM3 core   */
/*   services' interrupts and the peripheral interrupts. ARM core services    */
/*   and peripheral interrupts have their own seperate interrupt registers,   */
/*   the CMSIS presents a common interface at the software layer unifying     */
/*   access per function type.                                                */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               10-19 p152                                                   */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvGetInterruptPriority(int16_t  interruptSourceNumber,
                                       uint8_t *interruptPriority)
  {
/******************************************************************************/

  APV_ERROR_CODE priorityError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (((interruptSourceNumber >= APV_SYSTEM_INTERRUPT_ID_NMI) && (interruptSourceNumber <= APV_SYSTEM_INTERRUPT_ID_USAGE_FAULT)) ||
       (interruptSourceNumber == APV_SYSTEM_INTERRUPT_ID_SUPERVISOR_CALL) ||
      ((interruptSourceNumber >= APV_SYSTEM_INTERRUPT_ID_PEND_SUPERVISORY) && (interruptSourceNumber <= APV_SYSTEM_INTERRUPT_ID_SYSTEM_TICK)) || 
       (interruptSourceNumber <= APV_PERIPHERAL_IDS))
    {
    *interruptPriority = NVIC_GetPriority(interruptSourceNumber);
    }
  else
    {
    priorityError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }

/******************************************************************************/

  return(priorityError);

/******************************************************************************/
  } /* end of apvGetInterruptPriority                                         */

/******************************************************************************/
/* apvSetInterruptPriority() :                                                */
/*  -->  interruptSourceNumber     : APV_SYSTEM_INTERRUPT_ID_NMI              */
/*                                   { interrupt } APV_PERIPHERAL_IDS         */
/*  -->  interruptPriority         : [ 0 .. 15 ]                              */
/*  <--> deviceInterruptPriorities : record of device interrupt priorities    */
/*  <-- priorityError              : error codes                              */
/*                                                                            */
/* - set the priority level for an interrupt source, including the CM3 core   */
/*   services' interrupts and the peripheral interrupts. ARM core services    */
/*   and peripheral interrupts have their own seperate interrupt registers,   */
/*   the CMSIS presents a common interface at the software layer unifying     */
/*   access per function type                                                 */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               10-19 p152                                                   */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSetInterruptPriority(int16_t                      interruptSourceNumber,
                                       uint8_t                      interruptPriority,
                                       apvInterruptPriorityLevel_t *deviceInterruptPriorities)
  {
/******************************************************************************/

  APV_ERROR_CODE priorityError            = APV_ERROR_CODE_NONE;

  uint16_t        interruptPriorityOffset = 0, // used to compute the index into the device interrupt priority table
                  interruptPriorityPack   = 0;

/******************************************************************************/

  if (((interruptSourceNumber >= APV_SYSTEM_INTERRUPT_ID_NMI) && (interruptSourceNumber <= APV_SYSTEM_INTERRUPT_ID_USAGE_FAULT)) ||
       (interruptSourceNumber == APV_SYSTEM_INTERRUPT_ID_SUPERVISOR_CALL) ||
      ((interruptSourceNumber >= APV_SYSTEM_INTERRUPT_ID_PEND_SUPERVISORY) && (interruptSourceNumber <= APV_SYSTEM_INTERRUPT_ID_SYSTEM_TICK)) || 
      ((interruptSourceNumber >= APV_PERIPHERAL_ID_SUPC) && (interruptSourceNumber <= APV_PERIPHERAL_IDS)))
    {
    NVIC_SetPriority(interruptSourceNumber,
                     interruptPriority);

    /******************************************************************************/
    /* Device interrupt priorities are recorded locally                           */
    /******************************************************************************/

    interruptPriorityOffset = (interruptSourceNumber + (-APV_SYSTEM_INTERRUPT_ID_MEMORY_MANAGEMENT_FAULT)) / APV_DEVICE_INTERRUPT_PRIORITIES;
    interruptPriorityPack   = (interruptSourceNumber + (-APV_SYSTEM_INTERRUPT_ID_MEMORY_MANAGEMENT_FAULT)) % APV_DEVICE_INTERRUPT_PRIORITIES;
    
    (deviceInterruptPriorities + interruptPriorityOffset)->deviceInterruptPriorities = 
          (deviceInterruptPriorities + interruptPriorityOffset)->deviceInterruptPriorities & ((uint32_t)(~(SAM3A_INTERRUPT_PRIORITY_LEVEL_MASK << (interruptPriorityPack * SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS))));

    (deviceInterruptPriorities + interruptPriorityOffset)->deviceInterruptPriorities = 
          (deviceInterruptPriorities + interruptPriorityOffset)->deviceInterruptPriorities | ((uint32_t)(interruptPriority << (interruptPriorityPack * SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS)));

    /******************************************************************************/
    }
  else
    {
    priorityError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
    }

/******************************************************************************/

  return(priorityError);

/******************************************************************************/
  } /* end of apvSetInterruptPriority                                         */

/******************************************************************************/
/* SPI Peripheral Function Definitions :                                      */
/******************************************************************************/
/* apvSPIEnable() :                                                           */
/*  --> spiControlBlock_p : address of the SPI hardware peripheral block      */
/*  --> spiEnable         : [ false == disable SPI | true == enable SPI ]     */
/*  <-- spiError          : error codes                                       */
/*                                                                            */
/* - enable the SPI peripheral                                                */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.1 p693                                                  */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPIEnable(Spi  *spiControlBlock_p,
                            bool  spiEnable)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    if (spiEnable == true)
      {
      spiControlBlock_p->SPI_CR = SPI_CR_SPIEN;
      }
    else
      {
      spiControlBlock_p->SPI_CR = SPI_CR_SPIDIS;
      }
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }

/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSPIEnable                                                    */

/******************************************************************************/
/* apvSPISetNPCSEndOfCharacterState() :                                       */
/*  --> spiControlBlock_p : address of the SPI hardware peripheral block      */
/*  --> lastTransferState : [ chip select asserts | de-asserts ]              */
/*  <-- spiError          : error codes                                       */
/*                                                                            */
/* - the current chip select can be set to de-assert or remain asserted after */
/*   the current transmit character has finished transmission                 */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.1 p693                                                  */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPISetNPCSEndOfCharacterState(Spi                           *spiControlBlock_p,
                                                apvSPILastTransferNPCSState_t  lastTransferState)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    if (lastTransferState == APV_SPI_LAST_TRANSFER_NPCS_STATE_ASSERT)
      {
      spiControlBlock_p->SPI_CR = SPI_CR_LASTXFER;
      }
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }
    
/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* apvSPISetNPCSEndOfCharacterState                                       */

/******************************************************************************/
/* apvSPISetOperatingMode() :                                                 */
/*  --> spiControlBlock_p       : address of the SPI hardware peripheral      */
/*                                block                                       */
/*  --> controlMode             : operate as a master or a slave              */
/*  --> peripheralMode          : [ 0 == chip-selects 1:1 peripheral device | */
/*                                  1 == chip-selects --> 4-16 decoder ]      */
/*  --> chipSelectDecode        : chip-selects are direct or encode 4 bits to */
/*                                16 selects                                  */
/*  --> modeFaultDetect         : enable/disable mode fault detection         */
/*  --> waitOnDataRead          : (in master mode) no further transfers are   */
/*                                allowed until the receiver register is      */
/*                                empty - prevents receiver overrun           */
/*  --> loopbackEnable          : enable/disable local loopback               */
/*  --> delayBetweenChipSelects : inter-NPCS delay in nanoseconds (65.335us   */
/*                                maximum)                                    */
/*  <-- spiError                : error codes                                 */
/*                                                                            */
/* - set the SPI peripheral operating mode in one operation. IMPLIES the      */
/*   chip-select will be deferred until a transmit phase is initiated ((PS    */
/*   == 1) leave at default '00')                                             */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.2 p694                                                  */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPISetOperatingMode(Spi                      *spiControlBlock_p,
                                      apvSPIMasterSlaveMode_t   controlMode,
                                      apvSPIPeripheralSelect_t  peripheralMode,
                                      apvSPIChipSelectDecode_t  chipSelectDecode,
                                      apvSPIModeFaultDetect_t   modeFaultDetect,
                                      apvSPIWaitOnDataRead_t    waitOnDataRead,
                                      apvSPILoopbackEnable_t    loopbackEnable,
                                      uint16_t                  delayBetweenChipSelects)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError        = APV_ERROR_CODE_NONE;

  uint32_t       spiModeRegister = 0; // used to build the final write to the 
                                      // register
  uint16_t       chipSelectDelay = 0; // used to compute inter-chip-select 
                                      // delay counts

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    if (controlMode == APV_SPI_MASTER_MODE) // default to slave
      {
      spiModeRegister = SPI_MR_MSTR;
      }

    /* STRONGLY RECOMMEND THIS!!! */
    if (peripheralMode == APV_SPI_PERIPHERAL_SELECT_VARIABLE) // chip-select is defined at transmit-tme
      {
      spiModeRegister = spiModeRegister | SPI_MR_PS;
      }

    if (chipSelectDecode == APV_SPI_CHIP_SELECT_DECODE_4_TO_16) // default to 1:1 NPCS
      {
      spiModeRegister = spiModeRegister | SPI_MR_PCSDEC;
      }

    if (modeFaultDetect == APV_SPI_MODE_FAULT_DETECTION_ENABLED) // default to disabled
      {
      spiModeRegister = spiModeRegister & (~((uint32_t)APV_SPI_MODE_FAULT_DETECTION_ENABLED));
      }
    else
      {
      spiModeRegister = spiModeRegister | SPI_MR_MODFDIS;
      }

    if (waitOnDataRead == APV_SPI_WAIT_ON_DATA_READ_ENABLED) // default to "no wait"
      {
      spiModeRegister = spiModeRegister | SPI_MR_WDRBT;
      }

    if (loopbackEnable == APV_SPI_LOOPBACK_ENABLED) // default to "no loopback"
      {
      spiModeRegister = spiModeRegister | SPI_MR_LLB;
      }

    chipSelectDelay = delayBetweenChipSelects / ((uint16_t)APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL);
    
    if (chipSelectDelay < APV_SPI_CHIP_SELECT_DELAY_MINIMUM)
      {
      chipSelectDelay = APV_SPI_CHIP_SELECT_DELAY_MINIMUM;
      }
    else
      {
      if (chipSelectDelay >= APV_SPI_CHIP_SELECT_DELAY_MAXIMUM)
        {
        chipSelectDelay = APV_SPI_CHIP_SELECT_DELAY_MAXIMUM;
        }
      }

    spiModeRegister = spiModeRegister | SPI_MR_DLYBCS(((uint8_t)chipSelectDelay)); // 6 =< delays <= (delayBetweenChipSelects) / MCLK

    // Write the whole ensemble
    spiControlBlock_p->SPI_MR = spiModeRegister;
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }

/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSPISetOperatingMode                                          */

/******************************************************************************/
/* apvSPIDriveChipSelect() :                                                  */
/*  --> spiControlBlock_p : address of the SPI hardware peripheral block      */
/*  --> chipSelectCode    : direct or encoded NPCS                            */
/*  <-- spiError          : error codes                                       */
/*                                                                            */
/* - assert or de-assert SPI chip-select :                                    */
/*    - DIRECT  : 0 { NPCS }  3                                               */
/*    - ENCODED : 0 { NPCS } 14                                               */
/*                                                                            */
/*   NOTE : Arduino DUE does not connect SS3(PB23) - only 3 chip-selects are  */
/*          available!                                                        */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.2 p694                                                  */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPIDriveChipSelect(Spi     *spiControlBlock_p,
                                     uint8_t  chipSelectCode)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    if (spiControlBlock_p->SPI_MR & SPI_MR_PCSDEC) // chip-select decoding is selected
      {
      if (chipSelectCode <= APV_SPI_MAXIMUM_ENCODED_CHIP_SELECT)
        {
        spiControlBlock_p->SPI_MR = (spiControlBlock_p->SPI_MR & (~((uint32_t)SPI_MR_PCS_Msk))) | SPI_MR_PCS(chipSelectCode);
        }
      else
        {
        spiError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
        }
      }
    else
      {
      if (chipSelectCode <= APV_SPI_MAXIMUM_DIRECT_CHIP_SELECT)
        {
        spiControlBlock_p->SPI_MR = (spiControlBlock_p->SPI_MR & (~((uint32_t)SPI_MR_PCS_Msk))) | SPI_MR_PCS(chipSelectCode);
        }
      else
        {
        spiError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
        }
      }
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }
      
/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSPIDriveChipSelect                                           */

/******************************************************************************/
/* apvSPIReset() :                                                            */
/*  --> spiControlBlock_p : address of the SPI hardware peripheral block      */
/*  <-- spiError          : error codes                                       */
/*                                                                            */
/* - reset the SPI hardware using a software trigger                          */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.1 p693                                                  */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPIReset(Spi *spiControlBlock_p)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    spiControlBlock_p->SPI_CR = SPI_CR_SWRST;
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }
      
/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSPIReset                                                     */

/******************************************************************************/
/* apvSPISwitchInterrupt() :                                                  */
/*  --> spiControlBlock_p : address of the SPI hardware peripheral block      */
/*  --> interruptSelect   : the interrupt source to switch                    */
/*  --> interruptSwitch   : [ interrupt enable == false |                     */
/*                                                interrupt disable == true ] */
/*                                                                            */
/* - enable or disable one SPI peripheral interrupt                           */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.6/7 p700                                                */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPISwitchInterrupt(Spi                     *spiControlBlock_p,
                                     apvSPIInterruptSelect_t  interruptSelect,
                                     bool                     interruptSwitch)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError = APV_ERROR_CODE_NONE;

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    if (interruptSwitch == true)
      {
      switch(interruptSelect)
        {
        case APV_SPI_INTERRUPT_SELECT_RECEIVE_DATA   : spiControlBlock_p->SPI_IER = SPI_IER_RDRF;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_TRANSMIT_DATA  : spiControlBlock_p->SPI_IER = SPI_IER_TDRE;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_MODE_FAULT     : spiControlBlock_p->SPI_IER = SPI_IER_MODF;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_OVERRUN        : spiControlBlock_p->SPI_IER = SPI_IER_OVRES;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_NSS_RISING     : spiControlBlock_p->SPI_IER = SPI_IER_NSSR;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_TRANSMIT_EMPTY : spiControlBlock_p->SPI_IER = SPI_IER_TXEMPTY;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_UNDERRUN       : spiControlBlock_p->SPI_IER = SPI_IER_UNDES;
                                                       break;

        default :                                      break;
        }
      }
    else
      {
      switch(interruptSelect)
        {
        case APV_SPI_INTERRUPT_SELECT_RECEIVE_DATA   : spiControlBlock_p->SPI_IDR = SPI_IDR_RDRF;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_TRANSMIT_DATA  : spiControlBlock_p->SPI_IDR = SPI_IDR_TDRE;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_MODE_FAULT     : spiControlBlock_p->SPI_IDR = SPI_IDR_MODF;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_OVERRUN        : spiControlBlock_p->SPI_IDR = SPI_IDR_OVRES;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_NSS_RISING     : spiControlBlock_p->SPI_IDR = SPI_IDR_NSSR;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_TRANSMIT_EMPTY : spiControlBlock_p->SPI_IDR = SPI_IDR_TXEMPTY;
                                                       break;

        case APV_SPI_INTERRUPT_SELECT_UNDERRUN       : spiControlBlock_p->SPI_IDR = SPI_IDR_UNDES;
                                                       break;

        default :                                      break;
        }
      }
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }

/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSPISwitchInterrupt                                           */

/******************************************************************************/
/* apvSetChipSelectCharacteristics() :                                        */
/*  --> spiControlBlock_p               : address of the SPI hardware         */
/*                                        peripheral block                    */
/*  --> chipSelectRegisterNumber        : 0 { ... } 3                         */
/*  --> serialClockPolarity             : CPOL                                */
/*  --> serialClockDataChange           : CPHA                                */
/*  --> chipSelectSingleSlave           : CSNAAT                              */
/*  --> chipSelectChangeSlave           : CSAAT                               */
/*  --> busDataWidth                    : BITS ( 8 { ... } 16 )               */
/*  --> serialClockBaudRate             : SCBR ( 1 { ... } 255 )              */
/*  --> serialClockFirstTransitionDelay : delay before SPCK in nanoseconds    */
/*                                        (maximum 65535)                     */
/*  --> serialClockInterTransferDelay   : delay between transfers in nano-    */
/*                                        seconds (maximum 65535) - this is   */
/*                                        multiplied by 32 internally         *?
/*                                                                            */
/* - set the characteristics for the NPCS[0:3] in one operation. NOTE : this  */
/*   must ALWAYS be done as the BITS field of the underlying register is un-  */
/*   defined until written                                                    */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.8.9 p703                                                  */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSetChipSelectCharacteristics(Spi                                    *spiControlBlock_p,
                                               apvChipSelectRegisterInstance_t         chipSelectRegisterNumber,
                                               apvSPISerialClockPolarity_t             serialClockPolarity,
                                               apvSPISerialClockPhase_t                serialClockDataChange,
                                               apvSPIChipSelectBehaviourSingleSlave_t  chipSelectSingleSlave,
                                               apvSPIChipSelectBehaviourChangeSlave_t  chipSelectChangeSlave,
                                               uint8_t                                 busDataWidth,
                                               uint32_t                                serialClockBaudRate,
                                               uint16_t                                serialClockFirstTransitionDelay,
                                               uint16_t                                serialClockInterTransferDelay)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError      = APV_ERROR_CODE_NONE;

  uint32_t       spiCSRegister = 0; // used to build the final write to the 
                                    // register
  uint16_t       delayCounter  = 0; // used to compute the SPCK and inter-
                                    // transfer delays

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    { // Chip select register range = 0 { ... } 3
    if (chipSelectRegisterNumber <= APV_SPI_CHIP_SELECT_REGISTER_3)
      { // Data bus width range = 8 { ... } 16
      if ((busDataWidth >= APV_SPI_MINIMUM_BIT_TRANSFER_WIDTH) && (busDataWidth <= APV_SPI_MAXIMUM_BIT_TRANSFER_WIDTH))
        { // The serial clock baudrate divisor cannot be 0 or > 255
        if ((serialClockBaudRate >= APV_SPI_MINIMUM_BAUD_RATE) && (serialClockBaudRate <= APV_SPI_MAXIMUM_BAUD_RATE))
          { // Compute the baud rate divisor
          spiCSRegister = SPI_CSR_SCBR( ((uint8_t)( APV_SPI_MAXIMUM_BAUD_RATE / serialClockBaudRate )) );

          // Compute and load the pre-serial-clock delay
          delayCounter = serialClockFirstTransitionDelay / ((uint16_t)APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL);

          spiCSRegister = spiCSRegister | SPI_CSR_DLYBS(((uint8_t)delayCounter));

          // Compute and load the inter-transfer delay
          delayCounter = serialClockInterTransferDelay / ((uint16_t)APV_SYSTEM_TIMER_CLOCK_MINIMUM_INTERVAL);

          spiCSRegister = spiCSRegister | SPI_CSR_DLYBCT(((uint8_t)delayCounter));

          // Load the data-width in bits
          spiCSRegister = spiCSRegister | ((busDataWidth - APV_SPI_DATA_WIDTH_NORMALISE) << SPI_CSR_BITS_Pos);

          // Load the CSAAT
          if (chipSelectChangeSlave == APV_SPI_CHIP_SELECT_CHANGE_SLAVE_RISE_N)
            {
            spiCSRegister = spiCSRegister | SPI_CSR_CSAAT;
            }

          // Load the CSNAAT
          if (chipSelectSingleSlave == APV_SPI_CHIP_SELECT_SINGLE_SLAVE_RISE)
            {
            spiCSRegister = spiCSRegister | SPI_CSR_CSNAAT;
            }

          // Load the CPHA
          if (serialClockDataChange == APV_SPI_SERIAL_CLOCK_PHASE_DATA_CHANGE_FOLLOWING)
            {
            spiCSRegister = spiCSRegister | SPI_CSR_NCPHA;
            }

          // Load the CPOL
          if (serialClockPolarity == APV_SPI_SERIAL_CLOCK_POLARITY_ONE)
            {
            spiCSRegister = spiCSRegister | SPI_CSR_CPOL;
            }

          // Finally load the chip-select register
          spiControlBlock_p->SPI_CSR[chipSelectRegisterNumber] = spiCSRegister;
          }
        else
          {
          spiError = APV_ERROR_CODE_NULL_PARAMETER;
          }
        }
      else
        {
        spiError = APV_ERROR_CODE_NULL_PARAMETER;
        }
      }
    else
      {
      spiError = APV_ERROR_CODE_PARAMETER_OUT_OF_RANGE;
      }
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }

/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSetChipSelectCharacteristics                                 */

/******************************************************************************/
/* apvSPITransmitCharacter() :                                                */
/*  --> spiControlBlock_p    : address of the SPI hardware                    */
/*                                        peripheral block                    */
/*  --> spiPrefixCharacter   : 0 { ... } 8 bits - high-byte character         */
/*  --> spiTrafficCharacter  :           8 bits - low-character               */
/*  --> spiChipSelect        : [ 0 { ... } 2 | 0 { ... } 7 ]                  */
/*  --> lastTransfer         : 1 == de-assert NPCSx DISREGARDING 'CSAAT'      */
/*  <-- spiError             : error code                                     */
/*                                                                            */
/* - load an 8 [ .. ] 16-bit character into the transmit buffer. The          */
/*   character is passed here as two bytes to logically seperate the function */
/*   i.e. typically an 'address' byte and a 'data' byte. Chip-select          */
/*   behaviour is defined by the 'mode' flags i.e. direct one-to-one with a   */
/*   peripheral or indirectly via a 4-16 decoder. Chip-select de-assert can   */
/*   be forced here even if 'CSAAT' is set to hold off de-assert ('Variable   */
/*   Chip Select' (PS) must be '1')                                           */
/*                                                                            */
/*   Reference : Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section */
/*               32.28.4 p697                                                 */
/*                                                                            */
/******************************************************************************/

APV_ERROR_CODE apvSPITransmitCharacter(Spi                  *spiControlBlock_p,
                                       uint8_t               spiPrefixCharacter,
                                       uint8_t               spiTrafficCharacter,
                                       uint8_t               spiChipSelect,
                                       apvSPILastTransfer_t  lastTransfer)
  {
/******************************************************************************/

  APV_ERROR_CODE spiError     = APV_ERROR_CODE_NONE;
  uint32_t       transmitData = 0; // temporary to build the register value

/******************************************************************************/

  if (spiControlBlock_p != NULL)
    {
    transmitData = (uint32_t)((spiPrefixCharacter << APV_SPI_DATA_PREFIX_SHIFT) | spiTrafficCharacter);
    transmitData = transmitData | SPI_TDR_PCS(spiChipSelect);

    if (lastTransfer == APV_SPI_LAST_TRANSFER_ACTIVE)
      {
      transmitData = transmitData | SPI_TDR_LASTXFER;
      }
    }
  else
    {
    spiError = APV_ERROR_CODE_NULL_PARAMETER;
    }

/******************************************************************************/

  return(spiError);

/******************************************************************************/
  } /* end of apvSPITransmitCharacter                                         */

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

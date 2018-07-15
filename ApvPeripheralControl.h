/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/* ApvPeripheralControl.h                                                     */
/* 04.05.18                                                                   */
/* Paul O'Brien                                                               */
/*                                                                            */
/*  - initialisation and setup of the chip environment for proper peripheral  */
/*    behaviour                                                               */
/*                                                                            */
/******************************************************************************/

#ifndef _APV_PERIPHERAL_CONTROL_H_
#define _APV_PERIPHERAL_CONTROL_H_

/******************************************************************************/
/* Include Files :                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <sam3x8e.h>
#include "ApvUtilities.h"

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

// The UART minimum baud rate for this project is defined here as 9600
#define APV_UART_BAUD_RATE_9600                ((uint32_t)9600)
// The UART always divides MCK by 16
#define APV_UART_MCK_FIXED_DIVIDE_16           ((uint32_t)16)

/******************************************************************************/
/* SAM3A Interrupt Priority Levels 0 { .. } 15 :                              */
/*  - these are applied to all system interrupts ( -12 { .. } -1 ) and all    */
/*    peripheral interrupts ( 0 { .. } 44 )                                   */
/* References: Atmel-11057C-ATARM-SAM3X-SAM3A-Datasheet_23-Mar-15", Section   */
/*             10 - 4 p50 and Section 10-19 p152                              */
/******************************************************************************/

#define SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS     (4)
#define SAM3A_INTERRUPT_PRIORITY_LEVEL_MASK     ((1 << SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS) - 1)

#define SAM3A_INTERRUPT_PRIORITY_LEVEL_MINIMUM  ((1 << SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS) - 1)
#define SAM3A_INTERRUPT_PRIORITY_LEVEL_MAXIMUM  (0)

#define APV_DEVICE_INTERRUPT_PRIORITIES         SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS // used to pack priorities

#define APV_DEVICE_INTERRUPT_PRIORITY_RANGE     ((-(APV_SYSTEM_INTERRUPT_ID_MEMORY_MANAGEMENT_FAULT)) + APV_PERIPHERAL_IDS)   // -( -12 ) + 45  == 57 : 08.07.18
#define APV_DEVICE_INTERRUPT_PRIORITIES_PACKED  ((APV_DEVICE_INTERRUPT_PRIORITY_RANGE / APV_DEVICE_INTERRUPT_PRIORITIES) + 1) // ( 57 / 4 ) + 1 == 15 : 08.07.18
 
/******************************************************************************/
/*                *** PROCESS INTERRUPT PRIORITY LEVELS ***                   */
/******************************************************************************/

#define APV_DEVICE_INTERRUPT_PRIORITY_BASE      APV_INTERRUPT_LEVEL_PRIORITY_MINIMUM

#define APV_DEVICE_INTERRUPT_PRIORITY_SYSTICK   APV_INTERRUPT_LEVEL_PRIORITY_14 // SysTick has elevated priority : 080718

/******************************************************************************/
/* SPI Constants :                                                            */
/******************************************************************************/

// Direct NPCS are 0 { ... } 3
#define APV_SPI_MINIMUM_DIRECT_CHIP_SELECT   (0)
#define APV_SPI_MAXIMUM_DIRECT_CHIP_SELECT   (2) // Arduino DUE only connects SS0-SS2!
// Encoded NPCS are 1 { ... } 15 using an external 4-16 encoder
#define APV_SPI_MINIMUM_ENCODED_CHIP_SELECT  (0)
#define APV_SPI_MAXIMUM_ENCODED_CHIP_SELECT  (7) // Arduino DUE only connects SS0-SS2 ( 0 { ... } 7 ) !
// SPI bus transfers are 8 - 16 bits wide
#define APV_SPI_MINIMUM_BIT_TRANSFER_WIDTH   (8)
#define APV_SPI_MAXIMUM_BIT_TRANSFER_WIDTH  (16)
// Data bits are numbered in the chip-select register field as :
//  0 == 8-bits
//  1 == 9-bits
//  2 == 10-bits
//  3 == 11-bits
//  4 == 12-bits
//  5 == 13-bits
//  6 == 14-bits
//  7 == 15-bits
//  8 == 16-bits
#define APV_SPI_DATA_WIDTH_NORMALISE         (8) // i.e. subtract 8!

#define APV_SPI_MINIMUM_BAUD_DIVISOR         (1)
#define APV_SPI_MAXIMUM_BAUD_DIVISOR       (255)
#define APV_SPI_MAXIMUM_BAUD_RATE            APV_SYSTEM_TIMER_TIMEBASE_BASECLOCK                                 // this may not be realisable!
#define APV_SPI_MINIMUM_BAUD_RATE           (APV_SYSTEM_TIMER_TIMEBASE_BASECLOCK / APV_SPI_MAXIMUM_BAUD_DIVISOR) // 329411 bps

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvUartControlAction_tTag
  {
  APV_UART_CONTROL_ACTION_RESET = 0,
  APV_UART_CONTROL_ACTION_ENABLE,
  APV_UART_CONTROL_ACTION_DISABLE,
  APV_UART_CONTROL_ACTION_RESET_STATUS,
  APV_UART_CONTROL_ACTIONS
  } apvUartControlAction_t;

typedef enum apvUartParity_tTag
  {
  APV_UART_PARITY_EVEN = 0,
  APV_UART_PARITY_ODD,
  APV_UART_PARITY_SPACE,
  APV_UART_PARITY_MARK,
  APV_UART_PARITY_NONE,
  APV_UART_PARITY_SET
  } apvUartParity_t;

typedef enum apvUartChannelMode_tTag
  {
  APV_UART_CHANNEL_MODE_NORMAL = 0,
  APV_UART_CHANNEL_MODE_AUTOMATIC,
  APV_UART_CHANNEL_MODE_LOCAL_LOOPBACK,
  APV_UART_CHANNEL_MODE_REMOTE_LOOPBACK,
  APV_UART_CHANNEL_MODE_SET
  } apvUartChannelMode_t;

typedef enum apvUartBaudRateSelection_tTag
  {
  APV_UART_BAUD_RATE_SELECT_9600 = 0,
  APV_UART_BAUD_RATE_SELECT_19200,
  APV_UART_BAUD_RATE_SELECT_38400,
  APV_UART_BAUD_RATE_SELECT_57600,
  APV_UART_BAUD_RATE_SELECT_76800,
  APV_UART_BAUD_RATE_SELECT_96000,
  APV_UART_BAUD_RATE_SELECT_152000,
  APV_UART_BAUD_RATE_SELECT_SET
  } apvUartBaudRateSelection_t;

typedef enum apvUartInterruptSelect_tTag
  {
  APV_UART_INTERRUPT_SELECT_TRANSMIT = 0,
  APV_UART_INTERRUPT_SELECT_RECEIVE,
  APV_UART_INTERRUPT_SELECT_DUPLEX,
  APV_UART_INTERRUPT_SELECT_SET
  } apvUartInterruptSelect_t;

typedef enum apvInterruptPriorityLevels_tTag
  {
  APV_INTERRUPT_LEVEL_PRIORITY_MAXIMUM = SAM3A_INTERRUPT_PRIORITY_LEVEL_MAXIMUM,
  APV_INTERRUPT_LEVEL_PRIORITY_0       = APV_INTERRUPT_LEVEL_PRIORITY_MAXIMUM,
  APV_INTERRUPT_LEVEL_PRIORITY_1,
  APV_INTERRUPT_LEVEL_PRIORITY_2,
  APV_INTERRUPT_LEVEL_PRIORITY_3,
  APV_INTERRUPT_LEVEL_PRIORITY_4,
  APV_INTERRUPT_LEVEL_PRIORITY_5,
  APV_INTERRUPT_LEVEL_PRIORITY_6,
  APV_INTERRUPT_LEVEL_PRIORITY_7,
  APV_INTERRUPT_LEVEL_PRIORITY_8,
  APV_INTERRUPT_LEVEL_PRIORITY_9,
  APV_INTERRUPT_LEVEL_PRIORITY_10,
  APV_INTERRUPT_LEVEL_PRIORITY_11,
  APV_INTERRUPT_LEVEL_PRIORITY_12,
  APV_INTERRUPT_LEVEL_PRIORITY_13,
  APV_INTERRUPT_LEVEL_PRIORITY_14,
  APV_INTERRUPT_LEVEL_PRIORITY_15,
  APV_INTERRUPT_LEVEL_PRIORITY_MINIMUM = SAM3A_INTERRUPT_PRIORITY_LEVEL_MINIMUM,
  APV_INTERRUPT_LEVEL_PRIORITIES       = (SAM3A_INTERRUPT_PRIORITY_LEVEL_MINIMUM + 1)
  } apvInterruptPriorityLevels_t;

/******************************************************************************/
/* To save a little memory peripheral interrupt priority levels are packed in */
/* blocks of 4-bits (priorities are 0 { .. } 15)                              */
/******************************************************************************/

typedef struct apvInterruptPriorities_tTag
  {
  uint16_t apvInterruptPriority0 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  uint16_t apvInterruptPriority1 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  uint16_t apvInterruptPriority2 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  uint16_t apvInterruptPriority3 : SAM3A_INTERRUPT_PRIORITY_LEVEL_BITS;
  } apvInterruptPriorities_t;

typedef union apvInterruptPriorityLevel_tTag
  {
  apvInterruptPriorities_t interruptPriorities;
  uint16_t                 deviceInterruptPriorities;
  } apvInterruptPriorityLevel_t;

/******************************************************************************/
/* SPI Peripheral Definitions :                                               */
/******************************************************************************/

typedef enum apvSPILastTransferNPCSState_tTag
  {
  APV_SPI_LAST_TRANSFER_NPCS_STATE_DEASSERT = 0,
  APV_SPI_LAST_TRANSFER_NPCS_STATE_ASSERT   = 1,
  APV_SPI_LAST_TRANSFER_NPCS_STATES
  } apvSPILastTransferNPCSState_t;

typedef enum apvSPIMasterSlaveMode_tTag
  {
  APV_SPI_SLAVE_MODE = 0,
  APV_SPI_MASTER_MODE,
  APV_SPI_CONTROL_MODES
  } apvSPIMasterSlaveMode_t;

typedef enum apvSPIPeripheralSelect_tTag
  {
  APV_SPI_PERIPHERAL_SELECT_FIXED = 0,
  APV_SPI_PERIPHERAL_SELECT_VARIABLE,
  APV_SPI_PERIPHERAL_SELECT_SET
  } apvSPIPeripheralSelect_t;

typedef enum apvSPIChipSelectDecode_tTag
  {
  APV_SPI_CHIP_SELECT_DECODE_DIRECT = 0,
  APV_SPI_CHIP_SELECT_DECODE_4_TO_16,
  APV_SPI_CHIP_SELECT_DECODE_SET
  } apvSPIChipSelectDecode_t;

typedef enum apvSPIModeFaultDetect_tTag
  {
  APV_SPI_MODE_FAULT_DETECTION_ENABLED = 0,
  APV_SPI_MODE_FAULT_DETECTION_DISABLED,
  APV_SPI_MODE_FAULT_DETECTION_SET
  } apvSPIModeFaultDetect_t;

typedef enum apvSPIWaitOnDataRead_tTag
  {
  APV_SPI_WAIT_ON_DATA_READ_DISABLED = 0,
  APV_SPI_WAIT_ON_DATA_READ_ENABLED,
  APV_SPI_WAIT_ON_DATA_READ_SET
  } apvSPIWaitOnDataRead_t;

typedef enum apvSPILoopbackEnable_tTag
  {
  APV_SPI_LOOPBACK_DISABLED = 0,
  APV_SPI_LOOPBACK_ENABLED,
  APV_SPI_LOOPBACK_SET
  } apvSPILoopbackEnable_t;

typedef enum apvSPIInterruptSelect_tTag
  {
  APV_SPI_INTERRUPT_SELECT_RECEIVE_DATA = 0,
  APV_SPI_INTERRUPT_SELECT_TRANSMIT_DATA,
  APV_SPI_INTERRUPT_SELECT_MODE_FAULT,
  APV_SPI_INTERRUPT_SELECT_OVERRUN,
  APV_SPI_INTERRUPT_SELECT_NSS_RISING,
  APV_SPI_INTERRUPT_SELECT_TRANSMIT_EMPTY,
  APV_SPI_INTERRUPT_SELECT_UNDERRUN,
  APV_SPI_INTERRUPT_SELECT_SET
  } apvSPIInterruptSelect_t;

// CPOL : The logic level of the SPCK "inactive" state
typedef enum apvSPISerialClockPolarity_tTag
  {
  APV_SPI_SERIAL_CLOCK_POLARITY_ZERO = 0,
  APV_SPI_SERIAL_CLOCK_POLARITY_ONE,
  APV_SPI_SERIAL_CLOCK_POLARITY_SET
  } apvSPISerialClockPolarity_t;

// CPHA : The edge of SPCK when data is changed (data is captured on the opposite edge)
typedef enum apvSPISerialClockPhase_tTag
  {
  APV_SPI_SERIAL_CLOCK_PHASE_DATA_CHANGE_LEADING = 0,
  APV_SPI_SERIAL_CLOCK_PHASE_DATA_CHANGE_FOLLOWING,
  APV_SPI_SERIAL_CLOCK_PHASE_DATA_CHANGE_SET
  } apvSPISerialClockPhase_t;

// CSNAAT : Defines the chip select behaviour between data transfers when the same chip-
// select is used (when the transmit buffer is reloaded before the previous 
// transfer has completed). THIS IS CONDITIONAL ON (CSAAT == 0) :
//   "apvSPIChipSelectBehaviourChangeSlave_t" == "APV_SPI_CHIP_SELECT_CHANGE_SLAVE_RISE"
// 0 == chip-select is held low (active)
// 1 == chip-select always rises after each transfer
typedef enum apvSPIChipSelectBehaviourSingleSlave_tTag
  {
  APV_SPI_CHIP_SELECT_SINGLE_SLAVE_RISE_N = 0,
  APV_SPI_CHIP_SELECT_SINGLE_SLAVE_RISE,
  APV_SPI_CHIP_SELECT_SINGLE_SLAVE_RISE_SET
  } apvSPIChipSelectBehaviourSingleSlave_t;

// CSAAT : Defines the chip select behaviour after a data transfer : 
// 0 == chip-select always rises
// 1 == chip-select never rises until a transfer is requested using a different 
//      chip-select
typedef enum apvSPIChipSelectBehaviourChangeSlave_tTag
  {
  APV_SPI_CHIP_SELECT_CHANGE_SLAVE_RISE   = 0,
  APV_SPI_CHIP_SELECT_CHANGE_SLAVE_RISE_N,
  APV_SPI_CHIP_SELECT_CHANGE_SLAVE_RISE_SET
  } apvSPIChipSelectBehaviourChangeSlave_t;

typedef enum apvChipSelectRegisterInstance_tTag
  {
  APV_SPI_CHIP_SELECT_REGISTER_0 = 0,
  APV_SPI_CHIP_SELECT_REGISTER_1,
  APV_SPI_CHIP_SELECT_REGISTER_2,
  APV_SPI_CHIP_SELECT_REGISTER_3,
  APV_SPI_CHIP_SELECT_REGISTER_SET
  } apvChipSelectRegisterInstance_t;

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

extern          Pmc                          ApvPeripheralControlBlock;                                      // shadow peripheral control block
extern          Pmc                         *ApvPeripheralControlBlock_p;                                    // physical block address

extern          Uart                         ApvUartControlBlock;                                            // shadow UART control block
extern volatile Uart                        *ApvUartControlBlock_p;                                          // physical block address

extern          apvInterruptPriorityLevel_t  apvInterruptPriorities[APV_DEVICE_INTERRUPT_PRIORITIES_PACKED]; // shadow device interrupt priorities

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvSwitchPeripheralClock(apvPeripheralId_t peripheralId,
                                               bool              peripheralSwitch);
extern APV_ERROR_CODE apvSwitchPeripheralLines(apvPeripheralId_t peripheralLineId,
                                               bool              peripheralLineSwitch);
extern APV_ERROR_CODE apvSwitchNvicDeviceIrq(apvPeripheralId_t peripheralIrqId,
                                             bool              peripheralIrqSwitch);
extern APV_ERROR_CODE apvConfigureUart(apvUartParity_t            uartParity,
                                       apvUartChannelMode_t       uartChannelMode,
                                       apvUartBaudRateSelection_t uartBaudRate);
extern APV_ERROR_CODE apvControlUart(apvUartControlAction_t uartControlAction);
extern APV_ERROR_CODE apvUartCharacterTransmit(uint8_t transmitBuffer);
extern APV_ERROR_CODE apvUartCharacterReceive(uint8_t *receiveBuffer);
extern APV_ERROR_CODE apvUartSwitchInterrupt(apvUartInterruptSelect_t interruptSelect,
                                             bool                     interruptSwitch);
extern APV_ERROR_CODE apvGetInterruptPriority(int16_t  interruptSource,
                                              uint8_t *interruptPriority);
extern APV_ERROR_CODE apvSetInterruptPriority(int16_t                      interruptSourceNumber,
                                              uint8_t                      interruptPriority,
                                              apvInterruptPriorityLevel_t *deviceInterruptPriorities);

/******************************************************************************/
/* SPI Peripheral Function Declarations :                                     */
/******************************************************************************/

extern APV_ERROR_CODE apvSPIEnable(Spi  *spiControlBlock_p,
                                   bool  spiEnable);
extern APV_ERROR_CODE apvSPIReset(Spi *spiControlBlock_p);
extern APV_ERROR_CODE apvSPISetNPCSEndOfCharacterState(Spi                           *spiControlBlock_p,
                                                       apvSPILastTransferNPCSState_t  lastTransferState);
extern APV_ERROR_CODE apvSPISetOperatingMode(Spi                      *spiControlBlock_p,
                                             apvSPIMasterSlaveMode_t   controlMode,
                                             apvSPIPeripheralSelect_t  peripheralMode,
                                             apvSPIChipSelectDecode_t  chipSelectDecode,
                                             apvSPIModeFaultDetect_t   modeFaultDetect,
                                             apvSPIWaitOnDataRead_t    waitOnDataRead,
                                             apvSPILoopbackEnable_t    loopbackEnable,
                                             uint8_t                   delayBetweenChipSelects);
extern APV_ERROR_CODE apvSPIDriveChipSelect(Spi     *spiControlBlock_p,
                                            uint8_t  chipSelectCode);
extern APV_ERROR_CODE apvSPISwitchInterrupt(Spi                     *spiControlBlock_p,
                                            apvSPIInterruptSelect_t  interruptSelect,
                                            bool                     interruptSwitch);
extern APV_ERROR_CODE apvSetChipSelectCharacteristics(Spi                                    *spiControlBlock_p,
                                                      uint8_t                                 chipSelectRegisterNumber,
                                                      apvSPISerialClockPolarity_t             serialClockPolarity,
                                                      apvSPISerialClockPhase_t                serialClockDataChange,
                                                      apvSPIChipSelectBehaviourSingleSlave_t  chipSelectSingleSlave,
                                                      apvSPIChipSelectBehaviourChangeSlave_t  chipSelectChangeSlave,
                                                      uint8_t                                 busDataWidth,
                                                      uint32_t                                serialClockBaudRate,
                                                      uint8_t                                 serialClockFirstTransitionDelay,
                                                      uint8_t                                 serialClockInterTransferDelay);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

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

/******************************************************************************/
/* Definitions :                                                              */
/******************************************************************************/

// The physcial addresses of the four peripheral I/O controller blocks
#define APV_PIO_BLOCK_A ((Pio *)0x400E0E00U)
#define APV_PIO_BLOCK_B ((Pio *)0x400E1000U)
#define APV_PIO_BLOCK_C ((Pio *)0x400E1200U)
#define APV_PIO_BLOCK_D ((Pio *)0x400E1400U)

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef enum apvPeripheralLineGroup_tTag
  {
  APV_PERIPHERAL_LINE_GROUP_A = 0,
  APV_PERIPHERAL_LINE_GROUP_B,
  APV_PERIPHERAL_LINE_GROUP_C,
  APV_PERIPHERAL_LINE_GROUP_D,
  APV_PERIPHERAL_LINE_GROUPS
  } apvPeripheralLineGroup_;

typedef enum apvPeripheralId_tTag
  {
  APV_PERIPHERAL_ID_SUPC   = ID_SUPC,
  APV_PERIPHERAL_ID_RSTC   = ID_RSTC,
  APV_PERIPHERAL_ID_RTC    = ID_RTC,
  APV_PERIPHERAL_ID_RTT    = ID_RTT,
  APV_PERIPHERAL_ID_WDT    = ID_WDT,
  APV_PERIPHERAL_ID_PMC    = ID_PMC,
  APV_PERIPHERAL_ID_EFC0   = ID_EFC0,
  APV_PERIPHERAL_ID_EFC1   = ID_EFC1,
  APV_PERIPHERAL_ID_UART   = ID_UART,
  APV_PERIPHERAL_ID_SMC    = ID_SMC,
  APV_PERIPHERAL_ID_PIOA   = ID_PIOA,
  APV_PERIPHERAL_ID_PIOB   = ID_PIOB,
  APV_PERIPHERAL_ID_PIOC   = ID_PIOC,
  APV_PERIPHERAL_ID_PIOD   = ID_PIOD,
  APV_PERIPHERAL_ID_USART0 = ID_USART0,
  APV_PERIPHERAL_ID_USART1 = ID_USART1,
  APV_PERIPHERAL_ID_USART2 = ID_USART2,
  APV_PERIPHERAL_ID_USART3 = ID_USART3,
  APV_PERIPHERAL_ID_HSMCI  = ID_HSMCI,
  APV_PERIPHERAL_ID_TWI0   = ID_TWI0,
  APV_PERIPHERAL_ID_TWI1   = ID_TWI1,
  APV_PERIPHERAL_ID_SPI0   = ID_SPI0,
  APV_PERIPHERAL_ID_SSC    = ID_SSC,
  APV_PERIPHERAL_ID_TC0    = ID_TC0,
  APV_PERIPHERAL_ID_TC1    = ID_TC1,
  APV_PERIPHERAL_ID_TC2    = ID_TC2,
  APV_PERIPHERAL_ID_TC3    = ID_TC3,
  APV_PERIPHERAL_ID_TC4    = ID_TC4,
  APV_PERIPHERAL_ID_TC5    = ID_TC5,
  APV_PERIPHERAL_ID_TC6    = ID_TC6,
  APV_PERIPHERAL_ID_TC7    = ID_TC7,
  APV_PERIPHERAL_ID_TC8    = ID_TC8,
  APV_PERIPHERAL_ID_PWM    = ID_PWM,
  APV_PERIPHERAL_ID_ADC    = ID_ADC,
  APV_PERIPHERAL_ID_DACC   = ID_DACC,
  APV_PERIPHERAL_ID_DMAC   = ID_DMAC,
  APV_PERIPHERAL_ID_UOTGHS = ID_UOTGHS,
  APV_PERIPHERAL_ID_TRNG   = ID_TRNG,
  APV_PERIPHERAL_ID_EMAC   = ID_EMAC,
  APV_PERIPHERAL_ID_CAN0   = ID_CAN0,
  APV_PERIPHERAL_ID_CAN1   = ID_CAN1,
  APV_PERIPHERAL_IDS
  } apvPeripheralId_t;

/******************************************************************************/
/* Global Variable Definitions :                                              */
/******************************************************************************/

extern Pmc  ApvPeripheralControlBlock;   // shadow peripheral control block
extern Pmc *ApvPeripheralControlBlock_p; // physical block address
extern Pio  ApvPeripheralLineControlBlock[APV_PERIPHERAL_LINE_GROUPS];   // shadow PIO control blocks
extern Pio *ApvPeripheralLineControlBlock_p[APV_PERIPHERAL_LINE_GROUPS]; // physical block addresses

/******************************************************************************/
/* Function Declarations :                                                    */
/******************************************************************************/

extern APV_ERROR_CODE apvSwitchPeripheralClock(apvPeripheralId_t peripheralId,
                                               bool              peripheralSwitch);
extern APV_ERROR_CODE apvSwitchPeripheralLines(apvPeripheralId_t peripheralLineId,
                                               bool              peripheralLineSwitch);
extern APV_ERROR_CODE apvSwitchNvicDeviceIrq(apvPeripheralId_t peripheralIrqId,
                                             bool              peripheralIrqSwitch);

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

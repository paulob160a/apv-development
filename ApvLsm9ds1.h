/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/
/*                                                                            */
/*  Lsm9ds1.h                                                                 */
/*  09.07.18                                                                  */
/*  Paul O'Brien                                                              */
/*                                                                            */
/*  - definition of communications with the STM LSM9DS1 combined 3-axis       */
/*    gyroscope, accelerometer and magnetometer SPI SLAVE chip                */
/*    Reference : STMicroelectronics NV, "iNEMO inertial module", DocId025715 */
/*    Rev 3, 2015                                                             */
/*                                                                            */
/******************************************************************************/

#ifndef _LSM9DS1_H_
#define _LSM9DS1_H_

/******************************************************************************/
/* Includes :                                                                 */
/******************************************************************************/

#include "stdint.h"

/******************************************************************************/
/* Constants :                                                                */
/******************************************************************************/

// Section 5.2, p31
#define APV_LSM9DS1_TRANSACTION_READ                     ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_TRANSACTION_WRITE                    ((APV_LSM9DS1_FIELD_SIZE)0x00)

// Section 6, p38
#define APV_LSM9DS1_ACCELEROMETER_GYRO_REGISTER_SET_SIZE (0x38)

// Section 6, p40
#define APV_LSM9DS1_MAGNETOMETER_REGISTER_SET_SIZE       (0x34)

// Accelerometer and gyro defaults
#define APV_LSM9DS1_REGISTER_COMMON_DEFAULT              ((APV_LSM9DS1_FIELD_SIZE)0x00) // "common" because it occurs mostly (but not always)!
#define APV_LSM9DS1_REGISTER_WHO_AM_I_DEFAULT            ((APV_LSM9DS1_FIELD_SIZE)0x68)
#define APV_LSM9DS1_REGISTER_CTRL_REG4_DEFAULT           ((APV_LSM9DS1_FIELD_SIZE)0x38)
#define APV_LSM9DS1_REGISTER_CTRL_REG5_XL_DEFAULT        ((APV_LSM9DS1_FIELD_SIZE)0x38)
#define APV_LSM9DS1_REGISTER_CTRL_REG8_DEFAULT           ((APV_LSM9DS1_FIELD_SIZE)0x04)

// Magnetometer defaults
#define APV_LSM9DS1_REGISTER_M_WHO_AM_I_DEFAULT          ((APV_LSM9DS1_FIELD_SIZE)0x3D)
#define APV_LSM9DS1_REGISTER_M_CTRL_REG1_M_DEFAULT       ((APV_LSM9DS1_FIELD_SIZE)0x10)
#define APV_LSM9DS1_REGISTER_M_CTRL_REG3_M_DEFAULT       ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_REGISTER_M_INT_CFG_M_DEFAULT         ((APV_LSM9DS1_FIELD_SIZE)0x08)

/******************************************************************************/
// Register bit descriptions and masks :                                      */
/******************************************************************************/
// ACT_THS : activity threshold/gyro inactivity operating mode
#define APV_LSM9DS1_ACT_THS_GYRO_OPERATING_MODE_MASK     ((APV_LSM9DS1_FIELD_SIZE)0x80)
#define APV_LSM9DS1_ACT_THS_GYRO_OPERATING_MODE_OFF      ((APV_LSM9DS1_FIELD_SIZE)0x00)
#define APV_LSM9DS1_ACT_THS_GYRO_OPERATING_MODE_SLEEP    APV_LSM9DS1_ACT_THS_GYRO_OPERATING_MODE_MASK
#define APV_LSM9DS1_ACT_THS_INACTIVITY_THRESHOLD_MASK    ((APV_LSM9DS1_FIELD_SIZE)(~APV_LSM9DS1_ACT_THS_GYRO_OPERATING_MODE_MASK))
// ACT_DUR : inactivity duration
#define APV_LSM9DS1_ACT_DUR_INACTIVITY_DURATION_MASK     ((APV_LSM9DS1_FIELD_SIZE)0xFF) 
// INT_GEN_CFG_XL : linear acceleration sensor
#define APV_LSM9DS1_INT_GEN_CFG_XL_XLIE_MASK             ((APV_LSM9DS1_FIELD_SIZE)0x01)              // enable X-LOW  interrupt
#define APV_LSM9DS1_INT_GEN_CFG_XL_XHIE_MASK             (APV_LSM9DS1_INT_GEN_CFG_XL_XLIE_MASK << 1) // enable X-HIGH interrupt
#define APV_LSM9DS1_INT_GEN_CFG_XL_YLIE_MASK             (APV_LSM9DS1_INT_GEN_CFG_XL_XHIE_MASK << 1) // enable Y-LOW  interrupt
#define APV_LSM9DS1_INT_GEN_CFG_XL_YHIE_MASK             (APV_LSM9DS1_INT_GEN_CFG_XL_YLIE_MASK << 1) // enable Y-HIGH interrupt
#define APV_LSM9DS1_INT_GEN_CFG_XL_ZLIE_MASK             (APV_LSM9DS1_INT_GEN_CFG_XL_YHIE_MASK << 1) // enable Z-LOW  interrupt
#define APV_LSM9DS1_INT_GEN_CFG_XL_ZHIE_MASK             (APV_LSM9DS1_INT_GEN_CFG_XL_ZLIE_MASK << 1) // enable Z-HIGH interrupt
#define APV_LSM9DS1_INT_GEN_CFG_6D_MASK                  (APV_LSM9DS1_INT_GEN_CFG_XL_ZHIE_MASK << 1) // enable "6-direction" interrupt
#define APV_LSM9DS1_INT_GEN_CFG_AOI_MASK                 (APV_LSM9DS1_INT_GEN_CFG_6D_MASK      << 1) // [ 0 == OR | 1 == AND ] inerrupt events
// INT_GEN_THS_X_XL :  linear acceleration sensor X-axis interrupt threshold register
#define APV_LSM9DS1_INT_GEN_THS_X_XL_THRESHOLD_MASK      ((APV_LSM9DS1_FIELD_SIZE)0xFF) 
// INT_GEN_THS_Y_XL :  linear acceleration sensor Y-axis interrupt threshold register
#define APV_LSM9DS1_INT_GEN_THS_Y_XL_THRESHOLD_MASK      ((APV_LSM9DS1_FIELD_SIZE)0xFF) 
// INT_GEN_THS_X_XL :  linear acceleration sensor Z-axis interrupt threshold register
#define APV_LSM9DS1_INT_GEN_THS_Z_XL_THRESHOLD_MASK      ((APV_LSM9DS1_FIELD_SIZE)0xFF) 
// INT_GEN_DUR_XL : linear acceleration sensor interrupt duration register
#define APV_LSM9DS1_INT_GEN_DUR_XL_DURATION_MASK         ((APV_LSM9DS1_FIELD_SIZE)0x7F)
#define APV_LSM9DS1_INT_GEN_DUR_XL_WAIT_MASK             ((APV_LSM9DS1_FIELD_SIZE)(~APV_LSM9DS1_INT_GEN_DUR_XL_DURATION_MASK))
// REFERENCE_G : angular rate sensor reference for digital Hp-filter
#define APV_LSM9DS1_REFERENCE_G_MASK                     ((APV_LSM9DS1_FIELD_SIZE)0xFF) 
// INT1_CTRL : interrupt 1 A/G pin control
#define APV_LSM9DS1_INT1_CTRL_IG_G_MASK                  ((APV_LSM9DS1_FIELD_SIZE)0x80)              // enable gyroscope interrupt on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_IG_XL_MASK                 (APV_LSM9DS1_INT1_CTRL_IG_G_MASK      >> 1) // enable accelerometer interrupt on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_FSS5_MASK                  (APV_LSM9DS1_INT1_CTRL_IG_XL_MASK     >> 1) // enable "FSS5" interrupt on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_OVR_MASK                   (APV_LSM9DS1_INT1_CTRL_FSS5_MASK      >> 1) // enable overrun interrupt on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_FTH_MASK                   (APV_LSM9DS1_INT1_CTRL_OVR_MASK       >> 1) // enable FIFO threshold interrupt on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_BOOT_MASK                  (APV_LSM9DS1_INT1_CTRL_FTH_MASK       >> 1) // enable "boot status available" on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_DRDY_G_MASK                (APV_LSM9DS1_INT1_CTRL_BOOT_MASK      >> 1) // enable "gyroscope data ready" on INT1 A/G pin
#define APV_LSM9DS1_INT1_CTRL_DRDY_XL_MASK               (APV_LSM9DS1_INT1_CTRL_DRDY_G_MASK    >> 1) // enable "accelerometer data ready" on INT1 A/G pin
// INT2_CTRL : interrupt 2 A/G pin control
#define APV_LSM9DS1_INT2_INACT_MASK                      ((APV_LSM9DS1_FIELD_SIZE)0x80)              // enable inactivity interrupt on INT2 A/G pin
#define APV_LSM9DS1_INT2_EMPTY_MASK                      (APV_LSM9DS1_INT2_INACT_MASK          >> 1) // no function assigned
#define APV_LSM9DS1_INT2_CTRL_FSS5_MASK                  (APV_LSM9DS1_INT2_EMPTY_MASK          >> 1) // enable "FSS5" interrupt on INT2 A/G pin
#define APV_LSM9DS1_INT2_CTRL_OVR_MASK                   (APV_LSM9DS1_INT2_CTRL_FSS5_MASK      >> 1) // enable overrun interrupt on INT2 A/G pin
#define APV_LSM9DS1_INT2_CTRL_FTH_MASK                   (APV_LSM9DS1_INT2_CTRL_OVR_MASK       >> 1) // enable FIFO threshold interrupt on INT2 A/G pin
#define APV_LSM9DS1_INT2_CTRL_DRDY_TEMP_MASK             (APV_LSM9DS1_INT2_CTRL_FTH_MASK       >> 1) // enable "temperature data ready" on INT2 A/G pin
#define APV_LSM9DS1_INT2_CTRL_DRDY_G_MASK                (APV_LSM9DS1_INT2_CTRL_DRDY_TEMP_MASK >> 1) // enable "gyroscope data ready" on INT1 A/G pin
#define APV_LSM9DS1_INT2_CTRL_DRDY_XL_MASK               (APV_LSM9DS1_INT1_CTRL_DRDY_G_MASK    >> 1) // enable "accelerometer data ready" on INT1 A/G pin
// WHO_AM_I : "who am I ?"
#define APV_LSM9DS1_WHO_AM_I_MASK                        ((APV_LSM9DS1_FIELD_SIZE)0xFF)

/******************************************************************************/
/* Type Definitions :                                                         */
/******************************************************************************/

typedef uint8_t APV_LSM9DS1_FIELD_SIZE; // data fields and address fields are
                                        // eight bits wide

typedef enum apvLsm9ds1AccelGyroRegisterAddress_tTag
  {
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_00 = 0,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_01,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_02,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_03,
  APV_LSM9DS1_REGISTER_ADDRESS_ACT_THS,
  APV_LSM9DS1_REGISTER_ADDRESS_ACT_DUR,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_CFG_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_X_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_Y_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_Z_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_DUR_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_REFERENCE_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT1_CTRL,
  APV_LSM9DS1_REGISTER_ADDRESS_INT2_CTRL,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_0D,
  APV_LSM9DS1_REGISTER_ADDRESS_WHO_AM_I,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG1_G,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG2_G,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG3_G,
  APV_LSM9DS1_REGISTER_ADDRESS_ORIENT_CFG_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_SRC_G,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_TEMP_L,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_TEMP_H,
  APV_LSM9DS1_REGISTER_ADDRESS_STATUS_17,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_G,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_G,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_G,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_G,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_G,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_G,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG4,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG5_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG6_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG7_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG8,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG9,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG10,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_25,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_SRC_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_STATUS_27,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_XL,
  APV_LSM9DS1_REGISTER_ADDRESS_FIFO_CTRL,
  APV_LSM9DS1_REGISTER_ADDRESS_FIFO_SRC,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_CFG_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_XH_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_XL_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_YH_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_YL_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_ZH_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_ZL_G,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_DUR_G
  } apvLsm9ds1AccelGyroRegisterAddress_t;

typedef enum apvLsm9ds1MagnetometerRegisterAddress_tTag
  {
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_00,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_01,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_02,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_03,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_04,
  APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_X_REG_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_X_REG_H_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Y_REG_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Y_REG_H_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Z_REG_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Z_REG_H_M,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0B,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0C,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0D,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0E,
  APV_LSM9DS1_REGISTER_ADDRESS_WHO_AM_I_M,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_10,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_11,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_12,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_13,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_14,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_15,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_16,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_17,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_18,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_19,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1A,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1B,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1C,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1D,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1E,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1F,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG1_M,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG2_M,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG3_M,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG4_M,
  APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG5_M,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_25,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_26,
  APV_LSM9DS1_REGISTER_ADDRESS_STATUS_REG_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_M,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_2E,
  APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_2F,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_CFG_M,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_SRC_M,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_THS_L_M,
  APV_LSM9DS1_REGISTER_ADDRESS_INT_THS_H_M
  } apvLsm9ds1MagnetometerRegisterAddress_t;

typedef enum apvLsm9ds1RegisterAccess_tTag
  {
  APV_LSM9DS1_REGISTER_READ_ONLY  = 0,
  APV_LSM9DS1_REGISTER_WRITE_ONLY,
  APV_LSM9DS1_REGISTER_READ_WRITE,
  APV_LSM9DS1_REGISTER_RESERVED
  } apvLsm9ds1RegisterAccess_t;

typedef struct apvLsm9ds1RegisterDescriptor_tTag
  {
  apvLsm9ds1AccelGyroRegisterAddress_t lsm9ds1RegisterAddress;
  apvLsm9ds1RegisterAccess_t           lsm9ds1RegisterAccess;
  APV_LSM9DS1_FIELD_SIZE               lsm9ds1RegisterDefault;
  } apvLsm9ds1RegisterDescriptor_t;

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern apvLsm9ds1RegisterDescriptor_t apvLsm9ds1AccelerometerAndGyroRegisters[APV_LSM9DS1_ACCELEROMETER_GYRO_REGISTER_SET_SIZE];
extern apvLsm9ds1RegisterDescriptor_t apvLsm9ds1MagnetometerRegisters[APV_LSM9DS1_MAGNETOMETER_REGISTER_SET_SIZE];

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

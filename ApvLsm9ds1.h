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

// CTRL_REG1_G : gyro control #1
#define APV_LSM9DS1_CTRL_REG1_G_BANDWIDTH_SELECT_MASK    ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT        ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_SHIFT  (3) // bit 2 is empty
#define APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_MASK   (APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT << APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT         ((APV_LSM9DS1_FIELD_SIZE)0x07)
#define APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT   (2 + APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_MASK    (APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)
// bandwidth select :  these are dependent on the data rates see Table 47
#define APV_LSM9DS1_CTRL_REG1_G_BANDWIDTH_LOW            (0x00)
#define APV_LSM9DS1_CTRL_REG1_G_BANDWIDTH_LMID1          (0x01)
#define APV_LSM9DS1_CTRL_REG1_G_BANDWIDTH_LMID2          (0x02)
#define APV_LSM9DS1_CTRL_REG1_G_BANDWIDTH_LHIGH          (0x03)
// gyro full-scale selection (degrees per second) [ 245 | 500 | n/a | 2000 ]
#define APV_LSM9DS1_CTRL_REG1_G_245_dps                  (0x00 << APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_500_dps                  (0x01 << APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_2000_dps                 (0x03 << APV_LSM9DS1_CTRL_REG1_G_FULL_SCALE_SELECT_SHIFT)
// data rates
#define APV_LSM9DS1_CTRL_REG1_G_14p9_Hz                  (0x01 << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_59p5_Hz                  (0x02 << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_119_Hz                   (0x03 << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_238_Hz                   (0x04 << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_476_Hz                   (0x05 << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_952_Hz                   (0x06 << APV_LSM9DS1_CTRL_REG1_G_DATA_RATE_SELECT_SHIFT)

// CTRL_REG2_G : ... control #2
#define APV_LSM9DS1_CTRL_REG2_G_OUTPUT_SELECT_MASK       ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG2_G_INTERRUPT_SELECT         ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG2_G_INTERRUPT_SELECT_SHIFT   (2)
#define APV_LSM9DS1_CTRL_REG2_G_INTERRUPT_SELECT_MASK    (APV_LSM9DS1_CTRL_REG2_G_INTERRUPT_SELECT << APV_LSM9DS1_CTRL_REG2_G_INTERRUPT_SELECT_SHIFT)

// CTRL_REG3_G : ... control #3
#define APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_CUTOFF_MASK   ((APV_LSM9DS1_FIELD_SIZE)0x0F) // see Table 52
#define APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_ENABLE        ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_ENABLE_SHIFT  (6) // bits 4,5 are empty
#define APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_ENABLE_MASK   (APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_ENABLE << APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_ENABLE_SHIFT)
#define APV_LSM9DS1_CTRL_REG3_G_LOW_POWER_ENABLE        ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG3_G_LOW_POWER_ENABLE_SHIFT  (1 + APV_LSM9DS1_CTRL_REG3_G_HIGH_PASS_ENABLE_SHIFT)
#define APV_LSM9DS1_CTRL_REG3_G_LOW_POWER_ENABLE_MASK   (APV_LSM9DS1_CTRL_REG3_G_LOW_POWER_ENABLE << APV_LSM9DS1_CTRL_REG3_G_LOW_POWER_ENABLE_SHIFT)

// ORIENT_CFG_G : ... sign and orientation register
#define APV_LSM9DS1_ORIENT_CFG_G_ORIENTATION_MASK       ((APV_LSM9DS1_FIELD_SIZE)0x07)
#define APV_LSM9DS1_ORIENT_CFG_G_Z_AXIS_YAW_SIGN        ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_ORIENT_CFG_G_Z_AXIS_YAW_SHIFT       (3)
#define APV_LSM9DS1_ORIENT_CFG_G_Z_AXIS_YAW             (APV_LSM9DS1_ORIENT_CFG_G_Z_AXIS_YAW_SIGN << APV_LSM9DS1_ORIENT_CFG_G_Z_AXIS_YAW_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_Y_AXIS_ROLL_SIGN       ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_ORIENT_CFG_G_Y_AXIS_ROLL_SHIFT      (1 + APV_LSM9DS1_ORIENT_CFG_G_Z_AXIS_YAW_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_Y_AXIS_ROLL_MASK       (APV_LSM9DS1_ORIENT_CFG_G_Y_AXIS_ROLL_SIGN << APV_LSM9DS1_ORIENT_CFG_G_Y_AXIS_ROLL_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_X_AXIS_PITCH_SIGN      ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_ORIENT_CFG_G_X_AXIS_PITCH_SHIFT     (1 + APV_LSM9DS1_ORIENT_CFG_G_Y_AXIS_ROLL_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_X_AXIS_PITCH_MASK      (APV_LSM9DS1_ORIENT_CFG_G_X_AXIS_PITCH_SIGN << APV_LSM9DS1_ORIENT_CFG_G_X_AXIS_PITCH_SHIFT)

// INT_GEN_SRC_G : ... interrupt source register see Table 56
#define APV_LSM9DS1_INT_GEN_SRC_G_ACTIVE_MASK           (APV_LSM9DS1_FIELD_SIZE)0x40)
#define APV_LSM9DS1_INT_GEN_SRC_G_Z_AXIS_HIGH_MASK      (APV_LSM9DS1_INT_GEN_SRC_G_ACTIVE_MASK      >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_Z_AXIS_LOW_MASK       (APV_LSM9DS1_INT_GEN_SRC_G_Z_AXIS_HIGH_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_Y_AXIS_HIGH_MASK      (APV_LSM9DS1_INT_GEN_SRC_G_Z_AXIS_LOW_MASK  >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_Y_AXIS_LOW_MASK       (APV_LSM9DS1_INT_GEN_SRC_G_Y_AXIS_HIGH_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_X_AXIS_HIGH_MASK      (APV_LSM9DS1_INT_GEN_SRC_G_Y_AXIS_LOW_MASK  >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_X_AXIS_LOW_MASK       (APV_LSM9DS1_INT_GEN_SRC_G_X_AXIS_HIGH_MASK >> 1)

// OUT_TEMP_L, OUT_TEMP_H : temperature registers
#define APV_LSM9DS1_OUT_TEMP_L_BITS_0_7_MASK            ((APV_LSM9DS1_FIELD_SIZE)0xFF)
#define APV_LSM9DS1_OUT_TEMP_H_BITS_8_11_MASK           ((APV_LSM9DS1_FIELD_SIZE)0x0F)
#define APV_LSM9DS1_OUT_TEMP_H_SIGN_EXTEND_MASK         ((APV_LSM9DS1_FIELD_SIZE)(~APV_LSM9DS1_OUT_TEMP_H_BITS_8_11_MASK))

// Get the temperature : add the two 8-bit registers and sign for a signed 16-bit result :
//  - temperature  : int16_t
//  - registerLow  : OUT_TEMP_L
//  - registerHigh : OUT_TEMP_H
#define APV_LSM9DS1_OUT_TEMP(temperature,registerLow,registerHigh) \\
          { \\
          (temperature) = (int16_t)((uint8_t)(registerLow)); \\
          (temperature) = (int16_t)(((uint16_t)(temperature)) + ((uint16_t)((uint8_t)(registerHigh)))) \\
          }

// STATUS_REG_17 :  see Table 61
#define APV_LSM9DS1_STATUS_REG_17_IG_XL_MASK            (APV_LSM9DS1_FIELD_SIZE)0x40)               // accelerometer interrupt
#define APV_LSM9DS1_STATUS_REG_17_IG_G_MASK             (APV_LSM9DS1_STATUS_REG_17_IG_XL_MASK >> 1) // gyro interrupt
#define APV_LSM9DS1_STATUS_REG_17_INACT_MASK            (APV_LSM9DS1_STATUS_REG_17_IG_G_MASK  >> 1) // inactivity
#define APV_LSM9DS1_STATUS_REG_17_BOOT_MASK             (APV_LSM9DS1_STATUS_REG_17_INACT_MASK >> 1) // boot running
#define APV_LSM9DS1_STATUS_REG_17_TDA_MASK              (APV_LSM9DS1_STATUS_REG_17_BOOT_MASK  >> 1) // new temperature data
#define APV_LSM9DS1_STATUS_REG_17_GDA_MASK              (APV_LSM9DS1_STATUS_REG_17_TDA_MASK   >> 1) // new gyro data
#define APV_LSM9DS1_STATUS_REG_17_XLDA_MASK             (APV_LSM9DS1_STATUS_REG_17_GDA_MASK   >> 1) // new accelerometer data

// OUT_X_G (OUT_X_L_G, OUT_X_H_G) : pitch axis angular rate
#define APV_LSM9DS1_OUT_L_G_BITS_0_7_MASK               ((APV_LSM9DS1_FIELD_SIZE)0xFF)
#define APV_LSM9DS1_OUT_L_G_BITS_8_15_MASK              ((APV_LSM9DS1_FIELD_SIZE)0x7F)
#define APV_LSM9DS1_OUT_L_G_SIGN_EXTEND_MASK            ((APV_LSM9DS1_FIELD_SIZE)(~APV_LSM9DS1_OUT_L_G_BITS_8_15_MASK))

// Get the rate sensor axes' angular rate
#define APV_LSM9DS1_OUT_G(angularRate,registerLow,registerHigh) \\
  { \\
  (angularRate) = (int16_t)((uint8_t)(registerLow)); \\
  (angularRate) = (int16_t)(((uint16_t)(angularRate)) + ((uint16_t)((uint8_t)(registerHigh)))) \\
  }

#define APV_LSM9DS1_OUT_X_G APV_LSM9DS1_OUT_G
#define APV_LSM9DS1_OUT_Y_G APV_LSM9DS1_OUT_G
#define APV_LSM9DS1_OUT_Z_G APV_LSM9DS1_OUT_G

// CTRL_REG4 :  gyroscope output enable see Table 63
#define APV_LSM9DS1_CTRL_REG4_ZEN_G_MASK                ((APV_LSM9DS1_FIELD_SIZE)0x20)           // enable gyro Z-axis
#define APV_LSM9DS1_CTRL_REG4_YEN_G_MASK                (APV_LSM9DS1_CTRL_REG4_ZEN_G_MASK  >> 1) // enable gyro Y-axis
#define APV_LSM9DS1_CTRL_REG4_XEN_G_MASK                (APV_LSM9DS1_CTRL_REG4_YEN_G_MASK  >> 1) // enable gyro X-axis
#define APV_LSM9DS1_CTRL_REG4_LIR_XL_MASK               (APV_LSM9DS1_CTRL_REG4_XEN_G_MASK  >> 2) // latch interrupts (bit 2 is empty)
#define APV_LSM9DS1_CTRL_REG4_4D_XL_MASK                (APV_LSM9DS1_CTRL_REG4_LIR_XL_MASK >> 1) // [ 0 == 6D | 1 == 4D ] position recognition for interrupt

// CTRL_REG5_XL : accelerometer output enable see Table 65
#define APV_LSM9DS1_CTRL_REG5_XL_DEC_MASK               ((APV_LSM9DS1_FIELD_SIZE)0xC0)       // update decimation
#define APV_LSM9DS1_CTRL_REG5_XL_ZEN_XL_MASK            ((APV_LSM9DS1_FIELD_SIZE)0x20)       // enable accelerometer Z-axis
#define APV_LSM9DS1_CTRL_REG5_XL_YEN_XL_MASK            (APV_LSM9DS1_CTRL_REG_4_ZEN_XL >> 1) // enable accelerometer Y-axis
#define APV_LSM9DS1_CTRL_REG5_XL_XEN_XL_MASK            (APV_LSM9DS1_CTRL_REG_4_YEN_XL >> 1) // enable accelerometer X-axis

// CTRL_REG6_XL : accelerometer data output control see Table 67
#define APV_LSM9DS1_CTRL_REG6_XL_BW_XL_MASK             ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR            ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_SHIFT      (2)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_MASK       (APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR << APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL                  ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT            (1 + APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL_MASK             (APV_LSM9DS1_CTRL_REG6_XL_FS_XL << APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL                 ((APV_LSM9DS1_FIELD_SIZE)0x07)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT           (2 + APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_MASK            (APV_LSM9DS1_CTRL_REG6_XL_ODR_XL << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
// bandwidth :
#define APV_LSM9DS1_CTRL_REG6_XL_BW_XL_408_Hz           (0x00)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_XL_211_Hz           (0x01)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_XL_105_Hz           (0x02)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_XL_50_Hz            (0x03)
// bandwidth selection :
#define APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_ODR_XL     (0x00 << APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_BW_XL      (0x01 << APV_LSM9DS1_CTRL_REG6_XL_BW_SCAL_ODR_SHIFT)
// accelerometer full-scale :
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL_2g               (0x00 << APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL_6g               (0x01 << APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL_4g               (0x02 << APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_FS_XL_8g               (0x03 << APV_LSM9DS1_CTRL_REG6_XL_FS_XL_SHIFT)
// output data-rate : see Table 68
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_none            (0x00 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_10_Hz           (0x01 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_50_Hz           (0x02 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_119_Hz          (0x03 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_238_Hz          (0x04 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_476_Hz          (0x05 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)
#define APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_952_Hz          (0x06 << APV_LSM9DS1_CTRL_REG6_XL_ODR_XL_SHIFT)

// CTRL_REG7_XL : 
#define APV_LSM9DS1_CTRL_REG7_XL_HPIS1_MASK             ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG7_XL_FDS                    ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG7_XL_FDS_SHIFT              (2) // bit 1 is empty
#define APV_LSM9DS1_CTRL_REG7_XL_FDS_MASK               (APV_LSM9DS1_CTRL_REG7_XL_FDS << APV_LSM9DS1_CTRL_REG7_XL_FDS_SHIFT)
#define APV_LSM9DS1_CTRL_REG7_XL_DCF                    ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT              (3 + APV_LSM9DS1_CTRL_REG7_XL_FDS_SHIFT) // bits 4,5 empty
#define APV_LSM9DS1_CTRL_REG7_XL_DCF_MASK               (APV_LSM9DS1_CTRL_REG7_XL_DCF << APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT)
#define APV_LSM9DS1_CTRL_REG7_XL_HR                     ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG7_XL_HR_SHIFT               (1 + APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT)
#define APV_LSM9DS1_CTRL_REG7_XL_HR_MASK                (APV_LSM9DS1_CTRL_REG7_XL_HR << APV_LSM9DS1_CTRL_REG7_XL_HR_SHIFT)
// high-pass filter enable
#define APV_LSM9DS1_CTRL_REG7_XL_HPIS1_ENABLE           APV_LSM9DS1_CTRL_REG7_XL_HPIS1_MASK
// filtered data selection
#define APV_LSM9DS1_CTRL_REG7_XL_FDS_ENABLE             APV_LSM9DS1_CTRL_REG7_XL_FDS_MASK
// accelerometer filter cut-offs (on HR == 1 (high-resolution mode)) see Table 71
#define APV_LSM9DS1_CTRL_REG7_XL_DCF_ODR_BY_50          (0x00 << APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT)
#define APV_LSM9DS1_CTRL_REG7_XL_DCF_ODR_BY_100         (0x01 << APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT)
#define APV_LSM9DS1_CTRL_REG7_XL_DCF_ODR_BY_9           (0x02 << APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT)
#define APV_LSM9DS1_CTRL_REG7_XL_DCF_ODR_BY_400         (0x03 << APV_LSM9DS1_CTRL_REG7_XL_DCF_SHIFT)
// high-resolution mode enable
#define APV_LSM9DS1_CTRL_REG7_XL_HR_ENABLE              APV_LSM9DS1_CTRL_REG7_XL_HR_MASK

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

extern const apvLsm9ds1RegisterDescriptor_t apvLsm9ds1AccelerometerAndGyroRegisters[APV_LSM9DS1_ACCELEROMETER_GYRO_REGISTER_SET_SIZE];
extern const apvLsm9ds1RegisterDescriptor_t apvLsm9ds1MagnetometerRegisters[APV_LSM9DS1_MAGNETOMETER_REGISTER_SET_SIZE];

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

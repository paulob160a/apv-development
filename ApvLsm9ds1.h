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
/* Accelerometers (linear displacement/rate) and Gyroscopes (angular          */
/* displacement/rate) :                                                       */
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
#define APV_LSM9DS1_INT_GEN_CFG_AOI_MASK                 (APV_LSM9DS1_INT_GEN_CFG_6D_MASK      << 1) // [ 0 == OR | 1 == AND ] interrupt events

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
#define APV_LSM9DS1_WHO_AM_I                             ((APV_LSM9DS1_FIELD_SIZE)0x68) // section 7.11 p45

// CTRL_REG1_G : gyro control #1
#define APV_LSM9DS1_CTRL_REG1_G_BW_MASK                 ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG1_G_FS_G                    ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG1_G_FS_G_SHIFT              (3) // bit 2 is empty
#define APV_LSM9DS1_CTRL_REG1_G_FS_G_MASK               (APV_LSM9DS1_CTRL_REG1_G_FS_G << APV_LSM9DS1_CTRL_REG1_G_FS_G_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_ODR_SELECT              ((APV_LSM9DS1_FIELD_SIZE)0x07)
#define APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT               (2 + APV_LSM9DS1_CTRL_REG1_G_FS_G_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_ODR_MASK                (APV_LSM9DS1_CTRL_REG1_G_ODR << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)
// bandwidth select :  these are dependent on the data rates see Table 47
#define APV_LSM9DS1_CTRL_REG1_G_BW_LOW                  (0x00)
#define APV_LSM9DS1_CTRL_REG1_G_BW_LMID1                (0x01)
#define APV_LSM9DS1_CTRL_REG1_G_BW                      (0x02)
#define APV_LSM9DS1_CTRL_REG1_G_BW_LHIGH                (0x03)
// gyro full-scale selection (degrees per second) [ 245 | 500 | n/a | 2000 ]
#define APV_LSM9DS1_CTRL_REG1_G_245_dps                 (0x00 << APV_LSM9DS1_CTRL_REG1_G_FS_G_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_500_dps                 (0x01 << APV_LSM9DS1_CTRL_REG1_G_FS_G_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_2000_dps                (0x03 << APV_LSM9DS1_CTRL_REG1_G_FS_G_SHIFT)
// data rates
#define APV_LSM9DS1_CTRL_REG1_G_14p9_Hz                 (0x01 << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_59p5_Hz                 (0x02 << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_119_Hz                  (0x03 << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_238_Hz                  (0x04 << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_476_Hz                  (0x05 << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_G_952_Hz                  (0x06 << APV_LSM9DS1_CTRL_REG1_G_ODR_SHIFT)

// CTRL_REG2_G : ... control #2
#define APV_LSM9DS1_CTRL_REG2_G_OUT_SEL_MASK            ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG2_G_INT_SEL                 ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG2_G_INT_SEL_SHIFT           (2)
#define APV_LSM9DS1_CTRL_REG2_G_INT_SEL_MASK            (APV_LSM9DS1_CTRL_REG2_G_INT_SEL << APV_LSM9DS1_CTRL_REG2_G_INT_SEL_SHIFT)

// CTRL_REG3_G : ... control #3
#define APV_LSM9DS1_CTRL_REG3_G_HPCF_G_MASK             ((APV_LSM9DS1_FIELD_SIZE)0x0F) // see Table 52
#define APV_LSM9DS1_CTRL_REG3_G_HP_EN                   ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG3_G_HP_EN_SHIFT             (6) // bits 4,5 are empty
#define APV_LSM9DS1_CTRL_REG3_G_HP_EN_MASK              (APV_LSM9DS1_CTRL_REG3_G_HP_EN << APV_LSM9DS1_CTRL_REG3_G_HP_EN_SHIFT)
#define APV_LSM9DS1_CTRL_REG3_G_LP_mode                 ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG3_G_LP_mode_SHIFT           (1 + APV_LSM9DS1_CTRL_REG3_G_HP_EN_SHIFT)
#define APV_LSM9DS1_CTRL_REG3_G_LP_mode_MASK            (APV_LSM9DS1_CTRL_REG3_G_LP_mode << APV_LSM9DS1_CTRL_REG3_G_LP_mode_SHIFT)

// ORIENT_CFG_G : ... sign and orientation register
#define APV_LSM9DS1_ORIENT_CFG_G_Orient_MASK            ((APV_LSM9DS1_FIELD_SIZE)0x07)
#define APV_LSM9DS1_ORIENT_CFG_G_Z_SIGN_Z_G             ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_ORIENT_CFG_G_Z_SIGN_Z_G_SHIFT       (3)
#define APV_LSM9DS1_ORIENT_CFG_G_Z_SIGN_Z_G_MASK        (APV_LSM9DS1_ORIENT_CFG_G_Z_SIGN_Z_G << APV_LSM9DS1_ORIENT_CFG_G_Z_SIGN_Z_G_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_Y_SIGN_Y_G             ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_ORIENT_CFG_G_Y_SIGN_Y_G_SHIFT       (1 + APV_LSM9DS1_ORIENT_CFG_G_Z_SIGN_Z_G_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_Y_SIGN_Y_G_MASK        (APV_LSM9DS1_ORIENT_CFG_G_Y_SIGN_Y_G << APV_LSM9DS1_ORIENT_CFG_G_Y_SIGN_Y_G_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_X_SIGN_X_G             ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_ORIENT_CFG_G_X_SIGN_X_G_SHIFT       (1 + APV_LSM9DS1_ORIENT_CFG_G_Y_SIGN_Y_G_SHIFT)
#define APV_LSM9DS1_ORIENT_CFG_G_X_SIGN_X_G_MASK        (APV_LSM9DS1_ORIENT_CFG_G_X_SIGN_X_G << APV_LSM9DS1_ORIENT_CFG_G_X_SIGN_X_G_SHIFT)

// INT_GEN_SRC_G : ... interrupt source register see Table 56
#define APV_LSM9DS1_INT_GEN_SRC_IA_G_MASK               (APV_LSM9DS1_FIELD_SIZE)0x40)
#define APV_LSM9DS1_INT_GEN_SRC_G_ZH_G_MASK             (APV_LSM9DS1_INT_GEN_SRC_IA_G_MASK   >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_ZL_G_MASK             (APV_LSM9DS1_INT_GEN_SRC_G_ZH_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_YH_G_MASK             (APV_LSM9DS1_INT_GEN_SRC_G_ZL_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_YL_G_MASK             (APV_LSM9DS1_INT_GEN_SRC_G_YH_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_XH_G_MASK             (APV_LSM9DS1_INT_GEN_SRC_G_YL_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_SRC_G_XL_G_MASK             (APV_LSM9DS1_INT_GEN_SRC_G_XH_G_MASK >> 1)

// OUT_TEMP_L, OUT_TEMP_H : temperature registers
#define APV_LSM9DS1_OUT_TEMP_L_BITS_0_7_MASK            ((APV_LSM9DS1_FIELD_SIZE)0xFF)
#define APV_LSM9DS1_OUT_TEMP_H_BITS_8_11_MASK           ((APV_LSM9DS1_FIELD_SIZE)0xFF)
#define APV_LSM9DS1_OUT_TEMP_H_SIGN_EXTEND_MASK         ((APV_LSM9DS1_FIELD_SIZE)(~APV_LSM9DS1_OUT_TEMP_H_BITS_8_11_MASK))

// Get the temperature : add the two 8-bit registers and sign for a signed 16-bit result (Q11) :
//  - temperature  : int16_t
//  - registerLow  : OUT_TEMP_L
//  - registerHigh : OUT_TEMP_H
#define APV_LSM9DS1_OUT_TEMP(temperature,registerLow,registerHigh) \\
          { \\
          (temperature) = (int16_t)((uint8_t)(registerLow)); \\
          (temperature) = (int16_t)((temperature) + ((int16_t)((int8_t)(registerHigh)))) \\
          }

// STATUS_REG_17 :  see Table 61
#define APV_LSM9DS1_STATUS_REG_17_IG_XL_MASK            (APV_LSM9DS1_FIELD_SIZE)0x40)               // accelerometer interrupt
#define APV_LSM9DS1_STATUS_REG_17_IG_G_MASK             (APV_LSM9DS1_STATUS_REG_17_IG_XL_MASK       >> 1) // gyro interrupt
#define APV_LSM9DS1_STATUS_REG_17_INACT_MASK            (APV_LSM9DS1_STATUS_REG_17_IG_G_MASK        >> 1) // inactivity
#define APV_LSM9DS1_STATUS_REG_17_BOOT_STATUS_MASK      (APV_LSM9DS1_STATUS_REG_17_INACT_MASK       >> 1) // boot running
#define APV_LSM9DS1_STATUS_REG_17_TDA_MASK              (APV_LSM9DS1_STATUS_REG_17_BOOT_STATUS_MASK >> 1) // new temperature data
#define APV_LSM9DS1_STATUS_REG_17_GDA_MASK              (APV_LSM9DS1_STATUS_REG_17_TDA_MASK         >> 1) // new gyro data
#define APV_LSM9DS1_STATUS_REG_17_XLDA_MASK             (APV_LSM9DS1_STATUS_REG_17_GDA_MASK         >> 1) // new accelerometer data

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
#define APV_LSM9DS1_CTRL_REG5_XL_DEC_MASK               ((APV_LSM9DS1_FIELD_SIZE)0xC0)      // update decimation
#define APV_LSM9DS1_CTRL_REG5_XL_ZEN_XL_MASK            ((APV_LSM9DS1_FIELD_SIZE)0x20)      // enable accelerometer Z-axis
#define APV_LSM9DS1_CTRL_REG5_XL_YEN_XL_MASK            (APV_LSM9DS1_CTRL_REG5_ZEN_XL >> 1) // enable accelerometer Y-axis
#define APV_LSM9DS1_CTRL_REG5_XL_XEN_XL_MASK            (APV_LSM9DS1_CTRL_REG5_YEN_XL >> 1) // enable accelerometer X-axis

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

// CTRL_REG8 : see Table 73
#define APV_LSM9DS1_CTRL_REG8_SW_RESET_MASK             ((APV_LSM9DS1_FIELD_SIZE)0x01)               // [ 0 == normal | 1 ==  reset ]
#define APV_LSM9DS1_CTRL_REG8_BLE_MASK                  (APV_LSM9DS1_CTRL_REG8_SW_RESET_MASK   << 1) // [ 0 == little- | 1 == big- ] endianness
#define APV_LSM9DS1_CTRL_REG8_IF_ADD_INC_MASK           (APV_LSM9DS1_CTRL_REG8_BLE_MASK        << 1) // [ 0 == disabled | 1 == enable ] auto-increment I2C/SPI addresses
#define APV_LSM9DS1_CTRL_REG8_SIM_MASK                  (APV_LSM9DS1_CTRL_REG8_IF_ADD_INC_MASK << 1) // [ 0 == 4-wire | 1 == 3-wire ] SPI mode
#define APV_LSM9DS1_CTRL_REG8_PP_OD_MASK                (APV_LSM9DS1_CTRL_REG8_SIM_MASK        << 1) // [ 0 == push-pull | 1 == open-drain ] on INT1_A/G and INT2_A/G
#define APV_LSM9DS1_CTRL_REG8_H_LACTIVE_MASK            (APV_LSM9DS1_CTRL_REG8_PP_OD_MASK      << 1) // [ 0 == high | 1 == low ] interrupt activation level
#define APV_LSM9DS1_CTRL_REG8_BDU_MASK                  (APV_LSM9DS1_CTRL_REG8_H_LACTIVE_MASK  << 1) // [ 0 == continuous update | 1 == MSB + LSB ] block data update
#define APV_LSM9DS1_CTRL_REG8_BOOT_MASK                 (APV_LSM9DS1_CTRL_REG8_BDU_MASK        << 1) // [ 0 == normal | 1 == reboot ] memory

// CTRL_REG9 : see Table 75
#define APV_LSM9DS1_CTRL_REG9_STOP_ON_FTH_MASK          ((APV_LSM9DS1_FIELD_SIZE)0x01)                  // 1 == limit FIFO depth to threshold level
#define APV_LSM9DS1_CTRL_REG9_FIFO_EN_MASK              (APV_LSM9DS1_CTRL_REG9_STOP_ON_FTH_MASK   << 1) // 1 == enable FIFO
#define APV_LSM9DS1_CTRL_REG9_I2C_DISABLE_MASK          (APV_LSM9DS1_CTRL_REG9_FIFO_EN_MASK       << 1) // [ 0 == I2C + SPI | 1 == SPI only ]
#define APV_LSM9DS1_CTRL_REG9_DRDY_mask_bit_MASK        (APV_LSM9DS1_CTRL_REG9_I2C_DISABLE_MASK   << 1) // 1 == data available enabled
#define APV_LSM9DS1_CTRL_REG9_FIFO_TEMP_EN_MASK         (APV_LSM9DS1_CTRL_REG9_DRDY_mask_bit_MASK << 1) // 1 == temperature data stored in FIFO
#define APV_LSM9DS1_CTRL_REG9_SLEEP_G_MASK              (APV_LSM9DS1_CTRL_REG9_FIFO_TEMP_EN_MASK  << 2) // [ 0 == disabled | 1 == enabled ] sleep - bit 5 is empty

// CTRL_REG10 : see Table 77
#define APV_LSM9DS1_CTRL_REG10_ST_XL_MASK               ((APV_LSM9DS1_FIELD_SIZE)0x01)           // 1 == accelerometer-sensor self-test
#define APV_LSM9DS1_CTRL_REG10_ST_G_MASK                (APV_LSM9DS1_CTRL_REG10_ST_XL_MASK << 2) // 1 == gyro self-test, bit 1 is empty

// INT_GEN_SRC_XL : accelerometer interrupt source register - see Table 79
#define APV_LSM9DS1_INT_GEN_SRC_XL_IA_XL_MASK           ((APV_LSM9DS1_FIELD_SIZE)0x40)
#define APV_LSM9DS1_NT_GEN_SRC_XL_ZH_XL_MASK            (APV_LSM9DS1_INT_GEN_SRC_XL_IA_XL_MASK >> 1)
#define APV_LSM9DS1_NT_GEN_SRC_XL_ZL_XL_MASK            (APV_LSM9DS1_NT_GEN_SRC_XL_ZH_XL_MASK  >> 1)
#define APV_LSM9DS1_NT_GEN_SRC_XL_YH_XL_MASK            (APV_LSM9DS1_NT_GEN_SRC_XL_ZL_XL_MASK  >> 1)
#define APV_LSM9DS1_NT_GEN_SRC_XL_YL_XL_MASK            (APV_LSM9DS1_NT_GEN_SRC_XL_YH_XL_MASK  >> 1)
#define APV_LSM9DS1_NT_GEN_SRC_XL_XH_XL_MASK            (APV_LSM9DS1_NT_GEN_SRC_XL_YL_XL_MASK  >> 1)
#define APV_LSM9DS1_NT_GEN_SRC_XL_XL_XL_MASK            (APV_LSM9DS1_NT_GEN_SRC_XL_XH_XL_MASK  >> 1)

// STATUS_REG_27 : miscellaneous status register - see Table 81
#define APV_LSM9DS1_STATUS_REG_27_IG_XL_MASK            ((APV_LSM9DS1_FIELD_SIZE)0x40)                    // [ 0 == none | 1 == event ] accelerometer interrupts
#define APV_LSM9DS1_STATUS_REG_27_IG_G_MASK             (APV_LSM9DS1_STATUS_REG_27_IG_XL_MASK       >> 1) // [ 0 == none | 1 == event ] gyro interrupts
#define APV_LSM9DS1_STATUS_REG_27_INACT_MASK            (APV_LSM9DS1_STATUS_REG_27_IG_G_MASK        >> 1) // [ 0 == none | 1 == event ] interrupt events
#define APV_LSM9DS1_STATUS_REG_27_BOOT_STATUS_MASK      (APV_LSM9DS1_STATUS_REG_27_INACT_MASK       >> 1) // [ 0 == not | 1 == running ] boot
#define APV_LSM9DS1_STATUS_REG_27_TDA_MASK              (APV_LSM9DS1_STATUS_REG_27_BOOT_STATUS_MASK >> 1) // [ 0 == none | 1 == available ] temperature data
#define APV_LSM9DS1_STATUS_REG_27_GDA_MASK              (APV_LSM9DS1_STATUS_REG_27_TDA_MASK         >> 1) // [ 0 == none | 1 == available ] gyro data
#define APV_LSM9DS1_STATUS_REG_27_XLDA_MASK             (APV_LSM9DS1_STATUS_REG_27_GDA_MASK         >> 1) // [ 0 == none | 1 == available ] accelerometer data

// OUT_XL (OUT_X_XL, OUT_Y_XL, OUT_Z_XL) : accelerometer axes output
#define APV_LSM9DS1_OUT_X_XL APV_LSM9DS1_OUT_G
#define APV_LSM9DS1_OUT_Y_XL APV_LSM9DS1_OUT_G
#define APV_LSM9DS1_OUT_Z_XL APV_LSM9DS1_OUT_G

// FIFO_CTRL : FIFO control - see Tables 83 and 84
#define APV_LSM9DS1_FIFO_CTRL_FTH_MASK                  ((APV_LSM9DS1_FIELD_SIZE)0x1F)
#define APV_LSM9DS1_FIFO_CTRL_FMODE_SHIFT               (5)
#define APV_LSM9DS1_FIFO_CTRL_FMODE_MASK                ((APV_LSM9DS1_FIELD_SIZE)~APV_LSM9DS1_FIFO_CTRL_FTH_MASK)
// FMODE2 : FIFO mode selection
#define APV_LSM9DS1_FIFO_CTRL_FMODE_OFF_MASK            (0x00 << APV_LSM9DS1_FIFO_CTRL_FMODE_SHIFT) // bypass, FIFO off
#define APV_LSM9DS1_FIFO_CTRL_FMODE_STOP_ON_FULL_MASK   (0x01 << APV_LSM9DS1_FIFO_CTRL_FMODE_SHIFT) // stop on FIFO full
#define APV_LSM9DS1_FIFO_CTRL_FMODE_TRIGGER_N_FIFO_MASK (0x03 << APV_LSM9DS1_FIFO_CTRL_FMODE_SHIFT) // [ trigger == continuous | trigger_n == FIFO ]
#define APV_LSM9DS1_FIFO_CTRL_FMODE_TRIGGER_BYPASS_MASK (0x04 << APV_LSM9DS1_FIFO_CTRL_FMODE_SHIFT) // [ trigger == bypass     | trigger_n == continuous ]
#define APV_LSM9DS1_FIFO_CTRL_FMODE_CONTINUOUS_MASK     (0x06 << APV_LSM9DS1_FIFO_CTRL_FMODE_SHIFT) // continuous == FIFO in ring-buffer mode

// FIFO_SRC : the FIFO is 32 (8-bit ?) sanples deep - see Table 87
#define APV_LSM9DS1_FIFO_SRC_FSS_MASK                   ((APV_LSM9DS1_FIELD_SIZE)0x3F)                                 // number of unread samples in the FIFO
#define APV_LSM9DS1_FIFO_SRC_OVRN                       ((APV_LSM9DS1_FIELD_SIZE)0x01)                                 // [ 0 == FIFO not full | 1 == FIFO overran ]
#define APV_LSM9DS1_FIFO_SRC_OVRN_SHIFT                 (6)
#define APV_LSM9DS1_FIFO_SRC_OVRN_MASK                  (APV_LSM9DS1_FIFO_SRC_OVRN << APV_LSM9DS1_FIFO_SRC_OVRN_SHIFT)
#define APV_LSM9DS1_FIFO_SRC_FTH                        ((APV_LSM9DS1_FIELD_SIZE)0x01)                                 // [ 0 == FIFO < threshold | 1 == FIFO >= threshold ]
#define APV_LSM9DS1_FIFO_SRC_FTH_SHIFT                  (1 + APV_LSM9DS1_FIFO_SRC_OVRN_SHIFT)
#define APV_LSM9DS1_FIFO_SRC_FTH_MASK                   (APV_LSM9DS1_FIFO_SRC_FTH << APV_LSM9DS1_FIFO_SRC_FTH_SHIFT)

// INT_GEN_CFG_G :  gyro interrupt configuration - see Table 89
#define APV_LSM9DS1_INT_GEN_CFG_G_AOI_G_MASK            ((APV_LSM9DS1_FIELD_SIZE)0x80)               // [ 0 == OR | 1 == AND ] gyro interrupt sources
#define APV_LSM9DS1_INT_GEN_CFG_G_LIR_G_MASK            (APV_LSM9DS1_INT_GEN_CFG_G_AOI_G_MASK  >> 1) // [ 0 == latch | 1 == latch ] interrupt events
#define APV_LSM9DS1_INT_GEN_CFG_G_ZHIE_G_MASK           (APV_LSM9DS1_INT_GEN_CFG_G_LIR_G_MASK  >> 1)
#define APV_LSM9DS1_INT_GEN_CFG_G_ZLIE_G_MASK           (APV_LSM9DS1_INT_GEN_CFG_G_ZHIE_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_CFG_G_YHIE_G_MASK           (APV_LSM9DS1_INT_GEN_CFG_G_ZLIE_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_CFG_G_YLIE_G_MASK           (APV_LSM9DS1_INT_GEN_CFG_G_YHIE_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_CFG_G_XHIE_G_MASK           (APV_LSM9DS1_INT_GEN_CFG_G_YLIE_G_MASK >> 1)
#define APV_LSM9DS1_INT_GEN_CFG_G_XLIE_G_MASK           (APV_LSM9DS1_INT_GEN_CFG_G_XHIE_G_MASK >> 1)

// INT_GEN_THS_G (INT_GEN_THS_X_G, INT_GEN_THS_Y_G, INT_GEN_THS_Z_G) : gyro threshold registers
#define APV_LSM9DS1_INT_GEN_THS_G_MASK                  ((APV_LSM9DS1_FIELD_SIZE)0x7F)                            // threshold mask
#define APV_LSM9DS1_INT_GEN_THS_G_SHIFT                 (8)
#define APV_LSM9DS1_INT_GEN_THS_G_DCRM_G_MASK           ((APV_LSM9DS1_FIELD_SIZE)~APV_LSM9DS1_INT_GEN_THS_G_MASK) // [ 0 == reset | 1 == decrement ]
#define APV_LSM9DS1_INT_GEN_THS_G_DCRM_G_SHIFT          (7)

#define APV_LSM9DS1_INT_GEN_THS_G_READ(threshold,registerLow,registerHigh) \\
          { \\
          (threshold) = (int16_t)((uint8_t)(registerLow)); \\
          (threshold) = (int16_t)(((uint16_t)(threshold)) + ((uint16_t)(((uint8_t)registerHigh) & APV_LSM9DS1_INT_GEN_THS_G_MASK))); \\
          }

// DCRM is only present in the 'X' threshold register (see "INT_GEN_DUR_G")
#define APV_LSM9DS1_INT_GEN_THS_DCRM_G_READ(dcrm,registerHigh) \\
          { \\
          (dcrm) = ((registerHigh) & APV_LSM9DS1_INT_GEN_THS_G_DCRM_G_MASK) >> APV_LSM9DS1_INT_GEN_THS_G_DCRM_G_SHIFT; \\
          }

// DCRM is only present in the 'X' threshold register MUST be '0' otherwise
#define APV_LSM9DS1_INT_GEN_THS_G_WRITE(threshold,dcrm,registerLow,registerHigh) \\
          { \\
          (registerlow)  = (uint8_t)((uint16_t)(threshold)); \\
          (registerHigh) = (uint8_t)((((uint16_t)(threshold)) >> APV_LSM9DS1_INT_GEN_THS_G_SHIFT) & APV_LSM9DS1_INT_GEN_THS_G_MASK); \\
          (registerHigh) = ((uint8_t)(registerHigh)) + ((dcrm) << APV_LSM9DS1_INT_GEN_THS_G_DCRM_G_SHIFT); \\
          }

// Read macros
#define APV_LSM9DS1_INT_GEN_THS_X_G_READ      APV_LSM9DS1_INT_GEN_THS_G_READ
#define APV_LSM9DS1_INT_GEN_THS_Y_G_READ      APV_LSM9DS1_INT_GEN_THS_G_READ
#define APV_LSM9DS1_INT_GEN_THS_Z_G_READ      APV_LSM9DS1_INT_GEN_THS_G_READ

#define APV_LSM9DS1_INT_GEN_THS_X_DCRM_G_READ APV_LSM9DS1_INT_GEN_THS_DCRM_G_READ
#define APV_LSM9DS1_INT_GEN_THS_Y_DCRM_G_READ APV_LSM9DS1_INT_GEN_THS_DCRM_G_READ
#define APV_LSM9DS1_INT_GEN_THS_Z_DCRM_G_READ APV_LSM9DS1_INT_GEN_THS_DCRM_G_READ
// Write macros
#define APV_LSM9DS1_INT_GEN_THS_X_G_WRITE     APV_LSM9DS1_INT_GEN_THS_G_WRITE
#define APV_LSM9DS1_INT_GEN_THS_Y_G_WRITE     APV_LSM9DS1_INT_GEN_THS_G_WRITE
#define APV_LSM9DS1_INT_GEN_THS_Z_G_WRITE     APV_LSM9DS1_INT_GEN_THS_G_WRITE

// INT_GEN_DUR_G : see Table 100 and "APV_LSM9DS1_INT_GEN_THS_X_DCRM_G_READ"
#define APV_LSM9DS1_INT_GEN_DUR_G_DUR_G_MASK  ((APV_LSM9DS1_FIELD_SIZE)0x7F)                                  // interrupt duration mask
#define APV_LSM9DS1_INT_GEN_DUR_G_WAIT_G_MASK ((APV_LSM9DS1_FIELD_SIZE)~APV_LSM9DS1_INT_GEN_DUR_G_DUR_G_MASK) // [ 0 == no wait | 1 == wait ] for "DUR_G" samples 
                                                                                                              // before sampling and interrupt exit
/******************************************************************************/
/* Magnetometers :                                                            */
/******************************************************************************/

// OFFSET_X_REG_M (OFFSET_X_REG_L_M, OFFSET_X_REG_H_M) : see Tables 101/102
#define APV_LSM9DS1_OFFSET_REG_M_SHIFT (8)

#define APV_LSM9DS1_OFFSET_X_REG_M_READ APV_LSM9DS1_OUT_G
#define APV_LSM9DS1_OFFSET_Y_REG_M_READ APV_LSM9DS1_OUT_G
#define APV_LSM9DS1_OFFSET_Z_REG_M_READ APV_LSM9DS1_OUT_G

#define APV_LSM9DS1_OFFSET_REG_M_WRITE(environmentOffset,registerLow,registerHigh) \\
          { \\
          (registerlow)  = (uint8_t)((uint16_t)(environmentOffset)); \\
          (registerHigh) = (uint8_t)((uint16_t)(environmentOffset >> APV_LSM9DS1_OFFSET_REG_M_SHIFT)); \\
          }

#define APV_LSM9DS1_OFFSET_X_REG_M_WRITE APV_LSM9DS1_OFFSET_REG_M_WRITE
#define APV_LSM9DS1_OFFSET_Y_REG_M_WRITE APV_LSM9DS1_OFFSET_REG_M_WRITE
#define APV_LSM9DS1_OFFSET_Z_REG_M_WRITE APV_LSM9DS1_OFFSET_REG_M_WRITE

// WHO_AM_I_M : "who am I ?"
#define APV_LSM9DS1_WHO_AM_I_M_MASK               ((APV_LSM9DS1_FIELD_SIZE)0xFF)

// CTRL_REG1_M : see Table 109 et. seq
#define APV_LSM9DS1_CTRL_REG1_M_ST_MASK           ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG1_M_FAST_ODR          ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG1_M_FAST_ODR_SHIFT    (1)
#define APV_LSM9DS1_CTRL_REG1_M_FAST_ODR_MASK     (APV_LSM9DS1_CTRL_REG1_M_FAST_ODR << APV_LSM9DS1_CTRL_REG1_M_FAST_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO                ((APV_LSM9DS1_FIELD_SIZE)0x07)
#define APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT          (1 + APV_LSM9DS1_CTRL_REG1_M_FAST_ODR_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_MASK           (APV_LSM9DS1_CTRL_REG1_M_DO << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_OM                ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG1_M_OM_SHIFT          (3 + APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_OM_MASK           (APV_LSM9DS1_CTRL_REG1_M_OM << APV_LSM9DS1_CTRL_REG1_M_OM_MASK)
#define APV_LSM9DS1_CTRL_REG1_M_TEMP_COMP         ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG1_M_TEMP_COMP_SHIFT   (2 + APV_LSM9DS1_CTRL_REG1_M_OM_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_TEMP_COMP_MASK    (APV_LSM9DS1_CTRL_REG1_M_TEMP_COMP << APV_LSM9DS1_CTRL_REG1_M_TEMP_COMP_SHIFT)
// X- and Y-axis mode :
#define APV_LSM9DS1_CTRL_REG1_M_OM_LOW_POWER      (0x00 << APV_LSM9DS1_CTRL_REG1_M_OM_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_OM_MEDIUM         (0x01 << APV_LSM9DS1_CTRL_REG1_M_OM_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_OM_HIGH           (0x02 << APV_LSM9DS1_CTRL_REG1_M_OM_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_OM_ULTRA          (0x03 << APV_LSM9DS1_CTRL_REG1_M_OM_SHIFT)
// Ouptput data rate
#define APV_LSM9DS1_CTRL_REG1_M_DO_0p625_Hz       (0x00 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_1p25_Hz        (0x01 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_2p5_Hz         (0x02 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_5_Hz           (0x03 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_10_Hz          (0x04 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_20_Hz          (0x05 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_40_Hz          (0x06 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)
#define APV_LSM9DS1_CTRL_REG1_M_DO_80_Hz          (0x07 << APV_LSM9DS1_CTRL_REG1_M_DO_SHIFT)

// CTRL_REG2_M : see Table 112 et. seq
#define APV_LSM9DS1_CTRL_REG2_M_SOFT_RST          ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG2_M_SOFT_RST_SHIFT    (2) // bits 0,1 are empty
#define APV_LSM9DS1_CTRL_REG2_M_SOFT_RST_MASK     (APV_LSM9DS1_CTRL_REG2_M_SOFT_RST << APV_LSM9DS1_CTRL_REG2_M_SOFT_RST_SHIFT)
#define APV_LSM9DS1_CTRL_REG2_M_REBOOT            ((APV_LSM9DS1_FIELD_SIZE)0x01)
#define APV_LSM9DS1_CTRL_REG2_M_REBOOT_SHIFT      (1 + APV_LSM9DS1_CTRL_REG2_M_SOFT_RST_SHIFT)
#define APV_LSM9DS1_CTRL_REG2_M_REBOOT_MASK       (APV_LSM9DS1_CTRL_REG2_M_REBOOT << APV_LSM9DS1_CTRL_REG2_M_REBOOT_SHIFT)
#define APV_LSM9DS1_CTRL_REG2_M_FS                ((APV_LSM9DS1_FIELD_SIZE)(0x03))
#define APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT          (2 + APV_LSM9DS1_CTRL_REG2_M_REBOOT_SHIFT) // bit 4 is empty
#define APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT_MASK     (APV_LSM9DS1_CTRL_REG2_M_FS << APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT)
// Full-scale :
#define APV_LSM9DS1_CTRL_REG2_M_FS_s4_GAUSS       (0x00 << APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT)
#define APV_LSM9DS1_CTRL_REG2_M_FS_s8_GAUSS       (0x01 << APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT)
#define APV_LSM9DS1_CTRL_REG2_M_FS_s12_GAUSS      (0x02 << APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT)
#define APV_LSM9DS1_CTRL_REG2_M_FS_s16_GAUSS      (0x03 << APV_LSM9DS1_CTRL_REG2_M_FS_SHIFT)

// CTRL_REG3_M : see Table 115 et. seq
#define APV_LSM9DS1_CTRL_REG3_M_MD_MASK           ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG3_M_SIM               ((APV_LSM9DS1_FIELD_SIZE)0x01) // [ 0 == SPI write-only | 1 == SPI read-write ]
#define APV_LSM9DS1_CTRL_REG3_M_SIM_SHIFT         (2)
#define APV_LSM9DS1_CTRL_REG3_M_SIM_MASK          (APV_LSM9DS1_CTRL_REG3_M_SIM << APV_LSM9DS1_CTRL_REG3_M_SIM_SHIFT)
#define APV_LSM9DS1_CTRL_REG3_M_LP                ((APV_LSM9DS1_FIELD_SIZE)0x01) // [ 0 == DO | 1 == 0p625 ] Hz
#define APV_LSM9DS1_CTRL_REG3_M_LP_SHIFT          (3 + APV_LSM9DS1_CTRL_REG3_M_SIM_SHIFT) // bits 3,4 are empty
#define APV_LSM9DS1_CTRL_REG3_M_LP_MASK           (APV_LSM9DS1_CTRL_REG3_M_LP << APV_LSM9DS1_CTRL_REG3_M_LP_SHIFT)
#define APV_LSM9DS1_CTRL_REG3_M_I2C_DISABLE       ((APV_LSM9DS1_FIELD_SIZE)0x01) // [ 0 == I2C | 1 == !I2C ]
#define APV_LSM9DS1_CTRL_REG3_M_I2C_DISABLE_SHIFT (2 + APV_LSM9DS1_CTRL_REG3_M_LP_SHIFT) // bit 6 is empty
#define APV_LSM9DS1_CTRL_REG3_M_I2C_DISABLE_MASK  (APV_LSM9DS1_CTRL_REG3_M_I2C_DISABLE << APV_LSM9DS1_CTRL_REG3_M_I2C_DISABLE_SHIFT)

// CTRL_REG4_M : see Table 118 et. seq
#define APV_LSM9DS1_CTRL_REG4_M_BLE               ((APV_LSM9DS1_FIELD_SIZE)0x01) // [ 0 == little | 1 == big- ] endian
#define APV_LSM9DS1_CTRL_REG4_M_BLE_SHIFT         (1) // bit 0 is empty
#define APV_LSM9DS1_CTRL_REG4_M_BLE_MASK          (APV_LSM9DS1_CTRL_REG4_M_BLE << APV_LSM9DS1_CTRL_REG4_M_BLE_SHIFT)
#define APV_LSM9DS1_CTRL_REG4_M_OMZ               ((APV_LSM9DS1_FIELD_SIZE)0x03)
#define APV_LSM9DS1_CTRL_REG4_M_OMZ_SHIFT         (1 + APV_LSM9DS1_CTRL_REG4_M_BLE_SHIFT)
#define APV_LSM9DS1_CTRL_REG4_M_OMZ_MASK          (APV_LSM9DS1_CTRL_REG4_M_OMZ << APV_LSM9DS1_CTRL_REG4_M_OMZ_SHIFT)
// Z-axis operation :
#define APV_LSM9DS1_CTRL_REG4_M_OMZ_LOW_POWER     (0x00 << APV_LSM9DS1_CTRL_REG4_M_OMZ_SHIFT)
#define APV_LSM9DS1_CTRL_REG4_M_OMZ_MEDIUM        (0x01 << APV_LSM9DS1_CTRL_REG4_M_OMZ_SHIFT)
#define APV_LSM9DS1_CTRL_REG4_M_OMZ_HIGH          (0x02 << APV_LSM9DS1_CTRL_REG4_M_OMZ_SHIFT)
#define APV_LSM9DS1_CTRL_REG4_M_OMZ_ULTRA         (0x03 << APV_LSM9DS1_CTRL_REG4_M_OMZ_SHIFT)

// CTRL_REG5_M : see Table 121 et. seq
#define APV_LSM9DS1_CTRL_REG5_M_BDU               ((APV_LSM9DS1_FIELD_SIZE)0x01) // [ 0 == continuous | LSB+MSB ] block update
#define APV_LSM9DS1_CTRL_REG5_M_BDU_SHIFT         (6) // bits 0 { .. } 5 empty
#define APV_LSM9DS1_CTRL_REG5_M_BDU_MASK          (APV_LSM9DS1_CTRL_REG5_M_BDU << APV_LSM9DS1_CTRL_REG5_M_BDU_SHIFT)
#define APV_LSM9DS1_CTRL_REG5_M_FAST_READ         ((APV_LSM9DS1_FIELD_SIZE)0x01) // [ 0 == !FAST_READ | 1 == FAT_READ ]
#define APV_LSM9DS1_CTRL_REG5_M_FAST_READ_SHIFT   (1 + APV_LSM9DS1_CTRL_REG5_M_BDU_SHIFT)
#define APV_LSM9DS1_CTRL_REG5_M_FAST_READ_MASK    (APV_LSM9DS1_CTRL_REG5_M_FAST_READ << APV_LSM9DS1_CTRL_REG5_M_FAST_READ_SHIFT)

// STATUS_REG_M : see Table 123 et. seq
#define APV_LSM9DS1_STATUS_REG_M_ZYXOR            ((APV_LSM9DS1_FIELD_SIZE)0x80)
#define APV_LSM9DS1_STATUS_REG_M_ZOR              (APV_LSM9DS1_STATUS_REG_M_ZYXOR >> 1)
#define APV_LSM9DS1_STATUS_REG_M_YOR              (APV_LSM9DS1_STATUS_REG_M_ZOR   >> 1)
#define APV_LSM9DS1_STATUS_REG_M_XOR              (APV_LSM9DS1_STATUS_REG_M_YOR   >> 1)
#define APV_LSM9DS1_STATUS_REG_M_ZYXDA            (APV_LSM9DS1_STATUS_REG_M_XOR   >> 1)
#define APV_LSM9DS1_STATUS_REG_M_ZDA              (APV_LSM9DS1_STATUS_REG_M_ZYXDA >> 1)
#define APV_LSM9DS1_STATUS_REG_M_YDA              (APV_LSM9DS1_STATUS_REG_M_ZDA   >> 1)
#define APV_LSM9DS1_STATUS_REG_M_XDA              (APV_LSM9DS1_STATUS_REG_M_YDA   >> 1)

// OUT_X_M (OUT_X_L_M, OUT_X_H_M) : 
#define APV_LSM9DS1_OUT_X_M_READ APV_LSM9DS1_OUT_G
// OUT_Y_M (OUT_Y_L_M, OUT_Y_H_M) :
#define APV_LSM9DS1_OUT_Y_M_READ APV_LSM9DS1_OUT_G
// OUT_Z_M (OUT_Z_L_M, OUT_Z_H_M) :
#define APV_LSM9DS1_OUT_Z_M_READ APV_LSM9DS1_OUT_G

// INT_CFG_M : see Table 125 et. seq
#define APV_LSM9DS1_INT_CFG_M_XIEN_MASK           ((APV_LSM9DS1_FIELD_SIZE)0x80)
#define APV_LSM9DS1_INT_CFG_M_YIEN_MASK           (APV_LSM9DS1_INT_CFG_M_XIEN_MASK >> 1)
#define APV_LSM9DS1_INT_CFG_M_ZIEN_MASK           (APV_LSM9DS1_INT_CFG_M_YIEN_MASK >> 1)
#define APV_LSM9DS1_INT_CFG_M_IEA_MASK            (APV_LSM9DS1_INT_CFG_M_ZIEN_MASK >> 3) // bits 3,4 are empty
#define APV_LSM9DS1_INT_CFG_M_IEL_MASK            (APV_LSM9DS1_INT_CFG_M_IEA_MASK  >> 1)
#define APV_LSM9DS1_INT_CFG_M_IEN_MASK            (APV_LSM9DS1_INT_CFG_M_IEL_MASK  >> 1)

// INT_SRC_M : see Table 127 et. seq
#define APV_LSM9DS1_INT_SRC_M_PTH_X_MASK          ((APV_LSM9DS1_FIELD_SIZE)0x80)
#define APV_LSM9DS1_INT_SRC_M_PTH_Y_MASK          (APV_LSM9DS1_INT_SRC_M_PTH_X_MASK >> 1)
#define APV_LSM9DS1_INT_SRC_M_PTH_Z_MASK          (APV_LSM9DS1_INT_SRC_M_PTH_Y_MASK >> 1)
#define APV_LSM9DS1_INT_SRC_M_NTH_X_MASK          (APV_LSM9DS1_INT_SRC_M_PTH_Z_MASK >> 1)
#define APV_LSM9DS1_INT_SRC_M_NTH_Y_MASK          (APV_LSM9DS1_INT_SRC_M_NTH_X_MASK >> 1)
#define APV_LSM9DS1_INT_SRC_M_NTH_Z_MASK          (APV_LSM9DS1_INT_SRC_M_NTH_Y_MASK >> 1)
#define APV_LSM9DS1_INT_SRC_M_MROI_MASK           (APV_LSM9DS1_INT_SRC_M_NTH_Z_MASK >> 1)
#define APV_LSM9DS1_INT_SRC_M_INT_MASK            (APV_LSM9DS1_INT_SRC_M_MROI_MASK  >> 1)

// INT_THS : see Table 129 et. seq
#define APV_LSM9DS1_INT_THS_M_MASK                ((APV_LSM9DS1_FIELD_SIZE)0x7F)

#define APV_LSM9DS1_INT_THS_M_READ(threshold,registerLow,registerHigh) \\
          { \\
          (threshold) = (uint16_t)((uint8_t)registerLow); \\
          (threshold) = (threshold) + ((uint16_t)(((uint8_t)(registerHigh) & APV_LSM9DS1_INT_THS_MASK)); \\
          }

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
/* Global Function Declarations :                                             */
/******************************************************************************/

extern APV_ERROR_CODE apvInitialiseLsm9ds1(Spi                  *spiControlBlock_p,
                                           apvCoreTimerBlock_t  *apvCoreTimerBlock);

extern void           apvLsm9ds1StateTimer(void *durationEventMessage);

/******************************************************************************/
/* Global Variable Declarations :                                             */
/******************************************************************************/

extern          bool  apvSPI0ReceiverReady;
extern          bool  apvSPI0Mutex;
extern volatile bool  apvLsm9ds1TimerFlag;

extern const apvLsm9ds1RegisterDescriptor_t apvLsm9ds1AccelerometerAndGyroRegisters[APV_LSM9DS1_ACCELEROMETER_GYRO_REGISTER_SET_SIZE];
extern const apvLsm9ds1RegisterDescriptor_t apvLsm9ds1MagnetometerRegisters[APV_LSM9DS1_MAGNETOMETER_REGISTER_SET_SIZE];

/******************************************************************************/

#endif

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

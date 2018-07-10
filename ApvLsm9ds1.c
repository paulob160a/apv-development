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
/* Includes :                                                                 */
/******************************************************************************/

#include "ApvLsm9ds1.h"

/******************************************************************************/
/* Global Variables :                                                         */
/******************************************************************************/

// Section 6, "Register Mapping", Table 21
const apvLsm9ds1RegisterDescriptor_t apvLsm9ds1AccelerometerAndGyroRegisters[APV_LSM9DS1_ACCELEROMETER_GYRO_REGISTER_SET_SIZE] = 
  {
    {
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_00,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_01,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_02,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_03,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_ACT_THS,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_ACT_DUR,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_CFG_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_X_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_Y_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_Z_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_DUR_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_REFERENCE_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT1_CTRL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT2_CTRL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_0D,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_WHO_AM_I,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_WHO_AM_I_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG1_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG2_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG3_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_ORIENT_CFG_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_SRC_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_TEMP_L,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_TEMP_H,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_STATUS_17,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG4,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_CTRL_REG4_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG5_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_CTRL_REG5_XL_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG6_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG7_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG8,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_CTRL_REG8_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG9,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG10,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_25,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_SRC_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_STATUS_27,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_FIFO_CTRL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_FIFO_SRC,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_CFG_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_XH_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_XL_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_YH_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_YL_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_ZH_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_ZL_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_DUR_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    }
  };

// Section 6, "Register Mapping", Table 22
const apvLsm9ds1RegisterDescriptor_t apvLsm9ds1MagnetometerRegisters[APV_LSM9DS1_MAGNETOMETER_REGISTER_SET_SIZE] = 
  {
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_00,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_01,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_02,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_03,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_04,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_X_REG_L_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_X_REG_H_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Y_REG_L_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Y_REG_H_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Z_REG_L_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OFFSET_Z_REG_H_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0B,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0C,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0D,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_0E,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_WHO_AM_I_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_M_WHO_AM_I_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_10,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_11,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_12,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_13,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_14,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_15,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_16,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_17,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_18,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_19,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1A,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1B,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1C,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1D,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1E,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_1F,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG1_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_M_CTRL_REG1_M_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG2_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG3_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_M_CTRL_REG3_M_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG4_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG5_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_25,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_26,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_STATUS_REG_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_2E,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_M_2F,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_INT_CFG_M,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_M_INT_CFG_M_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_INT_SRC_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_INT_THS_L_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    {  
    APV_LSM9DS1_REGISTER_ADDRESS_INT_THS_H_M,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    }
  };

/******************************************************************************/
/* (C) PulsingCoreSoftware Limited 2018 (C)                                   */
/******************************************************************************/

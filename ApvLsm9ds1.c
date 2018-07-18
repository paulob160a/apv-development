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
    { // Reserved
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_00,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Reserved
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_01,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Reserved
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_02,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Reserved
      APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_03,
      APV_LSM9DS1_REGISTER_RESERVED,
      APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Inactivity threshold : 0:6 --> inactivity threshold default 0x00
      // Inactivity mode      : 7   --> [ 0 == power-down | 1 == sleep ] default 0
    APV_LSM9DS1_REGISTER_ADDRESS_ACT_THS,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Inactivity duration : 0:7 --> inactivity duration default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_ACT_DUR,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer interrupt configuration :
      // 0x01 : enable XLIE
      // 0x02 : enable XHIE
      // 0x04 : enable YLIE
      // 0x08 : enable YHIE
      // 0x10 : enable ZLIE
      // 0x20 : enable ZHIE
      // 0x40 : enable 6D detection
      // 0x80 : [ 0 == OR | 1 == AND ] interrupt event combination
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_CFG_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer X-axis interrupt threshold : 0:7 interrupt threshold default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_X_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer Y-axis interrupt threshold : 0:7 interrupt threshold default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_Y_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer Z-axis interrupt threshold : 0:7 interrupt threshold default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_Z_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer interrupt duration : 0:6 --> interrupt duration default 0x00
      // Wait on interrupt                : 7   --> [ 0 == no wait on interrupt exit | 1 == wait for "duration" on interrupt exit ]
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_DUR_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro high-pass filter reference : 0:7 --> hp filter reference default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_REFERENCE_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer/gyro pin INT1_A/G control : default 0x00
      // 0x01 : enable accelerometer "data-ready" interrupt on INT1_A/G
      // 0x02 : enable gyroscope "data-ready" interrupt on INT1_A/G
      // 0x04 : enable "boot status ready" interrupt on INT1_A/G 
      // 0x08 : enable FIFO threshold interrupt on INT1_A/G
      // 0x10 : enable overrun interrupt on INT1_A/G
      // 0x20 : enable FSS5 interrupt on INT1_A/G
      // 0x40 : enable accelerometer interrupt on INT1_A/G
      // 0x80 : enable gyro interrupt on INT1_A/G
    APV_LSM9DS1_REGISTER_ADDRESS_INT1_CTRL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer/gyro pin INT2_A/G control : default 0x00
      // 0x01 : enable accelerometer "data-ready" interrupt on INT2_A/G
      // 0x02 : enable gyroscope "data-ready" interrupt on INT2_A/G
      // 0x04 : enable "temperature ready" interrupt on INT2_A/G
      // 0x08 : enable FIFO threshold interrupt on INT2_A/G
      // 0x10 : enable overrun interrupt on INT2_A/G
      // 0x20 : enable FSS5 interrupt on INT2_A/G
      // 0x80 : enable inactivity interrupt on INT2_A/G
    APV_LSM9DS1_REGISTER_ADDRESS_INT2_CTRL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Reserved
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_0D,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Who am I ? : 0:7 --> accelerometer.gyro register set id default 0x68
    APV_LSM9DS1_REGISTER_ADDRESS_WHO_AM_I,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_WHO_AM_I_DEFAULT
    },
    { // Gyro control register : default 0x00
      // 0x03 : bandwidth selection
      // 0x18 : full-scale selection
      // 0xe0 : output data-rate selection
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG1_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro control register : default 0x00
      // 0x03 : output selector
      // 0xC0 : interrupt selector
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG2_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro control register :  : default 0x00
      // 0x0F : high-pass filter cutoff selection (output data-rate dependent)
      // 0x40 : high-pass filter enable [ 0 == disable | 1 == enable ]
      // 0x80 : low pass-filter enable  [ 0 == disable | 1 == enable ]
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG3_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro orientation/sign control : default 0x00
      // 0x07 : directional user orientation
      // 0x08 : yaw angular rate sign   [ 0 == '+' | 1 == '-' ]
      // 0x10 : roll angular rate sign  [ 0 == '+' | 1 == '-' ]
      // 0x20 : pitch angular rate sign [ 0 == '+' | 1 == '-' ]
    APV_LSM9DS1_REGISTER_ADDRESS_ORIENT_CFG_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro interrupt configuration : default n/a
      // 0x01 : pitch low interrupt has occurred
      // 0x02 : pitch high interrupt has occurred
      // 0x04 : roll low interrupt has occurred
      // 0x08 : roll high interrupt has occurred
      // 0x10 : yaw low interrupt has occurred
      // 0x20 : yaw high interrupt has occurred
      // 0x40 : [ 0 == no  interrupt has occurred | 1 == interrupt has occurred ]
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_SRC_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Temperature data low : 2's complement temperature low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_TEMP_L,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Temperature data low : 2's complement temperature high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_TEMP_H,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer + gyro status register : default n/a
      // 0x01 : accelerometer data available
      // 0x02 : gyro data available
      // 0x04 : temperature data available
      // 0x08 : boot atatus : [ 0 == no boot | 1 == booting ]
      // 0x10 : inactivity interrupt status : [ 0 == no interrupt | 1 == interrupt ]
      // 0x20 : gyro interrupt status : [ 0 == no interrupt | 1 == interrupt ]
      // 0x40 : accelerometer interrupt status : [ 0 == no interrupt | 1 == interrupt ]
    APV_LSM9DS1_REGISTER_ADDRESS_STATUS_17,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement X-rate low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement X-rate high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Y-rate low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Y-rate high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Z-rate low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Z-rate high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_G,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer + gyro control register : default 0x38
      // 0x01 : position recognition interrupt : [ 0 == 6D | 1 == 4D ]
      // 0x02 : interrupt latch : [ 0 == no latch | 1 == latch ]
      // 0x08 : gyro pitch axis output enable [ 0 == not enabled | 1 = enabled ]
      // 0x10 : gyro roll axis output enable  [ 0 == not enabled | 1 = enabled ]
      // 0x20 : gyro yaw axis output enable   [ 0 == not enabled | 1 = enabled ]
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG4,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_CTRL_REG4_DEFAULT
    },
    { // Accelerometer control register : default 0x38
      // 0x08 : x-axis output enable [ 0 == not enabled | 1 = enabled ]
      // 0x10 : y-axis output enable [ 0 == not enabled | 1 = enabled ]
      // 0x20 : z-axis output enable [ 0 == not enabled | 1 = enabled ]
      // 0xC0 : sampling rate decimation
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG5_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_CTRL_REG5_XL_DEFAULT
    },
    { // Accelerometer control register : default 0x00
      // 0x03 : anti-aliasing filter bandwidth
      // 0x04 : bandwidth selection
      // 0x18 : full-scale selection
      // 0xE0 : output data rate selection
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG6_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer control register : default 0x00
      // 0x01 : high-pass filter enable [ 0 == bypassed | 1 = enabled ]
      // 0x04 : filtered data enable    [ 0 == bypassed | 1 = enabled ]
      // 0x60 : low- and high-pass filters' bandwidth select
      // 0x80 : high-resolution mode [ 0 == bypassed | 1 = enabled ]
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG7_XL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Device control register : default 0x04
      // 0x01 : softwaze reset [ 0 == normal | 1 == reset ]
      // 0x02 : endian-ness [ 0 == little | 1 == big ]
      // 0x04 : register address auto-increment [ 0 == no | 1 == yes ]
      // 0x08 : SPI interface mode [ 0 == 4-wire | 1 == 3-wire ]
      // 0x10 : INT1_A/G and INT2_A/G drive mode [ 0 == push/pull | 1 == open-drain ]
      // 0x20 : interrupt active level [ 0 == active high | 1 == active low ]
      // 0x40 : block data update mode [ 0 == continuous | 1 == wait on MSB/LSB read ]
      // 0x80 : memory reboot [ 0 == norma | 1 == reboot ]
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG8,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_CTRL_REG8_DEFAULT
    },
    { // Device control register : default 0x00
      // 0x01 : enable FIFO threshold [ 0 == not enabled | 1 == enable ]
      // 0x02 : FIFO enable           [ 0 == not enabled | 1 == enable ]
      // 0x04 : enable/disable I2C    [ 0 == I2C enabled | 1 == I2C not enabled ]
      // 0x08 : data available timer  [ 0 == not enabled | 1 == enable ]
      // 0x10 : store temperature data in FIFO [ 0 == not enabled | 1 == enable ]
      // 0x40 : gyro sleep mode [ 0 == not enabled | 1 == enable ]
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG9,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer + gyro control register : default 0x00
      // 0x01 : accelerometer self-test [ 0 == not enabled | 1 == enable ]
      // 0x04 : gyo self-test           [ 0 == not enabled | 1 == enable ]
    APV_LSM9DS1_REGISTER_ADDRESS_CTRL_REG10,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Reserved
    APV_LSM9DS1_REGISTER_ADDRESS_RESERVED_25,
    APV_LSM9DS1_REGISTER_RESERVED,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer interrupt source : default n/a
      // 0x01 : X-low event interrupt has occurred
      // 0x02 : X-high event interrupt has occurred
      // 0x04 : Y-low event interrupt has occurred
      // 0x08 : Y-high event interrupt has occurred
      // 0x10 : Z-low event interrupt has occurred
      // 0x20 : Z-high event interrupt has occurred
      // 0x40 : interrupt state [ 0 == no interrupt | 1 == interrupt ]
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_SRC_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Device status register : default n/a
      // 0x01 : accelerometer new data available [ 0 == none | 1 == new data ]
      // 0x02 : gyro new data available          [ 0 == none | 1 == new data ]
      // 0x04 : temperature new data available   [ 0 == none | 1 == new data ]
      // 0x08 : boot status [ 0 == no boot | 1 == boot ]
      // 0x10 : inactivity interrupt [ 0 == no interrupt | 1 == interrupt ]
      // 0x20 : gyro interrupt          [ 0 == no interrupt | 1 == interrupt ]
      // 0x40 : accelerometer interrupt [ 0 == no interrupt | 1 == interrupt ]
    APV_LSM9DS1_REGISTER_ADDRESS_STATUS_27,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer 2's complement X-axis low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_L_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer 2's complement X-axis high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_X_H_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer 2's complement Y-axis low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_L_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer 2's complement Y-axis high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Y_H_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer 2's complement Z-axis low : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_L_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Accelerometer 2's complement Z-axis high : default n/a
    APV_LSM9DS1_REGISTER_ADDRESS_OUT_Z_H_XL,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // FIFO control : default 0x00
      // 0x1F : threshold
      // 0xE0 : mode selection
    APV_LSM9DS1_REGISTER_ADDRESS_FIFO_CTRL,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // FIFO source : default n/a
      // 0x3F : FIFO sample occupancy
      // 0x40 : overrun               [ 0 == FIFO not full | FIFO full ]
      // 0x80 : FIFO threshold status [ 0 == FIFO < threshold | FIFO >= threshold ]
    APV_LSM9DS1_REGISTER_ADDRESS_FIFO_SRC,
    APV_LSM9DS1_REGISTER_READ_ONLY,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro interrupt source configuration : default 0x00
      // 0x01 : enable X-low event
      // 0x02 : enable X-high event
      // 0x04 : enable Y-low event
      // 0x08 : enable Y-high event
      // 0x10 : enable Z-low event
      // 0x20 : enable Z-high event
      // 0x40 : enable interrupt latch [ 0 == no latch | 1 == latch ]
      // 0x80 : [ 0 == OR interrupt events | 1 == AND interrupt events ]
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_CFG_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement X-rate interrupt threshold low : default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_XH_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement X-rate interrupt threshold high + counter mode : default 0x00
      // 0x7F : threshold high
      // 0x80 : counter mode [ 0 == reset | 1 == decrement ]
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_XL_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Y-rate interrupt threshold low : default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_YH_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Y-rate interrupt threshold high : default 0x00
      // 0x7F : threshold high
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_YL_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Z-rate interrupt threshold low : default 0x00
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_ZH_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro 2's complement Z-rate interrupt threshold high : default 0x00
      // 0x7F : threshold high
    APV_LSM9DS1_REGISTER_ADDRESS_INT_GEN_THS_ZL_G,
    APV_LSM9DS1_REGISTER_READ_WRITE,
    APV_LSM9DS1_REGISTER_COMMON_DEFAULT
    },
    { // Gyro interrupt exit wait : default 0x00
      // 0x7F : wait duration (see "Gyro 2's complement X-rate interrupt threshold high + counter mode")
      // 0x80 : wait function enable [ 0 == no wait | 1 == wait ]
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

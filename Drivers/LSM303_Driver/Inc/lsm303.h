/**
 * @file       lsm303.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       2022-08-02
 * @author     Long Huynh 
 * @author     Nam Nguyen
 *             
 * @brief      LSM303DLHC driver function prototype
 *             
 * @note          
 * @example    lsm303.c
 *             Example_1 description
 */
/* Includes ----------------------------------------------------------- */
#include "main.h"
#include"stm32f4xx_hal.h"

#define LSM303DLHC_MULTIPLE_FLAG		0x80

#define LSM303DLHC_ACCEL_ADDRESS		0x32
#define LSM303DLHC_MAG_ADDRESS			0x3C

// Accel
#define LSM303DLHC_CTRL_REG1_A			0x20	// Data rate, Low-power mode, Z,Y,X-axis enable.
#define LSM303DLHC_CTRL_REG2_A			0x21	// High-pass filter.
#define LSM303DLHC_CTRL_REG3_A			0x22	// Interrupts INT1.
#define LSM303DLHC_CTRL_REG4_A			0x23	// Update, Scale, Resolution, etc.
#define LSM303DLHC_CTRL_REG5_A			0x24	// Reboot memory, FIFO, etc.
#define LSM303DLHC_CTRL_REG6_A			0x25	// Interrupts PAD2
#define LSM303DLHC_REFERENCE_A			0x26
#define LSM303DLHC_STATUS_REG_A			0x27
#define LSM303DLHC_OUT_X_L_A			0x28
#define LSM303DLHC_OUT_X_H_A			0x29
#define LSM303DLHC_OUT_Y_L_A			0x2A
#define LSM303DLHC_OUT_Y_H_A			0x2B
#define LSM303DLHC_OUT_Z_L_A			0x2C
#define LSM303DLHC_OUT_Z_H_A			0x2D

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Acceleration data rate
 */
typedef enum {
	LSM303DLHC_ACCEL_DATA_RATE_OFF      = 0x0,
	LSM303DLHC_ACCEL_DATA_RATE_1HZ      = 0x1,
	LSM303DLHC_ACCEL_DATA_RATE_10HZ     = 0x2,
	LSM303DLHC_ACCEL_DATA_RATE_25HZ     = 0x3,
	LSM303DLHC_ACCEL_DATA_RATE_50HZ     = 0x4,
	LSM303DLHC_ACCEL_DATA_RATE_100HZ    = 0x5,
	LSM303DLHC_ACCEL_DATA_RATE_200HZ    = 0x6,
	LSM303DLHC_ACCEL_DATA_RATE_400HZ    = 0x7,
	LSM303DLHC_ACCEL_DATA_RATE_1_620kHZ = 0x8,	// Low-power mode
	LSM303DLHC_ACCEL_DATA_RATE_1_344kHZ = 0x9	// Low-power mode (5.376 kHz)
} LSM303DLHC_Accel_DataRate_Type;

/**
 * @brief Acceleration scale
 */
typedef enum {
	LSM303DLHC_ACCEL_SCALE_2G           = 0x0,
	LSM303DLHC_ACCEL_SCALE_4G           = 0x1,
	LSM303DLHC_ACCEL_SCALE_8G           = 0x2,
	LSM303DLHC_ACCEL_SCALE_16G          = 0x3
} LSM303DLHC_Accel_Scale_Type;
typedef enum{
	ON_PLANE,
	ERECT_VERTICAL,
	ERECT_HORIZONTAL,
	OTHER
}direction_t;
/**
 * @brief LSM303DLHC Object
 */

typedef struct {
	I2C_HandleTypeDef* i2c_dev;
	int16_t accel_X_adc;
	int16_t accel_Y_adc;
	int16_t accel_Z_adc;
	float accel_X;
	float accel_Y;
	float accel_Z;
	direction_t direct;
	uint8_t read_status;
} LSM303DLHC_TypeDef;
//public_struct_type_t;

#ifndef LMS303_DRIVER_INC_LMS303_H_
#define LMS303_DRIVER_INC_LMS303_H_

void lsm303_init(LSM303DLHC_TypeDef *dev, I2C_HandleTypeDef *i2c);
uint8_t lsm303_read_reg(LSM303DLHC_TypeDef *dev, uint8_t reg_address);
void lsm303_write_reg(LSM303DLHC_TypeDef *dev, uint8_t reg_address, uint8_t value);
void lsm303dlhc_setaccelerometerdatarate(LSM303DLHC_TypeDef *dev, LSM303DLHC_Accel_DataRate_Type rate);
void lsm303dlhc_setaccelerometerscale(LSM303DLHC_TypeDef *dev, LSM303DLHC_Accel_Scale_Type scale);
void lsm303dlhc_getaccelerometer(LSM303DLHC_TypeDef *dev);
void lsm303dlhc_getdirection(LSM303DLHC_TypeDef *dev);
void lsm303dlhc_reset(LSM303DLHC_TypeDef *dev);
#endif /* LMS303_DRIVER_INC_LMS303_H_ */

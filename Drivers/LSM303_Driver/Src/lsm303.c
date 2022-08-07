/**
 * @file       lsm303.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       2022-08-02
 * @author     Long Huynh
 * @author     Nam Nguyen
 *
 * @brief      RTC functions definition
 *
 */
/* Includes ----------------------------------------------------------- */
#include "../Inc/lsm303.h"

#include "stm32f4xx_hal.h"
#include "main.h"
#include <math.h>
#include <stdio.h>

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */
/**
 * @brief <enum descriptiton>
 */

float accel_sensitivity;
/**
 * @brief <structure descriptiton>
 */

/* Private macros ----------------------------------------------------- */
/**
 * @brief  <macro description>
 *
 * @param[in]     <param_name>  <param_despcription>
 * @param[out]    <param_name>  <param_despcription>
 * @param[inout]  <param_name>  <param_despcription>
 *
 * @attention  <API attention note>
 *
 * @return
 *  - 0: Success
 *  - 1: Error
 */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Function definitions ----------------------------------------------- */
void lsm303_init(LSM303DLHC_TypeDef *dev, I2C_HandleTypeDef *i2c)
{
	dev->i2c_dev = i2c;
	// Accel init
	lsm303dlhc_setaccelerometerdatarate(dev, LSM303DLHC_ACCEL_DATA_RATE_10HZ);
	lsm303dlhc_setaccelerometerscale(dev, LSM303DLHC_ACCEL_SCALE_8G);
}
uint8_t lsm303_read_reg(LSM303DLHC_TypeDef *dev, uint8_t reg_address)
{
	uint8_t value_read;
	HAL_I2C_Master_Transmit(dev->i2c_dev, LSM303DLHC_ACCEL_ADDRESS, &reg_address, 1, 1000);
	HAL_I2C_Master_Receive(dev->i2c_dev, LSM303DLHC_ACCEL_ADDRESS, &value_read, 1, 1000);
	return value_read;
}
void lsm303_write_reg(LSM303DLHC_TypeDef *dev, uint8_t reg_address, uint8_t value)
{
	uint8_t write_package[2] = {reg_address, value};
	HAL_I2C_Master_Transmit(dev->i2c_dev, LSM303DLHC_ACCEL_ADDRESS, write_package, 2, 1000);
}
void lsm303dlhc_setaccelerometerdatarate(LSM303DLHC_TypeDef *dev, LSM303DLHC_Accel_DataRate_Type rate)
{
	uint8_t tempval = 0;
	tempval |= (rate << 4); // Data rate selection.
							//	tempval |= (1 << 3);		// 1: low-power mode
	tempval |= (1 << 2);	// 1: Z-axis enabled
	tempval |= (1 << 1);	// 1: Y-axis enabled
	tempval |= (1 << 0);	// 1: X-axis enabled
	lsm303_write_reg(dev, LSM303DLHC_CTRL_REG1_A, tempval);
}
void lsm303dlhc_setaccelerometerscale(LSM303DLHC_TypeDef *dev, LSM303DLHC_Accel_Scale_Type scale)
{
	uint8_t tempval = 0;

	//	tempval |= (1 << 7);		// 0: continuous update, 1: output registers not updated until MSB and LSB have been read
	//	tempval |= (1 << 6);		// 0: data LSB @ lower address, 1: data MSB @ lower address
	tempval |= (scale << 4); // Full-scale selection.
	tempval |= (1 << 3);	 // 0: high-resolution disable (10-bit), 1: high-resolution enable (12-bit)
							 //	tempval |= (1 << 0);		// 0: 4-wire interface, 1: 3-wire interface

	lsm303_write_reg(dev, LSM303DLHC_CTRL_REG4_A, tempval);

	switch (scale)
	{
	case LSM303DLHC_ACCEL_SCALE_2G:
		accel_sensitivity = 1 / 1000.0; // mG
		break;
	case LSM303DLHC_ACCEL_SCALE_4G:
		accel_sensitivity = 2 / 1000.0; // mG;
		break;
	case LSM303DLHC_ACCEL_SCALE_8G:
		accel_sensitivity = 4 / 1000.0; // mG;
		break;
	case LSM303DLHC_ACCEL_SCALE_16G:
		accel_sensitivity = 12 / 1000.0; // mG;
		break;
	}
}
void lsm303dlhc_getaccelerometer(LSM303DLHC_TypeDef *dev)
{
	uint8_t data[6];
	data[0] = lsm303_read_reg(dev, (LSM303DLHC_OUT_X_L_A | LSM303DLHC_MULTIPLE_FLAG));
	data[1] = lsm303_read_reg(dev, (LSM303DLHC_OUT_X_H_A | LSM303DLHC_MULTIPLE_FLAG));
	data[2] = lsm303_read_reg(dev, (LSM303DLHC_OUT_Y_L_A | LSM303DLHC_MULTIPLE_FLAG));
	data[3] = lsm303_read_reg(dev, (LSM303DLHC_OUT_Y_H_A | LSM303DLHC_MULTIPLE_FLAG));
	data[4] = lsm303_read_reg(dev, (LSM303DLHC_OUT_Z_L_A | LSM303DLHC_MULTIPLE_FLAG));
	data[5] = lsm303_read_reg(dev, (LSM303DLHC_OUT_Z_H_A | LSM303DLHC_MULTIPLE_FLAG));

	// LSB first, shift values to 6 bits for 10-bit resolution and to 4 bits for 12-bit resolution.
	dev->accel_X_adc = (int16_t)((data[1] << 8) | data[0]) >> 4;
	dev->accel_Y_adc = (int16_t)((data[3] << 8) | data[2]) >> 4;
	dev->accel_Z_adc = (int16_t)((data[5] << 8) | data[4]) >> 4;

	dev->accel_X = (float)dev->accel_X_adc * accel_sensitivity;
	dev->accel_Y = (float)dev->accel_Y_adc * accel_sensitivity;
	dev->accel_Z = (float)dev->accel_Z_adc * accel_sensitivity;
}
void lsm303dlhc_getdirection(LSM303DLHC_TypeDef *dev)
{
	uint8_t rotation_state = 0;
	if (dev->accel_Y > 0.1)
		rotation_state = 1;
	switch (rotation_state)
	{
	case 0:
		dev->direct = OTHER;
		break;
	case 1:
		if (dev->accel_Z < 0.95)
			dev->direct = ERECT_HORIZONTAL;
		break;
	}
}
void lsm303dlhc_reset(LSM303DLHC_TypeDef *dev)
{
	dev->accel_X_adc = 0;
	dev->accel_Y_adc = 0;
	dev->accel_Z_adc = 0;

	dev->accel_X = 0;
	dev->accel_Y = 0;
	dev->accel_Z = 0;
}

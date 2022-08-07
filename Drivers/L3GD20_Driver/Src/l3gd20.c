/**
 * @file       L3GD20.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       2022-07-30
 * @author     Long Huynh
 * @author     Nam Nguyen
 *
 * @brief      Function L3GD20 definitions
 *
 */

/* Includes ----------------------------------------------------------- */
#include "../../L3GD20_Driver/Inc/l3gd20.h"

#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */
/**
 * @brief <enum descriptiton>
 */

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
extern SPI_HandleTypeDef hspi1;

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  <function description>
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

/* Function definitions ----------------------------------------------- */
void l3gd20_cs_init(spi_cs_t *cs, GPIO_TypeDef *gpio_x, uint16_t gpio_pin)
{
	cs->gpio_x = gpio_x;
	cs->gpio_pin = gpio_pin;
}
void l3gd20_write(spi_cs_t *cs, uint8_t reg_adress, uint8_t reg_data)
{
	HAL_GPIO_WritePin(cs->gpio_x, cs->gpio_pin, GPIO_PIN_RESET);
	HAL_Delay(20);
	cs->spitxbuf[0] = reg_adress;
	cs->spitxbuf[1] = reg_data;
	HAL_SPI_Transmit(&hspi1, cs->spitxbuf, 2, 50);
	HAL_GPIO_WritePin(cs->gpio_x, cs->gpio_pin, GPIO_PIN_SET);
	HAL_Delay(20);
}
void l3gd20_read(spi_cs_t *cs, uint8_t reg_adress, uint8_t i)
{
	HAL_GPIO_WritePin(cs->gpio_x, cs->gpio_pin, GPIO_PIN_RESET);
	cs->spitxbuf[0] = reg_adress | RW_READ_WRITE_CMD;
	HAL_SPI_Transmit(&hspi1, cs->spitxbuf, 1, 50);
	HAL_SPI_Receive(&hspi1, cs->spirxbuf+i, 1, 50);
	HAL_GPIO_WritePin(cs->gpio_x, cs->gpio_pin, GPIO_PIN_SET);
}
void l3gd20_init(spi_cs_t *cs)
{
	HAL_GPIO_WritePin(cs->gpio_x, cs->gpio_pin, GPIO_PIN_SET);
	HAL_Delay(20);
	l3gd20_write(cs, L3GD20_CTRL_REG1_ADDR, L3GD20_CTRL_REG1_VAL);
	l3gd20_write(cs, L3GD20_CTRL_REG2_ADDR, L3GD20_CTRL_REG2_VAL);
	l3gd20_write(cs, L3GD20_CTRL_REG3_ADDR, L3GD20_CTRL_REG3_VAL);
	l3gd20_write(cs, L3GD20_CTRL_REG4_ADDR, L3GD20_CTRL_REG4_VAL);
	l3gd20_write(cs, L3GD20_CTRL_REG5_ADDR, L3GD20_CTRL_REG5_VAL);
}

/* End of file -------------------------------------------------------- */



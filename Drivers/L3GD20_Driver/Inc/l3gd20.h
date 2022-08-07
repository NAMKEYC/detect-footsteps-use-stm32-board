/*
 * @file       L3GD20.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       2022-07-30
 * @author     Long Huynh
 * @author     Nam Nguyen
 *
 * @brief      L3GD20 funtion prototypes
 *
 * @note
 * @example    L3GD20.c
 *       
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef L3GD20_DRIVER_INC_L3GD20_H_
#define L3GD20_DRIVER_INC_L3GD20_H_

/* Includes ----------------------------------------------------------- */
#include "stm32f4xx_hal.h"
/* Public defines ----------------------------------------------------- */
/**
 * @brief   L3GD20 registers configuration
 */

/* Control registers 1
 * Output data rate = 760 Hz
 * Cut-Off = 100
 * Power mode = Normal
 * Enable X, Y, Z
 *  */
#define L3GD20_CTRL_REG1_VAL          ((uint8_t) 0xFF)

/* Control registers 2,
 * High-pass filter mode = Normal
 * High-pass filter cut off frequency = 51.4 Hz */
#define L3GD20_CTRL_REG2_VAL          ((uint8_t) 0x00)

/* Control registers 3 (Reset) */
#define L3GD20_CTRL_REG3_VAL          ((uint8_t) 0x00)

/* Control registers 4,
 * Full scale = 2000 dps */
#define L3GD20_CTRL_REG4_VAL          ((uint8_t) 0x20)
/* Control registers 5,
 * High-pass filter enabled */
#define L3GD20_CTRL_REG5_VAL          ((uint8_t) 0x10)

/**
 * @brief   L3GD20 mask commands
 */
#define RW_READ_WRITE_CMD               ((uint8_t) 0x80)
/* Control registers 1:5 */
#define L3GD20_CTRL_REG1_ADDR         ((uint8_t) 0x20)
#define L3GD20_CTRL_REG2_ADDR         ((uint8_t) 0x21)
#define L3GD20_CTRL_REG3_ADDR         ((uint8_t) 0x22)
#define L3GD20_CTRL_REG4_ADDR         ((uint8_t) 0x23)
#define L3GD20_CTRL_REG5_ADDR         ((uint8_t) 0x24)

/* Output Registers X,Y,z */
#define L3GD20_OUT_X_L_ADDR           ((uint8_t) 0x28)
#define L3GD20_OUT_X_H_ADDR           ((uint8_t) 0x29)
#define L3GD20_OUT_Y_L_ADDR           ((uint8_t) 0x2A)
#define L3GD20_OUT_Y_H_ADDR           ((uint8_t) 0x2B)
#define L3GD20_OUT_Z_L_ADDR           ((uint8_t) 0x2C)
#define L3GD20_OUT_Z_H_ADDR           ((uint8_t) 0x2D)

/**
 * @brief   Number of samples collected for offset calculations
 */
#define CALIBRATION_BUFFER_LENGTH            ((uint32_t) 2000u)
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief   configure cs_spi IO PIN
 */
typedef struct
{
    GPIO_TypeDef *gpio_x;
    uint16_t gpio_pin;
    uint8_t spitxbuf[2];
    uint8_t spirxbuf[7];
}spi_cs_t;
/* Public macros ------------------------------------------------------ */
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
#define PUBLIC_MACRO(a)  do_something_with(a)

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

 void l3gd20_cs_init(spi_cs_t *cs, GPIO_TypeDef *gpio_x, uint16_t gpio_pin);
 /*
 * @brief Write Control registers
 * @note
 * @param cs, regadress, regdata
 * @retval None
 */
void l3gd20_write(spi_cs_t *cs, uint8_t reg_adress, uint8_t reg_data);
/**
 * @brief   Read register data
 * @note
 * @param   cs, regadress, location i in spirxbuf buffer
 * @retval  None
 */
void l3gd20_read(spi_cs_t *cs,uint8_t reg_adress,uint8_t i);
/**
 * @brief Configure SPI1
 * @param cs
 */
void l3gd20_init(spi_cs_t *cs);
/**
 * @brief   L3GD20 main function
 * @note
 * @param   None
 * @retval  None
 */

/* End of file -------------------------------------------------------- */

#endif /* L3GD20_DRIVER_INC_L3GD20_H_ */

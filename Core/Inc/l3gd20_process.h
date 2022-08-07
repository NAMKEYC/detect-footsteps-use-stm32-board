/**
 * @file       L3GD20.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    major.minor.patch
 * @date       2022-August-01
 * @author     Nam Nguyen
 * @brief      RTC function prototypes
 *
 * @note
 */
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __L3GD20_H
#define __L3GD20_H
/* Includes ----------------------------------------------------------------- */
#include "stm32f4xx_hal.h"

#include "../../Drivers/L3GD20_Driver/Inc/l3gd20.h"
/* Private defines ---------------------------------------------------------- */
/**
 * @brief   L3GD20 registers configuration
 */

/* Control registers 1
 * Output data rate = 760 Hz
 * Cut-Off = 100
 * Power mode = Normal
 * Enable X, Y, Z
 *  */
/**
 * @brief   Number of samples collected for offset calculations
 */
#define CALIBRATION_BUFFER_LENGTH            ((uint32_t) 2000u)

/**
 * @brief   Number of samples collected for offset calculations
 */
#define AVERAGE_WINDOW_SIZE                  ((uint32_t) 10u)
/**
 * @brief   Set sensitivity scale correction
 * Sensitivity at 2000 range = 70 mdps/digit = 0.07
 */
#define L3GD20_SENSITIVITY                     ((float) 0.07)
/**
 * @brief   L3GD20 states definition
 */
/* Public enumerate/structure ----------------------------------------------- */
typedef enum
{
	L3GD20_FIRST,
	L3GD20_SECOND
}L3GD20_statetype_t;
/**
 * @brief   L3GD20 calibration status definition
 */
typedef enum
{
	L3GD20_COLLECT_CALIBRATION_SAMPLES,
	L3GD20_PROCESS_CALIBRATION_SAMPLES,
	L3GD20_CALIBRATED,
}L3GD20_calistatetype_t;
/**
 * @brief   L3GD20 data ready flag
 */
typedef enum
{
	L3GD20_DATA_NOT_READY,
	L3GD20_DATA_READY,
}L3GD20_datareadyflagtype_t;
/**
 * @brief   configure cs_spi IO PIN
 */
typedef struct
{
   float anglerate_x;
   float anglerate_y;
   float anglerate_z;
}ang_device_t;
/**
 * @brief  initilize cs_spi
 * @param  cs, GPIOx, GPIO_PIN
 */
/* Public APIs -------------------------------------------------------------- */
/**
 * @brief   L3GD20 main function
 * @note
 * @param   None
 * @retval  None
 */
void l3gd20_main(ang_device_t *dev, spi_cs_t *cs);
/* End files ----------------------------------------------------------------- */
#endif

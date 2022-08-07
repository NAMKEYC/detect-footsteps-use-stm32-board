/**
 * @file       button.h
 * @copyright
 * @license
 * @version    1.0.0
 * @date       2022-07-18
 * @author     Nam Nguyen
 * @brief      button mode
 *             button mode perform with 2 mode is holding and clicking
 *             if click change light intensity
 *             if hold turn on/off led state
 *             Capacity = <size> - 1
 * @note       None
 * @example    None
 */
 /* Define to prevent recursive inclusion ------------------------------ */
#ifndef _BUTTON_H_
#define _BUTTON_H_

/* Includes ----------------------------------------------------------- */
#include "stm32f4xx_hal.h"
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief button state
 */
typedef enum{
	RELEASE = 0,
	PRESS
}button_state_t;
/**
 * @brief button event
 */
typedef enum{
	NOEVENT,
	HOLD,
	CLICK
}button_event_t;
/**
 * @brief button structure
 */
typedef struct{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	button_state_t state;
	button_event_t event;
}button_t;
/* Public function prototypes ----------------------------------------- */
void button_init(button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void button_state(button_t *button);

#endif //_BUTTON_H_

/* End of file -------------------------------------------------------- */

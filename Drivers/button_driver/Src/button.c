/**
 * @file       button.c
 * @version    1.0.0
 * @date       2022-07-17
 * @author     Nam Nguyen
 */
/* Includes ----------------------------------------------------------- */
#include "../../button_driver/Inc/button.h"
#include "stm32f4xx_hal.h"
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/*subfunction ----------------------------------------------------------------*/
void button_init(button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	button->GPIO_Pin = GPIO_Pin;
	button->GPIOx = GPIOx;
	button->event = NOEVENT;
}
void button_state(button_t *button)
{
  if (HAL_GPIO_ReadPin(button->GPIOx, button->GPIO_Pin) == 0)
  {
    button->state = RELEASE;
  }
  if (HAL_GPIO_ReadPin(button->GPIOx, button->GPIO_Pin) == 1)
  {
    button->state = PRESS;
  }
}

/* End of file -------------------------------------------------------- */

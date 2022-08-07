/*
 * sys_button.c
 *
 *  Created on: Jul 20, 2022
 *      Author: Long
 */
#include "sys_button.h"
#include "../../Drivers/button_driver/Inc/button.h"
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/*subfunction ----------------------------------------------------------------*/
void button_get_event(button_t *button, uint32_t count_time)
{
  button_state(button);        //Check button state
  if (button->state == PRESS)                             //check button if release
  {
    if (count_time >= 60)  //check if time is greater than 3000
    {
      button->event = HOLD;              //set button event with holding
      count_time=0;
    }
  }
  if (button->state == RELEASE)                             //check button if release
  {
    if (count_time < 60)           //check if time is greater than 50 and less than 3000
    {
      button->event = CLICK;                                      //set button event with Click
      count_time=0;
    }
  }
}



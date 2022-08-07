/*
 * sys_button.h
 *
 *  Created on: Jul 20, 2022
 *      Author: Long
 */

#include "../../button_driver/Inc/button.h"
#ifndef INC_SYS_BUTTON_H_
#define INC_SYS_BUTTON_H_
/* Public function prototypes ----------------------------------------- */
void button_get_event(button_t *button, uint32_t count_time);

#endif /* INC_SYS_BUTTON_H_ */

#ifndef __USER_TIMER_H__
#define __USER_TIMER_H__
#include "sys.h"

extern TIM_HandleTypeDef TIM3_Handler;  /* 定时器句柄 */

void TIM3_Init(uint16_t arr, uint16_t psc);
void TIM5_Init(uint16_t arr, uint16_t psc);

#endif

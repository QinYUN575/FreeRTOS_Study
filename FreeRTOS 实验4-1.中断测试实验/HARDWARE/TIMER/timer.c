#include "timer.h"
#include "led.h"
#include "usart.h"

TIM_HandleTypeDef TIM3_Handler; /* 定时器句柄 */
TIM_HandleTypeDef TIM5_Handler; /* 定时器句柄 */

void TIM3_Init(uint16_t arr, uint16_t psc)
{
    TIM3_Handler.Instance = TIM3;                               /* 通用定时器3 */
    TIM3_Handler.Init.Prescaler = psc;                          /* 分频系数 */
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    TIM3_Handler.Init.Period = arr;                             /* 自动重装载值 */
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* 时钟分频因子 */
    HAL_TIM_Base_Init(&TIM3_Handler);                           /* 配置参数 */

    HAL_TIM_Base_Start_IT(&TIM3_Handler);                       /* 使能定时器和定时器更新中断 */
}

void TIM5_Init(uint16_t arr, uint16_t psc)
{
    TIM5_Handler.Instance = TIM5;                               /* 通用定时器3 */
    TIM5_Handler.Init.Prescaler = psc;                          /* 分频系数 */
    TIM5_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    TIM5_Handler.Init.Period = arr;                             /* 自动重装载值 */
    TIM5_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* 时钟分频因子 */
    HAL_TIM_Base_Init(&TIM5_Handler);                           /* 配置参数 */

    HAL_TIM_Base_Start_IT(&TIM5_Handler);                       /* 使能定时器和定时器更新中断 */
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();            /* 使能 TIM3 时钟 */
        HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);  /* 设置中断优先级,抢占优先级 4, 子优先级0*/
        HAL_NVIC_EnableIRQ(TIM3_IRQn);          /* 开启 TIM3 中断 */
    } else if (htim->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();            /* 使能 TIM5 时钟 */
        HAL_NVIC_SetPriority(TIM5_IRQn, 5, 0);  /* 设置中断优先级,抢占优先级 5, 子优先级0*/
        HAL_NVIC_EnableIRQ(TIM5_IRQn);          /* 开启 TIM5 中断 */
    }
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if ((&TIM3_Handler)==htim)
    {
        printf("TIM3 输出....\r\n");
    }
    else if ((&TIM5_Handler)==htim)
    {
        printf("TIM5 输出....\r\n");
    }
}

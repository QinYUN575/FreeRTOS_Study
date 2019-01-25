#include "timer.h"
#include "led.h"
#include "usart.h"

TIM_HandleTypeDef TIM3_Handler; /* ��ʱ����� */
TIM_HandleTypeDef TIM5_Handler; /* ��ʱ����� */

void TIM3_Init(uint16_t arr, uint16_t psc)
{
    TIM3_Handler.Instance = TIM3;                               /* ͨ�ö�ʱ��3 */
    TIM3_Handler.Init.Prescaler = psc;                          /* ��Ƶϵ�� */
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* ���ϼ����� */
    TIM3_Handler.Init.Period = arr;                             /* �Զ���װ��ֵ */
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* ʱ�ӷ�Ƶ���� */
    HAL_TIM_Base_Init(&TIM3_Handler);                           /* ���ò��� */

    HAL_TIM_Base_Start_IT(&TIM3_Handler);                       /* ʹ�ܶ�ʱ���Ͷ�ʱ�������ж� */
}

void TIM5_Init(uint16_t arr, uint16_t psc)
{
    TIM5_Handler.Instance = TIM5;                               /* ͨ�ö�ʱ��3 */
    TIM5_Handler.Init.Prescaler = psc;                          /* ��Ƶϵ�� */
    TIM5_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* ���ϼ����� */
    TIM5_Handler.Init.Period = arr;                             /* �Զ���װ��ֵ */
    TIM5_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* ʱ�ӷ�Ƶ���� */
    HAL_TIM_Base_Init(&TIM5_Handler);                           /* ���ò��� */

    HAL_TIM_Base_Start_IT(&TIM5_Handler);                       /* ʹ�ܶ�ʱ���Ͷ�ʱ�������ж� */
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();            /* ʹ�� TIM3 ʱ�� */
        HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);  /* �����ж����ȼ�,��ռ���ȼ� 4, �����ȼ�0*/
        HAL_NVIC_EnableIRQ(TIM3_IRQn);          /* ���� TIM3 �ж� */
    } else if (htim->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();            /* ʹ�� TIM5 ʱ�� */
        HAL_NVIC_SetPriority(TIM5_IRQn, 5, 0);  /* �����ж����ȼ�,��ռ���ȼ� 5, �����ȼ�0*/
        HAL_NVIC_EnableIRQ(TIM5_IRQn);          /* ���� TIM5 �ж� */
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
        printf("TIM3 ���....\r\n");
    }
    else if ((&TIM5_Handler)==htim)
    {
        printf("TIM5 ���....\r\n");
    }
}

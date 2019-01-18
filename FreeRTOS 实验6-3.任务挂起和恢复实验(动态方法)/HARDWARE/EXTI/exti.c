#include "sys.h"
#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"
#include "key.h"
#include "usart.h"
#include "led.h"

void EXTI_Init(void)
{
    /*
        KEY0    - PD10
        KEY1    - PD9
        KEY2    - PD8
        WK_UP   - PC13
    */
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOD_CLK_ENABLE();               /* 开启GPIOD时钟 */

    GPIO_Initure.Pin=GPIO_PIN_10;               /* PD10 */
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     /* 下降沿触发 GPIO_MODE_IT_FALLING */
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0); /* 抢占优先级为 6 ，子优先级为 0 */
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         /* 使能中断线 10 */
}

extern TaskHandle_t Task2Task_Handler;

void EXTI15_10_IRQHandler(void)
{
    printf("EXTI15_10_IRQHandler\r\n");
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);  /* 调用中断处理公用函数 */
}

/**
 * @brief   中断服务程序中需要做的事情,在HAL库中所有的外部中断服务函数都会调用此函数
 *
 * @param   GPIO_Pin    中断引脚号
 *
 * @return  void
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

    BaseType_t YieldRequired;

    delay_xms(50);  /* 消抖 */
    switch(GPIO_Pin)
    {
        case  GPIO_PIN_10:
            if( KEY0 == 0)
            {
                printf("KEY0 == 0\r\n");
                YieldRequired = xTaskResumeFromISR(Task2Task_Handler);  /* 恢复任务 2 */

                if (YieldRequired == pdTRUE)
                {
                    printf("Resume Task2\r\n");
                    /**
                    * 如果函数 xTaskResumeFromISR()返回值为 pdTRUE，那么说明要恢复的这个
                    * 任务的任务优先级等于或者高于正在运行的任务(被中断打断的任务),所以在
                    * 退出中断的时候一定要进行上下文切换！
                    */
                    portYIELD_FROM_ISR(YieldRequired);                  /* 上下文切换 */
                }
                LED_R_TogglePin();
                printf("LED_R_TogglePin\r\n");
            }
            break;
        default:
            break;
    }
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);                      /* 清除中断标志位 */

}

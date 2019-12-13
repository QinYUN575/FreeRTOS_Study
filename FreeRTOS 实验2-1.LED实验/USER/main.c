#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

//任务优先级
#define START_TASK_PRIO 1
//任务堆栈大小
#define START_STK_SIZE 128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO 2
//任务堆栈大小
#define LED0_STK_SIZE 50
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO 3
//任务堆栈大小
#define LED1_STK_SIZE 50
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);

//任务优先级
#define FLOAT_TASK_PRIO 4
//任务堆栈大小
#define FLOAT_STK_SIZE 128
//任务句柄
TaskHandle_t FLOATTask_Handler;
//任务函数
void float_task(void *pvParameters);

int main(void)
{

    HAL_Init(); //初始化HAL库
    SystemClock_Config();
    delay_init(80);    //初始化延时函数
    LED_Init();        //初始化LED
    uart_init(115200); //初始化串口

    printf("\r\n===========================================================\r\n");
    printf("\r\n");
    printf("    ***** FreeRTOS V10.0.1 *****\r\n");
    printf("    Compile Time:%s, %s\r\n", __TIME__, __DATE__);
    printf("    Author by QinYUN575\r\n");
    printf("\r\n");
    printf("===========================================================\r\n");

    //创建开始任务
    xTaskCreate((TaskFunction_t)start_task,          //任务函数
                (const char *)"start_task",          //任务名称
                (uint16_t)START_STK_SIZE,            //任务堆栈大小
                (void *)NULL,                        //传递给任务函数的参数
                (UBaseType_t)START_TASK_PRIO,        //任务优先级
                (TaskHandle_t *)&StartTask_Handler); //任务句柄
    vTaskStartScheduler();                           //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t)led0_task,
                (const char *)"led0_task",
                (uint16_t)LED0_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED0_TASK_PRIO,
                (TaskHandle_t *)&LED0Task_Handler);
    //创建LED1任务
    xTaskCreate((TaskFunction_t)led1_task,
                (const char *)"led1_task",
                (uint16_t)LED1_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED1_TASK_PRIO,
                (TaskHandle_t *)&LED1Task_Handler);
    //浮点测试任务
    xTaskCreate((TaskFunction_t)float_task,
                (const char *)"float_task",
                (uint16_t)FLOAT_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)FLOAT_TASK_PRIO,
                (TaskHandle_t *)&FLOATTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数
void led0_task(void *pvParameters)
{
    while (1)
    {
        LED_R_TogglePin();
        vTaskDelay(500);
    }
}

//LED1任务函数
void led1_task(void *pvParameters)
{
    while (1)
    {
        LED_G_TogglePin();
        vTaskDelay(200);
        LED_G_TogglePin();
        vTaskDelay(800);
    }
}

//浮点测试任务
void float_task(void *pvParameters)
{
    static float float_num = 0.00;
    while (1)
    {
        float_num += 0.001f;
        printf("float_num: %.4f\r\n", float_num);
        vTaskDelay(1000);
    }
}

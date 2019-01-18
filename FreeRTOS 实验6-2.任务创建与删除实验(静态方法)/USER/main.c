#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"

/********************************************************/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************/
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE]; /* 空闲任务任务堆栈 */
static StaticTask_t IdleTaskTCB;                            /* 空闲任务控制块 */

static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];    /* 定时器服务任务堆栈 */
static StaticTask_t TimerTaskTCB;                                   /* 定时器服务任务控制块 */

/**
 *
 * 获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
 * 静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该
 * 有用户来提供， FreeRTOS 提供了接口函数 vApplicationGetIdleTaskMemory()
 * 实现此函数即可
 * @Param ppxIdleTaskTCBBuffer: 任务控制块内存
 *
 * @Param ppxIdleTaskStackBuffer: 任务堆栈内存
 *
 * @Param pulIdleTaskStackSize: 任务堆栈大小
 *
 */
void vApplicationGetIdleTaskMemory(
    StaticTask_t **ppxIdleTaskTCBBuffer,
    StackType_t **ppxIdleTaskStackBuffer,
    uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(
    StaticTask_t **ppxTimerTaskTCBBuffer,
    StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer = TimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/********************************************************/
#define START_TASK_PRIO         1               /* 任务优先级 */
#define START_STK_SIZE          128             /* 任务堆栈大小 */
StackType_t StartTaskStack[START_STK_SIZE];     /* 任务堆栈 */
StaticTask_t StackTaskTCB;                      /* 任务控制块 */
TaskHandle_t StartTask_Handler;                 /* 任务句柄 */
void start_task(void *pvParameters);            /* 任务函数 */

/********************************************************/
#define TASK1_TASK_PRIO         2               /* 任务优先级 */
#define TASK1_STK_SIZE          128             /* 任务堆栈大小 */
StackType_t  Task1TaskStack[TASK1_STK_SIZE];    /* 任务堆栈 */
StaticTask_t Task1kTaskTCB;                     /* 任务控制块 */
TaskHandle_t Task1Task_Handler;                 /* 任务句柄 */
void task1_task(void *pvParameters);            /* 任务函数 */

/********************************************************/
#define TASK2_TASK_PRIO         3               /* 任务优先级 */
#define TASK2_STK_SIZE          128             /* 任务堆栈大小 */
StackType_t  Task2TaskStack[TASK2_STK_SIZE];    /* 任务堆栈 */
StaticTask_t Task2kTaskTCB;                     /* 任务控制块 */
TaskHandle_t Task2Task_Handler;                 /* 任务句柄 */
void task2_task(void *pvParamters);             /* 任务函数 */


/********************************************************/
//LCD刷屏时使用的颜色
int lcd_discolor[13]={  WHITE,  BLUE,   BRED,   GRED,   GBLUE,  RED,    MAGENTA,
                        GREEN,  CYAN,   YELLOW, BROWN,  BRRED,  GRAY };

/********************************************************/
int main(void)
{
    HAL_Init();                     /* 初始化HAL库 */
    SystemClock_Config();           /* 设置时钟, 80Mhz */
    delay_init(80);                 /* 初始化延时函数 */
    LED_Init();                     /* 初始化LED */
    uart_init(115200);              /* 初始化串口 */
    LCD_Init();                     /* 初始化LCD */

    POINT_COLOR = RED;
    LCD_ShowString(0,  0, 240, 32, 32, "Pandora STM32F4");
    LCD_ShowString(5, 32, 240, 24, 24, "FreeRTOS @" tskKERNEL_VERSION_NUMBER);
    LCD_ShowString(5, 60, 240, 16, 16, "Task Creat and Del");

    POINT_COLOR = BLUE;
    LCD_ShowString(5, 76, 240, 16, 16, "QinYUN575");
    LCD_ShowString(5, 92, 240, 16, 16, __TIME__ "@" __DATE__);
    POINT_COLOR = RED;

    printf("\r\n===========================================================\r\n");
    printf("\r\n");
    printf("    ***** FreeRTOS V10.0.1 *****\r\n");
    printf("    Compile Time:%s, %s\r\n", __TIME__, __DATE__);
    printf("    Author by QinYUN575\r\n");
    printf("\r\n");
    printf("===========================================================\r\n");

    // /* 创建开始任务 */
    // xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
    //             (const char*    )"start_task",          /* 任务名称 */
    //             (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
    //             (void*          )NULL,                  /* 传递给任务函数的参数 */
    //             (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
    //             (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */

    StartTask_Handler = xTaskCreateStatic((TaskFunction_t   )start_task,        /* 任务函数 */
                                            (const char*    )"start_task",      /* 任务名称 */
                                            (uint32_t       )START_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*          )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t    )START_TASK_PRIO,   /* 任务优先级 */
                                            (StackType_t*   )StartTaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*  )&StackTaskTCB);    /* 任务控制块 */


    vTaskStartScheduler(); /* 开启任务调度 */
}

/**
 * Start_task 任务函数
 *
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */

    // xTaskCreate((TaskFunction_t )task1_task,            /* 任务函数 */
    //             (const char*    )"task1_task",          /* 任务名称 */
    //             (uint16_t       )TASK1_STK_SIZE,        /* 任务堆栈大小 */
    //             (void*          )NULL,                  /* 传递给任务函数的参数 */
    //             (UBaseType_t    )TASK1_TASK_PRIO,       /* 任务优先级 */
    //             (TaskHandle_t*  )&Task1Task_Handler);    /* 任务句柄 */

    // xTaskCreate((TaskFunction_t )task2_task,            /* 任务函数 */
    //             (const char*    )"task2_task",          /* 任务名称 */
    //             (uint16_t       )TASK2_STK_SIZE,        /* 任务堆栈大小 */
    //             (void*          )NULL,                  /* 传递给任务函数的参数 */
    //             (UBaseType_t    )TASK2_TASK_PRIO,       /* 任务优先级 */
    //             (TaskHandle_t*  )&Task2Task_Handler);    /* 任务句柄 */

    Task1Task_Handler = xTaskCreateStatic(  (TaskFunction_t )task1_task,        /* 任务函数 */
                                            (const char*    )"task1_task",      /* 任务名称 */
                                            (uint32_t       )TASK1_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*          )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t    )TASK1_TASK_PRIO,   /* 任务优先级 */
                                            (StackType_t*   )Task1TaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*  )&Task1kTaskTCB);   /* 任务控制块 */

    Task2Task_Handler = xTaskCreateStatic(  (TaskFunction_t )task2_task,        /* 任务函数 */
                                            (const char*    )"task2_task",      /* 任务名称 */
                                            (uint32_t       )TASK2_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*          )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t    )TASK2_TASK_PRIO,   /* 任务优先级 */
                                            (StackType_t*   )Task2TaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*  )&Task2kTaskTCB);   /* 任务控制块 */

    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}

/**
 * Taks1_task 任务函数
 *
 */
void task1_task(void *pvParamters)
{
    uint8_t task1_count = 0;
    printf("Task1TaksHandler:%0X\r\n", (uint32_t)Task1Task_Handler);
    POINT_COLOR = BLUE;
    LCD_ShowString(5, 120, 115, 12, 12, "Task1 Run:000");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(5, 140, 95, 230);
    while (1)
    {
        printf("Task1 run count:%d\r\n", task1_count);
        LED_B_TogglePin();
        task1_count++;
        if (task1_count == 5)
        {
            printf("task1_count = %d, Task delete 'Task2'\r\n", task1_count);
            if (Task2Task_Handler != NULL)
            {
                vTaskDelete(Task2Task_Handler);
                Task2Task_Handler = NULL;
                printf("Task delete 'Task2'\r\n");
                POINT_COLOR = BLACK;
                LCD_DrawLine(125, 140, 215, 230);
                LCD_DrawLine(215, 140, 125, 230);
            }
            else
            {
                printf("Task2TaksHandler:%0X", (uint32_t)Task2Task_Handler);
            }
        }
        LCD_Fill(6, 141, 94, 229, lcd_discolor[task1_count%13]);
        LCD_ShowNum(65, 120, task1_count, 3, 12);
        vTaskDelay(800);
    }
}


/**
 * Taks2_task 任务函数
 *
 */
void task2_task(void *pvParamters)
{
    uint8_t task2_count = 0;
    printf("Task2TaksHandler:%0X\r\n", (uint32_t)Task2Task_Handler);

    POINT_COLOR = BLUE;
    LCD_ShowString(125, 120, 215, 12, 12, "Task2 Run:000");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(125, 140, 215, 230);
    while (1)
    {
        printf("Task2 run count:%d\r\n", task2_count++);
        LED_G_TogglePin();
        LCD_Fill(126, 141, 214, 229, lcd_discolor[task2_count%13]);
        LCD_ShowNum(185, 120, task2_count, 3, 12);
        vTaskDelay(500);
    }
}




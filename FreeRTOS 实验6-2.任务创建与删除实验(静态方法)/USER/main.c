#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"

/********************************************************/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************/
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE]; /* �������������ջ */
static StaticTask_t IdleTaskTCB;                            /* ����������ƿ� */

static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];    /* ��ʱ�����������ջ */
static StaticTask_t TimerTaskTCB;                                   /* ��ʱ������������ƿ� */

/**
 *
 * ��ȡ��������������ջ��������ƿ��ڴ棬��Ϊ������ʹ�õ�
 * ��̬�ڴ棬��˿�������������ջ��������ƿ���ڴ��Ӧ��
 * ���û����ṩ�� FreeRTOS �ṩ�˽ӿں��� vApplicationGetIdleTaskMemory()
 * ʵ�ִ˺�������
 * @Param ppxIdleTaskTCBBuffer: ������ƿ��ڴ�
 *
 * @Param ppxIdleTaskStackBuffer: �����ջ�ڴ�
 *
 * @Param pulIdleTaskStackSize: �����ջ��С
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
#define START_TASK_PRIO         1               /* �������ȼ� */
#define START_STK_SIZE          128             /* �����ջ��С */
StackType_t StartTaskStack[START_STK_SIZE];     /* �����ջ */
StaticTask_t StackTaskTCB;                      /* ������ƿ� */
TaskHandle_t StartTask_Handler;                 /* ������ */
void start_task(void *pvParameters);            /* ������ */

/********************************************************/
#define TASK1_TASK_PRIO         2               /* �������ȼ� */
#define TASK1_STK_SIZE          128             /* �����ջ��С */
StackType_t  Task1TaskStack[TASK1_STK_SIZE];    /* �����ջ */
StaticTask_t Task1kTaskTCB;                     /* ������ƿ� */
TaskHandle_t Task1Task_Handler;                 /* ������ */
void task1_task(void *pvParameters);            /* ������ */

/********************************************************/
#define TASK2_TASK_PRIO         3               /* �������ȼ� */
#define TASK2_STK_SIZE          128             /* �����ջ��С */
StackType_t  Task2TaskStack[TASK2_STK_SIZE];    /* �����ջ */
StaticTask_t Task2kTaskTCB;                     /* ������ƿ� */
TaskHandle_t Task2Task_Handler;                 /* ������ */
void task2_task(void *pvParamters);             /* ������ */


/********************************************************/
//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[13]={  WHITE,  BLUE,   BRED,   GRED,   GBLUE,  RED,    MAGENTA,
                        GREEN,  CYAN,   YELLOW, BROWN,  BRRED,  GRAY };

/********************************************************/
int main(void)
{
    HAL_Init();                     /* ��ʼ��HAL�� */
    SystemClock_Config();           /* ����ʱ��, 80Mhz */
    delay_init(80);                 /* ��ʼ����ʱ���� */
    LED_Init();                     /* ��ʼ��LED */
    uart_init(115200);              /* ��ʼ������ */
    LCD_Init();                     /* ��ʼ��LCD */

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

    // /* ������ʼ���� */
    // xTaskCreate((TaskFunction_t )start_task,            /* ������ */
    //             (const char*    )"start_task",          /* �������� */
    //             (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
    //             (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
    //             (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
    //             (TaskHandle_t*  )&StartTask_Handler);   /* ������ */

    StartTask_Handler = xTaskCreateStatic((TaskFunction_t   )start_task,        /* ������ */
                                            (const char*    )"start_task",      /* �������� */
                                            (uint32_t       )START_STK_SIZE,    /* �����ջ��С */
                                            (void*          )NULL,              /* ���ݸ��������Ĳ��� */
                                            (UBaseType_t    )START_TASK_PRIO,   /* �������ȼ� */
                                            (StackType_t*   )StartTaskStack,    /* �����ջ */
                                            (StaticTask_t*  )&StackTaskTCB);    /* ������ƿ� */


    vTaskStartScheduler(); /* ����������� */
}

/**
 * Start_task ������
 *
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */

    // xTaskCreate((TaskFunction_t )task1_task,            /* ������ */
    //             (const char*    )"task1_task",          /* �������� */
    //             (uint16_t       )TASK1_STK_SIZE,        /* �����ջ��С */
    //             (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
    //             (UBaseType_t    )TASK1_TASK_PRIO,       /* �������ȼ� */
    //             (TaskHandle_t*  )&Task1Task_Handler);    /* ������ */

    // xTaskCreate((TaskFunction_t )task2_task,            /* ������ */
    //             (const char*    )"task2_task",          /* �������� */
    //             (uint16_t       )TASK2_STK_SIZE,        /* �����ջ��С */
    //             (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
    //             (UBaseType_t    )TASK2_TASK_PRIO,       /* �������ȼ� */
    //             (TaskHandle_t*  )&Task2Task_Handler);    /* ������ */

    Task1Task_Handler = xTaskCreateStatic(  (TaskFunction_t )task1_task,        /* ������ */
                                            (const char*    )"task1_task",      /* �������� */
                                            (uint32_t       )TASK1_STK_SIZE,    /* �����ջ��С */
                                            (void*          )NULL,              /* ���ݸ��������Ĳ��� */
                                            (UBaseType_t    )TASK1_TASK_PRIO,   /* �������ȼ� */
                                            (StackType_t*   )Task1TaskStack,    /* �����ջ */
                                            (StaticTask_t*  )&Task1kTaskTCB);   /* ������ƿ� */

    Task2Task_Handler = xTaskCreateStatic(  (TaskFunction_t )task2_task,        /* ������ */
                                            (const char*    )"task2_task",      /* �������� */
                                            (uint32_t       )TASK2_STK_SIZE,    /* �����ջ��С */
                                            (void*          )NULL,              /* ���ݸ��������Ĳ��� */
                                            (UBaseType_t    )TASK2_TASK_PRIO,   /* �������ȼ� */
                                            (StackType_t*   )Task2TaskStack,    /* �����ջ */
                                            (StaticTask_t*  )&Task2kTaskTCB);   /* ������ƿ� */

    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

/**
 * Taks1_task ������
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
 * Taks2_task ������
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




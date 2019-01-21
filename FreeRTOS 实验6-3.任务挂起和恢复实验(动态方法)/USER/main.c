#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"

/********************************************************/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************/
#define START_TASK_PRIO         1       /* �������ȼ� */
#define START_STK_SIZE          128     /* �����ջ��С */
TaskHandle_t StartTask_Handler;         /* ������ */
void start_task(void *pvParameters);    /* ������ */

/********************************************************/
#define KEY_TASK_PRIO           2       /* �������ȼ� */
#define KEY_STK_SIZE            128     /* �����ջ��С */
TaskHandle_t KeyTask_Handler;           /* ������ */
void key_task(void *pvParamters);       /* ������ */

/********************************************************/
#define TASK1_TASK_PRIO         3       /* �������ȼ� */
#define TASK1_STK_SIZE          128     /* �����ջ��С */
TaskHandle_t Task1Task_Handler;         /* ������ */
void task1_task(void *pvParameters);     /* ������ */

/********************************************************/
#define TASK2_TASK_PRIO         4       /* �������ȼ� */
#define TASK2_STK_SIZE          128     /* �����ջ��С */
TaskHandle_t Task2Task_Handler;         /* ������ */
void task2_task(void *pvParamters);     /* ������ */



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
    KEY_Init();                     /* ��ʼ���������� */
    EXTI_Init();                    /* �ⲿ�жϳ�ʼ�� */
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

    /* ������ʼ���� */
    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */
    vTaskStartScheduler(); /* ����������� */
}

/**
 * Start_task ������
 *
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */

    xTaskCreate((TaskFunction_t )task1_task,            /* ������ */
                (const char*    )"task1_task",          /* �������� */
                (uint16_t       )TASK1_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )TASK1_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&Task1Task_Handler);    /* ������ */

    xTaskCreate((TaskFunction_t )task2_task,            /* ������ */
                (const char*    )"task2_task",          /* �������� */
                (uint16_t       )TASK2_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )TASK2_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&Task2Task_Handler);    /* ������ */

    xTaskCreate((TaskFunction_t )key_task,              /* ������ */
                (const char*    )"key_task",            /* �������� */
                (uint16_t       )KEY_STK_SIZE,          /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )KEY_TASK_PRIO,         /* �������ȼ� */
                (TaskHandle_t*  )&KeyTask_Handler);     /* ������ */

    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}

/**
 * Key ������
 *
 */
void key_task(void *pvParamters)
{
    uint8_t key;
    while (1)
    {
        key = KEY_Scan(0);
        switch (key)
        {
            case WKUP_PRES:
                vTaskSuspend(Task1Task_Handler);
                POINT_COLOR = BLACK;
                LCD_DrawLine(5, 140, 95, 230);
                LCD_DrawLine(95, 140, 5, 230);
                printf("Taks1 Suspend!\r\n");
                break;

            case KEY1_PRES:
                vTaskResume(Task1Task_Handler);
                printf("Taks1 Resume!\r\n");
                break;

            case KEY2_PRES:
                vTaskSuspend(Task2Task_Handler);
                POINT_COLOR = BLACK;
                LCD_DrawLine(125, 140, 215, 230);
                LCD_DrawLine(215, 140, 125, 230);
                printf("Taks2 Suspend!\r\n");
                break;

            default:
                break;
        }
        vTaskDelay(10/portTICK_RATE_MS);
    }
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
        LCD_Fill(6, 141, 94, 229, lcd_discolor[task1_count%13]);
        LCD_ShowNum(65, 120, task1_count, 3, 12);
        // vTaskDelay(800);
        vTaskDelay(800/portTICK_RATE_MS);
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
        // vTaskDelay(500);
        vTaskDelay(500/portTICK_RATE_MS);
    }
}

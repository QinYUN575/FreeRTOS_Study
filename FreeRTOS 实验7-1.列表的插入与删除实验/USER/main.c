#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"

/********************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

/********************************************************/
#define START_TASK_PRIO         1       /* 任务优先级 */
#define START_STK_SIZE          128     /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;         /* 任务句柄 */
void start_task(void *pvParameters);    /* 任务函数 */


/********************************************************/
#define TASK1_TASK_PRIO         2       /* 任务优先级 */
#define TASK1_STK_SIZE          128     /* 任务堆栈大小 */
TaskHandle_t Task1Task_Handler;         /* 任务句柄 */
void task1_task(void *pvParameters);     /* 任务函数 */

/********************************************************/
#define LIST_TASK_PRIO          3       /* 任务优先级 */
#define LIST_STK_SIZE           128     /* 任务堆栈大小 */
TaskHandle_t ListTask_Handler;          /* 任务句柄 */
void list_task(void *pvParamters);      /* 任务函数 */



/********************************************************/
List_t TestList;                        /* 测试用列表 */
ListItem_t listItem1;                   /* 测试用列表项 1 */
ListItem_t listItem2;                   /* 测试用列表项 2 */
ListItem_t listItem3;                   /* 测试用列表项 3 */

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
    KEY_Init();                     /* 初始化按键函数 */
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

    /* 创建开始任务 */
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler(); /* 开启任务调度 */
}

/**
 * Start_task 任务函数
 *
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */

    xTaskCreate((TaskFunction_t )task1_task,            /* 任务函数 */
                (const char*    )"task1_task",          /* 任务名称 */
                (uint16_t       )TASK1_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )TASK1_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&Task1Task_Handler);    /* 任务句柄 */

    xTaskCreate((TaskFunction_t )list_task,             /* 任务函数 */
                (const char*    )"list_task",           /* 任务名称 */
                (uint16_t       )LIST_STK_SIZE,         /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )LIST_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t*  )&ListTask_Handler);    /* 任务句柄 */

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
    // printf("Task1TaksHandler:%0X\r\n", (uint32_t)Task1Task_Handler);
    POINT_COLOR = BLUE;
    LCD_ShowString(5, 120, 115, 12, 12, "Task1 Run:000");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(5, 140, 95, 230);
    while (1)
    {
        // printf("Task1 run count:%d\r\n", task1_count);
        LED_B_TogglePin();
        task1_count++;
        LCD_Fill(6, 141, 94, 229, lcd_discolor[task1_count%13]);
        LCD_ShowNum(65, 120, task1_count, 3, 12);
        // vTaskDelay(800);
        vTaskDelay(800/portTICK_RATE_MS);
    }
}

/**
 * list_task 任务函数
 *
 *
 */
void list_task(void *pvParamters)
{
    /* 第一步: 初始化 列表&&列表项 */
    printf("Step 1:\r\n");
    vListInitialise(&TestList);
    vListInitialiseItem(&listItem1);
    vListInitialiseItem(&listItem2);
    vListInitialiseItem(&listItem3);

    /* 设置列表项数值 */
    listItem1.xItemValue = 40;
    listItem2.xItemValue = 50;
    listItem3.xItemValue = 60;

    /* print list && listitem memory address. */
    /* 第二步: 打印列表和其他列表项的地址*/
    printf("Step 2:\r\n");
    printf("/****************** List and ListItem Address ******************/\r\n");
    printf("Item                            Address \r\n");
    printf("TestList                        %#X     \r\n", (int)&TestList);
    printf("TestList->uxNumberOfItems       %2d     \r\n", (TestList.uxNumberOfItems));
    printf("TestList->pxIndex               %#X     \r\n", (int)&TestList.pxIndex);
    printf("TestList->xListEnd              %#X     \r\n", (int)&(TestList.xListEnd));
    printf("listItem1                       %#X     \r\n", (int)&listItem1);
    printf("listItem2                       %#X     \r\n", (int)&listItem2);
    printf("listItem3                       %#X     \r\n", (int)&listItem3);
    printf("/******************           END         **********************/\r\n");
    printf("\r\n");

    printf("Press the KEY_UP to continue.\r\n");
    while (KEY_Scan(0)!= WKUP_PRES) delay_ms(10);
    printf("\r\n");

    /* print list && listitem memory address. */
    /**
     * 第三步: 向列表 TestList 添加列表项 listItem1, 并通过串口打印所有列表项中的
     * 成员 pxNext 和 pxPrevious 的值,通过这两个值观察列表项在列表中的连接情况.
     */
    printf("Step 3:\r\n");
    vListInsert(&TestList, &listItem1);
    printf("/****************** add ListItem listItem1 ******************/\r\n");
    printf("Item                            Address \r\n");
    printf("TestList->uxNumberOfItems       %2d     \r\n", TestList.uxNumberOfItems);
    printf("TestList->pxIndex->pxNext       %#X     \r\n", (int)(TestList.xListEnd.pxNext));
    printf("listItem1->pxNext               %#X     \r\n", (int)(listItem1.pxNext));
    printf("\r\n");
    printf("TestList.xListEnd.pxPrevious    %#X     \r\n", (int)(TestList.xListEnd.pxPrevious));
    printf("listItem1.pxPrevious            %#X     \r\n", (int)(listItem1.pxPrevious));
    printf("/******************           END         **********************/\r\n");
    printf("\r\n");

    printf("Press the KEY_UP to continue.\r\n");
    while (KEY_Scan(0)!= WKUP_PRES) delay_ms(10);
    printf("\r\n");

    /* print list && listitem memory address. */
    /**
     * 第四步: 向列表 TestList 添加列表项 listItem2, 并通过串口打印所有列表项中的
     * 成员 pxNext 和 pxPrevious 的值,通过这两个值观察列表项在列表中的连接情况.
     */
    printf("Step 4:\r\n");
    vListInsert(&TestList, &listItem2);
    printf("/****************** add ListItem listItem2 ******************/\r\n");
    printf("Item                            Address \r\n");
    printf("TestList->uxNumberOfItems       %2d     \r\n", (TestList.uxNumberOfItems));
    printf("TestList->pxIndex->pxNext       %#X     \r\n", (int)(TestList.xListEnd.pxNext));
    printf("listItem1->pxNext               %#X     \r\n", (int)(listItem1.pxNext));
    printf("listItem2->pxNext               %#X     \r\n", (int)(listItem2.pxNext));
    printf("\r\n");
    printf("TestList.xListEnd.pxPrevious    %#X     \r\n", (int)(TestList.xListEnd.pxPrevious));
    printf("listItem1.pxPrevious            %#X     \r\n", (int)(listItem1.pxPrevious));
    printf("listItem2.pxPrevious            %#X     \r\n", (int)(listItem2.pxPrevious));
    printf("/******************           END         **********************/\r\n");
    printf("\r\n");

    printf("Press the KEY_UP to continue.\r\n");
    while (KEY_Scan(0)!= WKUP_PRES) delay_ms(10);
    printf("\r\n");

    /* print list && listitem memory address. */
    /**
     * 第五步: 向列表 TestList 添加列表项 listItem3, 并通过串口打印所有列表项中的
     * 成员 pxNext 和 pxPrevious 的值,通过这两个值观察列表项在列表中的连接情况.
     */
    printf("Step 5:\r\n");
    vListInsert(&TestList, &listItem3);
    printf("/****************** add ListItem listItem3 ******************/\r\n");
    printf("Item                            Address \r\n");
    printf("TestList->uxNumberOfItems       %2d     \r\n", (TestList.uxNumberOfItems));
    printf("TestList->pxIndex->pxNext       %#X     \r\n", (int)(TestList.xListEnd.pxNext));
    printf("listItem1->pxNext               %#X     \r\n", (int)listItem1.pxNext);
    printf("listItem2->pxNext               %#X     \r\n", (int)listItem2.pxNext);
    printf("listItem3->pxNext               %#X     \r\n", (int)listItem3.pxNext);
    printf("\r\n");
    printf("TestList.xListEnd.pxPrevious    %#X     \r\n", (int)(TestList.xListEnd.pxPrevious));
    printf("listItem1.pxPrevious            %#X     \r\n", (int)(listItem1.pxPrevious));
    printf("listItem2.pxPrevious            %#X     \r\n", (int)(listItem2.pxPrevious));
    printf("listItem3.pxPrevious            %#X     \r\n", (int)(listItem3.pxPrevious));
    printf("/******************           END         **********************/\r\n");
    printf("\r\n");

    printf("Press the KEY_UP to continue.\r\n");
    while (KEY_Scan(0)!= WKUP_PRES) delay_ms(10);
    printf("\r\n");

    /* print list && listitem memory address. */
    /**
     * 第六步: 向列表 TestList 删除列表项 listItem2, 并通过串口打印所有列表项中的
     * 成员 pxNext 和 pxPrevious 的值,通过这两个值观察列表项在列表中的连接情况.
     */
    printf("Step 6:\r\n");
    uxListRemove(&listItem2);
    printf("/**************** remove ListItem listItem2 ***************/\r\n");
    printf("Item                            Address \r\n");
    printf("TestList->uxNumberOfItems       %2d     \r\n", TestList.uxNumberOfItems);
    printf("TestList->pxIndex->pxNext       %#X     \r\n", (int)&TestList.xListEnd.pxNext);
    printf("listItem1->pxNext               %#X     \r\n", (int)&listItem1.pxNext);
    printf("listItem3->pxNext               %#X     \r\n", (int)&listItem3.pxNext);
    printf("\r\n");
    printf("TestList.xListEnd.pxPrevious    %#X     \r\n", (int)&TestList.xListEnd.pxPrevious);
    printf("listItem1.pxPrevious            %#X     \r\n", (int)&listItem1.pxPrevious);
    printf("listItem3.pxPrevious            %#X     \r\n", (int)&listItem3.pxPrevious);
    printf("/******************           END         **********************/\r\n");
    printf("\r\n");

    printf("Press the KEY_UP to continue.\r\n");
    while (KEY_Scan(0)!= WKUP_PRES) delay_ms(10);
    printf("\r\n");

    /* print list && listitem memory address. */
    /**
     * 第七步: 向列表 TestList 删除列表项 listItem2, 并通过串口打印所有列表项中的
     * 成员 pxNext 和 pxPrevious 的值,通过这两个值观察列表项在列表中的连接情况.
     */
    printf("Step 7:\r\n");
     /**
      * pxIndex 向后移动一项, 这样 pxIndex 就会指向 listItem1 */
    TestList.pxIndex = TestList.pxIndex->pxNext;
    vListInsertEnd(&TestList, &listItem2);
    printf("/****************** add ListItem listItem2 ******************/\r\n");
    printf("Item                            Address \r\n");
    printf("TestList->uxNumberOfItems       %2d     \r\n", TestList.uxNumberOfItems);
    printf("TestList->pxIndex->pxNext       %#X     \r\n", (int)&TestList.xListEnd.pxNext);
    printf("listItem1->pxNext               %#X     \r\n", (int)&listItem1.pxNext);
    printf("listItem2.pxPrevious            %#X     \r\n", (int)&listItem2.pxPrevious);
    printf("listItem3->pxNext               %#X     \r\n", (int)&listItem3.pxNext);
    printf("\r\n");
    printf("TestList.xListEnd.pxPrevious    %#X     \r\n", (int)&TestList.xListEnd.pxPrevious);
    printf("listItem1.pxPrevious            %#X     \r\n", (int)&listItem1.pxPrevious);
    printf("listItem2.pxPrevious            %#X     \r\n", (int)&listItem2.pxPrevious);
    printf("listItem3.pxPrevious            %#X     \r\n", (int)&listItem3.pxPrevious);
    printf("/******************           END         **********************/\r\n");
    printf("\r\n");

    printf("Press the KEY_UP to continue.\r\n");
    while (KEY_Scan(0)!= WKUP_PRES) delay_ms(10);
    printf("\r\n");


    while (1)
    {
        LED_R_TogglePin();
        vTaskDelay(1000/portTICK_RATE_MS);
    }
}

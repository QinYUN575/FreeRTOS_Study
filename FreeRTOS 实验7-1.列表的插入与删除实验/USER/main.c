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
#define START_TASK_PRIO         1       /* �������ȼ� */
#define START_STK_SIZE          128     /* �����ջ��С */
TaskHandle_t StartTask_Handler;         /* ������ */
void start_task(void *pvParameters);    /* ������ */


/********************************************************/
#define TASK1_TASK_PRIO         2       /* �������ȼ� */
#define TASK1_STK_SIZE          128     /* �����ջ��С */
TaskHandle_t Task1Task_Handler;         /* ������ */
void task1_task(void *pvParameters);     /* ������ */

/********************************************************/
#define LIST_TASK_PRIO          3       /* �������ȼ� */
#define LIST_STK_SIZE           128     /* �����ջ��С */
TaskHandle_t ListTask_Handler;          /* ������ */
void list_task(void *pvParamters);      /* ������ */



/********************************************************/
List_t TestList;                        /* �������б� */
ListItem_t listItem1;                   /* �������б��� 1 */
ListItem_t listItem2;                   /* �������б��� 2 */
ListItem_t listItem3;                   /* �������б��� 3 */

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

    xTaskCreate((TaskFunction_t )list_task,             /* ������ */
                (const char*    )"list_task",           /* �������� */
                (uint16_t       )LIST_STK_SIZE,         /* �����ջ��С */
                (void*          )NULL,                  /* ���ݸ��������Ĳ��� */
                (UBaseType_t    )LIST_TASK_PRIO,        /* �������ȼ� */
                (TaskHandle_t*  )&ListTask_Handler);    /* ������ */

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
 * list_task ������
 *
 *
 */
void list_task(void *pvParamters)
{
    /* ��һ��: ��ʼ�� �б�&&�б��� */
    printf("Step 1:\r\n");
    vListInitialise(&TestList);
    vListInitialiseItem(&listItem1);
    vListInitialiseItem(&listItem2);
    vListInitialiseItem(&listItem3);

    /* �����б�����ֵ */
    listItem1.xItemValue = 40;
    listItem2.xItemValue = 50;
    listItem3.xItemValue = 60;

    /* print list && listitem memory address. */
    /* �ڶ���: ��ӡ�б�������б���ĵ�ַ*/
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
     * ������: ���б� TestList ����б��� listItem1, ��ͨ�����ڴ�ӡ�����б����е�
     * ��Ա pxNext �� pxPrevious ��ֵ,ͨ��������ֵ�۲��б������б��е��������.
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
     * ���Ĳ�: ���б� TestList ����б��� listItem2, ��ͨ�����ڴ�ӡ�����б����е�
     * ��Ա pxNext �� pxPrevious ��ֵ,ͨ��������ֵ�۲��б������б��е��������.
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
     * ���岽: ���б� TestList ����б��� listItem3, ��ͨ�����ڴ�ӡ�����б����е�
     * ��Ա pxNext �� pxPrevious ��ֵ,ͨ��������ֵ�۲��б������б��е��������.
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
     * ������: ���б� TestList ɾ���б��� listItem2, ��ͨ�����ڴ�ӡ�����б����е�
     * ��Ա pxNext �� pxPrevious ��ֵ,ͨ��������ֵ�۲��б������б��е��������.
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
     * ���߲�: ���б� TestList ɾ���б��� listItem2, ��ͨ�����ڴ�ӡ�����б����е�
     * ��Ա pxNext �� pxPrevious ��ֵ,ͨ��������ֵ�۲��б������б��е��������.
     */
    printf("Step 7:\r\n");
     /**
      * pxIndex ����ƶ�һ��, ���� pxIndex �ͻ�ָ�� listItem1 */
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

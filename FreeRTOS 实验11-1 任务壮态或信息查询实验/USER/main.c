#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include <string.h>
/********************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "list.h"

/********************************************************/
#define START_TASK_PRIO 1            /* �������ȼ� */
#define START_STK_SIZE 128           /* �����ջ��С */
TaskHandle_t StartTask_Handler;      /* ������ */
void start_task(void *pvParameters); /* ������ */

/********************************************************/
#define LED0_TASK_PRIO 2            /* �������ȼ� */
#define LED0_STK_SIZE 128           /* �����ջ��С */
TaskHandle_t Led0Task_Handler;      /* ������ */
void led0_task(void *pvParameters); /* ������ */

/********************************************************/
#define QUERY_TASK_PRIO 3           /* �������ȼ� */
#define QUERY_STK_SIZE 256          /* �����ջ��С */
TaskHandle_t QueryTask_Handler;     /* ������ */
void query_task(void *pvParamters); /* ������ */

/********************************************************/
char InfoBuffer[1000]; /* ������Ϣ������ */

/********************************************************/
int main(void)
{
    HAL_Init();           /* ��ʼ��HAL�� */
    SystemClock_Config(); /* ����ʱ��, 80Mhz */
    delay_init(80);       /* ��ʼ����ʱ���� */
    LED_Init();           /* ��ʼ��LED */
    uart_init(115200);    /* ��ʼ������ */
    KEY_Init();           /* ��ʼ���������� */
                          //	SDRAM_Init();					//��ʼ��SDRAM
    LCD_Init();           /* ��ʼ��LCD */

    POINT_COLOR = RED;
    LCD_ShowString(0, 0, 240, 32, 32, "Pandora STM32F4");
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
    xTaskCreate((TaskFunction_t)start_task,          /* ������ */
                (const char *)"start_task",          /* �������� */
                (uint16_t)START_STK_SIZE,            /* �����ջ��С */
                (void *)NULL,                        /* ���ݸ��������Ĳ��� */
                (UBaseType_t)START_TASK_PRIO,        /* �������ȼ� */
                (TaskHandle_t *)&StartTask_Handler); /* ������ */
    vTaskStartScheduler();                           /* ����������� */
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //�����ٽ���
    //����LED0����
    xTaskCreate((TaskFunction_t)led0_task,
                (const char *)"led0_task",
                (uint16_t)LED0_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED0_TASK_PRIO,
                (TaskHandle_t *)&Led0Task_Handler);
    //����QUERY����
    xTaskCreate((TaskFunction_t)query_task,
                (const char *)"query_task",
                (uint16_t)QUERY_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)QUERY_TASK_PRIO,
                (TaskHandle_t *)&QueryTask_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//led0������
void led0_task(void *pvParameters)
{

    while (1)
    {
        LED_B_TogglePin();
        vTaskDelay(500); //��ʱ500ms��Ҳ����500��ʱ�ӽ���
    }
}

//query������
void query_task(void *pvParameters)
{
    u32 TotalRunTime;
    UBaseType_t ArraySize, x;
    TaskStatus_t *StatusArray;

    //��һ��������uxTaskGetSystemState()��ʹ��
    printf("/********��һ��������uxTaskGetSystemState()��ʹ��**********/\r\n");
    ArraySize = uxTaskGetNumberOfTasks();                         //��ȡϵͳ��������
    StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t)); //�����ڴ�
    if (StatusArray != NULL)                                      //�ڴ�����ɹ�
    {
        ArraySize = uxTaskGetSystemState((TaskStatus_t *)StatusArray, //������Ϣ�洢����
                                         (UBaseType_t)ArraySize,      //������Ϣ�洢�����С
                                         (uint32_t *)&TotalRunTime);  //����ϵͳ�ܵ�����ʱ��
        printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
        for (x = 0; x < ArraySize; x++)
        {
            //ͨ�����ڴ�ӡ����ȡ����ϵͳ������й���Ϣ�������������ơ�
            //�������ȼ��������š�
            printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",
                   StatusArray[x].pcTaskName,
                   (int)StatusArray[x].uxCurrentPriority,
                   (int)StatusArray[x].xTaskNumber);
        }
    }
    vPortFree(StatusArray); //�ͷ��ڴ�
    printf("/**************************����***************************/\r\n");
    printf("����KEY_UP������!\r\n\r\n\r\n");
    while (KEY_Scan(0) != WKUP_PRES)
        delay_ms(10); //�ȴ�KEY_UP������

    //�ڶ���������vTaskGetInfo()��ʹ��
    TaskHandle_t TaskHandle;
    TaskStatus_t TaskStatus;

    printf("/************�ڶ���������vTaskGetInfo()��ʹ��**************/\r\n");
    TaskHandle = xTaskGetHandle("led0_task"); //������������ȡ��������
    //��ȡLED0_Task��������Ϣ
    vTaskGetInfo((TaskHandle_t)TaskHandle,    //������
                 (TaskStatus_t *)&TaskStatus, //������Ϣ�ṹ��
                 (BaseType_t)pdTRUE,          //����ͳ�������ջ��ʷ��Сʣ���С
                 (eTaskState)eInvalid);       //�����Լ���ȡ��������׳̬
    //ͨ�����ڴ�ӡ��ָ��������й���Ϣ��
    printf("������:                %s\r\n", TaskStatus.pcTaskName);
    printf("������:              %d\r\n", (int)TaskStatus.xTaskNumber);
    printf("����׳̬:              %d\r\n", TaskStatus.eCurrentState);
    printf("����ǰ���ȼ�:        %d\r\n", (int)TaskStatus.uxCurrentPriority);
    printf("��������ȼ�:          %d\r\n", (int)TaskStatus.uxBasePriority);
    printf("�����ջ����ַ:        %#x\r\n", (int)TaskStatus.pxStackBase);
    printf("�����ջ��ʷʣ����Сֵ:%d\r\n", TaskStatus.usStackHighWaterMark);
    printf("/**************************����***************************/\r\n");
    printf("����KEY_UP������!\r\n\r\n\r\n");
    while (KEY_Scan(0) != WKUP_PRES)
        delay_ms(10); //�ȴ�KEY_UP������

    //������������eTaskGetState()��ʹ��
    eTaskState TaskState;
    char TaskInfo[10];
    printf("/***********������������eTaskGetState()��ʹ��*************/\r\n");
    TaskHandle = xTaskGetHandle("query_task"); //������������ȡ��������
    TaskState = eTaskGetState(TaskHandle);     //��ȡquery_task���������׳̬
    // memset(TaskInfo, 0, 10);
    switch ((int)TaskState)
    {
    case 0:
        sprintf(TaskInfo, "Running");
        break;
    case 1:
        sprintf(TaskInfo, "Ready");
        break;
    case 2:
        sprintf(TaskInfo, "Suspend");
        break;
    case 3:
        sprintf(TaskInfo, "Delete");
        break;
    case 4:
        sprintf(TaskInfo, "Invalid");
        break;
    }
    printf("����׳ֵ̬:%d,��Ӧ��׳̬Ϊ:%s\r\n", TaskState, TaskInfo);
    printf("/**************************����**************************/\r\n");
    printf("����KEY_UP������!\r\n\r\n\r\n");
    while (KEY_Scan(0) != WKUP_PRES)
        delay_ms(10); //�ȴ�KEY_UP������

    //���Ĳ�������vTaskList()��ʹ��
    printf("/*************������������vTaskList()��ʹ��*************/\r\n");
    vTaskList(InfoBuffer);        //��ȡ�����������Ϣ
    printf("%s\r\n", InfoBuffer); //ͨ�����ڴ�ӡ�����������Ϣ
    printf("/**************************����**************************/\r\n");
    while (1)
    {
        LED_R_TogglePin();
        vTaskDelay(1000); //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}

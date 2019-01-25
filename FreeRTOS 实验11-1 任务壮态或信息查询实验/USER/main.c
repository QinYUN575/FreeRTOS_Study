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
#define START_TASK_PRIO 1            /* 任务优先级 */
#define START_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;      /* 任务句柄 */
void start_task(void *pvParameters); /* 任务函数 */

/********************************************************/
#define LED0_TASK_PRIO 2            /* 任务优先级 */
#define LED0_STK_SIZE 128           /* 任务堆栈大小 */
TaskHandle_t Led0Task_Handler;      /* 任务句柄 */
void led0_task(void *pvParameters); /* 任务函数 */

/********************************************************/
#define QUERY_TASK_PRIO 3           /* 任务优先级 */
#define QUERY_STK_SIZE 256          /* 任务堆栈大小 */
TaskHandle_t QueryTask_Handler;     /* 任务句柄 */
void query_task(void *pvParamters); /* 任务函数 */

/********************************************************/
char InfoBuffer[1000]; /* 保存信息的数组 */

/********************************************************/
int main(void)
{
    HAL_Init();           /* 初始化HAL库 */
    SystemClock_Config(); /* 设置时钟, 80Mhz */
    delay_init(80);       /* 初始化延时函数 */
    LED_Init();           /* 初始化LED */
    uart_init(115200);    /* 初始化串口 */
    KEY_Init();           /* 初始化按键函数 */
                          //	SDRAM_Init();					//初始化SDRAM
    LCD_Init();           /* 初始化LCD */

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

    /* 创建开始任务 */
    xTaskCreate((TaskFunction_t)start_task,          /* 任务函数 */
                (const char *)"start_task",          /* 任务名称 */
                (uint16_t)START_STK_SIZE,            /* 任务堆栈大小 */
                (void *)NULL,                        /* 传递给任务函数的参数 */
                (UBaseType_t)START_TASK_PRIO,        /* 任务优先级 */
                (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */
    vTaskStartScheduler();                           /* 开启任务调度 */
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
                (TaskHandle_t *)&Led0Task_Handler);
    //创建QUERY任务
    xTaskCreate((TaskFunction_t)query_task,
                (const char *)"query_task",
                (uint16_t)QUERY_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)QUERY_TASK_PRIO,
                (TaskHandle_t *)&QueryTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//led0任务函数
void led0_task(void *pvParameters)
{

    while (1)
    {
        LED_B_TogglePin();
        vTaskDelay(500); //延时500ms，也就是500个时钟节拍
    }
}

//query任务函数
void query_task(void *pvParameters)
{
    u32 TotalRunTime;
    UBaseType_t ArraySize, x;
    TaskStatus_t *StatusArray;

    //第一步：函数uxTaskGetSystemState()的使用
    printf("/********第一步：函数uxTaskGetSystemState()的使用**********/\r\n");
    ArraySize = uxTaskGetNumberOfTasks();                         //获取系统任务数量
    StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t)); //申请内存
    if (StatusArray != NULL)                                      //内存申请成功
    {
        ArraySize = uxTaskGetSystemState((TaskStatus_t *)StatusArray, //任务信息存储数组
                                         (UBaseType_t)ArraySize,      //任务信息存储数组大小
                                         (uint32_t *)&TotalRunTime);  //保存系统总的运行时间
        printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
        for (x = 0; x < ArraySize; x++)
        {
            //通过串口打印出获取到的系统任务的有关信息，比如任务名称、
            //任务优先级和任务编号。
            printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",
                   StatusArray[x].pcTaskName,
                   (int)StatusArray[x].uxCurrentPriority,
                   (int)StatusArray[x].xTaskNumber);
        }
    }
    vPortFree(StatusArray); //释放内存
    printf("/**************************结束***************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while (KEY_Scan(0) != WKUP_PRES)
        delay_ms(10); //等待KEY_UP键按下

    //第二步：函数vTaskGetInfo()的使用
    TaskHandle_t TaskHandle;
    TaskStatus_t TaskStatus;

    printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
    TaskHandle = xTaskGetHandle("led0_task"); //根据任务名获取任务句柄。
    //获取LED0_Task的任务信息
    vTaskGetInfo((TaskHandle_t)TaskHandle,    //任务句柄
                 (TaskStatus_t *)&TaskStatus, //任务信息结构体
                 (BaseType_t)pdTRUE,          //允许统计任务堆栈历史最小剩余大小
                 (eTaskState)eInvalid);       //函数自己获取任务运行壮态
    //通过串口打印出指定任务的有关信息。
    printf("任务名:                %s\r\n", TaskStatus.pcTaskName);
    printf("任务编号:              %d\r\n", (int)TaskStatus.xTaskNumber);
    printf("任务壮态:              %d\r\n", TaskStatus.eCurrentState);
    printf("任务当前优先级:        %d\r\n", (int)TaskStatus.uxCurrentPriority);
    printf("任务基优先级:          %d\r\n", (int)TaskStatus.uxBasePriority);
    printf("任务堆栈基地址:        %#x\r\n", (int)TaskStatus.pxStackBase);
    printf("任务堆栈历史剩余最小值:%d\r\n", TaskStatus.usStackHighWaterMark);
    printf("/**************************结束***************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while (KEY_Scan(0) != WKUP_PRES)
        delay_ms(10); //等待KEY_UP键按下

    //第三步：函数eTaskGetState()的使用
    eTaskState TaskState;
    char TaskInfo[10];
    printf("/***********第三步：函数eTaskGetState()的使用*************/\r\n");
    TaskHandle = xTaskGetHandle("query_task"); //根据任务名获取任务句柄。
    TaskState = eTaskGetState(TaskHandle);     //获取query_task任务的任务壮态
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
    printf("任务壮态值:%d,对应的壮态为:%s\r\n", TaskState, TaskInfo);
    printf("/**************************结束**************************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while (KEY_Scan(0) != WKUP_PRES)
        delay_ms(10); //等待KEY_UP键按下

    //第四步：函数vTaskList()的使用
    printf("/*************第三步：函数vTaskList()的使用*************/\r\n");
    vTaskList(InfoBuffer);        //获取所有任务的信息
    printf("%s\r\n", InfoBuffer); //通过串口打印所有任务的信息
    printf("/**************************结束**************************/\r\n");
    while (1)
    {
        LED_R_TogglePin();
        vTaskDelay(1000); //延时1s，也就是1000个时钟节拍
    }
}

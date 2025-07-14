/**
 * 使用HAL_Delay的前提：系统时钟为其它定时器，并且其优先级不在RTOS管理范围内
 * vTaskDelay中有开关中断的操作
 *
 *  */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "stdio.h"
#include "stdarg.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

char Task_Printf[200]; // 打印任务状态

void Uart_printf(UART_HandleTypeDef *huart, char *format, ...)
{
    static char buf[1024]; // 定义临时数组，根据实际发送大小微�?
    va_list args;
    va_start(args, format);
    uint32_t len = vsnprintf((char *)buf, sizeof(buf), (char *)format, args);
    va_end(args);
    HAL_UART_Transmit_DMA(huart, (uint8_t *)buf, len);
}

#define START_TASK_STACK 128  // 任务堆栈内存，这里为128 x 2
#define START_TASK_PRIORITY 1 // 任务优先级
TaskHandle_t Start_Task;
void Task_Init(void *pvParaMeters);

#define TASK1_STACK 128 // 动态任务1
#define TASK1_PRIORITY 1
TaskHandle_t Task1;
void Task_1(void *pvParaMeters);

#define TASK2_STACK 128 // 动态任务2
#define TASK2_PRIORITY 1
TaskHandle_t Task2;
void Task_2(void *pvParaMeters);

#define TASK_STATIC_STACK 128 // 静态任务
#define TASK_STATIC_PRIORITY 1
TaskHandle_t Task_St; // 任务句柄
void Task_Static(void *pvParaMeters);
StackType_t Task_Stack[TASK_STATIC_STACK]; // 静态任务栈,以数组形式存储
StaticTask_t Task_Sta;                     // 静态任务句柄

StaticTask_t TaskIdle;                         // 空闲任务块
StackType_t TaskIdle_Stack[TASK_STATIC_STACK]; // 空闲任务内存

StaticTask_t TaskTimer;                         // 软件定时器任务块
StackType_t TaskTimer_Stack[TASK_STATIC_STACK]; // 软件定时器任务内存

/**
 * @brief 创建初始任务函数(开始时调用)
 */
void FreeRTOS_Start(void)
{
    xTaskCreate((TaskFunction_t)Task_Init,        // 任务函数地址
                (char *)"Task_Init",              // 函数名
                (uint16_t)START_TASK_STACK,       // 任务堆栈大小
                (void *)NULL,                     // 传递给任务的参数
                (UBaseType_t)START_TASK_PRIORITY, // 任务优先级
                (TaskHandle_t *)Start_Task);      // 生成的任务句柄

    vTaskStartScheduler(); // 启动任务调度器
}

/**
 * @brief 创建主任务函数
 */
void Task_Init(void *pvParaMeters)
{
    taskENTER_CRITICAL(); // 进入临界区

    xTaskCreate((TaskFunction_t)Task_1,
                (char *)"Task_1",
                (uint16_t)TASK1_STACK,
                (void *)NULL,
                (UBaseType_t)TASK1_PRIORITY,
                &Task1);

    xTaskCreate((TaskFunction_t)Task_2,
                (char *)"Task_2",
                (uint16_t)TASK2_STACK,
                (void *)NULL,
                (UBaseType_t)TASK2_PRIORITY,
                &Task2);

    /**
     * 启动任务调度器时自动创建空闲任务和定时器任务
     * 因此需要手动为其分配内存
     * vApplicationGetIdleTaskMemory
     * vApplicationGetTimerTaskMemory(如果开启软件定时器任务，则这个也要操作，否则报错)
     */
    xTaskCreateStatic((TaskFunction_t)Task_Static,
                      (char *)"Task_Static",
                      (uint16_t)TASK_STATIC_STACK,
                      (void *)NULL,
                      (UBaseType_t)TASK_STATIC_PRIORITY,
                      Task_Stack,
                      &Task_Sta);

    taskEXIT_CRITICAL(); // 退出临界区

    vTaskDelete(NULL); // 先退出，再删除当前任务
}

/**
 * @brief 为空闲任务分配内存
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &TaskIdle;
    *ppxIdleTaskStackBuffer = TaskIdle_Stack;
    *pulIdleTaskStackSize = TASK_STATIC_STACK;
}

/**
 * @brief 为定时器任务分配内存
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &TaskTimer;
    *ppxTimerTaskStackBuffer = TaskTimer_Stack;
    *pulTimerTaskStackSize = TASK_STATIC_STACK;
}

void Task_1(void *pvParaMeters)
{
    while (1)
    {
        // portDISABLE_INTERRUPTS(); // 开关任务中断
        // HAL_Delay(10000);
        // portENABLE_INTERRUPTS();
        // HAL_Delay(10000);
    }
}
void Task_2(void *pvParaMeters)
{
    while (1)
    {
        // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
        // vTaskDelay(1000);
    }
}

void Task_Static(void *pvParaMeters)
{
    while (1)
    {
        // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);
        // vTaskDelay(250);
    }
}

/**
 * 使用HAL_Delay的前提：系统时钟为其它定时器，并且其优先级不在RTOS管理范围内
 * vTaskDelay中有开关任务中断的操作
 *
 *uxTaskPriorityGet()
 * 获取任务优先级←
 * vTaskPrioritySet()
 * 设置任务优先级←
 * uxTaskGetNumberOfTasks()
 * 获取系统中任务的数量
 * uxTaskGetSystemState()
 * 获取所有任务状态信息←
 * vTaskGetInfo()
 * 获取指定单个的任务信息
 * xTaskGetCurrentTaskHandle()
 * 获取当前任务的任务句柄←
 * xTaskGetHandle()
 * 根据任务名获取该任务的任务句柄
 * uxTaskGetStackHighWaterMark()
 * 获取任务的任务栈历史剩余最小值
 * eTaskGetState()
 * 获取任务状态
 * vTaskList()
 * 以“表格”形式获取所有任务的信息
 * vTaskGetRunTimeStats()
 * 获取任务的运行时间←
 *
 * 使用任务计时器时，任务计时器频率 >= 系统时钟的频率 x 10
 * vTaskGetRunTimeStats()这里使用TIM4，打印串口时显示的是TIM4的计数值，注意开始时要启动时钟（中断）并且在CallBack函数中让特定值自增
 *
 * vTaskDelayUntil()函数是让任务的运行周期固定，不会因为任务里面程序的运行时间变化而变化。使用时要保证时间片的时间大于任务运行总时间（比如时间片是10ms，但在任务里HAL_Delay(20)，这样Until就不会起作用）。需要注意，如果在CubeMX中设置的系统时钟源为其它定时器而不是SysTick，那么HAL_Delay和vTaskDelayUntil的执行方式就不一样。举个例子，我把系统时钟源设置为TIM1，那么此时HAL_Delay使用的还是SysTick时钟，该延迟多少毫秒就延迟多少毫秒。但vTaskDelay和vTaskDelayUntil就使用TIM1的频率了，TIM1的频率在CubeMX中配置。最终，vTask函数的真正的执行频率就由下面的因素决定：
 *
 * 1、你配置的TIM时钟频率
 * 2、configCPU_CLOCK_HZ ((unsigned long)72000000)
 * 3、configTICK_RATE_HZ ((TickType_t)10)
 * （2、3点在FreeRTOSConfig.h中配置，2要随着1改动）
 *
 * xqueue函数注意；
 * 1.临界区卡死
 * 2.char * 和 char[]
 * 3.portMAX_DELAY
 *
 * 创建队列时要注意队列长度和使用对应的函数，否则有可能卡死
 * xQueueSend()←
 * 往队列的尾部写入消息
 * xQueueSendToBack()
 * 同 xQueueSend()
 * xQueueSendToFront()
 * 往队列的头部写入消息
 * xQueueOverwrite()
 * 覆写队列消息（只用于队列长度为1的情况）←
 * xQueueSendFromISR()
 * 在中断中往队列的尾部写入消息
 * xQueueSendToBackFromISR()
 * 同xQueueSendFromISR()
 * xQueueSendToFrontFromISR()
 * 在中断中往队列的头部写入消息←
 * xQueueOverwriteFromISR()
 * 在中断中覆写队列消息（只用于队列长度为1的情况）←
 * xQueueReceive()
 * 从队列头部读取消息，并删除消息←
 * xQueuePeek()
 * 从队列头部读取消息←
 * xQueueReceiveFromISR()
 * 在中断中从队列头部读取消息，并删除消息已
 * xQueuePeekFromISR()
 * 在中断中从队列头部读取消息←
 *
 * xSemaphoreTake(Semaphore_Queue_Count, 100)（针对计数信号量） 在信号量充足时拿信号量可以立刻拿，但不足时只能等100ms再退出
 * xSemaphoreGive则可以立即给
 *
 * 创建任务的任务要有临界区，否则被打断后会影响任务创建
 * 如果程序不在while循环内，则只执行一次
 *
 * xTaskCreate可以初始化任务句柄，但xTaskCreateStatic内部不能初始化句柄，必须Task_Static=xTaskCreateStatic()，否则Task_Static只是
 * 一个空句柄，相当于NULL。导致挂起任务无效
 * 可能出现乱码的原因：由于FreeRTOS切换任务快，Uart_Printf耗时，所以在后面加上HAL_Delay(1)
 * 高优先级的任务要有让出CPU的操作，否则一直循环执行
 *
 * 互斥信号量类型和二值信号量相同，但是有优先级继承，不能在中断中使用。会把任务的优先级拉高
 * 二值信号量初始值为0，互斥信号量初始值为1
 *
 * 当T1拿走互斥锁时，T2想拿，就会升高T1的优先级。如果T1没有释放互斥锁的程序，就会导致T2阻塞（使用portMAX_Delay下），T1优先级一直较高
 * 同时，T1拿锁，只能T1放。其它任务不能放。所以拿放锁程序应成对出现，同时避免使用portMAX_Delay
 *
 * 向队列集里添加队列时，队列或者信号量必须为空，否则失败。而vSemaphoreCreate默认为满，因此要用xSemaphoreCreateBinary创建信号量
 * 队列集检索时只要检索一次即可        QueueSetMember = xQueueSelectFromSet(QueueSet, 200); // 从队列集中选择一个队列
 *
 * 事件标志组类似于信号量，但是侧重于事件的完成，信号量侧重于资源的分配
 *
 * xEventGroupCreate()
 * 使用动态方式创建事件标志组
 * xEventGroupCreateStatic()
 * 使用静态方式创建事件标志组
 * xEventGroupClearBits()
 * 清零事件标志位←
 * xEventGroupClearBitsFromISR()
 * 在中断中清零事件标志位←
 * xEventGroupSetBits()
 * 设置事件标志位
 * xEventGroupSetBitsFromISR()
 * 在中断中设置事件标志位←
 * xEventGroupWaitBits()
 * 等待事件标志位()
 * xEventGroupSync()
 * 设置事件标志位，并等待事件标志位←
 *
 * 事件标志组的个数由configUSE_16_BIT_TICKS决定，为0则有24位，为1有8位
 *
 * xTaskNotify()
 * 发送通知，带有通知值
 * xTaskNotifyAndQuery()
 * 发送通知，带有通知值并且保留接收任务的原通知值
 * xTaskNotifyGive()
 * 发送通知，不带通知值
 * xTaskNotifyFromISR()
 * 在中断中发送任务通知
 * xTaskNotifyAndQueryFromISR()
 * vTaskNotifyGiveFromISR()
 * ulTaskNotifyTake()
 * 获取任务通知，可选退出函数时对通知值清零或减1
 * xTaskNotifyWait()
 * 获取任务通知，可获取通知值和清除通知值的指定位
 *
 * 注意：发送通知有相关ISR函数，接收通知没有ISR函数，不能在ISR中接收任务通知。
 *
 * xTaskNotifyIndexed相比于xTaskNotify，是指T1发消息到T2的指定索引
 *
 * xTimerCreate()
 * 动态方式创建软件定时器←
 * xTimerCreateStatic()
 * 静态方式创建软件定时器
 * xTimerStart()
 * 开启软件定时器定时←
 * xTimerStartFromISR()
 * 在中断中开启软件定时器定时
 * xTimerStop()
 * 停止软件定时器定时←
 * xTimerStopFromISR()
 * 在中断中停止软件定时器定时←
 * xTimerReset()
 * 复位软件定时器定时←
 * xTimerResetFromISR()
 * 在中断中复位软件定时器定时
 * xTimerChangePeriod()
 * 更改软件定时器的定时超时时间
 * xTimerChangePeriodFromISR()
 * 在中断中更改定时超时时间
 *
 * 软件定时器也相当于一个任务，它的优先级由configTIMER_TASK_PRIORITY决定
 * 在FreeRTOSConfig.h中配置configTIMER_TASK_PRIORITY时要使它的优先级较高，不然会被其它任务阻塞
 *
 * FreeRTOS低功耗模式：使用Stm32的睡眠模式（只关闭CPU时钟），当处在空闲任务时，自动关闭Systick，唤醒时补上
 * configUSE_TICKLESS_IDLE
 * 使能低功耗Tickless模式，默认0
 * configEXPECTED_IDLE_TIME_BEFORE_SLEEP
 * 系统进入相应低功耗模式的最短时长，默认2
 * configPRE SLEEP_PROCESSING(x)
 * 在系统进入低功耗模式前执行的事务，比如关闭外设时钟←
 * configPOST_SLEEP_PROCESSING(x)
 * 系统退出低功耗模式后执行的事务，比如开启之前关闭的外设时钟←
 *
 * 内存管理方法文件：
 * heap_l：最简单，不允许释放内存。
 * heap_2：允许释放内存，但不会合并相邻的空闲块。“
 * heap_3：简单包装了标准mallocO和freeO，以保证线程安全。
 * heap_4：合并相邻的空闲块以避免碎片化。包含绝对地址放置选项。
 * heap_5：如同heap_4，能够跨越多个不相邻内存区域的堆。
 *  */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "stdio.h"
#include "stdarg.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

char Task_Printf[200]; // 打印任务状态

uint32_t FindTask_TimerTick = 0; // 测量单个任务运行时间的计数器

void Uart_printf(UART_HandleTypeDef *huart, char *format, ...) // 串口打印函数
{
    static char buf[1024]; // 定义临时数组，根据实际发送大小微调
    va_list args;
    va_start(args, format);
    uint32_t len = vsnprintf((char *)buf, sizeof(buf), (char *)format, args);
    va_end(args);
    HAL_UART_Transmit_DMA(huart, (uint8_t *)buf, len);
    HAL_Delay(1);
}

#define START_TASK_STACK 128  // 任务堆栈内存，这里为128 x 2
#define START_TASK_PRIORITY 4 // 任务优先级
TaskHandle_t Start_Task;
void Task_Init(void *pvParaMeters);

#define TASK1_STACK 128 // 动态任务1
#define TASK1_PRIORITY 2
TaskHandle_t Task1;
void Task_1(void *pvParaMeters);

#define TASK2_STACK 128 // 动态任务2
#define TASK2_PRIORITY 2
TaskHandle_t Task2;
void Task_2(void *pvParaMeters);

#define TASK_STATIC_STACK 128 // 静态任务
#define TASK_STATIC_PRIORITY 2
TaskHandle_t Task_St; // 任务句柄
void Task_Static(void *pvParaMeters);

StackType_t Task_Stack[TASK_STATIC_STACK]; // 静态任务栈,以数组形式存储
StaticTask_t Task_Sta;                     // 静态任务句柄

StaticTask_t TaskIdle;                         // 空闲任务块
StackType_t TaskIdle_Stack[TASK_STATIC_STACK]; // 空闲任务内存

StaticTask_t TaskTimer;                         // 软件定时器任务块
StackType_t TaskTimer_Stack[TASK_STATIC_STACK]; // 软件定时器任务内存

QueueHandle_t Queue1;                 // 队列1
QueueHandle_t Queue2;                 // 队列2
QueueHandle_t Semaphore_Queue_Binary; // 二值信号队列，只有空和满
QueueHandle_t Semaphore_Queue_Count;  // 计数类型队列，当有任务使用时信号量
QueueHandle_t Semaphore_Queue_Mutex;  // 互斥信号量队列，只有一个任务可以使用
QueueSetHandle_t QueueSet;            // 队列集

EventGroupHandle_t EventGroup; // 事件标志组

TimerHandle_t Timer1; // 软件定时器1

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
 * @brief 创建定时器回调函数
 */
void Timer1_CallBack_Function(TimerHandle_t xTimer)
{
    if (xTimer != Timer1) // 确保是Timer1触发的回调
    {
        return; // 如果不是Timer1触发的回调，直接返回
    }
    Uart_printf(&huart1, "Ring\n");
    if (xTimerChangePeriod(Timer1, 3000, 100) == 1) // 修改定时器周期为3000ms
    {
        Uart_printf(&huart1, "Timer1 Change Period Success\n");
    }
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
    Task_St = xTaskCreateStatic((TaskFunction_t)Task_Static,
                                (char *)"Task_Static",
                                (uint16_t)TASK_STATIC_STACK,
                                (void *)NULL,
                                (UBaseType_t)TASK_STATIC_PRIORITY,
                                Task_Stack,
                                &Task_Sta);

    Queue1 = xQueueCreate(5, sizeof(char *));
    Queue2 = xQueueCreate(5, sizeof(char *));

    QueueSet = xQueueCreateSet(3);

    Semaphore_Queue_Binary = xSemaphoreCreateBinary();
    Semaphore_Queue_Count = xSemaphoreCreateCounting(1, 1);
    Semaphore_Queue_Mutex = xSemaphoreCreateMutex();

    EventGroup = xEventGroupCreate();

    Timer1 = xTimerCreate("Tim1",                    // 定时器名称
                          1000,                      // 延时时间（num x 时间片）
                          1,                         // 是否重装
                          (void *)1,                 // 序号
                          Timer1_CallBack_Function); // 回调函数

    if (xTimerStart(Timer1, 100) == 1)
    {
        Uart_printf(&huart1, "Timer1 Start Success\n");
    }

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

void Task_1(void *pvParaMeters) // 动态任务1
{
    while (1)
    {
    }
}

void Task_2(void *pvParaMeters) // 动态任务2
{
    while (1)
    {
    }
}

void Task_Static(void *pvParaMeters) // 静态任务
{
    while (1)
    {
        vTaskDelay(10000);
    }
}

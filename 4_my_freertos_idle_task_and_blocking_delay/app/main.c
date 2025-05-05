#include "task.h"

uint32_t flag1, flag2;
extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

/* 定义任务控制块与STACK */
#define TASK1_STACK_SIZE   20
TaskHandle_t Task1_Handle;
StackType_t Task1Stack[TASK1_STACK_SIZE];
TCB_t Task1TCB;

#define TASK2_STACK_SIZE   20
TaskHandle_t Task2_Handle;
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;

/* 空闲任务相关 */
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
TCB_t IdleTaskTCB;
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* 延时函数 */
void delay(uint32_t cnt)
{
    for (; cnt != 0; cnt--) {}
}

/* 任务1函数 */
void Task1_Entry(void *p)
{
    while (1)
    {
        flag1 = 1;
        vTaskDelay(20);
        flag1 = 0;
        vTaskDelay(20);
    }
}

/* 任务2函数 */
void Task2_Entry(void *p)
{
    while (1)
    {
        flag2 = 1;
        vTaskDelay(20);
        flag2 = 0;
        vTaskDelay(20);
    }
}

int main()
{
    /* 初始化任务相关：就绪列表 */
    prvInitialiseTaskLists();

    /* 创建任务 */
    Task1_Handle = xTaskCreateStatic(Task1_Entry, "Task1", TASK1_STACK_SIZE, NULL, Task1Stack, &Task1TCB);
    Task2_Handle = xTaskCreateStatic(Task2_Entry, "Task2", TASK2_STACK_SIZE, NULL, Task2Stack, &Task2TCB);

    /* 将任务添加到就绪列表 */
    vListInsertEnd(&(pxReadyTasksLists[1]), &(((TCB_t *)(&Task1TCB))->xStateListItem));
    vListInsertEnd(&(pxReadyTasksLists[2]), &(((TCB_t *)(&Task2TCB))->xStateListItem));

    /* 启动调度器 */
    vTaskStartScheduler();
    
    for (;;)
    {

    }
}

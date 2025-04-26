#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

/* 任务控制块定义，在task.c中定义，暂时放在task.h */


/* 当前正在运行的任务的任务控制块指针，默认初始化为NULL */
TCB_t *volatile pxCurrentTCB = NULL;

/* 任务就绪列表 */
List_t pxReadyTasksLists[configMAX_PRIORITIES];

/* 创建新任务 */
static void prvInitialiseNewTask(   TaskFunction_t pxTaskCode,  /* 入口 */
                                    const char *const pcName,   /* 名称 */
                                    const uint32_t ulStackDepth,/* 栈大小，单位：字 */
                                    void *const pvParameters,   /* 形参 */
                                    TaskHandle_t *const pxCreatedTask,  /* 句柄 */
                                    TCB_t *pxNewTCB )   /* 任务控制块指针 */
{
    StackType_t *pxTopOfStack;
    UBaseType_t x;

    /* 获取栈顶地址 */
    pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - (uint32_t)1);

    /* 向下8字节对齐 */
    pxTopOfStack = (StackType_t *)(((uint32_t)pxTopOfStack) & (~((uint32_t)0x0007)));

    /* 将任务名字存储在TCB中 */
    for (x = (UBaseType_t)0; x < (UBaseType_t)configMAX_TASK_NAME_LEN; x++)
    {
        pxNewTCB->pcTaskName[x] = pcName[x];
        if (pcName[x] == 0x00)
        {
            break;
        }
    }
    /* 限制任务名字长度 */
    pxNewTCB->pcTaskName[configMAX_TASK_NAME_LEN - 1] = '\0';

    /* 初始化TCB的xStateListItem节点，表示节点还没有插入任何链表 */
    vListInitialiseItem(&(pxNewTCB->xStateListItem));

    /* 设置xStateListItem节点的拥有者，即拥有这个节点本身的TCB */
    listSET_LIST_ITEM_OWNER(&(pxNewTCB->xStateListItem), pxNewTCB);

    /* 初始化任务栈 */
    pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxTopOfStack, pxTaskCode, pvParameters);

    /* 让任务句柄指向任务控制块 */
    if ((void *)pxCreatedTask != NULL)
    {
        *pxCreatedTask = (TaskHandle_t)pxNewTCB;
    }
}

/* 静态创建任务 */
#if (configSUPPORT_STATIC_ALLOCATION == 1)

TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,  /* 入口 */
                                const char *const pcName,   /* 名称 */
                                const uint32_t ulStackDepth,/* 栈大小，单位：字 */
                                void *const pvParameters,   /* 形参 */
                                StackType_t *const puxStackBuffer,  /* 任务栈起始地址 */
                                TCB_t *const pxTaskBuffer )   /* 任务控制块指针 */
{
    TCB_t *pxNewTCB;    /* 句柄，指向TCB */
    TaskHandle_t xReturn;

    if ((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
    {
        /* 保存任务控制块 */
        pxNewTCB = (TCB_t *)pxTaskBuffer;
        pxNewTCB->pxStack = (StackType_t *)puxStackBuffer;

        /* 创建新任务 */
        prvInitialiseNewTask(   pxTaskCode,     /* 入口 */
                                pcName,         /* 名称 */
                                ulStackDepth,   /* 栈大小，单位：字 */
                                pvParameters,   /* 形参 */
                                &xReturn,       /* 句柄 */
                                pxNewTCB    );  /* 任务控制块指针 */
    }
    else
    {
        xReturn = NULL;
    }

    return xReturn;
}

#endif  /* configSUPPORT_STATIC_ALLOCATION */

/* 就序列表初始化 */
void prvInitialiseTaskLists(void)
{
    UBaseType_t uxPriority;

    /* 初始化configMAX_PRIORITIES个列表 */
    for (uxPriority = (UBaseType_t)0U; uxPriority < (UBaseType_t)configMAX_PRIORITIES; uxPriority++)
    {
        vListInitialise(&(pxReadyTasksLists[uxPriority]));
    }
}

/* 启动调度器 */
extern TCB_t Task1TCB;
extern TCB_t Task2TCB;

void vTaskStartScheduler(void)
{
    /* 手动指定第一个运行的任务 */
    pxCurrentTCB = &Task1TCB;

    if (xPortStartScheduler() != pdFALSE)
    {
        /* 调度器启动成功就不会运行到这里 */
    }
}

/* 任务切换上下文 */
void vTaskSwitchContext(void)
{
    /* 两个任务轮流切换 */
    if (pxCurrentTCB == &Task1TCB)
    {
        pxCurrentTCB = &Task2TCB;
    }
    else
    {
        pxCurrentTCB = &Task1TCB;
    }
}

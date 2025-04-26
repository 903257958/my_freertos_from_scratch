#ifndef TASK_H
#define TASK_H

#include "FreeRTOS.h"
#include "list.h"

/* 任务切换 */
#define taskYIELD() portYIELD()

/* 进入临界段，不带中断保护，不能嵌套 */
#define taskENTER_CRITICAL()    portENTER_CRITICAL()

/* 退出临界段，不带中断保护，不能嵌套 */
#define taskEXIT_CRITICAL() portEXIT_CRITICAL()

/* 进入临界段，带中断保护，能嵌套 */
#define taskENTER_CRITICAL_FROM_ISR()    portSET_INTERRUPT_MASK_FROM_ISR()

/* 退出临界段，带中断保护，能嵌套 */
#define taskEXIT_CRITICAL_FROM_ISR(x)    portCLEAR_INTERRUPT_MASK_FROM_ISR(x)

/* 任务控制块定义，在task.c中定义，暂时放在task.h */
typedef struct tskTaskControlBlock
{
    volatile StackType_t *pxTopOfStack; /* 栈顶 */
    ListItem_t xStateListItem;  /* 任务节点 */
    StackType_t *pxStack;       /* 任务栈起始地址 */
    char pcTaskName[configMAX_TASK_NAME_LEN];   /* 名称 */
}tskTCB;
typedef tskTCB TCB_t;

/* 任务句柄 */
typedef void *TaskHandle_t;

/* 函数声明 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,  /* 入口 */
                                const char *const pcName,   /* 名称 */
                                const uint32_t ulStackDepth,/* 栈大小，单位：字 */
                                void *const pvParameters,   /* 形参 */
                                StackType_t *const puxStackBuffer,  /* 任务栈起始地址 */
                                TCB_t *const pxTaskBuffer );   /* 任务控制块指针 */
#endif /* configSUPPORT_STATIC_ALLOCATION */

void prvInitialiseTaskLists(void);
void vTaskStartScheduler(void);
void vTaskSwitchContext(void);

#endif

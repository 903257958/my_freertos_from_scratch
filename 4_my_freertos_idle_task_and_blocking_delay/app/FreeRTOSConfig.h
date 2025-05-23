#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* CPU内核时钟频率 */
#define configCPU_CLOCK_HZ  ((unsigned long)25000000)

/* 系统节拍中断的频率 */
#define configTICK_RATE_HZ  ((TickType_t)1000)

/* 可使用的最大优先级 */
#define configMAX_PRIORITIES    (32)

/* 空闲任务使用的栈大小 */
#define configMINIMAL_STACK_SIZE    ((unsigned short)128)

/* 任务名字字符串最大长度 */
#define configMAX_TASK_NAME_LEN (16)

/* 系统节拍计数器变量数据类型 */
#define configUSE_16_BIT_TICKS  0

/* 支持静态内存 */
#define configSUPPORT_STATIC_ALLOCATION 1

/* Cortex-M内核中断优先级寄存器值转换 */
#define configKERNEL_INTERRUPT_PRIORITY 		255   /* 高四位有效，即等于0xff，或者是15 */

/* FreeRTOS可管理的最高中断优先级对应寄存器值 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191   /* 高四位有效，即等于0xb0，或者是11 */

/* 修改FreeRTOS中SVC、PendSV和SysTick中断服务函数的名称 */
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
#define vPortSVCHandler     SVC_Handler

#endif

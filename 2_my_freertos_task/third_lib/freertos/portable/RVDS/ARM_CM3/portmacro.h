#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"

#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  uint32_t
#define portBASE_TYPE   long
    
typedef portSTACK_TYPE  StackType_t;
typedef long            BaseType_t;
typedef unsigned long   UBaseType_t;

#if (configUSE_16_BIT_TICKS == 1)
typedef uint16_t TickType_t;
#define portMAX_DELAY   (TickType_t)0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY   (TickType_t)0xffffffffUL
#endif

/* 
 * 中断控制状态寄存器：0xe000ed04
 * Bit 28 PENDSVSET: PendSV 悬起位
 */
#define portNVIC_INT_CTRL_REG   (*((volatile uint32_t *)0xe000ed04))    /* 定义ICSR寄存器指针 */
#define portNVIC_PENDSVSET_BIT  (1UL << 28UL)   /* 生成PENDSVSET位掩码 */
#define portSY_FULL_READ_WRITE  (15)    /* DSB/ISB屏障操作类型（全系统读写屏障）*/
#define portYIELD() \
{ \
    /* 触发PendSV，产生上下文切换 */ \
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; /* 置位 PENDSVSET，挂起 PendSV 异常 */ \
    __dsb(portSY_FULL_READ_WRITE); /* 数据同步屏障，确保寄存器写入完成 */ \
    __isb(portSY_FULL_READ_WRITE); /* 指令同步屏障，清空流水线，保证后续指令在正确上下文中执行 */ \
}

#endif

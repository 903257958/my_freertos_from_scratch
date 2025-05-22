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

/* 关中断，不带中断保护（带返回值，不能嵌套，不能在中断中使用） */
#define portDISABLE_INTERRUPTS()    vPortRaiseBASEPRI()

/* 开中断，不带中断保护，直接将BASEPRI的值设置为0，与portDISABLE_INTERRUPTS成对使用 */
#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)

/* 进入临界段/关中断，带中断保护（带返回值，可以嵌套，可以在中断中使用） */
#define portSET_INTERRUPT_MASK_FROM_ISR()   ulPortRaiseBASEPRI()

/* 退出临界段/开中断，带中断保护，将上一次关中断时保存的BASEPRI的值作为形参，与portSET_INTERRUPT_MASK_FROM_ISR成对使用 */
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    vPortSetBASEPRI(x)

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

/* 进入临界段，不带中断保护 */
#define portENTER_CRITICAL()    vPortEnterCritical()

/* 退出临界段，不带中断保护 */
#define portEXIT_CRITICAL() vPortExitCritical()

/* 定义任务函数的格式 */
#define portTASK_FUNCTION(vFunction, pvParameters)    void vFunction(void *pvParameters)

/* 普通建议内联 */
#define portINLINE __inline

/* 强制内联 */
#ifndef portFORCE_INLINE
	#define portFORCE_INLINE __forceinline
#endif

#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	/* 检测优先级配置 */
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif

	/* 根据优先级设置/清除优先级位图中相应的位 */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) __clz( ( uxReadyPriorities ) ) )

#endif

/* 不带返回值的关中断函数，不能嵌套，不能在中断中使用 */
static portFORCE_INLINE void vPortRaiseBASEPRI(void)
{
    uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

    __asm
    {
        msr basepri, ulNewBASEPRI
        dsb
        isb
    }
}

/* 带返回值的关中断函数，可以嵌套，可以在中断中使用 */
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI(void)
{
    uint32_t ulReturn, ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

    __asm
    {
        mrs ulReturn, basepri       /* 保存BASEPRI的值，记录当前哪些中断被关闭 */
        msr basepri, ulNewBASEPRI   /* 更新BASEPRI 的值 */
        dsb
        isb
    }

    return ulReturn;    /* 返回原来BASEPRI 的值 */
}

/* 开中断函数，将传进来的形参更新到BASEPRI寄存器 */
/* 根据传进来形参的不同，分为中断保护版本与非中断保护版本 */
static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI)
{
    __asm
    {
        msr basepri, ulBASEPRI
    }
}

/* 开中断，相当于vPortSetBASEPRI(0) */
static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
	__asm
	{
		msr basepri, #0
	}
}

#endif

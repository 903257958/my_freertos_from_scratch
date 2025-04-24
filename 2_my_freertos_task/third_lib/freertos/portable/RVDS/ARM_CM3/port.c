#include "FreeRTOS.h"
#include "task.h"

/* 配置ARM Cortex-M处理器中系统异常优先级 */
/* 内核外设SCB中SHPR3寄存器用于设置SysTick和PendSV的异常优先级
 * Bits 31:24：SysTick异常的优先级（PRI_15）
 * Bits 23:16：PendSV异常的优先级（PRI_14） */
#define portNVIC_SYSPRI2_REG    (*((volatile uint32_t *)0xe000ed20))

/* 设置PendSV异常的优先级，将优先级值对齐到SCB_SHPR3寄存器的Bits 23:16 */
#define portNVIC_PENDSV_PRI     (((uint32_t)configKERNEL_INTERRUPT_PRIORITY) << 16UL)

/* 设置SysTick异常的优先级，将优先级值对齐到SCB_SHPR3寄存器的Bits 31:24 */
#define portNVIC_SYSTICK_PRI    (((uint32_t)configKERNEL_INTERRUPT_PRIORITY) << 24UL)

/* 设置初始化栈用的常量 */
#define portINITIAL_XPSR    (0x01000000)

/* 为了严格遵守Cortex-M规范，任务开始地址应清除位0，因为它是在退出ISR时加载到PC中的 */
#define portSTART_ADDRESS_MASK  ((StackType_t)0xfffffffeUL)

/* 函数声明 */
void prvStartFirstTask(void);
void vPortSVCHandler(void );
void xPortPendSVHandler(void);

/* 捕获任务异常退出 */
static void prvTaskExitError(void)
{
    for(;;);
}

/* 初始化任务栈 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, 
                                    TaskFunction_t pxCode, 
                                    void *pvParameters  )
{
    /* 预留空间并设置初始程序状态寄存器 (xPSR) */
    pxTopOfStack--;                     /* 栈指针下移（地址减小） */
    *pxTopOfStack = portINITIAL_XPSR;   /* 写入初始xPSR值，xPSR的bit24必须置1 */
    
    /* 设置任务入口地址（PC寄存器R15） */
    pxTopOfStack--;
    *pxTopOfStack = ((StackType_t)pxCode)&portSTART_ADDRESS_MASK;   /* 任务开始地址清除位0 */

    /* 设置LR寄存器R14（指向任务退出错误处理函数） */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t)prvTaskExitError;

    /* 跳过R12, R3-R0共5个寄存器，默认初始化为0 */
    pxTopOfStack -=5;

    /* 设置R0寄存器（任务参数） */
    *pxTopOfStack = (StackType_t)pvParameters;

    /* 异常发生时，需要手动加载到CPU 寄存器的内容（R11-R4） */
    pxTopOfStack -= 8;

    return pxTopOfStack;    /* 返回栈顶指针 */
}

/* 端口启动调度器 */
BaseType_t xPortStartScheduler(void)
{
    /* 配置PendSV和SysTick的中断优先级为最低 */
    portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

    /* 启动第一个任务不再返回 */
    prvStartFirstTask();

    /* 程序不会运行到这里 */
    return 0;
}

/*
 * 在Cortex-M中，内核外设SCB的地址范围为：0xE000ED00-0xE000ED3F
 * 0xE000ED008为SCB外设中SCB_VTOR这个寄存器的地址，里面存放的是向量表的起始地址，即MSP的地址
 */
/* 启动第一个任务 */
__asm void prvStartFirstTask(void)
{
    PRESERVE8   /* 8字节对齐 */

    ldr r0, =0xE000ED08 /* 将寄存器r0加载为0xE000ED08（SCB_VTOR寄存器的地址） */
    ldr r0, [r0]        /* 从r0指定的地址（即SCB_VTOR）读取值到r0，获取当前向量表基地址 */
    ldr r0, [r0]        /* 从向量表基地址处读取第一个条目，即初始主堆栈指针（MSP）值 */

    /* 设置主堆栈指针msp的值 */
    msr msp, r0         /* 将r0的值写入主堆栈指针（MSP），初始化堆栈 */

    /* 使能全局中断 */
    cpsie i     /* 清除PRIMASK，允许可屏蔽中断（IRQ） */
    cpsie f     /* 清除FAULTMASK，允许硬件错误异常（如HardFault） */
    dsb         /* 确保之前的内存操作完成 */
    isb         /* 清空流水线，保证后续指令从新上下文执行 */

    /* 调用SVC去启动第一个任务 */
    svc 0   /* 触发SVC异常，切换到特权模式并执行SVC处理程序 */
    nop     /* 空操作指令，用于填充流水线或对齐代码 */
    nop
}

/* SVC中断服务函数 */
__asm void vPortSVCHandler(void)
{
    extern pxCurrentTCB;    /* 指向当前正在运行或者即将要运行的任务的任务控制块 */

    PRESERVE8

    ldr r3, =pxCurrentTCB   /* 将pxCurrentTCB的地址加载到r3（r3 = &pxCurrentTCB） */
    ldr r1, [r3]            /* 从r3读取值到r1，获取当前TCB指针（r1 = *pxCurrentTCB） */
    ldr r0, [r1]            /* 从TCB的第一个字段读取任务堆栈指针（r0 = TCB->pxTopOfStack） */
    
    ldmia r0!, {r4-r11} /* 从堆栈中恢复r4-r11寄存器，同时更新r0（出栈操作） */
    msr psp, r0         /* 将更新后的堆栈指针存入PSP（进程堆栈指针） */
    isb                 /* 指令同步屏障，确保PSP写入完成 */

    mov r0, #0          /* 清零r0，用于设置BASEPRI寄存器（允许所有中断） */
    msr basepri, r0     /* 写BASEPRI寄存器（取消中断屏蔽） */

    orr r14, #0xd       /* 向r14 寄存器最后4 位按位或0x0D，修改EXC_RETURN值（LR），强制返回线程模式并使用PSP */
    bx r14              /* 异常返回，触发上下文切换至新任务 */
}

/* PendSV中断服务函数 */
__asm void xPortPendSVHandler(void)
{
    extern pxCurrentTCB;
    extern vTaskSwitchContext;

    PRESERVE8

    mrs r0, psp             /* 将当前任务的进程栈指针PSP值读取到R0中 */
    isb                     /* 指令同步屏障，确保之前的指令执行完成 */

    ldr r3, =pxCurrentTCB   /* 将pxCurrentTCB的地址加载到R3（TCB指针的指针） */
    ldr r2, [r3]            /* 通过R3间接寻址，获取pxCurrentTCB的实际值（当前TCB首地址） */

    stmdb r0!, {r4-r11}     /* 将R4-R11寄存器压入当前任务栈（PSP指向的栈），并更新R0的值 */
    str r0, [r2]            /* 将更新后的栈顶地址（R0）存入当前TCB的pxTopOfStack字段 */

    stmdb sp!, {r3, r14}    /* 将R3（pxCurrentTCB地址）和R14（LR）压入主栈（MSP） */
    mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY /* 屏蔽低优先级中断 */
    msr basepri, r0         /* 设置basepri寄存器，屏蔽优先级低于该值的中断 */
    dsb                     /* 数据同步屏障 */
    isb                     /* 指令同步屏障 */
    bl vTaskSwitchContext   /* 调用vTaskSwitchContext，选择下一个要运行的任务 */
    mov r0, #0
    msr basepri, r0         /* 清除basepri，重新允许所有中断 */
    ldmia sp!, {r3, r14}    /* 从主栈恢复R3（pxCurrentTCB地址）和R14（LR） */

    ldr r1, [r3]            /* 获取新的pxCurrentTCB值（新任务的TCB地址） */
    ldr r0, [r1]            /* 读取新任务的栈顶指针（pxTopOfStack字段） */
    ldmia r0!, {r4-r11}     /* 从新任务栈中恢复R4-R11寄存器，并更新R0（栈顶指针） */
    msr psp, r0             /* 将新栈顶指针写入PSP寄存器 */
    isb                     /* 确保PSP更新完成 */
    bx r14                  /* 异常返回，硬件自动恢复R0-R3、R12、LR、PC、xPSR */

    nop                     /* 空操作（对齐或占位） */
}

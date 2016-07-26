/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		1                               //1，抢占式调度，0协同调度
#define configUSE_IDLE_HOOK			0                               //空闲回调函数使能控制，vApplicationIdleHook
#define configUSE_TICK_HOOK			0                               //SYSTEM TICK的回调函数，用于OS功能扩展，需要你自己编相应函数
#define configCPU_CLOCK_HZ			( ( unsigned long ) 72000000 )	//系统CPU频率，单位是Hz
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )         //系统SYSTEM TICK每秒钟的发生次数， 数值越大系统反应越快，但是CPU用在任务切换的开销就越多
#define configMAX_PRIORITIES		( 5 )                           //系统任务优先级数,这个数目越大耗费RAM越多
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 128 )      //系统最小堆栈尺寸，注意128不是128字节，而是128个入栈。比如ARM32位，128个入栈就是512字节
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 17 * 1024 ) )    //系统可用内存
#define configMAX_TASK_NAME_LEN		( 16 )                          //任务的PC名字最大长度
#define configUSE_TRACE_FACILITY	0                               //是否设定成追踪，由PC端TraceCon.exe记录，也可以转到系统显示屏上
#define configUSE_16_BIT_TICKS		0                               //SYSTEM TICK的长度，16是16位，如果是16位以下CPU， 一般选1；如果是32位系统，一般选0
#define configIDLE_SHOULD_YIELD		1                               //简单理解以下就是和IDLE TASK同样优先级的任务执行情况。建议设成1，对系统影响不大

/* Co-routine definitions. */
//适用协程。协程公用堆栈，节省RAM，但是没有任务优先级高，也无法和任务通讯
#define configUSE_CO_ROUTINES 		0
//所有协程的最大优先级数，协程优先级永远低于任务。就是系统先执行任务，所有任务执行完了才执行协程
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1   //设定可以改变任务优先度
#define INCLUDE_uxTaskPriorityGet		1   //设定可以查询任务优先度
#define INCLUDE_vTaskDelete				1   //设定可以删除任务
#define INCLUDE_vTaskCleanUpResources	0   //据说是可以回收删除任务后的资源（RAM等）
#define INCLUDE_vTaskSuspend			1   //设置可以把任务挂起
#define INCLUDE_vTaskDelayUntil			1   //设置任务延迟的绝对时间
#define INCLUDE_vTaskDelay				1   //设置任务延时，比如延迟30分钟，相对的时间，现在什么时间，不需要知道

/* This is the raw value as per the Cortex-M3 NVIC.  Values can be 255
(lowest) to 0 (1?) (highest). */
//系统内核的中断优先级，中断优先级越低，越不会影响其他中断。一般设成最低
#define configKERNEL_INTERRUPT_PRIORITY 		255
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
//系统SVC中断优先级，这两项都在在M3和PIC32上应用
//191——>0XBF——>右移4位——>0X0B（11），0~10不可屏蔽中断，11~15可屏蔽中断
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	191 /* equivalent to 0xb0, or priority 11. */


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
/*这是被用作每对ST库，其允许16的值优先级值，为0-15这必须对应于configKERNEL_INTERRUPT_PRIORITY
设置。这里15对应于最低255 NVIC价值*/
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY	15

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */


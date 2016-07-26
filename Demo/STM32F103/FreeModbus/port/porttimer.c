/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

//
//外部头文件
//
#include "stm32f10x.h"

//端口定义
#define MB_TIME               TIM2            //定时器
#define MB_TIME_IRQn          TIM2_IRQn       //定时器中断
#define MB_TIME_IRQHandler    TIM2_IRQHandler //定时器中断函数名

#define MB_TIME_RCC           RCC_APB1Periph_TIM2 //定时器时钟

/* ----------------------- static functions ---------------------------------*/
void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
//    return FALSE;
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef        NVIC_InitStructure;
	
	uint16_t PrescalerValue = 0;
	
	RCC_APB1PeriphClockCmd(MB_TIME_RCC, ENABLE);
	
	//
	//HCLK为72MHz
	//时基频率72 / （1 + Prescaler) = 20KHz
	//
	PrescalerValue = (uint16_t)((SystemCoreClock / 20000) - 1);
	//
	//初始化定时器参数
	//
	TIM_TimeBaseStructure.TIM_Period = (uint16_t)usTim1Timerout50us;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MB_TIME, &TIM_TimeBaseStructure);
	//
	//使能预装
	//
	TIM_ARRPreloadConfig(MB_TIME, ENABLE);
	
	//
	//初始化中断优先级
	//
	NVIC_InitStructure.NVIC_IRQChannel                   = MB_TIME_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearITPendingBit(MB_TIME, TIM_IT_Update);
	TIM_ITConfig(MB_TIME, TIM_IT_Update, DISABLE);
	TIM_Cmd(MB_TIME, DISABLE);
	
	return TRUE;
}


void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
	
	TIM_ClearITPendingBit(MB_TIME, TIM_IT_Update);
	TIM_ITConfig(MB_TIME, TIM_IT_Update, ENABLE);
	TIM_SetCounter(MB_TIME, 0x00000000);
	TIM_Cmd(MB_TIME, ENABLE);
}

void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
	
	TIM_ClearITPendingBit(MB_TIME, TIM_IT_Update);
	TIM_ITConfig(MB_TIME, TIM_IT_Update, DISABLE);
	TIM_SetCounter(MB_TIME, 0x00000000);
	TIM_Cmd(MB_TIME, DISABLE);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

/**
  *****************************************************************************
  * @Name   : 定时器中断服务函数
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void MB_TIME_IRQHandler(void)
{
	if (TIM_GetITStatus(MB_TIME, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(MB_TIME, TIM_IT_Update);
		prvvTIMERExpiredISR();
	}
}

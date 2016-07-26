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
//�ⲿͷ�ļ�
//
#include "stm32f10x.h"

//�˿ڶ���
#define MB_TIME               TIM2            //��ʱ��
#define MB_TIME_IRQn          TIM2_IRQn       //��ʱ���ж�
#define MB_TIME_IRQHandler    TIM2_IRQHandler //��ʱ���жϺ�����

#define MB_TIME_RCC           RCC_APB1Periph_TIM2 //��ʱ��ʱ��

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
	//HCLKΪ72MHz
	//ʱ��Ƶ��72 / ��1 + Prescaler) = 20KHz
	//
	PrescalerValue = (uint16_t)((SystemCoreClock / 20000) - 1);
	//
	//��ʼ����ʱ������
	//
	TIM_TimeBaseStructure.TIM_Period = (uint16_t)usTim1Timerout50us;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(MB_TIME, &TIM_TimeBaseStructure);
	//
	//ʹ��Ԥװ
	//
	TIM_ARRPreloadConfig(MB_TIME, ENABLE);
	
	//
	//��ʼ���ж����ȼ�
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
  * @Name   : ��ʱ���жϷ�����
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

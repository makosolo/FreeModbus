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
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

//
//添加外部头文件
//
#include "stm32f10x.h"

//端口定义
#define MB_USART               USART1            //串口
#define MB_USART_IRQn          USART1_IRQn       //串口中断
#define MB_USART_IRQHandler    USART1_IRQHandler //串口中断函数名

#define MB_RCC_ALL             (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA) //串口时钟
#define MB_GPIO_PORT_TX        GPIOA       //发送端口
#define MB_GPIO_PIN_TX         GPIO_Pin_9  //发送引脚
#define MB_GPIO_PORT_RX        GPIOA       //接收端口
#define MB_GPIO_PIN_RX         GPIO_Pin_10 //接收引脚
//#define MB_GPIO_PORT_RE        GPIOG       //485方向控制端口
//#define MB_GPIO_PIN_RE         GPIO_Pin_9   //485方向控制引脚

/* ----------------------- static functions ---------------------------------*/
void prvvUARTTxReadyISR( void );
void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	
	if (xRxEnable)  //接收使能
	{
		USART_ITConfig(MB_USART, USART_IT_RXNE, ENABLE); //使能接收中断
//		GPIO_ResetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE); //接收
	}
	else  //失能
	{
		USART_ITConfig(MB_USART, USART_IT_RXNE, DISABLE); //失能接收中断
//		GPIO_SetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE);    //恢复发送
	}
	
	if (xTxEnable)  //发送使能
	{
		USART_ITConfig(MB_USART, USART_IT_TXE, ENABLE); //使能
//		GPIO_SetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE);  //发送
	}
	else  //失能
	{
		USART_ITConfig(MB_USART, USART_IT_TXE, DISABLE); //失能
//		GPIO_ResetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE); //设置接收
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
//    return FALSE;
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	(void)ucPORT;  //不修改串口号
	(void)ucDataBits;  //不修改数据位长度
	(void)eParity;  //不修改检验格式
	
    //GPIO端口设置
	RCC_APB2PeriphClockCmd(MB_RCC_ALL, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(MB_USART);  //复位串口1
	 //USARTX_TX
    GPIO_InitStructure.GPIO_Pin   = MB_GPIO_PIN_TX; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(MB_GPIO_PORT_TX, &GPIO_InitStructure); //初始化PA9
   
    //USARTX_RX
    GPIO_InitStructure.GPIO_Pin = MB_GPIO_PIN_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(MB_GPIO_PORT_RX, &GPIO_InitStructure);  //初始化PA10

	//485芯片发送接收控制管脚
//    GPIO_InitStructure.GPIO_Pin   = MB_GPIO_PIN_RE; 
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_Init(MB_GPIO_PORT_RE, &GPIO_InitStructure); 

   //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = MB_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = ulBaudRate;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(MB_USART, &USART_InitStructure); //初始化串口
    USART_ITConfig(MB_USART, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(MB_USART, ENABLE);                    //使能串口 
	
	return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    
	USART_SendData(MB_USART, ucByte);  //发送一个字节
	
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    
	*pucByte = USART_ReceiveData(MB_USART);  //接收一个字节
	
	return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/**
  *****************************************************************************
  * @Name   : 串口1中断服务函数
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
void MB_USART_IRQHandler(void)
{
	if (USART_GetITStatus(MB_USART, USART_IT_RXNE) == SET) //接收中断
	{
		prvvUARTRxISR();
		USART_ClearITPendingBit(MB_USART, USART_IT_RXNE);
	}
	
	if (USART_GetITStatus(MB_USART, USART_IT_TXE) == SET)  //发送中断
	{
		prvvUARTTxReadyISR();
		USART_ClearITPendingBit(MB_USART, USART_IT_TXE);
	}
}

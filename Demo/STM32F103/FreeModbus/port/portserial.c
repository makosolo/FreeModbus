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
//����ⲿͷ�ļ�
//
#include "stm32f10x.h"

//�˿ڶ���
#define MB_USART               USART1            //����
#define MB_USART_IRQn          USART1_IRQn       //�����ж�
#define MB_USART_IRQHandler    USART1_IRQHandler //�����жϺ�����

#define MB_RCC_ALL             (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA) //����ʱ��
#define MB_GPIO_PORT_TX        GPIOA       //���Ͷ˿�
#define MB_GPIO_PIN_TX         GPIO_Pin_9  //��������
#define MB_GPIO_PORT_RX        GPIOA       //���ն˿�
#define MB_GPIO_PIN_RX         GPIO_Pin_10 //��������
//#define MB_GPIO_PORT_RE        GPIOG       //485������ƶ˿�
//#define MB_GPIO_PIN_RE         GPIO_Pin_9   //485�����������

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
	
	if (xRxEnable)  //����ʹ��
	{
		USART_ITConfig(MB_USART, USART_IT_RXNE, ENABLE); //ʹ�ܽ����ж�
//		GPIO_ResetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE); //����
	}
	else  //ʧ��
	{
		USART_ITConfig(MB_USART, USART_IT_RXNE, DISABLE); //ʧ�ܽ����ж�
//		GPIO_SetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE);    //�ָ�����
	}
	
	if (xTxEnable)  //����ʹ��
	{
		USART_ITConfig(MB_USART, USART_IT_TXE, ENABLE); //ʹ��
//		GPIO_SetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE);  //����
	}
	else  //ʧ��
	{
		USART_ITConfig(MB_USART, USART_IT_TXE, DISABLE); //ʧ��
//		GPIO_ResetBits(MB_GPIO_PORT_RE, MB_GPIO_PIN_RE); //���ý���
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
//    return FALSE;
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	(void)ucPORT;  //���޸Ĵ��ں�
	(void)ucDataBits;  //���޸�����λ����
	(void)eParity;  //���޸ļ����ʽ
	
    //GPIO�˿�����
	RCC_APB2PeriphClockCmd(MB_RCC_ALL, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(MB_USART);  //��λ����1
	 //USARTX_TX
    GPIO_InitStructure.GPIO_Pin   = MB_GPIO_PIN_TX; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(MB_GPIO_PORT_TX, &GPIO_InitStructure); //��ʼ��PA9
   
    //USARTX_RX
    GPIO_InitStructure.GPIO_Pin = MB_GPIO_PIN_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(MB_GPIO_PORT_RX, &GPIO_InitStructure);  //��ʼ��PA10

	//485оƬ���ͽ��տ��ƹܽ�
//    GPIO_InitStructure.GPIO_Pin   = MB_GPIO_PIN_RE; 
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_Init(MB_GPIO_PORT_RE, &GPIO_InitStructure); 

   //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = MB_USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = ulBaudRate;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(MB_USART, &USART_InitStructure); //��ʼ������
    USART_ITConfig(MB_USART, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(MB_USART, ENABLE);                    //ʹ�ܴ��� 
	
	return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    
	USART_SendData(MB_USART, ucByte);  //����һ���ֽ�
	
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    
	*pucByte = USART_ReceiveData(MB_USART);  //����һ���ֽ�
	
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
  * @Name   : ����1�жϷ�����
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
	if (USART_GetITStatus(MB_USART, USART_IT_RXNE) == SET) //�����ж�
	{
		prvvUARTRxISR();
		USART_ClearITPendingBit(MB_USART, USART_IT_RXNE);
	}
	
	if (USART_GetITStatus(MB_USART, USART_IT_TXE) == SET)  //�����ж�
	{
		prvvUARTTxReadyISR();
		USART_ClearITPendingBit(MB_USART, USART_IT_TXE);
	}
}

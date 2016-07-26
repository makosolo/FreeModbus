/**
  ******************************************************************************
  * @file    bsp_init.c
  * @author  solo
  * @version V0.0.1
  * @date    2016-02-16
  * @brief   硬件驱动
  * @verbatim 
  *   
  * @note    
  *
  * @endverbatim  
  *                                  
  */

/* 头文件 ------------------------------------------------------------------*/
#include "sys.h"
#include "bsp_usart.h"
#include "bsp_led.h"

#include "bsp_init.h"

/*******************************************************************************
* 函数名	: NVIC_Configuration
* 描述  	: 中断配置
* 参数  	: 无
* 返回值	: 无
*******************************************************************************/
void NVIC_Configuration(void)
{
//    SCB->VTOR = FLASH_BASE | 0x8000;
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//只有0-15的先占优先级
}

/*******************************************************************************
* 函数名	: HW_Init
* 描述  	: 硬件初始化
* 参数  	: 无
* 返回值	: 无
*******************************************************************************/
void bsp_Init(void)
{
	NVIC_Configuration();//中断分组

//    uart_init(9600);
    LED_Init();
}

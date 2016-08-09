#include "sys.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"

//#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_init.h"

#include "mb.h"  //FreeModbus
#include "mbport.h"
#include "port.h"

void vTaskTaskLED0(void *pvParameters);
void vTaskTaskLED1(void *pvParameters);
void vTaskTaskModbus(void *pvParameters);

int main(void)
{
    bsp_Init();//Ӳ����ʼ��
    
    xTaskCreate(    vTaskTaskLED0,   /* ������  */
                    "vTaskTaskLED0",     /* ������    */
                    512,               /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,              /* �������  */
                    2,                 /* �������ȼ�*/
                    NULL );            /* ������  */

    xTaskCreate(    vTaskTaskLED1,   /* ������  */
                    "vTaskTaskLED1",     /* ������    */
                    512,               /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,              /* �������  */
                    2,                 /* �������ȼ�*/
                    NULL );            /* ������  */
    
    xTaskCreate(    vTaskTaskModbus,   /* ������  */
                    "vTaskTaskModbus",     /* ������    */
                    512,               /* stack��С����λword��Ҳ����4�ֽ� */
                    NULL,              /* �������  */
                    1,                 /* �������ȼ�*/
                    NULL );            /* ������  */  
                    
    vTaskStartScheduler();
    
    while(1);
}

void vTaskTaskLED0(void *pvParameters)
{
    while(1)
    {
        LED0 = !LED0;
        vTaskDelay(500);
    }
}

void vTaskTaskLED1(void *pvParameters)
{
    while(1)
    {
        LED1 = !LED1;
        vTaskDelay(250);
    }
}

void vTaskTaskModbus(void *pvParameters)
{
    usRegInputBuf[0] = 0x00;
    usRegInputBuf[1] = 0x01;
    usRegInputBuf[2] = 0x02;
    usRegInputBuf[3] = 0x03;
        
    eMBInit(MB_RTU, 0x01, 1, 115200, MB_PAR_NONE);  //��ʼ��FreeModbus
    
    eMBEnable();  //����FreeModbus
        
    while(1)
    {
       (void)eMBPoll();  //��ѯ����֡
        vTaskDelay(10);
    }
}

#ifdef  USE_FULL_ASSERT
/** 
  * @brief  Reports the name of the source file and the source line number 
  *         where the assert_param error has occurred. 
  * @param  file: pointer to the source file name 
  * @param  line: assert_param error line source number 
  * @retval None 
  */  
void assert_failed(uint8_t* file, uint32_t line)  
{  
  /* User can add his own implementation to report the file name and line number, 
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */  
   
  /* Infinite loop */  
  while (1)  
  {  
  }  
}
#else
void __aeabi_assert(const char * x1, const char * x2, int x3)
{
}
#endif

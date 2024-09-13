#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include <intrins.h>
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb_req_class.h"
#include "Para.h"

/******************************输入操作******************************/
#define USB_DP		P31		//USB DP线 上电拉高再拉低进入Boot
#define BOOT		P32		//上电拉低进入Boot
#define HALL		P33		//霍尔开关 有磁场低电平
#define KEY			P36		//按键 按下低电平
/********************************************************************/

/***********************************输出操作***********************************/
//#define LED1		P10		//LED1驱动引脚(绿)
//#define LED2		P11		//LED2驱动引脚(蓝)
#define LED1_ON		(P1_PULL_UP_ENABLE(GPIO_Pin_0))		//LED1亮
#define LED1_OFF	(P1_PULL_UP_DISABLE(GPIO_Pin_0))	//LED1灭
#define LED2_ON		(P1_PULL_UP_ENABLE(GPIO_Pin_1))		//LED2亮
#define LED2_OFF	(P1_PULL_UP_DISABLE(GPIO_Pin_1))	//LED2灭
#define LEDR_ON		LED1_ON			//右LED亮(USB接口朝上)
#define LEDR_OFF	LED1_OFF		//右LED灭
#define LEDL_ON		LED2_ON			//左LED亮
#define LEDL_OFF	LED2_OFF		//左LED灭
/******************************************************************************/

/******************************其他******************************/
//#define CS			P54		//FLASH片选
//#define CSN			P35		//无线模块片选
//#define CE			P34		//无线模块使能
//#define IRQ			P37		//无线模块中断
//#define NC1			P16		//未连接
//#define NC2			P17		//未连接
/****************************************************************/

#define ADC_VN		16	//ADC转换次数
//#define colorAngle	167	//色环的1/6

/**************************************************HID打印**************************************************/
#define PRINT_SEND(buf,n)		EP2_Send_Data(buf,n,' ')
#define PRINT0(s)				PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s))
#define PRINT1(s,x1)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1))
#define PRINT2(s,x1,x2)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2))
#define PRINT3(s,x1,x2,x3)		PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2,x3))
extern uint8_t PRINT_BUF[64];
/***********************************************************************************************************/

/******************************extern变量******************************/
extern bit CUSB_CUT, CUSB_EN, HUB_EN;//CUSB过流切断,CUSB使能,HUB使能
extern uint8_t keyNow[];//当前按键
extern uint8_t keyOld[];//上次按键

extern UINT16X ADC_DATA[3][ADC_VN + 2];
extern UINT16D ADC_USB_LIMIT;//限流值
extern uint8_t LedInput[9];//LED输入:0-NUM,1-CAPS,2-SCROLL,3-CUSB,4-HUB,5-hidCs,6-2.4G,7-CUSTOM1,8-CUSTOM2
/**********************************************************************/


void funcOutput(void);//各类输出
void funcHandle(void);//功能处理



#endif







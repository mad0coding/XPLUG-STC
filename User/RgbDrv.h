#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include <intrins.h>
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb_req_class.h"
#include "Para.h"

/******************************�������******************************/
#define USB_DP		P31		//USB DP�� �ϵ����������ͽ���Boot
#define BOOT		P32		//�ϵ����ͽ���Boot
#define HALL		P33		//�������� �дų��͵�ƽ
#define KEY			P36		//���� ���µ͵�ƽ
/********************************************************************/

/***********************************�������***********************************/
//#define LED1		P10		//LED1��������(��)
//#define LED2		P11		//LED2��������(��)
#define LED1_ON		(P1_PULL_UP_ENABLE(GPIO_Pin_0))		//LED1��
#define LED1_OFF	(P1_PULL_UP_DISABLE(GPIO_Pin_0))	//LED1��
#define LED2_ON		(P1_PULL_UP_ENABLE(GPIO_Pin_1))		//LED2��
#define LED2_OFF	(P1_PULL_UP_DISABLE(GPIO_Pin_1))	//LED2��
#define LEDR_ON		LED1_ON			//��LED��(USB�ӿڳ���)
#define LEDR_OFF	LED1_OFF		//��LED��
#define LEDL_ON		LED2_ON			//��LED��
#define LEDL_OFF	LED2_OFF		//��LED��
/******************************************************************************/

/******************************����******************************/
//#define CS			P54		//FLASHƬѡ
//#define CSN			P35		//����ģ��Ƭѡ
//#define CE			P34		//����ģ��ʹ��
//#define IRQ			P37		//����ģ���ж�
//#define NC1			P16		//δ����
//#define NC2			P17		//δ����
/****************************************************************/

#define ADC_VN		16	//ADCת������
//#define colorAngle	167	//ɫ����1/6

/**************************************************HID��ӡ**************************************************/
#define PRINT_SEND(buf,n)		EP2_Send_Data(buf,n,' ')
#define PRINT0(s)				PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s))
#define PRINT1(s,x1)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1))
#define PRINT2(s,x1,x2)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2))
#define PRINT3(s,x1,x2,x3)		PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2,x3))
extern uint8_t PRINT_BUF[64];
/***********************************************************************************************************/

/******************************extern����******************************/
extern bit CUSB_CUT, CUSB_EN, HUB_EN;//CUSB�����ж�,CUSBʹ��,HUBʹ��
extern uint8_t keyNow[];//��ǰ����
extern uint8_t keyOld[];//�ϴΰ���

extern UINT16X ADC_DATA[3][ADC_VN + 2];
extern UINT16D ADC_USB_LIMIT;//����ֵ
extern uint8_t LedInput[9];//LED����:0-NUM,1-CAPS,2-SCROLL,3-CUSB,4-HUB,5-hidCs,6-2.4G,7-CUSTOM1,8-CUSTOM2
/**********************************************************************/


void funcOutput(void);//�������
void funcHandle(void);//���ܴ���



#endif







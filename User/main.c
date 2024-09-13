
//#include "intrins.h"
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb.h"
#include "RgbDrv.h"
#include "drv_RF24L01.h"
#include "W25Qxx.h"
#include "Para.h"


/****************************************HID�����������****************************************/
UINT8I KeyBrd_if_send = 0;//���̱����Ƿ���
UINT8I Mouse_if_send = 0;//��걨���Ƿ���
UINT8I Point_if_send = 0;//ָ�뱨���Ƿ���
UINT8I Vol_if_send = 0;//ý�屨���Ƿ���

UINT8I KeyBrd_data[KB_len] = {1,0,0,0};//���1,���ܼ�,����0,��������
//���ܼ�:bit7-bit0�ֱ�ΪΪ��win alt shift ctrl,��win alt shift ctrl

UINT8I Mouse_data[5] = {2,0,0,0,0};//���2,���ܼ�,x,y,����
//���ܼ�:bit0Ϊ���,bit1Ϊ�Ҽ�,bit2Ϊ�м�,bit6Ϊx�Ƿ����,bit7Ϊy�Ƿ����

UINT8I Point_data[7] = {3,0x10,1,0xFF,0xFF,0xFF,0xFF};//���3,���ܼ�,id,x_L,x_H,y_L,y_H
//���ܼ�:bit0ΪTip Switch,bit1ΪBarrel Switch,bit2ΪInvert,bit3ΪEraser Switch,bit4ΪIn Range

UINT8I Vol_data[2] = {4,0};//���4,���ܼ�
//���ܼ�:bit0������,bit1������,bit2����,bit3������ͣ
/***********************************************************************************************/

void main(void)
{
    P_SW2 |= 0x80;  //��չ�Ĵ���(XFR)����ʹ��
	
	/****************************************�������ų�ʼ��****************************************/
	BOOT = HALL = KEY = 1;
	P3_MODE_IN_HIZ(GPIO_Pin_1);	//USB_DP����
	P3_MODE_IO_PU(GPIO_Pin_2);	//BOOT׼˫��
	P3_MODE_IO_PU(GPIO_Pin_3);	//HALL׼˫��
	P3_MODE_IO_PU(GPIO_Pin_6);	//KEY׼˫��
	/**********************************************************************************************/

	/****************************************Bootģʽ****************************************/
	delay_ms(50);	//DP�����ȴ��ⲿ����ƽ
	if(USB_DP){	//�ϵ�DP���߽���Boot
		uint8_t dpHighCount = 0;//��4λ��¼DP���� ��4λ��������
		LEDR_ON;	//��LED��
		for(dpHighCount = 0; (dpHighCount & 0x0F) < 10; dpHighCount++){
			delay_ms(1);
			if(USB_DP) dpHighCount += (1 << 4);//��μ��DP����
		}
		if(dpHighCount > (5 << 4)){	//����ʱ��DP����
			USBCON = 0x00;				//�ر�USB
			P3_MODE_IO_PU(GPIO_Pin_1);	//USB_DP׼˫��
			USB_DP = 0;					//USB_DP���� ��ֹ��ʱ��������������ʶ�����
			LEDR_OFF;	LEDL_ON;		//����ָʾ���л��󼴿ɳ����ⲿDP����
			while(KEY) delay_ms(1);		//�ȴ���������
			delay_ms(100);				//��������⵽�豸�γ�
			STC8H_SoftReset(1);			//����������Boot
		}	//�����ⲿDP����Ӧ������ǰ���
		LEDR_OFF;	//��LED��
	}
	/****************************************************************************************/
	
	if(!KEY){//�ϵ簴��Boot ���Դ���
		USBCON = 0x00;//�ر�USB
		delay_ms(200);//��������⵽�豸�γ�
		STC8H_SoftReset(1);//����������Boot
	}
	
	paraInit(0);//��ȡ���ò�����ʼ��ϵͳ
	
	/****************************************�ⲿ�жϳ�ʼ��****************************************/
	IE1 = 0;	//��INT1��־λ
	IT1 = 0;	//INT1˫�����ж�
	EX1 = 1;	//ʹ��INT1�ж�
	INTCLKO |= 0x10;	//ʹ��INT2�½����ж�
	/**********************************************************************************************/
	
	/****************************************��ʱ����ʼ��****************************************/
	PWMB_PSCR = 35000 - 1;	//����Ԥ��Ƶ�� ��Ӧ��Ƶ35M
	PWMB_ARR = 0xFFFF;		//�Զ���װ�ؼĴ���
	PWMB_CCER1 = 0x00;		//��ʹ�����벶��
	PWMB_CCMR1 = PWMB_CCMR2 = PWMB_CCMR3 = PWMB_CCMR4 = 0x00;	//��ʹ�ò���/�Ƚ�
	PWMB_CCR5 = PWMB_CCR6 = PWMB_CCR7 = PWMB_CCR8 = 0x00;		//����/�Ƚ���ֵΪ��
	PWMB_IER = 0x00;		//��ʹ���ж�
	PWMB_CR1 |= 0x81;		//ʹ�� ARR Ԥװ�أ���ʼ��ʱ
	/********************************************************************************************/
	
	/****************************************SPI��ʼ��****************************************/
	P1_MODE_OUT_PP(GPIO_Pin_5);	//SCK����
	P1_MODE_OUT_PP(GPIO_Pin_3);	//MOSI����
	P54 = P35 = 1;				//CS��CSN����
	P5_MODE_IO_PU(GPIO_Pin_4);	//CS׼˫��
	P3_MODE_IO_PU(GPIO_Pin_5);	//CSN׼˫��
	SPCTL = 0xD0;				//����SS���� ����ģʽ ģʽ0 Ƶ��SYSclk/4
	SPSTAT = 0xC0;				//���־
	/*****************************************************************************************/
	if(/*CFG_G24_MODE != 0*/1) NRF24L01_Config(0);//����ģ���ʼ��Ϊ����ģʽ
	
	/****************************************ADC��ʼ��****************************************/
//	ADCTIM = 0x3F;			//����ADC�ڲ�ʱ��
//	ADCCFG = 0x20 + 0xF;	//����Ҷ��� ����ADCʱ��Ϊϵͳʱ��/2/16
//	ADC_CONTR = 0x80;		//ʹ��ADCģ��
//	//ADCEXCFG = 0x07;		//�˼Ĵ�����ת���������ý���DMAģʽ����Ч
//	DMA_ADC_STA = 0x00;		//���ж�
//	DMA_ADC_CFG = 0x80;		//bit7 1:Enable Interrupt
//	DMA_ADC_RXAH = (uint8_t)((uint16_t)ADC_DATA >> 8);	//ADCת�����ݴ洢��ַ
//	DMA_ADC_RXAL = (uint8_t)ADC_DATA;
//	DMA_ADC_CFG2 = 0x0B;	//ÿ��ͨ��ת��16��
//	DMA_ADC_CHSW0 = 0xC0;	//ADCͨ��ʹ�� ADC6(P16) ADC7(P17)
//	DMA_ADC_CHSW1 = 0x80;	//ADCͨ��ʹ�� ADC15(1.19V�ο�)
//	DMA_ADC_CR = 0xC0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
	/*****************************************************************************************/
	
//	if(/*!(RSTFLAG & 0x04)*/(RSTFLAG & 0x10)) usbMode = !KEY;//����Ϊ�����λ

	usb_init();	//USB��ʼ��
	EA = 1;		//���жϿ���

    while(1){
		LEDL_ON;
		delay_ms(250);
		LEDR_ON;
		delay_ms(250);
		LEDL_OFF;
		delay_ms(250);
		LEDR_OFF;
		delay_ms(250);
		
//		funcHandle();	//���ܴ���

//		if(KeyBrd_if_send){	//�����̱�����Ҫ����
//			EP1_Send_Data(KeyBrd_data, sizeof(KeyBrd_data));
//			delay_ms(8);
//		}
//		if(Mouse_if_send){	//����걨����Ҫ����
//			EP1_Send_Data(Mouse_data, sizeof(Mouse_data));
//			delay_ms(8);
//		}
//		if(Point_if_send){	//������������Ҫ����
//			EP1_Send_Data(Point_data, sizeof(Point_data));
//			delay_ms(8);
//		}
//		if(Vol_if_send){	//��ý�屨����Ҫ����
//			EP1_Send_Data(Vol_data, sizeof(Vol_data));
//			delay_ms(8);
//		}
//		if(!(KeyBrd_if_send || Mouse_if_send || Point_if_send || Vol_if_send)) delay_ms(1);//�ޱ�����Ҫ����
    }
}



/********************* INT1�жϺ��� *************************/
void INT1_ISR(void) interrupt 2 {	//HALL INT1˫�����ж�
	if(!HALL){//�д�
		LEDL_ON;
	USBCON = 0x00;//�ر�USB
	delay_ms(200);//��������⵽�豸�γ�
	STC8H_SoftReset(1);//����������Boot
	}
	else{//�޴�
		LEDL_OFF;
	}
}

/********************* INT2�жϺ��� *************************/
void INT2_ISR(void) interrupt 10 {	//KEY INT2�½����ж�
//	USBCON = 0x00;//�ر�USB
//	delay_ms(200);//��������⵽�豸�γ�
//	STC8H_SoftReset(1);//����������Boot
}










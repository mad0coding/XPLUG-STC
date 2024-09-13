/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/*---------------------------------------------------------------------*/

#include "intrins.h"
#include "DEBUG.H"
#include "usb.h"
#include "usb_req_class.h"
#include "RgbDrv.h"
#include "Para.h"

#define LED_NUM		LedInput[0]//��������
#define LED_CAPS	LedInput[1]//��д����
#define LED_SCROLL	LedInput[2]//��������


BYTE bHidIdle;

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case GET_REPORT:	usb_get_report();	break;
    case SET_REPORT:	usb_set_report();	break;
    case GET_IDLE:		usb_get_idle();		break;
    case SET_IDLE:		usb_set_idle();		break;
    case GET_PROTOCOL:	usb_get_protocol();	break;
    case SET_PROTOCOL:	usb_set_protocol();	break;
    default:	usb_setup_stall();	return;
    }
}

void usb_get_report()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_set_report()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = UsbBuffer;
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}

void usb_get_idle()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = &bHidIdle;
    Ep0State.wSize = 1;

    usb_setup_in();
}

void usb_set_idle()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    bHidIdle = Setup.wValueH;

    usb_setup_status();
}

void usb_get_protocol(){
    usb_setup_stall();
}

void usb_set_protocol(){
    usb_setup_stall();
}

void EP1_Send_Data(BYTE *pdat, BYTE cnt){//�˵�1����
	UsbInBusy1 = 1;	//æ��־λ��λ
	IE2 &= ~0x80;   //EUSB = 0;
	usb_write_reg(INDEX, 1);		//�趨�˵�
	while(cnt--) usb_write_reg(FIFO1, *pdat++);	//��������FIFO
	usb_write_reg(INCSR1, INIPRDY);	//ָʾ���ݰ���׼�����
	IE2 |= 0x80;    //EUSB = 1;
}

void EP2_Send_Data(BYTE *pdat, BYTE cnt, BYTE pad){//�˵�2����
	BYTE i;
	UsbInBusy2 = 1;	//æ��־λ��λ
	IE2 &= ~0x80;	//EUSB = 0;
	usb_write_reg(INDEX, 2);		//�趨�˵�
	for(i = 0; i < 64; i++){		//��Ҫ64�ֽ�����
		if(i < cnt) usb_write_reg(FIFO2, *pdat++);	//��������FIFO
		else usb_write_reg(FIFO2, pad);				//����ֽ�����FIFO
	}
	usb_write_reg(INCSR1, INIPRDY);	//ָʾ���ݰ���׼�����
	IE2 |= 0x80;	//EUSB = 1;
}

void EP1_Out_Led(void)//����LED����
{
    BYTE led = usb_bulk_intr_out(UsbBuffer, 1);	//�ݴ����ݳ���
    if(led && led <= 2){	//1~2�ֽ����ݾ���LED����
        led = UsbBuffer[led - 1];
        LED_NUM = !!(led & 0x01);		//��������
        LED_CAPS = !!(led & 0x02);		//��д����
        LED_SCROLL = !!(led & 0x04);	//��������
    }
}

void EP2_Out_Cmd(void)//��������������
{
    BYTE len = usb_bulk_intr_out(UsbBuffer, 2);	//�ݴ����ݳ���
	if(len != 64) return;
	if(UsbBuffer[0] == 0x01 && UsbBuffer[1] == 64){//��������
		memcpy(para, UsbBuffer + 2, PARA_LEN);//��������
		UsbBuffer[1] = ~UsbBuffer[1];//��ת��Ϊ��Ӧ
		EP2_Send_Data(UsbBuffer, 2, 0);//�˵�2����
		paraWrite();//д�����
		paraInit(1);//�ò�����ʼ��ϵͳ
	}
	else if(UsbBuffer[0] == 'M'){//��������
		if(UsbBuffer[1] == 'A'){//ASCIIģʽ
			len = sprintf(UsbBuffer, "%4umV %4umA  %04Xh %04Xh", 
			(uint16_t)((uint32_t)ADC_DATA[0][ADC_VN + 1] * 825 / 512),				//��ѹ
			(uint16_t)((uint32_t)(4095 - ADC_DATA[1][ADC_VN + 1]) * 825 / 2048),	//����
			ADC_DATA[0][ADC_VN + 1], 4095 - ADC_DATA[1][ADC_VN + 1]);				//��ѹ��������ֵ
			EP2_Send_Data(UsbBuffer, len, ' ');//�˵�2����
		}
		else if(UsbBuffer[1] == 'B'){//������ģʽ
			UsbBuffer[1] = ~UsbBuffer[1];//��ת��Ϊ��Ӧ
			*(uint16_t*)(UsbBuffer + 2) = ADC_DATA[0][ADC_VN + 1];			//��ѹ
			*(uint16_t*)(UsbBuffer + 4) = 4095 - ADC_DATA[1][ADC_VN + 1];	//����
			*(uint16_t*)(UsbBuffer + 6) = ADC_DATA[2][ADC_VN + 1];			//1.19V�ο�����ֵ
			*(uint16_t*)(UsbBuffer + 8) = *(uint16_t*)&CHIPID7;				//1.19V�ο�Ӳ��ֵ
			*(uint16_t*)(UsbBuffer + 10) = ((uint8_t)CUSB_CUT << 4) | CUSB_EN;//CUSB�����ض�,CUSBʹ��
			EP2_Send_Data(UsbBuffer, len, 0);//�˵�2����
		}
	}
	else if(UsbBuffer[0] == 'R'){		//��λ����(����ʹ��)
		if(UsbBuffer[1] == 'A'){//��������
			USBCON = 0x00;		//�ر�USB
			delay_ms(200);		//��������⵽�豸�γ�
			STC8H_SoftReset(0);	//�����������û�����
		}
		else if(UsbBuffer[1] == 'B'){	//��ת��Boot
			USBCON = 0x00;		//�ر�USB
			delay_ms(200);		//��������⵽�豸�γ�
			STC8H_SoftReset(1);	//����������Boot
		}
	}
}













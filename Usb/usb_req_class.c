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

#define LED_NUM		LedInput[0]//数字锁定
#define LED_CAPS	LedInput[1]//大写锁定
#define LED_SCROLL	LedInput[2]//滚动锁定


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

void EP1_Send_Data(BYTE *pdat, BYTE cnt){//端点1发送
	UsbInBusy1 = 1;	//忙标志位置位
	IE2 &= ~0x80;   //EUSB = 0;
	usb_write_reg(INDEX, 1);		//设定端点
	while(cnt--) usb_write_reg(FIFO1, *pdat++);	//数据填入FIFO
	usb_write_reg(INCSR1, INIPRDY);	//指示数据包已准备完成
	IE2 |= 0x80;    //EUSB = 1;
}

void EP2_Send_Data(BYTE *pdat, BYTE cnt, BYTE pad){//端点2发送
	BYTE i;
	UsbInBusy2 = 1;	//忙标志位置位
	IE2 &= ~0x80;	//EUSB = 0;
	usb_write_reg(INDEX, 2);		//设定端点
	for(i = 0; i < 64; i++){		//需要64字节数据
		if(i < cnt) usb_write_reg(FIFO2, *pdat++);	//数据填入FIFO
		else usb_write_reg(FIFO2, pad);				//填充字节填入FIFO
	}
	usb_write_reg(INCSR1, INIPRDY);	//指示数据包已准备完成
	IE2 |= 0x80;	//EUSB = 1;
}

void EP1_Out_Led(void)//键盘LED控制
{
    BYTE led = usb_bulk_intr_out(UsbBuffer, 1);	//暂存数据长度
    if(led && led <= 2){	//1~2字节数据就是LED控制
        led = UsbBuffer[led - 1];
        LED_NUM = !!(led & 0x01);		//数字锁定
        LED_CAPS = !!(led & 0x02);		//大写锁定
        LED_SCROLL = !!(led & 0x04);	//滚动锁定
    }
}

void EP2_Out_Cmd(void)//控制与配置命令
{
    BYTE len = usb_bulk_intr_out(UsbBuffer, 2);	//暂存数据长度
	if(len != 64) return;
	if(UsbBuffer[0] == 0x01 && UsbBuffer[1] == 64){//配置命令
		memcpy(para, UsbBuffer + 2, PARA_LEN);//拷贝数据
		UsbBuffer[1] = ~UsbBuffer[1];//反转作为回应
		EP2_Send_Data(UsbBuffer, 2, 0);//端点2发送
		paraWrite();//写入参数
		paraInit(1);//用参数初始化系统
	}
	else if(UsbBuffer[0] == 'M'){//采样命令
		if(UsbBuffer[1] == 'A'){//ASCII模式
			len = sprintf(UsbBuffer, "%4umV %4umA  %04Xh %04Xh", 
			(uint16_t)((uint32_t)ADC_DATA[0][ADC_VN + 1] * 825 / 512),				//电压
			(uint16_t)((uint32_t)(4095 - ADC_DATA[1][ADC_VN + 1]) * 825 / 2048),	//电流
			ADC_DATA[0][ADC_VN + 1], 4095 - ADC_DATA[1][ADC_VN + 1]);				//电压电流采样值
			EP2_Send_Data(UsbBuffer, len, ' ');//端点2发送
		}
		else if(UsbBuffer[1] == 'B'){//二进制模式
			UsbBuffer[1] = ~UsbBuffer[1];//反转作为回应
			*(uint16_t*)(UsbBuffer + 2) = ADC_DATA[0][ADC_VN + 1];			//电压
			*(uint16_t*)(UsbBuffer + 4) = 4095 - ADC_DATA[1][ADC_VN + 1];	//电流
			*(uint16_t*)(UsbBuffer + 6) = ADC_DATA[2][ADC_VN + 1];			//1.19V参考测量值
			*(uint16_t*)(UsbBuffer + 8) = *(uint16_t*)&CHIPID7;				//1.19V参考硬件值
			*(uint16_t*)(UsbBuffer + 10) = ((uint8_t)CUSB_CUT << 4) | CUSB_EN;//CUSB过流关断,CUSB使能
			EP2_Send_Data(UsbBuffer, len, 0);//端点2发送
		}
	}
	else if(UsbBuffer[0] == 'R'){		//复位命令(调试使用)
		if(UsbBuffer[1] == 'A'){//重新运行
			USBCON = 0x00;		//关闭USB
			delay_ms(200);		//让主机检测到设备拔出
			STC8H_SoftReset(0);	//重启并进入用户程序
		}
		else if(UsbBuffer[1] == 'B'){	//跳转到Boot
			USBCON = 0x00;		//关闭USB
			delay_ms(200);		//让主机检测到设备拔出
			STC8H_SoftReset(1);	//重启并进入Boot
		}
	}
}













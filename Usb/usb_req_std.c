/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/*---------------------------------------------------------------------*/

//#include "DEBUG.H"//若引入该文件里的数据类型会造成USB异常
#include "stc.h"
#include "usb.h"
#include "usb_req_std.h"
#include "usb_req_class.h"

#define UINT8C	unsigned char code//这里直接宏定义该类型

extern uint8_t idata usbMode;//USB模式

/**************************************************HID模式描述符**************************************************/
/*设备描述符*/
UINT8C DevDesc[/*18*/] = {//设备描述符
	0x12,//1. 第一个字节 0x12 表示该设备描述符的长度为 18 字节。
	0x01,//2. 第二个字节 0x01 表示该描述符的类型为设备描述符 (Device Descriptor)。
	0x00,//0x10,//3. 第三个字节 0x10 表示USB规范的版本号 (USB Specification Release Number) 的低字节。
	0x02,//0x01,//4. 第四个字节 0x01 表示USB规范的版本号的高字节。
	0x00,//5. 第五个字节 0x00 表示该设备的设备类代码 (Device Class Code)。
	0x00,//6. 第六个字节 0x00 表示该设备的设备子类代码 (Device Subclass Code)。
	0x00,//7. 第七个字节 0x00 表示该设备的设备协议代码 (Device Protocol Code)。
	THIS_ENDP0_SIZE,//8. 第八个字节表示该设备的最大包长度 (Max Packet Size)。
	Vendor_ID & 0xFF,//9. 第九个字节表示该设备的厂商ID (Vendor ID) 的低字节。
	Vendor_ID >> 8,//10. 第十个字节表示该设备的厂商ID的高字节。
	Product_ID & 0xFF,//11. 第十一个字节表示该设备的产品ID (Product ID) 的低字节。
	Product_ID >> 8,//12. 第十二个字节表示该设备的产品ID的高字节。
	0x00,//13. 第十三个字节 0x00 表示该设备的设备版本号 (Device Release Number) 的低字节。
	0x01,//14. 第十四个字节 0x01 表示该设备的设备版本号的高字节。
	0x01,//15. 第十五个字节 0x01 表示该设备的制造商字符串描述符索引 (Manufacturer String Index)。
	0x02,//16. 第十六个字节 0x02 表示该设备的产品字符串描述符索引 (Product String Index)。
	0x00,//17. 第十七个字节 0x00 表示该设备的序列号字符串描述符索引 (Serial Number String Index)。
	0x01//18. 第十八个字节 0x01 表示该设备支持的配置数目 (Number of Configurations)。
};

/*字符串描述符*/
UINT8C MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };//语言描述符
UINT8C MyProdInfo[] = {20,0x03,'X',0,'P',0,'L',0,'U',0,'G',0,'-',0,'S',0,'T',0,'C',0};//产品名称
UINT8C MyManuInfo[] = {36,0x03,
	'L',0,'i',0,'g',0,'h',0,'t',0,'&',0,'E',0,'l',0,'e',0,'c',0,'t',0,'r',0,'i',0,'c',0,'i',0,'t',0,'y',0
};//制造者名称

/*HID类报文描述符*/
UINT8C KeyRepDesc[] = {//HID报文描述符
	//键盘
    0x05, 0x01,					//	USAGE_PAGE (Generic Desktop)
    0x09, 0x06,					//	USAGE (Keyboard)
    0xa1, 0x01,					//	COLLECTION (Application)
    0x85, 0x01,					//		REPORT_ID (1)
    0x05, 0x07,					//		USAGE_PAGE (Keyboard)
    0x19, 0xe0,					//		USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,					//		USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,					//		LOGICAL_MINIMUM (0)
    0x25, 0x01,					//		LOGICAL_MAXIMUM (1)
    0x75, 0x01,					//		REPORT_SIZE (1)
    0x95, 0x08,					//		REPORT_COUNT (8)
    0x81, 0x02,					//		INPUT (Data,Var,Abs)
    0x95, 0x01,					//		REPORT_COUNT (1)
    0x75, 0x08,					//		REPORT_SIZE (8)
    0x81, 0x03,					//		INPUT (Cnst,Var,Abs)
	
    0x95, 0x13,					//		REPORT_COUNT (19)按键数
    0x75, 0x08,					//		REPORT_SIZE (8)
    0x15, 0x00,					//		LOGICAL_MINIMUM (0)
    0x25, 0x65,					//		LOGICAL_MAXIMUM (101)
    0x05, 0x07,					//		USAGE_PAGE (Keyboard)
    0x19, 0x00,					//		USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,					//		USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,					//		INPUT (Data,Ary,Abs)
	
	0x95, 0x05,					//		REPORT_COUNT (5)
    0x75, 0x01,					//		REPORT_SIZE (1)
    0x05, 0x08,					//		USAGE_PAGE (LEDs)
    0x19, 0x01,					//		USAGE_MINIMUM (Num Lock)
    0x29, 0x05,					//		USAGE_MAXIMUM (Kana)
    0x91, 0x02,					//		OUTPUT (Data,Var,Abs)
    0x95, 0x01,					//		REPORT_COUNT (1)
    0x75, 0x03,					//		REPORT_SIZE (3)
    0x91, 0x03,					//		OUTPUT (Cnst,Var,Abs)
    0xc0,						//	END_COLLECTION
	
	//鼠标
    0x05, 0x01,					//	USAGE_PAGE (Generic Desktop)
    0x09, 0x02,					//	USAGE (Mouse)
    0xa1, 0x01,					//	COLLECTION (Application)
    0x85, 0x02,					//		REPORT_ID (2)
    0x09, 0x01,					//		USAGE (Pointer)
    0xa1, 0x00,					//		COLLECTION (Physical)
    0x05, 0x09,					//			USAGE_PAGE (Button)
    0x19, 0x01,					//			USAGE_MINIMUM (Button 1)
    0x29, 0x03,					//			USAGE_MAXIMUM (Button 3)
    0x15, 0x00,					//			LOGICAL_MINIMUM (0)
    0x25, 0x01,					//			LOGICAL_MAXIMUM (1)
    0x95, 0x03,					//			REPORT_COUNT (3)
    0x75, 0x01,					//			REPORT_SIZE (1)
    0x81, 0x02,					//			INPUT (Data,Var,Abs)
    0x95, 0x01,					//			REPORT_COUNT (1)
    0x75, 0x05,					//			REPORT_SIZE (5)
    0x81, 0x03,					//			INPUT (Cnst,Var,Abs)
    0x05, 0x01,					//			USAGE_PAGE (Generic Desktop)
    0x09, 0x30,					//			USAGE (X)
    0x09, 0x31,					//			USAGE (Y)
	0x09, 0x38,					//			USAGE (Wheel)	
    0x15, 0x81,					//			LOGICAL_MINIMUM (-127)
    0x25, 0x7f,					//			LOGICAL_MAXIMUM (127)
    0x75, 0x08,					//			REPORT_SIZE (8)
    0x95, 0x03,					//			REPORT_COUNT (3)
    0x81, 0x06,					//			INPUT (Data,Var,Rel)Rel相对值,Abs绝对值
    0xc0,						//		END_COLLECTION
    0xc0,						//	END_COLLECTION
	
	//指针位置
	0x05, 0x0d,					// USAGE_PAGE (Digitizers)
    //0x09, 0x02,					// USAGE (Pen)
	0x09, 0x04,					// USAGE (Touch Screen)
    0xa1, 0x01,					// COLLECTION (Application)
    0x85, 0x03,					//		REPORT_ID (3)
	0x09, 0x22,					//		USAGE (Finger)
    //0x09, 0x20,					//		USAGE (Stylus)
    0xa1, 0x00,					//		COLLECTION (Physical)
    0x09, 0x42,					//			USAGE (Tip Switch)
    0x09, 0x44,					//			USAGE (Barrel Switch)
    0x09, 0x3c,					//			USAGE (Invert)
    0x09, 0x45,					//			USAGE (Eraser Switch)
	0x09, 0x32,					//			USAGE (In Range)
    0x15, 0x00,					//			LOGICAL_MINIMUM (0)
    0x25, 0x01,					//			LOGICAL_MAXIMUM (1)
    0x75, 0x01,					//			REPORT_SIZE (1)
    0x95, 0x05,					//			REPORT_COUNT (5)
    0x81, 0x02,					//			INPUT (Data,Var,Abs)
    0x95, 0x01,					//			REPORT_COUNT (1)
    0x75, 0x03,					//			REPORT_SIZE (3)
    0x81, 0x03,					//			INPUT (Cnst,Var,Abs)
	0x75, 0x08,					//			REPORT_SIZE (8)
    0x09, 0x51,					//			USAGE (Contact Identifier)
    0x95, 0x01,					//			REPORT_COUNT (1)
    0x81, 0x02,					//			INPUT (Data,Var,Abs)
    0x05, 0x01,					//			USAGE_PAGE (Generic Desktop)
	0x75, 0x10,					//			REPORT_SIZE (16)
	0x26, 0xFF, 0x7F,			//			LOGICAL_MAXIMUM (32767)
	0x46, 0xFF, 0x7F,			//			PHYSICAL_MAXIMUM (32767)
    0x09, 0x30,					//			USAGE (X)
    0x09, 0x31,					//			USAGE (Y)
	0x95, 0x02,					//			REPORT_COUNT (2)
	0x81, 0x02,					//			INPUT (Data,Var,Abs)Rel相对值,Abs绝对值
    0xc0,						//		END_COLLECTION
    0xc0,						//	END_COLLECTION
	
	//媒体控制
	0x05,0x0C,					//	USAGE_PAGE (Consumer)
	0x09,0x01,					//	USAGE (Consumer Control)
	0xA1,0x01,					//	COLLECTION (Applicatior)
	0x85,0x04,					//		REPORT_ID (4)
	0xA1,0x00,					//		COLLECTION(Physical)
	0x09,0xE9,					//			USAGE (Volume Increment)
	0x09,0xEA,					//			USAGE (Volume Decrement)
	0x09,0xE2,					//			USAGE (Mute)
	0x09,0xCD,					//			USAGE (Play/Pause)
	0x09,0xB5,					//			USAGE (Scan Next Track)
	0x09,0xB6,					//			USAGE (Scan Previous Track)
	0x35,0x00,					//			PHYSICAL_MINIMUM (0)
	0x45,0x07,					//			PHYSICAL_MAXIMUM (7)
	0x15,0x00,					//			LOGICAL_MINIMUM (0)
	0x25,0x01,					//			LOCAL_MAXIMUM (1)
	0x75,0x01,					//			REPORT_SIZE (1)
	0x95,0x06,					//			REPORT_COUNT (6)
	0x81,0x02,					//			INPUT (Data,Var,Abs)
	0x75,0x01,					//			REPORT_SIZE (1)
	0x95,0x02,					//			REPORT_COUNT (2)
	0x81,0x01,					//			INPUT (Cnst,Ary,Abs)
	0xC0,						//		END_COLLECTION
	0xC0,						//	END_COLLECTION
};
UINT8C ComRepDesc[] = {//自定义HID报文描述符
	0x06, 0x00, 0xff, 	// Usage page Vendor defined
	0x09, 0x01, 		// Local usage 1
	0xa1, 0x01, 		// Collation Application
	0x09, 0x02, 		// Local usage 2
	0x15, 0x00, 		// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x75, 0x08,  		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x81, 0x06,  		// Input ( Data, Relative, Wrap )
	0x09, 0x02, 		// Local usage 2
	0x15, 0x00,  		// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x75, 0x08, 		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x91, 0x06, 		// Output ( Data, Relative, Wrap )
	0xc0,				// END_COLLECTION
};

/*配置描述符*/
UINT8C CfgDesc[/*9+32+32*/] = {//配置描述符
	0x09,//1. 第一个字节 0x09 表示该配置描述符的长度为 9 字节。
	0x02,//2. 第二个字节 0x02 表示该描述符的类型为配置描述符 (Configuration Descriptor)。
	9+32+32,//3. 第三个字节 xx 表示配置描述符的总长度 (Total Length) 的低字节。
	0x00,//4. 第四个字节 0x00 表示配置描述符的总长度的高字节。
	USBD_MAX_NUM_INTERFACES,//5. 第五个字节 0x02 表示该配置的接口数目 (NumInterfaces)。
	0x01,//6. 第六个字节 0x01 表示配置描述符的标识符 (Configuration Value)。
	0x00,//7. 第七个字节 0x00 表示该配置的描述字符串索引 (Configuration String Index)。
	0xA0,//8. 第八个字节 0xA0 表示该配置的特性标志 (Attributes)。A0=0b10100000，bit7必须1，bit6为是否自供电，bit5为是否远程唤醒
	0xF0,//9. 第九个字节 0x32 表示该配置的最大功率 (Max Power)。这里的值 0x32 表示设备使用的最大电流为 (50 mA * 0x32) = 100 mA。

    0x09,0x04,USBD_HID_INTERFACE,0x00,2,0x03,0x01,0x00,0x00,//HID接口描述符,1端点
    0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc)&0xFF,sizeof(KeyRepDesc)>>8,//HID类描述符
    0x07,0x05,HID_EPIN_ADDR,0x03,ENDP1_IN_SIZE,0x00,HID_FS_BINTERVAL,//端点描述符,IN端点1
	0x07,0x05,HID_EPOUT_ADDR,0x03,ENDP1_OUT_SIZE,0x00,HID_FS_BINTERVAL,//端点描述符,OUT端点1
	
	0x09,0x04,USBD_CUSTOM_HID_INTERFACE,0x00,2,0x03,0x00,0x00,0x00,//CustomHID接口描述符,2端点
    0x09,0x21,0x10,0x01,0x00,0x01,0X22,sizeof(ComRepDesc)&0xFF,sizeof(ComRepDesc)>>8,//HID类描述符
    0x07,0x05,CUSTOM_HID_EPIN_ADDR,0x03,ENDP2_IN_SIZE,0x00,CUSTOM_HID_FS_BINTERVAL,//端点描述符,IN端点2
	0x07,0x05,CUSTOM_HID_EPOUT_ADDR,0x03,ENDP2_OUT_SIZE,0x00,CUSTOM_HID_FS_BINTERVAL,//端点描述符,OUT端点2
};
/*****************************************************************************************************************/

/**************************************************U盘模式描述符**************************************************/
/*设备描述符*/
UINT8C UdiskDevDesc[/*18*/] = {//设备描述符
	0x12,//1. 第一个字节 0x12 表示该设备描述符的长度为 18 字节。
	0x01,//2. 第二个字节 0x01 表示该描述符的类型为设备描述符 (Device Descriptor)。
	0x00,//0x10,//3. 第三个字节 0x10 表示USB规范的版本号 (USB Specification Release Number) 的低字节。
	0x02,//0x01,//4. 第四个字节 0x01 表示USB规范的版本号的高字节。
	0x00,//5. 第五个字节 0x00 表示该设备的设备类代码 (Device Class Code)。
	0x00,//6. 第六个字节 0x00 表示该设备的设备子类代码 (Device Subclass Code)。
	0x00,//7. 第七个字节 0x00 表示该设备的设备协议代码 (Device Protocol Code)。
	THIS_ENDP0_SIZE,//8. 第八个字节表示该设备的最大包长度 (Max Packet Size)。
	Vendor_ID & 0xFF,//9. 第九个字节表示该设备的厂商ID (Vendor ID) 的低字节。
	Vendor_ID >> 8,//10. 第十个字节表示该设备的厂商ID的高字节。
	Product_ID_U & 0xFF,//11. 第十一个字节表示该设备的产品ID (Product ID) 的低字节。
	Product_ID_U >> 8,//12. 第十二个字节表示该设备的产品ID的高字节。
	0x00,//13. 第十三个字节 0x00 表示该设备的设备版本号 (Device Release Number) 的低字节。
	0x01,//14. 第十四个字节 0x01 表示该设备的设备版本号的高字节。
	0x01,//15. 第十五个字节 0x01 表示该设备的制造商字符串描述符索引 (Manufacturer String Index)。
	0x02,//16. 第十六个字节 0x02 表示该设备的产品字符串描述符索引 (Product String Index)。
	0x03,//17. 第十七个字节 0x00 表示该设备的序列号字符串描述符索引 (Serial Number String Index)。
	0x01//18. 第十八个字节 0x01 表示该设备支持的配置数目 (Number of Configurations)。
};

/*字符串描述符*/
UINT8C UdiskLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };//语言描述符
UINT8C UdiskProdInfo[] = {24,0x03,'X',0,'P',0,'L',0,'U',0,'G',0,'-',0,'U',0,'-',0,'S',0,'T',0,'C',0};//产品名称
UINT8C UdiskManuInfo[] = {36,0x03,
	'L',0,'i',0,'g',0,'h',0,'t',0,'&',0,'E',0,'l',0,'e',0,'c',0,'t',0,'r',0,'i',0,'c',0,'i',0,'t',0,'y',0
};//制造者名称
UINT8C UdiskSrNumInfo[] = {16,3,'0',0,'8',0,'H',0,'1',0,'6',0,'0',0,'0',0,};//序列号

/*配置描述符*/
UINT8C UdiskCfgDesc[/*9+23*/] = {//配置描述符
	// Configuration Descriptor
	0x09, // bLength
	0x02, // bDescriptorType (CONFIGURATION)
	0x20, 0x00, // wTotalLength (32)
	0x01, // bNumInterfaces
	0x01, // bConfigurationValue
	0x00, // iConfiguration
	0xA0, // bmAttributes
	0xFA, // bMaxPower
	/* ---------------------------------------------------- */
	// Interface 0: Control Data
	0x09, // bLength
	0x04, // bDescriptorType (INTERFACE)
	0x00, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x02, // bNumEndpoints
	0x08, // bInterfaceClass (Mass Storage)
	0x06, // bInterfaceSubClass (0x06 = SCSI transparent command set) (0x04 = UFI)
	0x50, // bInterfaceProtocol (Bulk-Only Transport)
	0x00, // iInterface
	// Endpoint 1: Send
	0x07, // bLength
	0x05, // bDescriptorType (ENDPOINT)
	0x81, // bEndpointAddress (IN, 1)
	0x02, // bmAttributes (TransferType=Bulk)
	0x40, 0x00, // wMaxPacketSize (64)
	0x00, // bInterval
	// Endpoint 1: Receive
	0x07, // bLength
	0x05, // bDescriptorType (ENDPOINT)
	0x01, // bEndpointAddress (OUT, 1)
	0x02, // bmAttributes (TransferType=Bulk)
	0x40, 0x00, // wMaxPacketSize (64)
	0x00, // bInterval
};
/*****************************************************************************************************************/
UINT8C PACKET0[2] = {0, 0};
UINT8C PACKET1[2] = {1, 0};



void usb_req_std()
{
	switch (Setup.bRequest)
	{
	case GET_STATUS:		usb_get_status();		break;
	case CLEAR_FEATURE:		usb_clear_feature();	break;
	case SET_FEATURE:		usb_set_feature();		break;
	case SET_ADDRESS:		usb_set_address();		break;
	case GET_DESCRIPTOR:	usb_get_descriptor();	break;
	case SET_DESCRIPTOR:	usb_set_descriptor();	break;
	case GET_CONFIGURATION:	usb_get_configuration();break;
	case SET_CONFIGURATION:	usb_set_configuration();break;
	case GET_INTERFACE:		usb_get_interface();	break;
	case SET_INTERFACE:		usb_set_interface();	break;
	case SYNCH_FRAME:		usb_synch_frame();		break;
	default:				usb_setup_stall();		return;
	}
}

void usb_get_status()
{
    BYTE ep;
    
    if (((Setup.bmRequestType & DIRECT_MASK) != IN_DIRECT) ||
        (Setup.wValueH != 0) || (Setup.wValueL != 0) ||
        (Setup.wLength != 2))
    {
        usb_setup_stall();
        return;
    }
    
    switch (Setup.bmRequestType & RECIPIENT_MASK)
    {
    case DEVICE_RECIPIENT:
        if ((Setup.wIndexH != 0) || (Setup.wIndexL != 0))
        {
            usb_setup_stall();
            return;
        }
        else
        {
            Ep0State.pData = PACKET0;
            Ep0State.wSize = 2;
        }
        break;
    case INTERFACE_RECIPIENT:
        if ((DeviceState != DEVSTATE_CONFIGURED) ||
            (Setup.wIndexH != 0) || (Setup.wIndexL != 0))
        {
            usb_setup_stall();
            return;
        }
        else
        {
            Ep0State.pData = PACKET0;
            Ep0State.wSize = 2;
        }
        break;
    case ENDPOINT_RECIPIENT:
        if ((DeviceState != DEVSTATE_CONFIGURED) || (Setup.wIndexH != 0))
        {
            usb_setup_stall();
            return;
        }
        else
        {
            ep = Setup.wIndexL & 0x0f;
            switch (Setup.wIndexL)
            {
#ifdef EN_EP1IN
            case EP1_IN:
#endif
#ifdef EN_EP2IN
            case EP2_IN:
#endif
#ifdef EN_EP3IN
            case EP3_IN:
#endif
#ifdef EN_EP4IN
            case EP4_IN:
#endif
#ifdef EN_EP5IN
            case EP5_IN:
#endif
#if (defined(EN_EP1IN) || defined(EN_EP2IN) || defined(EN_EP3IN) || defined(EN_EP4IN) || defined(EN_EP5IN))
                if (InEpState & (1 << ep)){
                    Ep0State.pData = PACKET1;
                    Ep0State.wSize = 2;
                }
                else{
                    Ep0State.pData = PACKET0;
                    Ep0State.wSize = 2;
                }
                break;
#endif
#ifdef EN_EP1OUT
            case EP1_OUT:
#endif
#ifdef EN_EP2OUT
            case EP2_OUT:
#endif
#ifdef EN_EP3OUT
            case EP3_OUT:
#endif
#ifdef EN_EP4OUT
            case EP4_OUT:
#endif
#ifdef EN_EP5OUT
            case EP5_OUT:
#endif
#if (defined(EN_EP1OUT) || defined(EN_EP2OUT) || defined(EN_EP3OUT) || defined(EN_EP4OUT) || defined(EN_EP5OUT))
                if (OutEpState & (1 << ep)){
                    Ep0State.pData = PACKET1;
                    Ep0State.wSize = 2;
                }
                else{
                    Ep0State.pData = PACKET0;
                    Ep0State.wSize = 2;
                }
                break;
#endif
            default:
                usb_setup_stall();
                return;
            }
        }
        break;
    default:
        usb_setup_stall();
        return;
    }

    usb_setup_in();
}

void usb_clear_feature()
{
    BYTE ep;
    
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | STANDARD_REQUEST | ENDPOINT_RECIPIENT)) ||
        (Setup.wIndexH != 0) ||
        (Setup.wValueH != 0) ||
        (Setup.wValueL != ENDPOINT_HALT) ||
        (Setup.wLength != 0))
    {
        usb_setup_stall();
        return;
    }
    
    ep = Setup.wIndexL & 0x0f;
    switch (Setup.wIndexL)
    {
#ifdef EN_EP1IN
    case EP1_IN:
#endif
#ifdef EN_EP2IN
    case EP2_IN:
#endif
#ifdef EN_EP3IN
    case EP3_IN:
#endif
#ifdef EN_EP4IN
    case EP4_IN:
#endif
#ifdef EN_EP5IN
    case EP5_IN:
#endif
#if (defined(EN_EP1IN) || defined(EN_EP2IN) || defined(EN_EP3IN) || defined(EN_EP4IN) || defined(EN_EP5IN))
        InEpState &= ~(1 << ep);
        usb_write_reg(INDEX, ep);
        usb_write_reg(INCSR1, INCLRDT);
        usb_write_reg(INDEX, 0);
        break;
#endif
#ifdef EN_EP1OUT
    case EP1_OUT:
#endif
#ifdef EN_EP2OUT
    case EP2_OUT:
#endif
#ifdef EN_EP3OUT
    case EP3_OUT:
#endif
#ifdef EN_EP4OUT
    case EP4_OUT:
#endif
#ifdef EN_EP5OUT
    case EP5_OUT:
#endif
#if (defined(EN_EP1OUT) || defined(EN_EP2OUT) || defined(EN_EP3OUT) || defined(EN_EP4OUT) || defined(EN_EP5OUT))
        OutEpState &= ~(1 << ep);
        usb_write_reg(INDEX, ep);
        usb_write_reg(OUTCSR1, OUTCLRDT);
        usb_write_reg(INDEX, 0);
        break;
#endif
    default:
        usb_setup_stall();
        return;
    }

    usb_setup_status();
}

void usb_set_feature()
{
    BYTE ep;
    
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | STANDARD_REQUEST | ENDPOINT_RECIPIENT)) ||
        (Setup.wIndexH != 0) ||
        (Setup.wValueH != 0) ||
        (Setup.wValueL != ENDPOINT_HALT) ||
        (Setup.wLength != 0))
    {
        usb_setup_stall();
        return;
    }
    
    ep = Setup.wIndexL & 0x0f;
    switch (Setup.wIndexL)
    {
#ifdef EN_EP1IN
    case EP1_IN:
#endif
#ifdef EN_EP2IN
    case EP2_IN:
#endif
#ifdef EN_EP3IN
    case EP3_IN:
#endif
#ifdef EN_EP4IN
    case EP4_IN:
#endif
#ifdef EN_EP5IN
    case EP5_IN:
#endif
#if (defined(EN_EP1IN) || defined(EN_EP2IN) || defined(EN_EP3IN) || defined(EN_EP4IN) || defined(EN_EP5IN))
        InEpState |= (1 << ep);
        usb_write_reg(INDEX, ep);
        usb_write_reg(INCSR1, INSDSTL);
        usb_write_reg(INDEX, 0);
        break;
#endif
#ifdef EN_EP1OUT
    case EP1_OUT:
#endif
#ifdef EN_EP2OUT
    case EP2_OUT:
#endif
#ifdef EN_EP3OUT
    case EP3_OUT:
#endif
#ifdef EN_EP4OUT
    case EP4_OUT:
#endif
#ifdef EN_EP5OUT
    case EP5_OUT:
#endif
#if (defined(EN_EP1OUT) || defined(EN_EP2OUT) || defined(EN_EP3OUT) || defined(EN_EP4OUT) || defined(EN_EP5OUT))
        OutEpState |= (1 << ep);
        usb_write_reg(INDEX, ep);
        usb_write_reg(OUTCSR1, OUTSDSTL);
        usb_write_reg(INDEX, 0);
        break;
#endif
    default:
        usb_setup_stall();
        return;
    }

    usb_setup_status();
}

void usb_set_address()
{
    BYTE addr;
    
    if ((Setup.bmRequestType != (OUT_DIRECT | STANDARD_REQUEST | DEVICE_RECIPIENT)) ||
        (Setup.wIndexH != 0) ||
        (Setup.wIndexL != 0) ||
        (Setup.wValueH != 0) ||
        (Setup.wValueL >= 0x80) ||
        (Setup.wLength != 0))
    {
        usb_setup_stall();
        return;
    }
    
    addr = Setup.wValueL;
    usb_write_reg(FADDR, addr);
    if(addr != 0){
        DeviceState = DEVSTATE_ADDRESS;
    }
    else{
        DeviceState = DEVSTATE_DEFAULT;
    }

    usb_setup_status();
}

void usb_get_descriptor()
{
    if ((Setup.bmRequestType & DIRECT_MASK) != IN_DIRECT)
    {
        usb_setup_stall();
        return;
    }
    
	if(usbMode == 0){	//HID模式
		switch (Setup.wValueH)
		{
		case DESC_DEVICE:			//设备描述符
			Ep0State.pData = DevDesc;
			Ep0State.wSize = sizeof(DevDesc);
			break;
		case DESC_CONFIGURATION:	//配置描述符
			Ep0State.pData = CfgDesc;
			Ep0State.wSize = sizeof(CfgDesc);
			break;
		case DESC_STRING:			//字符串描述符
			switch (Setup.wValueL)
			{
			case 0:						//语言描述符
				Ep0State.pData = MyLangDescr;
				Ep0State.wSize = sizeof(MyLangDescr);
				break;
			case 1:						//厂商描述符
				Ep0State.pData = MyManuInfo;
				Ep0State.wSize = sizeof(MyManuInfo);
				break;
			case 2:						//产品描述符
				Ep0State.pData = MyProdInfo;
				Ep0State.wSize = sizeof(MyProdInfo);
				break;
			default:
				usb_setup_stall();
				return;
			}
			break;
		case DESC_HIDREPORT:		//HID描述符
			if(Setup.wIndexL == 0){			//接口0报表描述符
				Ep0State.pData = KeyRepDesc;
				Ep0State.wSize = sizeof(KeyRepDesc);
			}
			else if(Setup.wIndexL == 1){	//接口1报表描述符
				Ep0State.pData = ComRepDesc;
				Ep0State.wSize = sizeof(ComRepDesc);
			}
			break;
		default:
			usb_setup_stall();
			return;
		}
	}
	else if(usbMode == 4){	//U盘模式
		switch (Setup.wValueH)
		{
		case DESC_DEVICE:			//设备描述符
			Ep0State.pData = UdiskDevDesc;
			Ep0State.wSize = sizeof(UdiskDevDesc);
			break;
		case DESC_CONFIGURATION:	//配置描述符
			Ep0State.pData = UdiskCfgDesc;
			Ep0State.wSize = sizeof(UdiskCfgDesc);
			break;
		case DESC_STRING:			//字符串描述符
			switch (Setup.wValueL)
			{
			case 0:						//语言描述符
				Ep0State.pData = UdiskLangDescr;
				Ep0State.wSize = sizeof(UdiskLangDescr);
				break;
			case 1:						//厂商描述符
				Ep0State.pData = UdiskManuInfo;
				Ep0State.wSize = sizeof(UdiskManuInfo);
				break;
			case 2:						//产品描述符
				Ep0State.pData = UdiskProdInfo;
				Ep0State.wSize = sizeof(UdiskProdInfo);
				break;
			case 3:						//序列号
				Ep0State.pData = UdiskSrNumInfo;
				Ep0State.wSize = sizeof(UdiskSrNumInfo);
				break;
			default:
				usb_setup_stall();
				return;
			}
			break;
		default:
			usb_setup_stall();
			return;
		}
	}
    
    if (Ep0State.wSize > Setup.wLength){
        Ep0State.wSize = Setup.wLength;
    }

    usb_setup_in();
}

void usb_set_descriptor()
{
    usb_setup_stall();
}

void usb_get_configuration()
{
    if ((Setup.bmRequestType != (IN_DIRECT | STANDARD_REQUEST | DEVICE_RECIPIENT)) ||
        (Setup.wValueH != 0) || (Setup.wValueL != 0) ||
        (Setup.wIndexH != 0) || (Setup.wIndexL != 0) ||
        (Setup.wLength != 1))
    {
        usb_setup_stall();
        return;
    }
    
    if (DeviceState == DEVSTATE_CONFIGURED){
        Ep0State.pData = PACKET1;
        Ep0State.wSize = 1;
    }
    else{
        Ep0State.pData = PACKET0;
        Ep0State.wSize = 1;
    }

    usb_setup_in();
}

void usb_set_configuration()
{
    if ((DeviceState == DEVSTATE_DEFAULT) ||
        (Setup.bmRequestType != (OUT_DIRECT | STANDARD_REQUEST | DEVICE_RECIPIENT)) ||
        (Setup.wValueH != 0) ||
        (Setup.wIndexH != 0) ||
        (Setup.wIndexL != 0) ||
        (Setup.wLength != 0))
    {
        usb_setup_stall();
        return;
    }
    
    if (Setup.wValueL == 1)
    {
        DeviceState = DEVSTATE_CONFIGURED;
        InEpState = 0x00;
        OutEpState = 0x00;
#ifdef EN_EP1IN
        usb_write_reg(INDEX, 1);
        usb_write_reg(INCSR2, INMODEIN);
        usb_write_reg(INMAXP, EP1IN_SIZE / 8);
#endif
#ifdef EN_EP2IN
        usb_write_reg(INDEX, 2);
        usb_write_reg(INCSR2, INMODEIN);
        usb_write_reg(INMAXP, EP2IN_SIZE / 8);
#endif
#ifdef EN_EP3IN
        usb_write_reg(INDEX, 3);
        usb_write_reg(INCSR2, INMODEIN);
        usb_write_reg(INMAXP, EP3IN_SIZE / 8);
#endif
#ifdef EN_EP4IN
        usb_write_reg(INDEX, 4);
        usb_write_reg(INCSR2, INMODEIN);
        usb_write_reg(INMAXP, EP4IN_SIZE / 8);
#endif
#ifdef EN_EP5IN
        usb_write_reg(INDEX, 5);
        usb_write_reg(INCSR2, INMODEIN);
        usb_write_reg(INMAXP, EP5IN_SIZE / 8);
#endif
#ifdef EN_EP1OUT
        usb_write_reg(INDEX, 1);
        usb_write_reg(INCSR2, INMODEOUT);
        usb_write_reg(OUTMAXP, EP1OUT_SIZE / 8);
#endif
#ifdef EN_EP2OUT
        usb_write_reg(INDEX, 2);
        usb_write_reg(INCSR2, INMODEOUT);
        usb_write_reg(OUTMAXP, EP2OUT_SIZE / 8);
#endif
#ifdef EN_EP3OUT
        usb_write_reg(INDEX, 3);
        usb_write_reg(INCSR2, INMODEOUT);
        usb_write_reg(OUTMAXP, EP3OUT_SIZE / 8);
#endif
#ifdef EN_EP4OUT
        usb_write_reg(INDEX, 4);
        usb_write_reg(INCSR2, INMODEOUT);
        usb_write_reg(OUTMAXP, EP4OUT_SIZE / 8);
#endif
#ifdef EN_EP5OUT
        usb_write_reg(INDEX, 5);
        usb_write_reg(INCSR2, INMODEOUT);
        usb_write_reg(OUTMAXP, EP5OUT_SIZE / 8);
#endif
        usb_write_reg(INDEX, 0);
    }
    else
    {
        DeviceState = DEVSTATE_ADDRESS;
        InEpState = 0xff;
        OutEpState = 0xff;
    }

    usb_setup_status();
}

void usb_get_interface()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | STANDARD_REQUEST | INTERFACE_RECIPIENT)) ||
        (Setup.wValueH != 0) ||
        (Setup.wIndexH != 0) ||
        (Setup.wLength != 1))
    {
        usb_setup_stall();
        return;
    }
    
    Ep0State.pData = PACKET0;
    Ep0State.wSize = 1;
    
    usb_setup_in();
}

void usb_set_interface()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | STANDARD_REQUEST | INTERFACE_RECIPIENT)) ||
        (Setup.wValueH != 0) ||
        (Setup.wIndexH != 0) ||
        (Setup.wLength != 0))
    {
        usb_setup_stall();
        return;
    }

    usb_setup_status();
}

void usb_synch_frame()
{
    usb_setup_stall();
}

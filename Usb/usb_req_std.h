#ifndef __USB_REQ_STD_H__
#define __USB_REQ_STD_H__


#define HID_EPIN_ADDR						0x81U//HID使用上行端点1
#define HID_EPOUT_ADDR						0x01U//HID使用下行端点1
#define CUSTOM_HID_EPIN_ADDR				0x82U//自定义HID使用上行端点2
#define CUSTOM_HID_EPOUT_ADDR				0x02U//自定义HID使用下行端点2

#define THIS_ENDP0_SIZE         EP0_SIZE		//端点0数据包大小
#define ENDP1_IN_SIZE           EP1IN_SIZE		//HID端点IN数据包大小
#define ENDP1_OUT_SIZE          EP1OUT_SIZE		//HID端点OUT数据包大小
#define ENDP2_IN_SIZE           EP2IN_SIZE		//自定义HID端点IN数据包大小
#define ENDP2_OUT_SIZE          EP2OUT_SIZE		//自定义HID端点OUT数据包大小

#define USBD_MAX_NUM_INTERFACES				2//接口个数
#define USBD_HID_INTERFACE					0//HID接口编号
#define USBD_CUSTOM_HID_INTERFACE			1//自定义HID接口编号

#define CUSTOM_HID_FS_BINTERVAL		0x1//自定义HID轮询间隔
#define HID_FS_BINTERVAL			0x1//HID轮询间隔


#define Vendor_ID		0x34BF	//厂商ID(STC)
#define Product_ID		22662	//产品ID(HID模式)
#define Product_ID_U	22663	//产品ID(U盘模式)





#define GET_STATUS              0x00
#define CLEAR_FEATURE           0x01
#define SET_FEATURE             0x03
#define SET_ADDRESS             0x05
#define GET_DESCRIPTOR          0x06
#define SET_DESCRIPTOR          0x07
#define GET_CONFIGURATION       0x08
#define SET_CONFIGURATION       0x09
#define GET_INTERFACE           0x0A
#define SET_INTERFACE           0x0B
#define SYNCH_FRAME             0x0C

void usb_req_std();

void usb_get_status();
void usb_clear_feature();
void usb_set_feature();
void usb_set_address();
void usb_get_descriptor();
void usb_set_descriptor();
void usb_get_configuration();
void usb_set_configuration();
void usb_get_interface();
void usb_set_interface();
void usb_synch_frame();

#endif

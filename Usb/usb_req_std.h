#ifndef __USB_REQ_STD_H__
#define __USB_REQ_STD_H__


#define HID_EPIN_ADDR						0x81U//HIDʹ�����ж˵�1
#define HID_EPOUT_ADDR						0x01U//HIDʹ�����ж˵�1
#define CUSTOM_HID_EPIN_ADDR				0x82U//�Զ���HIDʹ�����ж˵�2
#define CUSTOM_HID_EPOUT_ADDR				0x02U//�Զ���HIDʹ�����ж˵�2

#define THIS_ENDP0_SIZE         EP0_SIZE		//�˵�0���ݰ���С
#define ENDP1_IN_SIZE           EP1IN_SIZE		//HID�˵�IN���ݰ���С
#define ENDP1_OUT_SIZE          EP1OUT_SIZE		//HID�˵�OUT���ݰ���С
#define ENDP2_IN_SIZE           EP2IN_SIZE		//�Զ���HID�˵�IN���ݰ���С
#define ENDP2_OUT_SIZE          EP2OUT_SIZE		//�Զ���HID�˵�OUT���ݰ���С

#define USBD_MAX_NUM_INTERFACES				2//�ӿڸ���
#define USBD_HID_INTERFACE					0//HID�ӿڱ��
#define USBD_CUSTOM_HID_INTERFACE			1//�Զ���HID�ӿڱ��

#define CUSTOM_HID_FS_BINTERVAL		0x1//�Զ���HID��ѯ���
#define HID_FS_BINTERVAL			0x1//HID��ѯ���


#define Vendor_ID		0x34BF	//����ID(STC)
#define Product_ID		22662	//��ƷID(HIDģʽ)
#define Product_ID_U	22663	//��ƷID(U��ģʽ)





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

#ifndef __USB_REQ_CLASS_H__
#define __USB_REQ_CLASS_H__

#define GET_REPORT              0x01
#define GET_IDLE                0x02
#define GET_PROTOCOL            0x03
#define SET_REPORT              0x09
#define SET_IDLE                0x0A
#define SET_PROTOCOL            0x0B

#define DESC_HIDREPORT          0x22

//extern BYTE bHidIdle;

void usb_req_class(void);

void usb_get_report();
void usb_set_report();
void usb_get_idle();
void usb_set_idle();
void usb_get_protocol();
void usb_set_protocol();

void EP1_Send_Data(BYTE *pdat, BYTE cnt);//�˵�1����
void EP2_Send_Data(BYTE *pdat, BYTE cnt, BYTE pad);//�˵�2����
void EP1_Out_Led(void);//����LED����
void EP2_Out_Cmd(void);//��������������




#endif









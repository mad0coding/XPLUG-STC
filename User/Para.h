#ifndef __CFG_H__
#define __CFG_H__

#include "DEBUG.H"
#include "STC8G_H_EEPROM.h"
#include "usb.h"
#include "RgbDrv.h"
#include "drv_RF24L01.h"

#define PARA_ADDR	0x0000	//������Ե�ַ
#define PARA_LEN	62		//�������ݳ���

#define KB_len		22//���̱��ĳ���

/******************************�������ݶ���******************************/
#define CFG_LIGHT			para[0]//��Ч���� L R G B
#define CFG_BRIGHT			para[1]//��Ч����
#define CFG_LED_BRT(i)		para[2 + (i)]//������
#define CFG_LED(i)			para[6 + (i)]//������
#define CFG_LED_MAP(i)		(0x0F & CFG_LED(i))//��ӳ��
#define CFG_LED_DIR(i)		(CFG_LED(i) >> 7)//�Ʒ���
#define CFG_LED_DISK		para[10]//U�̶�дָʾ��

#define CFG_USB_LIMIT		para[12]//CUSB����ֵ
#define CFG_USB_DELAY		para[13]//CUSB�ض��ӳ�
#define CFG_USB_REST		para[14]//CUSB�ؿ��ӳ�
#define CFG_USB_DEFAULT		para[15]//CUSB���ܿ�ʱ��Ĭ������
#define CFG_BOOT_DISK		para[16]//����ʱ�������ɽ���U��
#define CFG_G24_MODE		para[17]//2.4Gģʽ
#define CFG_SW_DIR			para[18]//���ط���
#define CFG_SW_MODE			para[19]//����ģʽ

#define CFG_KEY_MODE(i)		para[20 + 4*hidCs + (i)]//����ģʽ
#define CFG_KEY_FUNC(i)		para[28 + 4*hidCs + (i)]//�������ܼ�
#define CFG_KEY_KV(i)		para[36 + 4*hidCs + (i)]//������ֵ//44

#define CFG_EC_DIR			para[50 + hidCs]//��ť����
#define CFG_TEST			para[0]
/************************************************************************/

/******************************��ֵ����******************************/
#define kv_report       234
#define kv_loop         235

#define kv_point		237
#define kv_vol_next     238
#define kv_vol_prev     239
#define kv_wheel_up     240
#define kv_wheel_down   241
#define kv_vol_up       242
#define kv_vol_down     243
#define kv_vol_mute     244
#define kv_vol_stop     245
#define kv_mouse_l      246
#define kv_mouse_m      247
#define kv_mouse_r      248
#define kv_ctrl         249
#define kv_shift        250
#define kv_alt          251
#define kv_win          252
#define kv_shortcut     253
#define kv_delay		254
/********************************************************************/

#define USB_MODE_CS(i)		{USBCON = 0x00;g24BufStart = g24BufEnd = 0;delay_ms(200);usbMode = (i);usb_init();}
#define USB_MODE_TOG		{USBCON = 0x00;g24BufStart = g24BufEnd = 0;delay_ms(200);usbMode = !usbMode;usb_init();}
#define HID_MODE_CS(i)		{hidCs = (i);}
#define HID_MODE_TOG		{hidCs = !hidCs;}

/******************************extern����******************************/
extern UINT8D para[PARA_LEN];//��������

extern uint8_t G24_Chip_State;//����оƬ״̬
extern UINT8I usbMode;//USBģʽ
extern UINT8I hidCs;//HID����ѡ��
/**********************************************************************/



void generateReport(void);//���û����������ò�������HID����

void paraInit(uint8_t ifHot);	//�ò�����ʼ��ϵͳ
void paraRead(void);	//��ȡ����
void paraWrite(void);	//д�����

uint8_t g24BufInput(void);	//2.4G������������
uint8_t g24BufOutput(void);	//2.4G�����������
uint8_t g24BufRead(uint8_t *buf, uint8_t len);	//��ȡ2.4G����
uint8_t g24BufWrite(uint8_t *buf, uint8_t len);	//д��2.4G����



#endif






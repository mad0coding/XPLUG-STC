#ifndef __CFG_H__
#define __CFG_H__

#include "DEBUG.H"
#include "STC8G_H_EEPROM.h"
#include "usb.h"
#include "RgbDrv.h"
#include "drv_RF24L01.h"

#define PARA_ADDR	0x0000	//参数相对地址
#define PARA_LEN	62		//参数数据长度

#define KB_len		22//键盘报文长度

/******************************配置数据定义******************************/
#define CFG_LIGHT			para[0]//灯效配置 L R G B
#define CFG_BRIGHT			para[1]//灯效亮度
#define CFG_LED_BRT(i)		para[2 + (i)]//灯亮度
#define CFG_LED(i)			para[6 + (i)]//灯配置
#define CFG_LED_MAP(i)		(0x0F & CFG_LED(i))//灯映射
#define CFG_LED_DIR(i)		(CFG_LED(i) >> 7)//灯反向
#define CFG_LED_DISK		para[10]//U盘读写指示灯

#define CFG_USB_LIMIT		para[12]//CUSB限流值
#define CFG_USB_DELAY		para[13]//CUSB关断延迟
#define CFG_USB_REST		para[14]//CUSB重开延迟
#define CFG_USB_DEFAULT		para[15]//CUSB不受控时的默认设置
#define CFG_BOOT_DISK		para[16]//启动时按按键可进入U盘
#define CFG_G24_MODE		para[17]//2.4G模式
#define CFG_SW_DIR			para[18]//开关方向
#define CFG_SW_MODE			para[19]//开关模式

#define CFG_KEY_MODE(i)		para[20 + 4*hidCs + (i)]//按键模式
#define CFG_KEY_FUNC(i)		para[28 + 4*hidCs + (i)]//按键功能键
#define CFG_KEY_KV(i)		para[36 + 4*hidCs + (i)]//按键键值//44

#define CFG_EC_DIR			para[50 + hidCs]//旋钮方向
#define CFG_TEST			para[0]
/************************************************************************/

/******************************键值定义******************************/
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

/******************************extern变量******************************/
extern UINT8D para[PARA_LEN];//参数缓存

extern uint8_t G24_Chip_State;//无线芯片状态
extern UINT8I usbMode;//USB模式
extern UINT8I hidCs;//HID配置选择
/**********************************************************************/



void generateReport(void);//由用户操作和配置参数生成HID报文

void paraInit(uint8_t ifHot);	//用参数初始化系统
void paraRead(void);	//读取参数
void paraWrite(void);	//写入参数

uint8_t g24BufInput(void);	//2.4G缓存数据输入
uint8_t g24BufOutput(void);	//2.4G缓存数据输出
uint8_t g24BufRead(uint8_t *buf, uint8_t len);	//读取2.4G缓存
uint8_t g24BufWrite(uint8_t *buf, uint8_t len);	//写入2.4G缓存



#endif







#include "Para.h"

UINT8D para[PARA_LEN];//参数缓存

//******************************报文发送******************************//
extern UINT8I KeyBrd_data[];//键盘报文
extern UINT8I Mouse_data[];//鼠标报文
extern UINT8I Point_data[];//指针报文
extern UINT8I Vol_data[];//音量报文
UINT8I KeyBrd_data_old[KB_len];//上次键盘报文
UINT8I Mouse_data_old = 0;//上次鼠标报文
UINT8I Vol_data_old = 0;//上次音量报文
extern UINT8I KeyBrd_if_send;//键盘报文是否发送
extern UINT8I Vol_if_send;//音量报文是否发送
extern UINT8I Point_if_send;//指针报文是否发送
extern UINT8I Mouse_if_send;//鼠标报文是否发送
//********************************************************************//

//******************************2.4G缓存数据******************************//
#define HID_G24_BUF		BUF_4K
extern UINT8X BUF_4K[4096];//HID模式下为环形缓冲区,U盘模式下为闪存缓存
uint8_t G24_RX_BUF[32];//无线接收缓冲区
uint16_t g24BufStart = 0, g24BufEnd = 0;//环形缓冲区起始和末尾
uint8_t G24_Chip_State = 0;//无线芯片状态:bit7为是否在位,bit6为是否执行过初始化

uint8_t G24_KeyBrd_data[KB_len - 4] = {1,0,0,0};//编号1,功能键,保留0,其他按键
uint8_t G24_Mouse_data[5] = {2,0,0,0,0};//编号2,功能键,x,y,滚轮
uint8_t G24_Point_data[7] = {3,0x10,1,0xFF,0xFF,0xFF,0xFF};//编号3,功能键,id,x_L,x_H,y_L,y_H
uint8_t G24_Vol_data[2] = {4,0};//编号4,功能键
//************************************************************************//

UINT8I usbMode = 0;//USB模式:0为HID模式,1为U盘模式
UINT8I hidCs = 0;//HID配置选择


void keyInsert(uint8_t r_i, uint8_t key_v){//单键填入
	if(key_v == kv_wheel_down) Mouse_data[4] += -1;//滚轮向下
	else if(key_v == kv_wheel_up) Mouse_data[4] += 1;//滚轮向上
	else if(key_v == kv_vol_up) Vol_data[1] |= 0x01;//音量加
	else if(key_v == kv_vol_down) Vol_data[1] |= 0x02;//音量减
	else if(key_v == kv_vol_mute) Vol_data[1] |= 0x04;//静音
	else if(key_v == kv_vol_stop) Vol_data[1] |= 0x08;//播放暂停
	else if(key_v == kv_vol_next) Vol_data[1] |= 0x10;//下一个
	else if(key_v == kv_vol_prev) Vol_data[1] |= 0x20;//上一个
	else if(key_v == kv_mouse_l) Mouse_data[1] |= 0x01;//鼠标左键
	else if(key_v == kv_mouse_m) Mouse_data[1] |= 0x04;//鼠标中键
	else if(key_v == kv_mouse_r) Mouse_data[1] |= 0x02;//鼠标右键
	else if(key_v == kv_ctrl) KeyBrd_data[1] |= 0x01;//ctrl
	else if(key_v == kv_shift) KeyBrd_data[1] |= 0x02;//shift
	else if(key_v == kv_alt) KeyBrd_data[1] |= 0x04;//alt
	else if(key_v == kv_win) KeyBrd_data[1] |= 0x08;//win
	else{//普通键盘按键
		if(r_i == 0xFF){//若使用自动填入
			for(r_i = 3; r_i < KB_len; r_i++){//搜索空位
				if(!KeyBrd_data[r_i]) break;//若此处为空
			}
		}
		if(r_i == 0xFF) return;//若未找到空位则退出
		KeyBrd_data[r_i] = key_v;//填入键值
	}
}

void generateReport(void){//由用户操作和配置参数生成HID报文
	uint8_t i = 0;//公共用计数变量
	
	//***********************************各报文及发送标志初始化***********************************//
	KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;//发送标志置零
	
	memcpy(KeyBrd_data_old + 1, KeyBrd_data + 1, KB_len - 1);//记录上一次报文
	memset(KeyBrd_data + 1, 0, KB_len - 1);//清除键盘报文

	Mouse_data_old = Mouse_data[1];//记录上一次报文
	memset(Mouse_data + 1, 0, 4);//清除鼠标报文

	Point_data[1] = 0x10;
	memset(Point_data + 3, 0xFF, 4);//清除指针报文

	Vol_data_old = Vol_data[1];//记录上一次报文
	Vol_data[1] = 0;//清除音量报文
	//********************************************************************************************//
	
	if(/*keyAddr[sys_cs][0] == */0) return /*0xFF*/;//若本配置数据错误则退出
	
	//****************************************开关处理****************************************//
	i = (CFG_SW_DIR == keyNow[4] ? 0 : 1);//暂存开关的逻辑状态
	if(keyNow[4] != keyOld[4]){//若为边沿
		if(CFG_SW_MODE == 1){//模式1:HID配置切换
			HID_MODE_CS(i);
		}
		else if(CFG_SW_MODE == 2){//模式2:CUSB开关
			CUSB_EN = i;
		}
		else if(CFG_SW_MODE == 3){//模式3:HUB复位
			HUB_EN = i;
		}
		else if(CFG_SW_MODE == 4){//模式4:USB模式切换
			USB_MODE_CS(i);
		}
		else if(CFG_SW_MODE == 5){//模式5:自定义1
			
		}
		else if(CFG_SW_MODE == 6){//模式5:自定义2
			
		}
	}
	//****************************************************************************************//
	
	//****************************************功能按键处理****************************************//
	for(i = 0; i < 4; i++){//对于4个按键
		if(keyNow[i]){//若按下
			if(CFG_KEY_MODE(i) == 1){//模式1:按键(单按键/快捷键)
				KeyBrd_data[1] |= CFG_KEY_FUNC(i);//填入功能键(若有)
				keyInsert(i + 3, CFG_KEY_KV(i));//填入键值
			}
			if(!keyOld[i]){//若刚按下(按下沿)
				if(CFG_KEY_MODE(i) == 2){//模式2:CUSB开关
					CUSB_EN = !CUSB_EN;//CUSB状态反转
				}
				else if(CFG_KEY_MODE(i) == 3){//模式3:CUSB掉电
					CUSB_EN = 0;
				}
				else if(CFG_KEY_MODE(i) == 4){//模式4:HUB复位
					HUB_EN = 0;
				}
				else if(CFG_KEY_MODE(i) == 7){//模式7:自定义1(这下面代码是Boot跳转功能,测试用)
					USBCON = 0x00;//关闭USB
					delay_ms(200);//让主机检测到设备拔出
					STC8H_SoftReset(1);//重启并进入Boot
				}
				else if(CFG_KEY_MODE(i) == 8){//模式8:自定义2(这下面代码是CH系单片机下载触发功能,测试用)
//					GATE_OFF;		//CUSB立即关
//					delay_ms(200);	//让主机检测到设备拔出
//					DP_PU_ON;		//CUSB的DP上拉
//					GATE_ON;		//CUSB立即开
//					CUSB_EN = 1;	//CUSB设置开(这之后开始CH系单片机下载)
				}
			}
		}
		else if(keyOld[i]){//若刚抬起(释放沿)
			if(CFG_KEY_MODE(i) == 3){//模式3:CUSB掉电
				CUSB_EN = 1;//CUSB上电
			}
			else if(CFG_KEY_MODE(i) == 4){//模式4:HUB复位
				HUB_EN = 1;//HUB使能
			}
			else if(CFG_KEY_MODE(i) == 5){//模式5:HID配置切换
				HID_MODE_TOG;
			}
			else if(CFG_KEY_MODE(i) == 6){//模式6:USB模式切换
				USB_MODE_TOG;
			}
			else if(CFG_KEY_MODE(i) == 7){//模式7:自定义1
				
			}
			else if(CFG_KEY_MODE(i) == 8){//模式8:自定义2(这下面代码是CH系单片机下载触发功能,测试用)
//				GATE_OFF;		//CUSB立即关
//				DP_PU_OFF;		//CUSB的DP取消上拉
//				delay_ms(200);	//让主机检测到设备拔出
//				GATE_ON;		//CUSB立即开
			}
		}
	}//处理完4个按键的主要内容
	//********************************************************************************************//
	
	if(usbMode == 0 && CFG_G24_MODE == 1 && (G24_Chip_State & 0x80)){//HID模式 启用2.4G 芯片在位
		g24BufOutput();//2.4G缓存数据输出
	}
	
	//***********************************判断各报文是否要发送***********************************//
	for(i = 1; i < KB_len; i++){
		if(KeyBrd_data_old[i] != KeyBrd_data[i]){//键盘报文与上一次不同则发送
			KeyBrd_if_send = 1;	break;
		}
	}
	if(Mouse_data[1] != Mouse_data_old) Mouse_if_send = 1;//鼠标按键与上次不同则发送
	else{
		for(i = 2; i < 5; i++){
			if(Mouse_data[i] != 0){//鼠标存在移动或滚动则发送
				Mouse_if_send = 1;	break;
			}
		}
	}
	for(i = 3; i < 7; i++){
		if(Point_data[i] != 0xFF){//触摸存在有效坐标则发送
			Point_if_send = 1;	break;
		}
	}
	if(Vol_data[1] != Vol_data_old) Vol_if_send = 1;//媒体报文与上次不同则发送
	
	if(usbMode != 0){//非HID模式
		KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;//全部静默
	}
	//******************************************************************************************//
}


void paraInit(uint8_t ifHot){//用参数初始化系统
	//uint8_t tmpSW = (CFG_SW_DIR == SW ? 0 : 1);//暂存开关的逻辑状态
	if(!ifHot) paraRead();//若为冷初始化则读取参数
	//默认配置
	HID_MODE_CS(0);
	CUSB_EN = !CFG_USB_DEFAULT;
	HUB_EN = 1;
	if(!ifHot){//冷切换
		usbMode = CFG_BOOT_DISK && !KEY;//上电时按键按下且允许此功能则进入U盘
		//usbMode = 1;//进入U盘 测试代码
	}
	else if(usbMode != 0){//热切换且需要切换
		USB_MODE_CS(0);
	}
	//开关决定的配置
	if(CFG_SW_MODE == 1){//模式1:HID配置切换
		//HID_MODE_CS(tmpSW);
	}
	else if(CFG_SW_MODE == 2){//模式2:CUSB开关
		//CUSB_EN = tmpSW;
	}
	else if(CFG_SW_MODE == 3){//模式3:HUB复位
		//HUB_EN = tmpSW;
	}
	else if(CFG_SW_MODE == 4){//模式4:USB模式切换
//		if(!ifHot) usbMode = tmpSW;//冷切换
//		else if(usbMode != tmpSW){//热切换且需要切换
//			USB_MODE_CS(tmpSW);
//		}
	}
	if(CFG_USB_LIMIT == 0) ADC_USB_LIMIT = 0;//不启用限流
	else ADC_USB_LIMIT = 4095 - CFG_USB_LIMIT * 273 / 11;//计算限流ADC值
	funcOutput();//各类输出
	if(ifHot && CFG_G24_MODE != 0 && !(G24_Chip_State & 0x40)){//热切换 启用2.4G 2.4G未初始化过
		NRF24L01_Config(0);//无线模块初始化为接收模式
	}
}

void paraRead(void){//读取参数
	EEPROM_read_n(PARA_ADDR, para, PARA_LEN);
}

void paraWrite(void){//写入参数
	EEPROM_SectorErase(PARA_ADDR);
	EEPROM_write_n(PARA_ADDR, para, PARA_LEN);
}


/**************************************************2.4G数据缓存与处理**************************************************/
uint16_t g24BufLen(void){//获取2.4G缓存长度
	if(g24BufEnd < g24BufStart) return (4096 + g24BufEnd - g24BufStart);
	return (g24BufEnd - g24BufStart);
}

uint8_t g24BufInput(void){//2.4G缓存数据输入
	static uint8_t g24_flag = 0, g24_state = 1;//2.4G接收标志及接收状态
	g24_flag = NRF24L01_RxPkg(G24_RX_BUF);	//0为未接到,0xFF为断连,其他为接收长度
	if(g24_flag == 0xFF){	//若判定断连
		if(g24_state >= 2){		//若为正常连接态
			g24_state = 1;			//进入断连预备
			//清除无线HID数组
			memset(G24_KeyBrd_data + 1, 0, KB_len - 1 - 4);//清除所有键盘报文
			memset(G24_Mouse_data + 1, 0, 4);//清除鼠标报文
			G24_Point_data[1] = 0x10;
			memset(G24_Point_data + 3, 0xFF, 4);//清除指针报文
			G24_Vol_data[1] = 0;//清除音量报文
		}	
		LedInput[6] = 0;//指示灯灭
	}
	else if(g24_flag > 0){	//若接到数据
		g24_state = 2;			//进入正常连接态
		g24BufWrite(G24_RX_BUF, g24_flag);//写入2.4G缓存
		LedInput[6] = 1;//指示灯亮
		return 1;
	}
	return 0;
}

uint8_t g24BufOutput(void){//2.4G缓存数据输出
	uint8_t pkgHead = 0;//帧头
	if(g24BufLen() == 0) goto Merge;//无数据
	pkgHead = HID_G24_BUF[g24BufStart];//读取帧头
	if(pkgHead == 1) g24BufRead(G24_KeyBrd_data, KB_len - 4);//键盘报文
	else if(pkgHead == 2) g24BufRead(G24_Mouse_data, 5);//鼠标报文
	else if(pkgHead == 3) g24BufRead(G24_Point_data, 7);//触摸报文
	else if(pkgHead == 4) g24BufRead(G24_Vol_data, 2);//媒体报文
	else return 0xFF;//数据错误
	
	Merge://合并报文
	KeyBrd_data[1] |= G24_KeyBrd_data[1];
	memcpy(KeyBrd_data + 3 + 4, G24_KeyBrd_data + 3, KB_len - 4 - 3);//本体最多占用前4个普通键字节
	
	Mouse_data[1] |= G24_Mouse_data[1];
	Mouse_data[2] += G24_Mouse_data[2];
	Mouse_data[3] += G24_Mouse_data[3];
	Mouse_data[4] += G24_Mouse_data[4];
	
	memcpy(Point_data, G24_Point_data, 7);//本体不产生触摸报文故直接覆盖
	
	Vol_data[1] |= G24_Vol_data[1];
	return pkgHead;
}

uint8_t g24BufRead(uint8_t *buf, uint8_t len){//读取2.4G缓存
	uint16_t g24BufLenNow = g24BufLen();
	if(g24BufLenNow == 0) return 0;//无数据
	if(g24BufLenNow < len) len = g24BufLenNow;
	while(len--){
		*buf++ = HID_G24_BUF[g24BufStart++];
		if(g24BufStart >= 4096) g24BufStart = 0;//绕一圈
	}
	return len;
}

uint8_t g24BufWrite(uint8_t *buf, uint8_t len){//写入2.4G缓存
	if((len != KB_len - 4 || buf[0] != 1) && (len != 5 || buf[0] != 2) 
			 && (len != 7 || buf[0] != 3) && (len != 2 || buf[0] != 4)) return 1;//长度或帧头不对
	if(g24BufLen() + len >= 4096) return 2;//空间不足
	while(len--){
		HID_G24_BUF[g24BufEnd++] = *buf++;
		if(g24BufEnd >= 4096) g24BufEnd = 0;//绕一圈
	}
	return 0;
}
/**********************************************************************************************************************/















#include "RgbDrv.h"

UINT32D Systime = 0;//系统时间

uint8_t PRINT_BUF[64];//打印缓存

uint8_t keyNow[5] = {0,0,0,0,0};//当前按键
uint8_t keyOld[5] = {0,0,0,0,0};//上次按键

UINT16X ADC_DATA[3][ADC_VN + 2];//ADC缓存
UINT16D ADC_USB_LIMIT = 0;//限流值(反向)

bit CUSB_CUT = 0, CUSB_EN = 0, HUB_EN = 1;//CUSB过流切断,CUSB使能,HUB使能

uint8_t LedInput[9];//LED输入:0-NUM,1-CAPS,2-SCROLL,3-CUSB,4-HUB,5-hidCs,6-2.4G,7-CUSTOM1,8-CUSTOM2
uint8_t LedState[4] = {0,0,0,0};//LED状态
UINT8D FrameBuf[1*3] = {0,0,0};//GRB帧缓存



//H:0~colorAngle*6,S:0~100(已用delta代替),V:0~255
//void rgbToHsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV){//RGB转HSV
//    uint8_t max = MAX(MAX(vR,vG),vB), min = MINI(MINI(vR,vG),vB);
//    uint8_t delta = max - min;
//    if(delta == 0) *pH = 0;
//    else if(max == vR) *pH = (uint16_t)colorAngle*(vG-vB)/delta;
//    else if(max == vG) *pH = (uint16_t)colorAngle*(vB-vR)/delta + colorAngle*2;
//    else if(max == vB) *pH = (uint16_t)colorAngle*(vR-vG)/delta + colorAngle*4;
//    if(*pH > colorAngle * 6) *pH += colorAngle * 6;
//    if(max == 0) *pS = 0;
//    else *pS = delta;//100 * delta / max;//注意此处S直接用delta代替,故函数外直接修改V不合法
//    *pV = max;
//}

//void hsvToRgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB){//HSV转RGB
//	uint8_t hi = (uint16_t)(vH / colorAngle) % 6;
//    uint16_t f = vH - hi * colorAngle;
//    uint8_t p = vV - vS;
//    uint8_t q = vV - (uint16_t)vS * f / colorAngle;
//    uint8_t t = vV - (uint16_t)vS * (colorAngle - f) / colorAngle;
//    if(hi == 0)     {*pR = vV;   *pG = t;    *pB = p;}
//    else if(hi == 1){*pR = q;    *pG = vV;   *pB = p;}
//    else if(hi == 2){*pR = p;    *pG = vV;   *pB = t;}
//    else if(hi == 3){*pR = p;    *pG = q;    *pB = vV;}
//    else if(hi == 4){*pR = t;    *pG = p;    *pB = vV;}
//    else if(hi == 5){*pR = vV;   *pG = p;    *pB = q;}
//}


UINT8C rgbCycle[3] = {3,6,30};//RGB周期表(单位s)
void sysRGB(){//系统RGB灯效
	uint8_t r = 0, g = 0, b = 0;
	uint16_t h, s, v;
	
	if(CFG_LIGHT >= 1 && CFG_LIGHT <= 3){//色彩变化模式
//		rgbToHsv(CFG_BRIGHT, 0, 0, &h, &s, &v);
//		h += (Systime / rgbCycle[CFG_LIGHT - 1]) % (colorAngle * 6);
//		if(h >= colorAngle * 6) h -= colorAngle * 6;
//		hsvToRgb(h, s, v, &r, &g, &b);
	}
	else if(CFG_LIGHT == 4){//电流指示模式
//		rgbToHsv(CFG_BRIGHT, 0, 0, &h, &s, &v);
//		h = colorAngle * 3 - colorAngle * 3 * (uint32_t)(4095 - ADC_DATA[1][ADC_VN + 1]) / (4095 - ADC_USB_LIMIT);//蓝绿黄红
//		if(h >= colorAngle * 6) h = 0;//处理溢出
//		if(CUSB_CUT) h = colorAngle * 5;//若过流关断则为紫色
//		hsvToRgb(h, s, v, &r, &g, &b);
	}
	else if(CFG_LIGHT == 5){//自定义1
		
	}
	else if(CFG_LIGHT == 6){//自定义2
		
	}

	FrameBuf[0] = g;
	FrameBuf[1] = r;
	FrameBuf[2] = b;
}

void funcOutput(void){//各类输出
	uint8_t i;
	//输出
	//if(!CUSB_CUT) GATE = !CUSB_EN;//若未处于过流关断则执行CUSB输出
	//CRST = HUB_EN;//执行HUB输出
	//灯输入
	//LedInput[3] = !GATE;//填入CUSB状态
	//LedInput[4] = CRST;//填入HUB状态
	LedInput[5] = hidCs;//填入hidCs
	//LedInput[6] = 0;//填入2.4G状态
	LedInput[7] = 0;//填入自定义1
	LedInput[8] = 0;//填入自定义2
	//灯映射
	for(i = 0; i < 4; i++){//L R G B
		if(CFG_LED_MAP(i)){//若启用映射
			LedState[i] = LedInput[CFG_LED_MAP(i) - 1];//从LED输入中映射
			if(CFG_LED_DIR(i)) LedState[i] = !LedState[i];//反向
		}
		else LedState[i] = 0xFF;//未启用映射
	}
	//灯显示或显存写入
//	if(LedState[0] == 0) LED_OFF;
//	else if(LedState[0] == 1) LED_ON;
	if(LedState[1] != 0xFF) FrameBuf[1] = LedState[1] * CFG_LED_BRT(1);//R
	if(LedState[2] != 0xFF) FrameBuf[0] = LedState[2] * CFG_LED_BRT(2);//G
	if(LedState[3] != 0xFF) FrameBuf[2] = LedState[3] * CFG_LED_BRT(3);//B
}


void readEncoder(int8_t newCnt){//读取编码器
	static uint8_t ifGap = 0;	//间隔标志
	static int8_t oldCnt = 0, realCnt = 0;	//旧计数值,实际格数
	int8_t difCnt = newCnt - oldCnt;		//计数差值
	difCnt = difCnt / 4;	//计数差值分频得格数差值
	oldCnt += difCnt * 4;	//旧计数值跟进
	realCnt += difCnt;		//格数差值加到实际格数上
	if(0) oldCnt = newCnt;	//若长时间不动则重新校准旧计数值
	memcpy(keyOld, keyNow, 2);	//记录旧按键
	if(realCnt == 0 || ifGap) memset(keyNow, 0, 2);	//若计数为0或为间隔期则等效为抬起
	else{	//计数不为0且不为间隔期
		if(realCnt > 0){	//计数＞0
			realCnt--;
			keyNow[!CFG_EC_DIR] = 1;
		}
		else{	//计数＜0
			realCnt++;
			keyNow[CFG_EC_DIR] = 1;
		}
	}
	ifGap = !ifGap;//间隔标志翻转
}

void readKey(void){//读取按键并滤波
	uint8_t i;
	static uint8_t keyRaw[3] = {0,0,0};		//按键原始采样
	static uint8_t keyFlt[3] = {0,0,0};		//按键滤波结果
	static uint8_t fltOld[3] = {0,0,0};		//按键滤波用的旧值
	static uint8_t fltCount[3] = {5,5,5};	//按键滤波计数
	//keyRaw[0] = !EC_KEY; keyRaw[1] = !KEY; keyRaw[2] = SW;	//读取输入
	for(i = 0; i < 3; i++){
		if(fltCount[i]) fltCount[i]--;	//若滤波计数未归零则递减
		if(fltOld[i] == keyRaw[i] && keyFlt[i] != keyRaw[i] && fltCount[i] == 0)
		{												//两次相同且与之前不同且滤波计数已归零
			keyFlt[i] = keyRaw[i];	//更新滤波后的值
			fltCount[i] = 25;		//设置滤波计数值
			if(i == 2) fltCount[i] = 150;	//SW额外增加滤波计数值
		}
		fltOld[i] = keyRaw[i];	//更新滤波旧值
	}
	memcpy(keyOld + 2, keyNow + 2, 3);	//记录旧按键
	memcpy(keyNow + 2, keyFlt, 3);		//载入新按键
}

void RSV_ISR(void) interrupt 13 {	//保留中断 用于处理DMA_ADC 约3.10ms
	static UINT16D limitTime = 0;//单位0.1ms

	DMA_ADC_STA = 0;	//清中断
	DMA_ADC_CR = 0xC0;	//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA

	if(CUSB_CUT){//若已被过流关断
		limitTime += 31;//粗略计时
		if(CFG_USB_REST && limitTime > CFG_USB_REST * 1000){//若允许重开且已到时间
			CUSB_CUT = 0;//允许重开
			limitTime = 0;//计时清零
		}
		return;//退出
	}
	if(ADC_USB_LIMIT){//若启用限流
		if(ADC_DATA[1][ADC_VN + 1] < ADC_USB_LIMIT){//过流
			if(limitTime > CFG_USB_DELAY * 10){//大于关断延迟时间
				//CUSB_CUT = GATE = 1;//关断
				limitTime = 0;//计时清零
			}
			else limitTime += 31;//粗略计时
		}
		else{//不过流
			if(limitTime){
				limitTime -= 31;//粗略计时
				if(limitTime > 60000) limitTime = 0;//若反向溢出则归零
			}
		}
	}
}


void funcHandle(void)//功能处理
{
	static uint16_t oldTime = 0, newTime = 0;
	
	//Systime += 5;//粗略计时(测试代码)
	//Systime += (UINT16D)(((PWMB_CNTRH << 8) | PWMB_CNTRL) - (UINT16D)Systime);//获取系统时间
	Systime += (UINT16D)(PWMB_CNTR - (UINT16D)Systime);//获取系统时间
	
	
	readKey();//读取按键
	readEncoder((PWMA_CNTRH << 8) | PWMA_CNTRL);//读取编码器
	
	if(usbMode == 0 && CFG_G24_MODE == 1 && (G24_Chip_State & 0x80)){//HID模式 启用2.4G 芯片在位
		g24BufInput();//2.4G缓存数据输入
	}
	else LedInput[6] = 0;//清零2.4G状态
	
	generateReport();//由用户操作和配置参数生成HID报文(HID模式核心函数)
	
	sysRGB();		//灯效处理
	funcOutput();	//各类输出
	//WS_Write_1();	//灯写入
	
	
//	newTime = (PWMB_CNTRH << 8) | PWMB_CNTRL;//获取新时间
	newTime = PWMB_CNTR;//获取新时间
	if((uint16_t)(newTime - oldTime) >= 1000){//定时处理 测试代码
		oldTime += 1000;
		//PRINT1("T:%u\n",newTime);//打印 测试代码
	}
}




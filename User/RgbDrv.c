
#include "RgbDrv.h"

UINT32D Systime = 0;//ϵͳʱ��

uint8_t PRINT_BUF[64];//��ӡ����

uint8_t keyNow[5] = {0,0,0,0,0};//��ǰ����
uint8_t keyOld[5] = {0,0,0,0,0};//�ϴΰ���

UINT16X ADC_DATA[3][ADC_VN + 2];//ADC����
UINT16D ADC_USB_LIMIT = 0;//����ֵ(����)

bit CUSB_CUT = 0, CUSB_EN = 0, HUB_EN = 1;//CUSB�����ж�,CUSBʹ��,HUBʹ��

uint8_t LedInput[9];//LED����:0-NUM,1-CAPS,2-SCROLL,3-CUSB,4-HUB,5-hidCs,6-2.4G,7-CUSTOM1,8-CUSTOM2
uint8_t LedState[4] = {0,0,0,0};//LED״̬
UINT8D FrameBuf[1*3] = {0,0,0};//GRB֡����



//H:0~colorAngle*6,S:0~100(����delta����),V:0~255
//void rgbToHsv(uint8_t vR, uint8_t vG, uint8_t vB, uint16_t* pH, uint16_t* pS, uint16_t* pV){//RGBתHSV
//    uint8_t max = MAX(MAX(vR,vG),vB), min = MINI(MINI(vR,vG),vB);
//    uint8_t delta = max - min;
//    if(delta == 0) *pH = 0;
//    else if(max == vR) *pH = (uint16_t)colorAngle*(vG-vB)/delta;
//    else if(max == vG) *pH = (uint16_t)colorAngle*(vB-vR)/delta + colorAngle*2;
//    else if(max == vB) *pH = (uint16_t)colorAngle*(vR-vG)/delta + colorAngle*4;
//    if(*pH > colorAngle * 6) *pH += colorAngle * 6;
//    if(max == 0) *pS = 0;
//    else *pS = delta;//100 * delta / max;//ע��˴�Sֱ����delta����,�ʺ�����ֱ���޸�V���Ϸ�
//    *pV = max;
//}

//void hsvToRgb(uint16_t vH, uint16_t vS, uint16_t vV, uint8_t* pR, uint8_t* pG, uint8_t* pB){//HSVתRGB
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


UINT8C rgbCycle[3] = {3,6,30};//RGB���ڱ�(��λs)
void sysRGB(){//ϵͳRGB��Ч
	uint8_t r = 0, g = 0, b = 0;
	uint16_t h, s, v;
	
	if(CFG_LIGHT >= 1 && CFG_LIGHT <= 3){//ɫ�ʱ仯ģʽ
//		rgbToHsv(CFG_BRIGHT, 0, 0, &h, &s, &v);
//		h += (Systime / rgbCycle[CFG_LIGHT - 1]) % (colorAngle * 6);
//		if(h >= colorAngle * 6) h -= colorAngle * 6;
//		hsvToRgb(h, s, v, &r, &g, &b);
	}
	else if(CFG_LIGHT == 4){//����ָʾģʽ
//		rgbToHsv(CFG_BRIGHT, 0, 0, &h, &s, &v);
//		h = colorAngle * 3 - colorAngle * 3 * (uint32_t)(4095 - ADC_DATA[1][ADC_VN + 1]) / (4095 - ADC_USB_LIMIT);//���̻ƺ�
//		if(h >= colorAngle * 6) h = 0;//�������
//		if(CUSB_CUT) h = colorAngle * 5;//�������ض���Ϊ��ɫ
//		hsvToRgb(h, s, v, &r, &g, &b);
	}
	else if(CFG_LIGHT == 5){//�Զ���1
		
	}
	else if(CFG_LIGHT == 6){//�Զ���2
		
	}

	FrameBuf[0] = g;
	FrameBuf[1] = r;
	FrameBuf[2] = b;
}

void funcOutput(void){//�������
	uint8_t i;
	//���
	//if(!CUSB_CUT) GATE = !CUSB_EN;//��δ���ڹ����ض���ִ��CUSB���
	//CRST = HUB_EN;//ִ��HUB���
	//������
	//LedInput[3] = !GATE;//����CUSB״̬
	//LedInput[4] = CRST;//����HUB״̬
	LedInput[5] = hidCs;//����hidCs
	//LedInput[6] = 0;//����2.4G״̬
	LedInput[7] = 0;//�����Զ���1
	LedInput[8] = 0;//�����Զ���2
	//��ӳ��
	for(i = 0; i < 4; i++){//L R G B
		if(CFG_LED_MAP(i)){//������ӳ��
			LedState[i] = LedInput[CFG_LED_MAP(i) - 1];//��LED������ӳ��
			if(CFG_LED_DIR(i)) LedState[i] = !LedState[i];//����
		}
		else LedState[i] = 0xFF;//δ����ӳ��
	}
	//����ʾ���Դ�д��
//	if(LedState[0] == 0) LED_OFF;
//	else if(LedState[0] == 1) LED_ON;
	if(LedState[1] != 0xFF) FrameBuf[1] = LedState[1] * CFG_LED_BRT(1);//R
	if(LedState[2] != 0xFF) FrameBuf[0] = LedState[2] * CFG_LED_BRT(2);//G
	if(LedState[3] != 0xFF) FrameBuf[2] = LedState[3] * CFG_LED_BRT(3);//B
}


void readEncoder(int8_t newCnt){//��ȡ������
	static uint8_t ifGap = 0;	//�����־
	static int8_t oldCnt = 0, realCnt = 0;	//�ɼ���ֵ,ʵ�ʸ���
	int8_t difCnt = newCnt - oldCnt;		//������ֵ
	difCnt = difCnt / 4;	//������ֵ��Ƶ�ø�����ֵ
	oldCnt += difCnt * 4;	//�ɼ���ֵ����
	realCnt += difCnt;		//������ֵ�ӵ�ʵ�ʸ�����
	if(0) oldCnt = newCnt;	//����ʱ�䲻��������У׼�ɼ���ֵ
	memcpy(keyOld, keyNow, 2);	//��¼�ɰ���
	if(realCnt == 0 || ifGap) memset(keyNow, 0, 2);	//������Ϊ0��Ϊ��������ЧΪ̧��
	else{	//������Ϊ0�Ҳ�Ϊ�����
		if(realCnt > 0){	//������0
			realCnt--;
			keyNow[!CFG_EC_DIR] = 1;
		}
		else{	//������0
			realCnt++;
			keyNow[CFG_EC_DIR] = 1;
		}
	}
	ifGap = !ifGap;//�����־��ת
}

void readKey(void){//��ȡ�������˲�
	uint8_t i;
	static uint8_t keyRaw[3] = {0,0,0};		//����ԭʼ����
	static uint8_t keyFlt[3] = {0,0,0};		//�����˲����
	static uint8_t fltOld[3] = {0,0,0};		//�����˲��õľ�ֵ
	static uint8_t fltCount[3] = {5,5,5};	//�����˲�����
	//keyRaw[0] = !EC_KEY; keyRaw[1] = !KEY; keyRaw[2] = SW;	//��ȡ����
	for(i = 0; i < 3; i++){
		if(fltCount[i]) fltCount[i]--;	//���˲�����δ������ݼ�
		if(fltOld[i] == keyRaw[i] && keyFlt[i] != keyRaw[i] && fltCount[i] == 0)
		{												//������ͬ����֮ǰ��ͬ���˲������ѹ���
			keyFlt[i] = keyRaw[i];	//�����˲����ֵ
			fltCount[i] = 25;		//�����˲�����ֵ
			if(i == 2) fltCount[i] = 150;	//SW���������˲�����ֵ
		}
		fltOld[i] = keyRaw[i];	//�����˲���ֵ
	}
	memcpy(keyOld + 2, keyNow + 2, 3);	//��¼�ɰ���
	memcpy(keyNow + 2, keyFlt, 3);		//�����°���
}

void RSV_ISR(void) interrupt 13 {	//�����ж� ���ڴ���DMA_ADC Լ3.10ms
	static UINT16D limitTime = 0;//��λ0.1ms

	DMA_ADC_STA = 0;	//���ж�
	DMA_ADC_CR = 0xC0;	//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA

	if(CUSB_CUT){//���ѱ������ض�
		limitTime += 31;//���Լ�ʱ
		if(CFG_USB_REST && limitTime > CFG_USB_REST * 1000){//�������ؿ����ѵ�ʱ��
			CUSB_CUT = 0;//�����ؿ�
			limitTime = 0;//��ʱ����
		}
		return;//�˳�
	}
	if(ADC_USB_LIMIT){//����������
		if(ADC_DATA[1][ADC_VN + 1] < ADC_USB_LIMIT){//����
			if(limitTime > CFG_USB_DELAY * 10){//���ڹض��ӳ�ʱ��
				//CUSB_CUT = GATE = 1;//�ض�
				limitTime = 0;//��ʱ����
			}
			else limitTime += 31;//���Լ�ʱ
		}
		else{//������
			if(limitTime){
				limitTime -= 31;//���Լ�ʱ
				if(limitTime > 60000) limitTime = 0;//��������������
			}
		}
	}
}


void funcHandle(void)//���ܴ���
{
	static uint16_t oldTime = 0, newTime = 0;
	
	//Systime += 5;//���Լ�ʱ(���Դ���)
	//Systime += (UINT16D)(((PWMB_CNTRH << 8) | PWMB_CNTRL) - (UINT16D)Systime);//��ȡϵͳʱ��
	Systime += (UINT16D)(PWMB_CNTR - (UINT16D)Systime);//��ȡϵͳʱ��
	
	
	readKey();//��ȡ����
	readEncoder((PWMA_CNTRH << 8) | PWMA_CNTRL);//��ȡ������
	
	if(usbMode == 0 && CFG_G24_MODE == 1 && (G24_Chip_State & 0x80)){//HIDģʽ ����2.4G оƬ��λ
		g24BufInput();//2.4G������������
	}
	else LedInput[6] = 0;//����2.4G״̬
	
	generateReport();//���û����������ò�������HID����(HIDģʽ���ĺ���)
	
	sysRGB();		//��Ч����
	funcOutput();	//�������
	//WS_Write_1();	//��д��
	
	
//	newTime = (PWMB_CNTRH << 8) | PWMB_CNTRL;//��ȡ��ʱ��
	newTime = PWMB_CNTR;//��ȡ��ʱ��
	if((uint16_t)(newTime - oldTime) >= 1000){//��ʱ���� ���Դ���
		oldTime += 1000;
		//PRINT1("T:%u\n",newTime);//��ӡ ���Դ���
	}
}




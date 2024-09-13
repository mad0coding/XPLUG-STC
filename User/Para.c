
#include "Para.h"

UINT8D para[PARA_LEN];//��������

//******************************���ķ���******************************//
extern UINT8I KeyBrd_data[];//���̱���
extern UINT8I Mouse_data[];//��걨��
extern UINT8I Point_data[];//ָ�뱨��
extern UINT8I Vol_data[];//��������
UINT8I KeyBrd_data_old[KB_len];//�ϴμ��̱���
UINT8I Mouse_data_old = 0;//�ϴ���걨��
UINT8I Vol_data_old = 0;//�ϴ���������
extern UINT8I KeyBrd_if_send;//���̱����Ƿ���
extern UINT8I Vol_if_send;//���������Ƿ���
extern UINT8I Point_if_send;//ָ�뱨���Ƿ���
extern UINT8I Mouse_if_send;//��걨���Ƿ���
//********************************************************************//

//******************************2.4G��������******************************//
#define HID_G24_BUF		BUF_4K
extern UINT8X BUF_4K[4096];//HIDģʽ��Ϊ���λ�����,U��ģʽ��Ϊ���滺��
uint8_t G24_RX_BUF[32];//���߽��ջ�����
uint16_t g24BufStart = 0, g24BufEnd = 0;//���λ�������ʼ��ĩβ
uint8_t G24_Chip_State = 0;//����оƬ״̬:bit7Ϊ�Ƿ���λ,bit6Ϊ�Ƿ�ִ�й���ʼ��

uint8_t G24_KeyBrd_data[KB_len - 4] = {1,0,0,0};//���1,���ܼ�,����0,��������
uint8_t G24_Mouse_data[5] = {2,0,0,0,0};//���2,���ܼ�,x,y,����
uint8_t G24_Point_data[7] = {3,0x10,1,0xFF,0xFF,0xFF,0xFF};//���3,���ܼ�,id,x_L,x_H,y_L,y_H
uint8_t G24_Vol_data[2] = {4,0};//���4,���ܼ�
//************************************************************************//

UINT8I usbMode = 0;//USBģʽ:0ΪHIDģʽ,1ΪU��ģʽ
UINT8I hidCs = 0;//HID����ѡ��


void keyInsert(uint8_t r_i, uint8_t key_v){//��������
	if(key_v == kv_wheel_down) Mouse_data[4] += -1;//��������
	else if(key_v == kv_wheel_up) Mouse_data[4] += 1;//��������
	else if(key_v == kv_vol_up) Vol_data[1] |= 0x01;//������
	else if(key_v == kv_vol_down) Vol_data[1] |= 0x02;//������
	else if(key_v == kv_vol_mute) Vol_data[1] |= 0x04;//����
	else if(key_v == kv_vol_stop) Vol_data[1] |= 0x08;//������ͣ
	else if(key_v == kv_vol_next) Vol_data[1] |= 0x10;//��һ��
	else if(key_v == kv_vol_prev) Vol_data[1] |= 0x20;//��һ��
	else if(key_v == kv_mouse_l) Mouse_data[1] |= 0x01;//������
	else if(key_v == kv_mouse_m) Mouse_data[1] |= 0x04;//����м�
	else if(key_v == kv_mouse_r) Mouse_data[1] |= 0x02;//����Ҽ�
	else if(key_v == kv_ctrl) KeyBrd_data[1] |= 0x01;//ctrl
	else if(key_v == kv_shift) KeyBrd_data[1] |= 0x02;//shift
	else if(key_v == kv_alt) KeyBrd_data[1] |= 0x04;//alt
	else if(key_v == kv_win) KeyBrd_data[1] |= 0x08;//win
	else{//��ͨ���̰���
		if(r_i == 0xFF){//��ʹ���Զ�����
			for(r_i = 3; r_i < KB_len; r_i++){//������λ
				if(!KeyBrd_data[r_i]) break;//���˴�Ϊ��
			}
		}
		if(r_i == 0xFF) return;//��δ�ҵ���λ���˳�
		KeyBrd_data[r_i] = key_v;//�����ֵ
	}
}

void generateReport(void){//���û����������ò�������HID����
	uint8_t i = 0;//�����ü�������
	
	//***********************************�����ļ����ͱ�־��ʼ��***********************************//
	KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;//���ͱ�־����
	
	memcpy(KeyBrd_data_old + 1, KeyBrd_data + 1, KB_len - 1);//��¼��һ�α���
	memset(KeyBrd_data + 1, 0, KB_len - 1);//������̱���

	Mouse_data_old = Mouse_data[1];//��¼��һ�α���
	memset(Mouse_data + 1, 0, 4);//�����걨��

	Point_data[1] = 0x10;
	memset(Point_data + 3, 0xFF, 4);//���ָ�뱨��

	Vol_data_old = Vol_data[1];//��¼��һ�α���
	Vol_data[1] = 0;//�����������
	//********************************************************************************************//
	
	if(/*keyAddr[sys_cs][0] == */0) return /*0xFF*/;//�����������ݴ������˳�
	
	//****************************************���ش���****************************************//
	i = (CFG_SW_DIR == keyNow[4] ? 0 : 1);//�ݴ濪�ص��߼�״̬
	if(keyNow[4] != keyOld[4]){//��Ϊ����
		if(CFG_SW_MODE == 1){//ģʽ1:HID�����л�
			HID_MODE_CS(i);
		}
		else if(CFG_SW_MODE == 2){//ģʽ2:CUSB����
			CUSB_EN = i;
		}
		else if(CFG_SW_MODE == 3){//ģʽ3:HUB��λ
			HUB_EN = i;
		}
		else if(CFG_SW_MODE == 4){//ģʽ4:USBģʽ�л�
			USB_MODE_CS(i);
		}
		else if(CFG_SW_MODE == 5){//ģʽ5:�Զ���1
			
		}
		else if(CFG_SW_MODE == 6){//ģʽ5:�Զ���2
			
		}
	}
	//****************************************************************************************//
	
	//****************************************���ܰ�������****************************************//
	for(i = 0; i < 4; i++){//����4������
		if(keyNow[i]){//������
			if(CFG_KEY_MODE(i) == 1){//ģʽ1:����(������/��ݼ�)
				KeyBrd_data[1] |= CFG_KEY_FUNC(i);//���빦�ܼ�(����)
				keyInsert(i + 3, CFG_KEY_KV(i));//�����ֵ
			}
			if(!keyOld[i]){//���հ���(������)
				if(CFG_KEY_MODE(i) == 2){//ģʽ2:CUSB����
					CUSB_EN = !CUSB_EN;//CUSB״̬��ת
				}
				else if(CFG_KEY_MODE(i) == 3){//ģʽ3:CUSB����
					CUSB_EN = 0;
				}
				else if(CFG_KEY_MODE(i) == 4){//ģʽ4:HUB��λ
					HUB_EN = 0;
				}
				else if(CFG_KEY_MODE(i) == 7){//ģʽ7:�Զ���1(�����������Boot��ת����,������)
					USBCON = 0x00;//�ر�USB
					delay_ms(200);//��������⵽�豸�γ�
					STC8H_SoftReset(1);//����������Boot
				}
				else if(CFG_KEY_MODE(i) == 8){//ģʽ8:�Զ���2(�����������CHϵ��Ƭ�����ش�������,������)
//					GATE_OFF;		//CUSB������
//					delay_ms(200);	//��������⵽�豸�γ�
//					DP_PU_ON;		//CUSB��DP����
//					GATE_ON;		//CUSB������
//					CUSB_EN = 1;	//CUSB���ÿ�(��֮��ʼCHϵ��Ƭ������)
				}
			}
		}
		else if(keyOld[i]){//����̧��(�ͷ���)
			if(CFG_KEY_MODE(i) == 3){//ģʽ3:CUSB����
				CUSB_EN = 1;//CUSB�ϵ�
			}
			else if(CFG_KEY_MODE(i) == 4){//ģʽ4:HUB��λ
				HUB_EN = 1;//HUBʹ��
			}
			else if(CFG_KEY_MODE(i) == 5){//ģʽ5:HID�����л�
				HID_MODE_TOG;
			}
			else if(CFG_KEY_MODE(i) == 6){//ģʽ6:USBģʽ�л�
				USB_MODE_TOG;
			}
			else if(CFG_KEY_MODE(i) == 7){//ģʽ7:�Զ���1
				
			}
			else if(CFG_KEY_MODE(i) == 8){//ģʽ8:�Զ���2(�����������CHϵ��Ƭ�����ش�������,������)
//				GATE_OFF;		//CUSB������
//				DP_PU_OFF;		//CUSB��DPȡ������
//				delay_ms(200);	//��������⵽�豸�γ�
//				GATE_ON;		//CUSB������
			}
		}
	}//������4����������Ҫ����
	//********************************************************************************************//
	
	if(usbMode == 0 && CFG_G24_MODE == 1 && (G24_Chip_State & 0x80)){//HIDģʽ ����2.4G оƬ��λ
		g24BufOutput();//2.4G�����������
	}
	
	//***********************************�жϸ������Ƿ�Ҫ����***********************************//
	for(i = 1; i < KB_len; i++){
		if(KeyBrd_data_old[i] != KeyBrd_data[i]){//���̱�������һ�β�ͬ����
			KeyBrd_if_send = 1;	break;
		}
	}
	if(Mouse_data[1] != Mouse_data_old) Mouse_if_send = 1;//��갴�����ϴβ�ͬ����
	else{
		for(i = 2; i < 5; i++){
			if(Mouse_data[i] != 0){//�������ƶ����������
				Mouse_if_send = 1;	break;
			}
		}
	}
	for(i = 3; i < 7; i++){
		if(Point_data[i] != 0xFF){//����������Ч��������
			Point_if_send = 1;	break;
		}
	}
	if(Vol_data[1] != Vol_data_old) Vol_if_send = 1;//ý�屨�����ϴβ�ͬ����
	
	if(usbMode != 0){//��HIDģʽ
		KeyBrd_if_send = Mouse_if_send = Point_if_send = Vol_if_send = 0;//ȫ����Ĭ
	}
	//******************************************************************************************//
}


void paraInit(uint8_t ifHot){//�ò�����ʼ��ϵͳ
	//uint8_t tmpSW = (CFG_SW_DIR == SW ? 0 : 1);//�ݴ濪�ص��߼�״̬
	if(!ifHot) paraRead();//��Ϊ���ʼ�����ȡ����
	//Ĭ������
	HID_MODE_CS(0);
	CUSB_EN = !CFG_USB_DEFAULT;
	HUB_EN = 1;
	if(!ifHot){//���л�
		usbMode = CFG_BOOT_DISK && !KEY;//�ϵ�ʱ��������������˹��������U��
		//usbMode = 1;//����U�� ���Դ���
	}
	else if(usbMode != 0){//���л�����Ҫ�л�
		USB_MODE_CS(0);
	}
	//���ؾ���������
	if(CFG_SW_MODE == 1){//ģʽ1:HID�����л�
		//HID_MODE_CS(tmpSW);
	}
	else if(CFG_SW_MODE == 2){//ģʽ2:CUSB����
		//CUSB_EN = tmpSW;
	}
	else if(CFG_SW_MODE == 3){//ģʽ3:HUB��λ
		//HUB_EN = tmpSW;
	}
	else if(CFG_SW_MODE == 4){//ģʽ4:USBģʽ�л�
//		if(!ifHot) usbMode = tmpSW;//���л�
//		else if(usbMode != tmpSW){//���л�����Ҫ�л�
//			USB_MODE_CS(tmpSW);
//		}
	}
	if(CFG_USB_LIMIT == 0) ADC_USB_LIMIT = 0;//����������
	else ADC_USB_LIMIT = 4095 - CFG_USB_LIMIT * 273 / 11;//��������ADCֵ
	funcOutput();//�������
	if(ifHot && CFG_G24_MODE != 0 && !(G24_Chip_State & 0x40)){//���л� ����2.4G 2.4Gδ��ʼ����
		NRF24L01_Config(0);//����ģ���ʼ��Ϊ����ģʽ
	}
}

void paraRead(void){//��ȡ����
	EEPROM_read_n(PARA_ADDR, para, PARA_LEN);
}

void paraWrite(void){//д�����
	EEPROM_SectorErase(PARA_ADDR);
	EEPROM_write_n(PARA_ADDR, para, PARA_LEN);
}


/**************************************************2.4G���ݻ����봦��**************************************************/
uint16_t g24BufLen(void){//��ȡ2.4G���泤��
	if(g24BufEnd < g24BufStart) return (4096 + g24BufEnd - g24BufStart);
	return (g24BufEnd - g24BufStart);
}

uint8_t g24BufInput(void){//2.4G������������
	static uint8_t g24_flag = 0, g24_state = 1;//2.4G���ձ�־������״̬
	g24_flag = NRF24L01_RxPkg(G24_RX_BUF);	//0Ϊδ�ӵ�,0xFFΪ����,����Ϊ���ճ���
	if(g24_flag == 0xFF){	//���ж�����
		if(g24_state >= 2){		//��Ϊ��������̬
			g24_state = 1;			//�������Ԥ��
			//�������HID����
			memset(G24_KeyBrd_data + 1, 0, KB_len - 1 - 4);//������м��̱���
			memset(G24_Mouse_data + 1, 0, 4);//�����걨��
			G24_Point_data[1] = 0x10;
			memset(G24_Point_data + 3, 0xFF, 4);//���ָ�뱨��
			G24_Vol_data[1] = 0;//�����������
		}	
		LedInput[6] = 0;//ָʾ����
	}
	else if(g24_flag > 0){	//���ӵ�����
		g24_state = 2;			//������������̬
		g24BufWrite(G24_RX_BUF, g24_flag);//д��2.4G����
		LedInput[6] = 1;//ָʾ����
		return 1;
	}
	return 0;
}

uint8_t g24BufOutput(void){//2.4G�����������
	uint8_t pkgHead = 0;//֡ͷ
	if(g24BufLen() == 0) goto Merge;//������
	pkgHead = HID_G24_BUF[g24BufStart];//��ȡ֡ͷ
	if(pkgHead == 1) g24BufRead(G24_KeyBrd_data, KB_len - 4);//���̱���
	else if(pkgHead == 2) g24BufRead(G24_Mouse_data, 5);//��걨��
	else if(pkgHead == 3) g24BufRead(G24_Point_data, 7);//��������
	else if(pkgHead == 4) g24BufRead(G24_Vol_data, 2);//ý�屨��
	else return 0xFF;//���ݴ���
	
	Merge://�ϲ�����
	KeyBrd_data[1] |= G24_KeyBrd_data[1];
	memcpy(KeyBrd_data + 3 + 4, G24_KeyBrd_data + 3, KB_len - 4 - 3);//�������ռ��ǰ4����ͨ���ֽ�
	
	Mouse_data[1] |= G24_Mouse_data[1];
	Mouse_data[2] += G24_Mouse_data[2];
	Mouse_data[3] += G24_Mouse_data[3];
	Mouse_data[4] += G24_Mouse_data[4];
	
	memcpy(Point_data, G24_Point_data, 7);//���岻�����������Ĺ�ֱ�Ӹ���
	
	Vol_data[1] |= G24_Vol_data[1];
	return pkgHead;
}

uint8_t g24BufRead(uint8_t *buf, uint8_t len){//��ȡ2.4G����
	uint16_t g24BufLenNow = g24BufLen();
	if(g24BufLenNow == 0) return 0;//������
	if(g24BufLenNow < len) len = g24BufLenNow;
	while(len--){
		*buf++ = HID_G24_BUF[g24BufStart++];
		if(g24BufStart >= 4096) g24BufStart = 0;//��һȦ
	}
	return len;
}

uint8_t g24BufWrite(uint8_t *buf, uint8_t len){//д��2.4G����
	if((len != KB_len - 4 || buf[0] != 1) && (len != 5 || buf[0] != 2) 
			 && (len != 7 || buf[0] != 3) && (len != 2 || buf[0] != 4)) return 1;//���Ȼ�֡ͷ����
	if(g24BufLen() + len >= 4096) return 2;//�ռ䲻��
	while(len--){
		HID_G24_BUF[g24BufEnd++] = *buf++;
		if(g24BufEnd >= 4096) g24BufEnd = 0;//��һȦ
	}
	return 0;
}
/**********************************************************************************************************************/














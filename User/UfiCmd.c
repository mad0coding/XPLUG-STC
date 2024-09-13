
#include "UfiCmd.h"

#pragma  NOAREGS

//UINT8X FlashBuf[1024] _at_ (1024);//���û�������
//uint16_t FlashBufStart = 0, FlashBufEnd = 0;

//uint8_t dbgLen = 0;
//uint32_t dbgBuf[32];
//uint16_t statePkg = 0;

bit UdiskFlagUp = 0;			//�����ϴ���־
bit UdiskFlagDown = 0;			//�����´���־
bit UdiskFlagCSW = 0;			//CSW�ϴ���־
bit FSTALL, lastFSTALL;		//���ݴ����־
bit pBufReSelect = 0;		//ָ����ѡ��־

uint8_t	bCswStatus;			//CSW״̬��־
uint8_t mSenseKey;
uint8_t mASC;

uint32_t dCBWTag = 0;	//�ݴ�CBW��ǩ�Թ�CSWʹ��
UINT32D Locate_Addr;

uint8_t *pBuf;	//���ݶ�ȡָ��

#define MAX_PACKET_SIZE	64

uint8_t MASK_UEP_T_RES,UEP_T_RES_ACK,MASK_UEP_R_RES;
//uint8_t UEP1_T_LEN = 13;
uint8_t UEP1_CTRL;	//�����ϴ�

uint8_t Ep1BulkBuf[64+64];
uint8_t USB_RX_LEN = 0;//�������ݳ���

enum _HOST_DEV_DISAGREE{
	CASEOK = 0,
	CASE1,
	CASE2,
	CASE3,
	CASE4,
	CASE5,
	CASE6,
	CASE7,
	CASE8,
	CASE9,
	CASE10,
	CASE11,
	CASE12,
	CASE13,
	CASECBW,
	CASECMDFAIL,
};

union{
	uint32_t mDataLength;	//���ݳ���
	uint8_t mdataLen[4];	//���ݳ��ȵ�4�ֽ�,������С��ת��
}LEN;


MASS_PARA MassPara;


//INQUIRY Info (36B)
UINT8C DBINQUITY[] = {
	0x00,	// �豸����
	0x80,	// bit7=1������Ƴ�����
	0x02,	// ISO Version(2bit) ECMA Version(3bit) ANSI Version(3bit)
	0x02,	// UFI�淶Ҫ��Ϊ1��SCSI�淶Ҫ����ڵ���2
	0x1f,	// ���ӳ���31�ƺ���ʾ���滹�ж����ֽ�
	0,0,0,	// 5~7 Reserved
	'w','c','h','.','c','n',' ',' ',	// 8~15 Vendor Information(����룬��ͬ)
//	0xc7,0xdf,0xba,0xe3,0xb5,0xe7,0xd7,0xd3,0x55,0xc5,0xcc,0xb7,0xbd,0xb0,0xb8,0x00,	// 16~31 Product Identification
	'X','P','L','U','G','-','C','H','-','U','d','i','s','k',' ',' ',	// 16~31 Product Identification
	'1','.','1','0',	// 32~35 Product Revision Level
};
UINT8C DBCAPACITY[] = {
	(DISK_SEC_LAST>>24)&0xFF, (DISK_SEC_LAST>>16)&0xFF, (DISK_SEC_LAST>>8)&0xFF, DISK_SEC_LAST&0xFF, 
	(DISK_SEC_LEN>>24)&0xFF, (DISK_SEC_LEN>>16)&0xFF, (DISK_SEC_LEN>>8)&0xFF, DISK_SEC_LEN&0xFF,
};	//last logic addr//block lenth
UINT8C DBFMTCAPACITY[] = {
	//Capacity List Header
	0,0,0,//Reserved
	8*1,//����ĳ���
	//Current/Maximum Capacity Descriptor
	(DISK_SEC_NUM>>24)&0xFF, (DISK_SEC_NUM>>16)&0xFF, (DISK_SEC_NUM>>8)&0xFF, DISK_SEC_NUM&0xFF, 
	0x02,//Descriptor Code(0x02 = Formatted Media - Current media capacity)
	/*(DISK_SEC_LEN>>24)&0xFF, */(DISK_SEC_LEN>>16)&0xFF, (DISK_SEC_LEN>>8)&0xFF, DISK_SEC_LEN&0xFF,
};
UINT8C modesense3F[] = {
	0x0b, 0x00, 0x00, 0x08, (DISK_SEC_NUM>>24)&0xFF, (DISK_SEC_NUM>>16)&0xFF, (DISK_SEC_NUM>>8)&0xFF, DISK_SEC_NUM&0xFF, 00, 00, 02, 00,
};	//����������
UINT8C mode5sense3F[] = {
	0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08,
	(DISK_SEC_NUM>>24)&0xFF, (DISK_SEC_NUM>>16)&0xFF, (DISK_SEC_NUM>>8)&0xFF, DISK_SEC_NUM&0xFF, 00, 00, 02, 00,
};	//����������


void BulkThirteen(uint8_t Case){
	switch(Case)
	{
	case CASEOK:
	case CASE1:     									/* Hn=Dn*/
	case CASE6:     									/* Hi=Di*/
		bCswStatus = 0;
		break;
	case CASE12:    									/* Ho=Do*/
		bCswStatus = 0;
		break;
	case CASE2:     									/* Hn<Di*/
	case CASE3:     									/* Hn<Do*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES;
		FSTALL = 1;										//�����ϴ��˵�����һ��STALL����������� // may or may-not
		bCswStatus = 2;
		break;
	case CASE4:     									/* Hi>Dn*/
	case CASE5:     									/* Hi>Di*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;										//�����ϴ��˵�����һ��STALL�����������
		bCswStatus = 1;									//CSW_GOOD or CSW_FAIL
		break;
	case CASE7:    										 /* Hi<Di*/
	case CASE8:    										 /* Hi<>Do */
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;										//�����ϴ��˵�����һ��STALL�����������
		bCswStatus = 2;
		break;
	case CASE9:    										 /* Ho>Dn*/
	case CASE11:    									 /* Ho>Do*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_R_RES ;
		FSTALL = 1;										//�����ϴ��˵�����һ��STALL�����������
		bCswStatus = 1;									//CSW_GOOD or CSW_FAIL
		break;
	case CASE10:    								/* Ho<>Di */
	case CASE13:    								/* Ho<Do*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;
													//�����ϴ��˵�����һ��STALL�����������
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_R_RES ;
													//�����ϴ��˵�����һ��STALL�����������
		bCswStatus = 2;
		break;
	case CASECBW:   								/* invalid CBW */
	    UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_R_RES ;
											//����˵�����һ��STALL�����������
		bCswStatus = 2;
		break;
	case CASECMDFAIL:
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL=1;
												//�����ϴ��˵�����һ��STALL�����������
		bCswStatus= 1;
		break;
	default:
		break;
	}
}

void UFI_inquiry(void){//��ѯU����Ϣ
	if(LEN.mDataLength > sizeof(DBINQUITY)) LEN.mDataLength = sizeof(DBINQUITY);
	pBuf = DBINQUITY;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_readCapacity(void){//��ȡ����
	if(LEN.mDataLength > sizeof(DBCAPACITY)) LEN.mDataLength = sizeof(DBCAPACITY);
	pBuf = (uint8_t*)DBCAPACITY;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_readFormatCapacity(void){//��ȡ��ʽ������
	if(LEN.mDataLength > sizeof(DBFMTCAPACITY)) LEN.mDataLength = sizeof(DBFMTCAPACITY);
	pBuf = (uint8_t*)DBFMTCAPACITY;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_modeSense(void){//ģʽ��ʶ
//	dbg1("s%bu",MassPara.Cbw.Cbwcb[2]);
	if(/*MassPara.Cbw.Cbwcb[2] == 0x3F*/1){
//		dbg("S");
		if(LEN.mDataLength > sizeof(modesense3F)) LEN.mDataLength = sizeof(modesense3F);
		pBuf = modesense3F;
		bCswStatus = mSenseKey = mASC = 0;
	}
	else{
		UdiskFlagUp = 0;
		mSenseKey = 5;
		mASC = 0x20;

		bCswStatus = 1;
		BulkThirteen(CASECMDFAIL);
	}
}
void UFI_modeSense5(void){//ģʽ��ʶ5(��δ���ֵ���)
	if(MassPara.Cbw.Cbwcb[2] == 0x3F){
		if(LEN.mDataLength > sizeof(mode5sense3F)) LEN.mDataLength = sizeof(mode5sense3F);
		pBuf = mode5sense3F;
		bCswStatus = mSenseKey = mASC = 0;
	}
	else{
		UdiskFlagUp = 0;
		mSenseKey = 5;
		mASC = 0x20;
		bCswStatus = 1;
		BulkThirteen(CASECMDFAIL);
	}
}
void UFI_requestSense(void){//������ʶ(��δ���ֵ���)
	if(FSTALL | lastFSTALL){
		lastFSTALL = FSTALL;
		FSTALL = 0;
		MassPara.Sense.ErrorCode = 0x70;
		MassPara.Sense.Reserved1 = 0;
		MassPara.Sense.SenseKey = mSenseKey;
		MassPara.Sense.Information[0] = MassPara.Sense.Information[1] = 0;
		MassPara.Sense.Information[2] = MassPara.Sense.Information[3] = 0;
		MassPara.Sense.AddSenseLength = 0x0a;
		MassPara.Sense.Reserved2[0] = MassPara.Sense.Reserved2[1] = 0;
		MassPara.Sense.Reserved2[2] = MassPara.Sense.Reserved2[3] = 0;
		MassPara.Sense.AddSenseCode = mASC;
		MassPara.Sense.AddSenseCodeQua = 00;
		MassPara.Sense.Reserved3[0] = MassPara.Sense.Reserved3[1] = 0;
		MassPara.Sense.Reserved3[2] = MassPara.Sense.Reserved3[3] = 0;
		pBuf = MassPara.buf;
		bCswStatus = 0;
	}else{
		lastFSTALL = FSTALL;
		FSTALL = 0;
		MassPara.Sense.ErrorCode = 0x70;
		MassPara.Sense.Reserved1 = 0;
		MassPara.Sense.SenseKey = 0x00;
		MassPara.Sense.Information[0] = MassPara.Sense.Information[1] = 0;
		MassPara.Sense.Information[2] = MassPara.Sense.Information[3] = 0;
		MassPara.Sense.AddSenseLength = 0x0a;
		MassPara.Sense.Reserved2[0] = MassPara.Sense.Reserved2[1] = 0;
		MassPara.Sense.Reserved2[2] = MassPara.Sense.Reserved2[3] = 0;
		MassPara.Sense.AddSenseCode = 0x00;
		MassPara.Sense.AddSenseCodeQua = 00;
		MassPara.Sense.Reserved3[0] = MassPara.Sense.Reserved3[1] = 0;
		MassPara.Sense.Reserved3[2] = MassPara.Sense.Reserved3[3] = 0;
		pBuf = MassPara.buf;
		bCswStatus = 0;
	}
}
void UFI_perOrMed(void){//�����Ƴ�����(��δ���ֵ���)
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_staStoUnit(void){//����װ��ж���豸(��δ���ֵ���)
	UdiskFlagDown = UdiskFlagUp = 0;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_verify(void){//У��洢���ռ�(��δ���ֵ���)
	bCswStatus = mSenseKey = mASC = 0;
	//��������ֻ����Ϊ��ʾ����û�������������洢����ʵ������һ�����Ҫ����
}
void UFI_testUnit(void){//����U���Ƿ�׼����
	UdiskFlagDown = UdiskFlagUp = 0;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_read10(void){//��ȡ����
	LEN.mDataLength = (((UINT32)MassPara.Cbw.Cbwcb[7]<<8) | (UINT32)MassPara.Cbw.Cbwcb[8])*DISK_SEC_LEN;
	Locate_Addr = ((UINT32)MassPara.Cbw.Cbwcb[2]<<24) | ((UINT32)MassPara.Cbw.Cbwcb[3]<<16) 
				| ((UINT32)MassPara.Cbw.Cbwcb[4]<<8) | (UINT32)MassPara.Cbw.Cbwcb[5];

	Locate_Addr = Locate_Addr * DISK_SEC_LEN;

	pBufReSelect = 1;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_write(void){
	uint16_t i, num;
	LEN.mDataLength = (((UINT32)MassPara.Cbw.Cbwcb[7]<<8) | (UINT32)MassPara.Cbw.Cbwcb[8])*DISK_SEC_LEN;	//д���ݳ���
	Locate_Addr = ((UINT32)MassPara.Cbw.Cbwcb[2]<<24) | ((UINT32)MassPara.Cbw.Cbwcb[3]<<16) 
				| ((UINT32)MassPara.Cbw.Cbwcb[4]<<8) | (UINT32)MassPara.Cbw.Cbwcb[5];
	Locate_Addr = Locate_Addr * DISK_SEC_LEN;
	
	num = (MassPara.Cbw.Cbwcb[7] << 8) | MassPara.Cbw.Cbwcb[8];   //�����д�������Խ��в���
	
	for(i = 0; i < num; i++) UdiskEraseFLASH(Locate_Addr + i*DISK_SEC_LEN);	//����FLASH

	bCswStatus = mSenseKey = mASC = 0;
}

void UdiskBOT(void){	//�����
	if(MassPara.Cbw.dCBWSignature != CBW_SIGN){	//����־
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES;	//��������ֹͣ
		return;
	}
	
	dCBWTag = MassPara.Cbw.dCBWTag;//��¼��ǩ
	
	LEN.mdataLen[3] = MassPara.Cbw.dCBWDatLen[0];//ȡ�����ݳ���
	LEN.mdataLen[2] = MassPara.Cbw.dCBWDatLen[1];
	LEN.mdataLen[1] = MassPara.Cbw.dCBWDatLen[2];
	LEN.mdataLen[0] = MassPara.Cbw.dCBWDatLen[3];//��USB��С������תΪ51�Ĵ������
	
	if(LEN.mDataLength){//�����ݳ��Ȳ�Ϊ0
		UdiskFlagDown = (MassPara.Cbw.bmCBWFlags & 0x80) ? 0:1;	//�ж��Ƿ�Ϊ�´�
		UdiskFlagUp = (MassPara.Cbw.bmCBWFlags & 0x80) ? 1:0;	//�ж��Ƿ�Ϊ�ϴ�
	}
	//if(!CBWLUN){	//ֻ֧��һ��������
	//}else;		//�˴�Ӧ��������
	UdiskFlagCSW = 1;
	
	switch(MassPara.Cbw.Cbwcb[0]){//UFIЭ�� �ж�����������
		case VERIFY:	UFI_verify();	break;
		case INQUIRY:	UFI_inquiry();	break;
		case READ:		UFI_read10();	break;
		case WRITE:		UFI_write();	break;
		case TES_UNIT:	UFI_testUnit();	break;
		case MODE_SENSE:	UFI_modeSense();	break;
		case MODE_SENSE5:	UFI_modeSense5();	break;
		case REQUEST_SENSE:	UFI_requestSense();	break;
		case READ_CAPACITY:	UFI_readCapacity();	break;
		case STA_STO_UNIT:	UFI_staStoUnit();	break;
		case PRE_OR_MED:	UFI_perOrMed();		break;
		case READ_F_CAPACITY:	UFI_readFormatCapacity();	break;
//		case MODE_SELECT:	UFI_modeSlect();	break;//��Щ�������ƺ�Ҳ���ԣ���ȷ��
//		case WRITE_BUFFER:	UFI_writeBuf();		break;
//		case FORMAT_UNIT:	UFI_format();		break;
//		case PREVENT:	break;
//		case RELEASE:	break;
//		case 0x23:	break;
		default:
			mSenseKey = 5;
			mASC = 0x20;
			bCswStatus = 1;
			UdiskFlagUp = 0;
			BulkThirteen(CASECBW);
		break;
	}
}

void UdiskDownData(void){	//�����´�
	uint8_t len, i;
//	WriteLedOn;
	
	len = USB_RX_LEN;	//��ȡ���յ����ݵĳ���
	
	for(i = 0; i != len; i++){	//���÷��ͻ�������������
		Ep1BulkBuf[MAX_PACKET_SIZE + i] = Ep1BulkBuf[i];
	}
	
	UdiskWriteFLASH(Locate_Addr, len, &Ep1BulkBuf[MAX_PACKET_SIZE]);	//дFLASH
	Locate_Addr += len;

	LEN.mDataLength -= len;	//ȫ�����ݳ��ȼ�����ǰ��õĳ���

	if(LEN.mDataLength == 0){	//�������Ϊ0˵���´����,����CSW
		UdiskFlagDown = 0;
		UdiskUpCSW();	//�ϴ�CSW
	}
//	WriteLedOff;
}

void UdiskUpData(void){	//�����ϴ�
	uint8_t len, i;
//	ReadLedOn;
	
	if(LEN.mDataLength > 0x40){	//������64
		len = 0x40;						//���Ƶ�64
		LEN.mDataLength -= 0x40;		//����ʣ�೤��
	}
	else{						//��������64
		len = (uint8_t)LEN.mDataLength;	//ֱ��ʹ�øó���
		LEN.mDataLength = 0;			//ʣ�೤�����
		UdiskFlagUp = 0;				//��־��������ϴ�
	}
	
	if(pBufReSelect){	//��Ϊ��FLASH����
		UdiskReadFLASH(Locate_Addr, len, &Ep1BulkBuf[MAX_PACKET_SIZE]);	//��FLASH

		Locate_Addr += len;
		if(LEN.mDataLength == 0) pBufReSelect = 0;	//������������ѡ��
	}
	else{				//����Ϊ��������
		for(i = 0; i < len; i++){
			Ep1BulkBuf[MAX_PACKET_SIZE + i] = *pBuf;//�ӻ���ָ�������
			pBuf++;
		}
	}
	
//	UEP1_T_LEN = len;
//	UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;	// �����ϴ�
	EP1_Send_Data(Ep1BulkBuf + MAX_PACKET_SIZE, len);//�˵�1����
//	ReadLedOff;
}

void UdiskUpCSW(void){ //CSW�ظ��ϴ�
	UdiskFlagCSW = 0;				//��־���CSW�ϴ�
	UdiskFlagUp = 0;				//ȡ�������ϴ�
	
	MassPara.Csw.dCSWSignature = CSW_SIGN;//��֤��־
	
	MassPara.Csw.dCSWTag = dCBWTag;//��ǩ����
	
	MassPara.Csw.dCSWDataRes[0] = LEN.mdataLen[3];//ʣ�����ݳ���
	MassPara.Csw.dCSWDataRes[1] = LEN.mdataLen[2];
	MassPara.Csw.dCSWDataRes[2] = LEN.mdataLen[1];
	MassPara.Csw.dCSWDataRes[3] = LEN.mdataLen[0];//��51�Ĵ������תΪUSB��С������

	MassPara.Csw.bCSWStatus = bCswStatus;//�ɹ���־
	
//	memcpy(Ep1BulkBuf + MAX_PACKET_SIZE, MassPara.buf, 13);	//�������ݵ����ͻ���
//	UEP1_T_LEN = 13;
//	UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;	//�����ϴ�
	memcpy(Ep1BulkBuf + MAX_PACKET_SIZE, MassPara.buf, 13);	//�������ݵ����ͻ���
	EP1_Send_Data(Ep1BulkBuf + MAX_PACKET_SIZE, 13);//�˵�1����
}




















//END






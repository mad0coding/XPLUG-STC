
#include "UfiCmd.h"

#pragma  NOAREGS

//UINT8X FlashBuf[1024] _at_ (1024);//配置缓存数组
//uint16_t FlashBufStart = 0, FlashBufEnd = 0;

//uint8_t dbgLen = 0;
//uint32_t dbgBuf[32];
//uint16_t statePkg = 0;

bit UdiskFlagUp = 0;			//数据上传标志
bit UdiskFlagDown = 0;			//数据下传标志
bit UdiskFlagCSW = 0;			//CSW上传标志
bit FSTALL, lastFSTALL;		//数据错误标志
bit pBufReSelect = 0;		//指针重选标志

uint8_t	bCswStatus;			//CSW状态标志
uint8_t mSenseKey;
uint8_t mASC;

uint32_t dCBWTag = 0;	//暂存CBW标签以供CSW使用
UINT32D Locate_Addr;

uint8_t *pBuf;	//数据读取指针

#define MAX_PACKET_SIZE	64

uint8_t MASK_UEP_T_RES,UEP_T_RES_ACK,MASK_UEP_R_RES;
//uint8_t UEP1_T_LEN = 13;
uint8_t UEP1_CTRL;	//允许上传

uint8_t Ep1BulkBuf[64+64];
uint8_t USB_RX_LEN = 0;//接收数据长度

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
	uint32_t mDataLength;	//数据长度
	uint8_t mdataLen[4];	//数据长度的4字节,用来大小端转换
}LEN;


MASS_PARA MassPara;


//INQUIRY Info (36B)
UINT8C DBINQUITY[] = {
	0x00,	// 设备类型
	0x80,	// bit7=1代表可移除介质
	0x02,	// ISO Version(2bit) ECMA Version(3bit) ANSI Version(3bit)
	0x02,	// UFI规范要求为1，SCSI规范要求大于等于2
	0x1f,	// 附加长度31似乎表示后面还有多少字节
	0,0,0,	// 5~7 Reserved
	'w','c','h','.','c','n',' ',' ',	// 8~15 Vendor Information(左对齐，下同)
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
	8*1,//后面的长度
	//Current/Maximum Capacity Descriptor
	(DISK_SEC_NUM>>24)&0xFF, (DISK_SEC_NUM>>16)&0xFF, (DISK_SEC_NUM>>8)&0xFF, DISK_SEC_NUM&0xFF, 
	0x02,//Descriptor Code(0x02 = Formatted Media - Current media capacity)
	/*(DISK_SEC_LEN>>24)&0xFF, */(DISK_SEC_LEN>>16)&0xFF, (DISK_SEC_LEN>>8)&0xFF, DISK_SEC_LEN&0xFF,
};
UINT8C modesense3F[] = {
	0x0b, 0x00, 0x00, 0x08, (DISK_SEC_NUM>>24)&0xFF, (DISK_SEC_NUM>>16)&0xFF, (DISK_SEC_NUM>>8)&0xFF, DISK_SEC_NUM&0xFF, 00, 00, 02, 00,
};	//物理扇区数
UINT8C mode5sense3F[] = {
	0x00, 0x06, 0x00, 0x00, 0x08, 0x00, 0x00, 0x08,
	(DISK_SEC_NUM>>24)&0xFF, (DISK_SEC_NUM>>16)&0xFF, (DISK_SEC_NUM>>8)&0xFF, DISK_SEC_NUM&0xFF, 00, 00, 02, 00,
};	//物理扇区数


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
		FSTALL = 1;										//这里上传端点设置一个STALL，待主机清掉 // may or may-not
		bCswStatus = 2;
		break;
	case CASE4:     									/* Hi>Dn*/
	case CASE5:     									/* Hi>Di*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;										//这里上传端点设置一个STALL，待主机清掉
		bCswStatus = 1;									//CSW_GOOD or CSW_FAIL
		break;
	case CASE7:    										 /* Hi<Di*/
	case CASE8:    										 /* Hi<>Do */
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;										//这里上传端点设置一个STALL，待主机清掉
		bCswStatus = 2;
		break;
	case CASE9:    										 /* Ho>Dn*/
	case CASE11:    									 /* Ho>Do*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_R_RES ;
		FSTALL = 1;										//这里上传端点设置一个STALL，待主机清掉
		bCswStatus = 1;									//CSW_GOOD or CSW_FAIL
		break;
	case CASE10:    								/* Ho<>Di */
	case CASE13:    								/* Ho<Do*/
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;
													//这里上传端点设置一个STALL，待主机清掉
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_R_RES ;
													//这里上传端点设置一个STALL，待主机清掉
		bCswStatus = 2;
		break;
	case CASECBW:   								/* invalid CBW */
	    UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL = 1;
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_R_RES ;
											//这里端点设置一个STALL，待主机清掉
		bCswStatus = 2;
		break;
	case CASECMDFAIL:
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES ;
		FSTALL=1;
												//这里上传端点设置一个STALL，待主机清掉
		bCswStatus= 1;
		break;
	default:
		break;
	}
}

void UFI_inquiry(void){//查询U盘信息
	if(LEN.mDataLength > sizeof(DBINQUITY)) LEN.mDataLength = sizeof(DBINQUITY);
	pBuf = DBINQUITY;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_readCapacity(void){//读取容量
	if(LEN.mDataLength > sizeof(DBCAPACITY)) LEN.mDataLength = sizeof(DBCAPACITY);
	pBuf = (uint8_t*)DBCAPACITY;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_readFormatCapacity(void){//读取格式化容量
	if(LEN.mDataLength > sizeof(DBFMTCAPACITY)) LEN.mDataLength = sizeof(DBFMTCAPACITY);
	pBuf = (uint8_t*)DBFMTCAPACITY;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_modeSense(void){//模式认识
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
void UFI_modeSense5(void){//模式认识5(暂未发现调用)
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
void UFI_requestSense(void){//请求认识(暂未发现调用)
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
void UFI_perOrMed(void){//允许移出磁盘(暂未发现调用)
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_staStoUnit(void){//请求装载卸载设备(暂未发现调用)
	UdiskFlagDown = UdiskFlagUp = 0;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_verify(void){//校验存储器空间(暂未发现调用)
	bCswStatus = mSenseKey = mASC = 0;
	//这里这里只是作为演示所以没有真正检测物理存储器但实际上这一步最好要处理
}
void UFI_testUnit(void){//测试U盘是否准备好
	UdiskFlagDown = UdiskFlagUp = 0;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_read10(void){//读取数据
	LEN.mDataLength = (((UINT32)MassPara.Cbw.Cbwcb[7]<<8) | (UINT32)MassPara.Cbw.Cbwcb[8])*DISK_SEC_LEN;
	Locate_Addr = ((UINT32)MassPara.Cbw.Cbwcb[2]<<24) | ((UINT32)MassPara.Cbw.Cbwcb[3]<<16) 
				| ((UINT32)MassPara.Cbw.Cbwcb[4]<<8) | (UINT32)MassPara.Cbw.Cbwcb[5];

	Locate_Addr = Locate_Addr * DISK_SEC_LEN;

	pBufReSelect = 1;
	bCswStatus = mSenseKey = mASC = 0;
}
void UFI_write(void){
	uint16_t i, num;
	LEN.mDataLength = (((UINT32)MassPara.Cbw.Cbwcb[7]<<8) | (UINT32)MassPara.Cbw.Cbwcb[8])*DISK_SEC_LEN;	//写数据长度
	Locate_Addr = ((UINT32)MassPara.Cbw.Cbwcb[2]<<24) | ((UINT32)MassPara.Cbw.Cbwcb[3]<<16) 
				| ((UINT32)MassPara.Cbw.Cbwcb[4]<<8) | (UINT32)MassPara.Cbw.Cbwcb[5];
	Locate_Addr = Locate_Addr * DISK_SEC_LEN;
	
	num = (MassPara.Cbw.Cbwcb[7] << 8) | MassPara.Cbw.Cbwcb[8];   //计算待写扇区数以进行擦除
	
	for(i = 0; i < num; i++) UdiskEraseFLASH(Locate_Addr + i*DISK_SEC_LEN);	//擦除FLASH

	bCswStatus = mSenseKey = mASC = 0;
}

void UdiskBOT(void){	//命令处理
	if(MassPara.Cbw.dCBWSignature != CBW_SIGN){	//检查标志
		UEP1_CTRL = UEP1_CTRL | MASK_UEP_T_RES;	//若不对则停止
		return;
	}
	
	dCBWTag = MassPara.Cbw.dCBWTag;//记录标签
	
	LEN.mdataLen[3] = MassPara.Cbw.dCBWDatLen[0];//取出数据长度
	LEN.mdataLen[2] = MassPara.Cbw.dCBWDatLen[1];
	LEN.mdataLen[1] = MassPara.Cbw.dCBWDatLen[2];
	LEN.mdataLen[0] = MassPara.Cbw.dCBWDatLen[3];//将USB的小端数据转为51的大端数据
	
	if(LEN.mDataLength){//若数据长度不为0
		UdiskFlagDown = (MassPara.Cbw.bmCBWFlags & 0x80) ? 0:1;	//判断是否为下传
		UdiskFlagUp = (MassPara.Cbw.bmCBWFlags & 0x80) ? 1:0;	//判断是否为上传
	}
	//if(!CBWLUN){	//只支持一个物理盘
	//}else;		//此处应做错误处理
	UdiskFlagCSW = 1;
	
	switch(MassPara.Cbw.Cbwcb[0]){//UFI协议 判断是哪种命令
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
//		case MODE_SELECT:	UFI_modeSlect();	break;//这些不处理似乎也可以，不确定
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

void UdiskDownData(void){	//数据下传
	uint8_t len, i;
//	WriteLedOn;
	
	len = USB_RX_LEN;	//获取接收到数据的长度
	
	for(i = 0; i != len; i++){	//借用发送缓冲区读入数据
		Ep1BulkBuf[MAX_PACKET_SIZE + i] = Ep1BulkBuf[i];
	}
	
	UdiskWriteFLASH(Locate_Addr, len, &Ep1BulkBuf[MAX_PACKET_SIZE]);	//写FLASH
	Locate_Addr += len;

	LEN.mDataLength -= len;	//全局数据长度减掉当前获得的长度

	if(LEN.mDataLength == 0){	//如果数据为0说明下传完毕,则传送CSW
		UdiskFlagDown = 0;
		UdiskUpCSW();	//上传CSW
	}
//	WriteLedOff;
}

void UdiskUpData(void){	//数据上传
	uint8_t len, i;
//	ReadLedOn;
	
	if(LEN.mDataLength > 0x40){	//若大于64
		len = 0x40;						//限制到64
		LEN.mDataLength -= 0x40;		//计算剩余长度
	}
	else{						//若不大于64
		len = (uint8_t)LEN.mDataLength;	//直接使用该长度
		LEN.mDataLength = 0;			//剩余长度清空
		UdiskFlagUp = 0;				//标志完成数据上传
	}
	
	if(pBufReSelect){	//若为读FLASH数据
		UdiskReadFLASH(Locate_Addr, len, &Ep1BulkBuf[MAX_PACKET_SIZE]);	//读FLASH

		Locate_Addr += len;
		if(LEN.mDataLength == 0) pBufReSelect = 0;	//若已完成则清空选择
	}
	else{				//否则为其他数据
		for(i = 0; i < len; i++){
			Ep1BulkBuf[MAX_PACKET_SIZE + i] = *pBuf;//从缓存指针读数据
			pBuf++;
		}
	}
	
//	UEP1_T_LEN = len;
//	UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;	// 允许上传
	EP1_Send_Data(Ep1BulkBuf + MAX_PACKET_SIZE, len);//端点1发送
//	ReadLedOff;
}

void UdiskUpCSW(void){ //CSW回复上传
	UdiskFlagCSW = 0;				//标志完成CSW上传
	UdiskFlagUp = 0;				//取消数据上传
	
	MassPara.Csw.dCSWSignature = CSW_SIGN;//验证标志
	
	MassPara.Csw.dCSWTag = dCBWTag;//标签环回
	
	MassPara.Csw.dCSWDataRes[0] = LEN.mdataLen[3];//剩余数据长度
	MassPara.Csw.dCSWDataRes[1] = LEN.mdataLen[2];
	MassPara.Csw.dCSWDataRes[2] = LEN.mdataLen[1];
	MassPara.Csw.dCSWDataRes[3] = LEN.mdataLen[0];//将51的大端数据转为USB的小端数据

	MassPara.Csw.bCSWStatus = bCswStatus;//成功标志
	
//	memcpy(Ep1BulkBuf + MAX_PACKET_SIZE, MassPara.buf, 13);	//复制数据到发送缓存
//	UEP1_T_LEN = 13;
//	UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;	//允许上传
	memcpy(Ep1BulkBuf + MAX_PACKET_SIZE, MassPara.buf, 13);	//复制数据到发送缓存
	EP1_Send_Data(Ep1BulkBuf + MAX_PACKET_SIZE, 13);//端点1发送
}




















//END






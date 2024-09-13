/*******************************************************************************
* File Name          : UfiCmd.H
* Author             : L&E
* Date               : 2024/2/28
* Description        : U盘BOT和UFI(SCSI)协议处理，数据收发
********************************************************************************/
#ifndef _UFICMD_H
#define _UFICMD_H

#include "DEBUG.H"
#include "W25Qxx.H"
//#include "UdiskUSB.h"
#include "usb_req_class.h"


#define FORMAT_UNIT 	0x04
#define INQUIRY 		0x12
#define MODE_SELECT 	0x15
#define MODE_SENSE5 	0x5A
#define MODE_SENSE 		0x1A
#define PER_RES_IN 		0x5E
#define PER_RES_OUT 	0x5F
#define PRE_OR_MED 		0x1E
#define READ 			0x28
#define READ_F_CAPACITY	0x23
#define READ_CAPACITY 	0x25
#define RELEASE 		0x17
#define REQUEST_SENSE 	0x03
#define RESERVE 		0x16
#define STA_STO_UNIT 	0x1B
#define SYN_CACHE 		0x35
#define TES_UNIT 		0x00
#define VERIFY 			0x2F
#define WRITE 			0x2A
#define WRITE_BUFFER 	0x3B

//修改宏定义的时候注意修改擦除函数，当前为擦除4k函数。需要相应修改这个函数即可。
//#define DISK_SEC_NUM   0x00000200			//总扇区数  共512物理扇区
#define DISK_SEC_NUM   0x00001000			//总扇区数  共4096物理扇区(4096*4096B=16MB)
#define DISK_SEC_LAST  (DISK_SEC_NUM - 1)	//最后一个逻辑扇区地址
#define DISK_SEC_LEN   0x00001000			//扇区大小 每个扇区4096字节大小

#define CBW_SIGN 0x55534243		//下行包头标志
#define CSW_SIGN 0x55534253		//上行包头标志

#define UdiskReadFLASH(addr,len,buf)		SPI_FLASH_BufferRead(buf,addr,len)		//读FLASH
#define UdiskWriteFLASH(addr,len,buf)		SPI_FLASH_BufferWrite(buf,addr,len)		//写FLASH
#define UdiskEraseFLASH(addr)				SPI_FLASH_SectorErase(addr)				//擦除FLASH

//#define ReadLedOn	(PWMG = 255)	//读指示灯亮
//#define ReadLedOff	(PWMG = 0)		//读指示灯灭
//#define WriteLedOn	(PWMB = 255)	//写指示灯亮
//#define WriteLedOff	(PWMB = 0)		//写指示灯灭

typedef union{
	uint8_t buf[64];
	struct SENSE{
		uint8_t ErrorCode;
		uint8_t Reserved1;
		uint8_t SenseKey;
		uint8_t Information[4];
		uint8_t AddSenseLength;
		uint8_t Reserved2[4];
		uint8_t AddSenseCode;
		uint8_t AddSenseCodeQua;
		uint8_t Reserved3[4];
	}Sense;
	struct CBW{
		uint32_t dCBWSignature;
		uint32_t dCBWTag;
		uint8_t dCBWDatLen[4];
		uint8_t bmCBWFlags;
		uint8_t bCBWLUN;
		uint8_t bCBWCBLength;
//		CBWCB Cbwcb;
		uint8_t Cbwcb[16];
	}Cbw;
	struct CSW{
		uint32_t dCSWSignature; //CSW的标识，固定值：53425355h (小端模式)
		uint32_t dCSWTag; //主机发送的一个命令块标识，设备需要原样复制dCBWTag（CBW中的一部分）再发送给Host;主要用于关联CSW到对应的CBW
		uint8_t dCSWDataRes[4]; //还需要传送的数据，此数据根据（dCBWDataTransferLength－本次已经传送的数据）得到 
		uint8_t bCSWStatus; //指示命令的执行状态。00h、01h、02h分别代表成功、命令失败、状态错误
	}Csw;
}MASS_PARA;


extern bit UdiskFlagUp;				//数据上传标志
extern bit UdiskFlagDown;			//数据下传标志
extern bit UdiskFlagCSW;			//CSW上传标志
extern bit FSTALL, lastFSTALL;		//数据错误标志

extern MASS_PARA MassPara;
extern uint8_t Ep1BulkBuf[];
extern uint8_t USB_RX_LEN;//接收数据长度

//extern UINT8X FlashBuf[1024];//配置缓存数组
//extern uint16_t FlashBufStart, FlashBufEnd;



void UdiskBOT(void);
void UdiskDownData(void);
void UdiskUpData(void);
void UdiskUpCSW(void);





#endif










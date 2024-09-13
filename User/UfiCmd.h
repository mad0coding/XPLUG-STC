/*******************************************************************************
* File Name          : UfiCmd.H
* Author             : L&E
* Date               : 2024/2/28
* Description        : U��BOT��UFI(SCSI)Э�鴦�������շ�
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

//�޸ĺ궨���ʱ��ע���޸Ĳ�����������ǰΪ����4k��������Ҫ��Ӧ�޸�����������ɡ�
//#define DISK_SEC_NUM   0x00000200			//��������  ��512��������
#define DISK_SEC_NUM   0x00001000			//��������  ��4096��������(4096*4096B=16MB)
#define DISK_SEC_LAST  (DISK_SEC_NUM - 1)	//���һ���߼�������ַ
#define DISK_SEC_LEN   0x00001000			//������С ÿ������4096�ֽڴ�С

#define CBW_SIGN 0x55534243		//���а�ͷ��־
#define CSW_SIGN 0x55534253		//���а�ͷ��־

#define UdiskReadFLASH(addr,len,buf)		SPI_FLASH_BufferRead(buf,addr,len)		//��FLASH
#define UdiskWriteFLASH(addr,len,buf)		SPI_FLASH_BufferWrite(buf,addr,len)		//дFLASH
#define UdiskEraseFLASH(addr)				SPI_FLASH_SectorErase(addr)				//����FLASH

//#define ReadLedOn	(PWMG = 255)	//��ָʾ����
//#define ReadLedOff	(PWMG = 0)		//��ָʾ����
//#define WriteLedOn	(PWMB = 255)	//дָʾ����
//#define WriteLedOff	(PWMB = 0)		//дָʾ����

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
		uint32_t dCSWSignature; //CSW�ı�ʶ���̶�ֵ��53425355h (С��ģʽ)
		uint32_t dCSWTag; //�������͵�һ��������ʶ���豸��Ҫԭ������dCBWTag��CBW�е�һ���֣��ٷ��͸�Host;��Ҫ���ڹ���CSW����Ӧ��CBW
		uint8_t dCSWDataRes[4]; //����Ҫ���͵����ݣ������ݸ��ݣ�dCBWDataTransferLength�������Ѿ����͵����ݣ��õ� 
		uint8_t bCSWStatus; //ָʾ�����ִ��״̬��00h��01h��02h�ֱ����ɹ�������ʧ�ܡ�״̬����
	}Csw;
}MASS_PARA;


extern bit UdiskFlagUp;				//�����ϴ���־
extern bit UdiskFlagDown;			//�����´���־
extern bit UdiskFlagCSW;			//CSW�ϴ���־
extern bit FSTALL, lastFSTALL;		//���ݴ����־

extern MASS_PARA MassPara;
extern uint8_t Ep1BulkBuf[];
extern uint8_t USB_RX_LEN;//�������ݳ���

//extern UINT8X FlashBuf[1024];//���û�������
//extern uint16_t FlashBufStart, FlashBufEnd;



void UdiskBOT(void);
void UdiskDownData(void);
void UdiskUpData(void);
void UdiskUpCSW(void);





#endif










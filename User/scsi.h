#ifndef __SCSI_H__
#define __SCSI_H__

#include "DEBUG.H"
#include "config.h"
//#include "stc.h"
#include "usb.h"
//#include "usb_desc.h"
#include "usb_req_vendor.h"
#include "W25Qxx.H"
#include "RgbDrv.h"

#define DISK_SEC_NUM   0x00001000			//总扇区数  共4096物理扇区(4096*4096B=16MB)
#define DISK_SEC_LAST  (DISK_SEC_NUM - 1)	//最后一个逻辑扇区地址
#define DISK_SEC_LEN   0x00001000			//扇区大小 每个扇区4096字节大小

#define dwSectorNumber		DISK_SEC_NUM
#define LBA					DISK_SEC_LAST
#define wSectorSize			DISK_SEC_LEN

#define DISK_LED_ON		LED1_ON
#define DISK_LED_OFF	LED1_OFF

//#define memory_init						flash_init
#define memory_check()					((SPI_FLASH_ReadID() & 0xFF0000) == 0xEF0000)//读取ID来检查芯片存在
#define memory_readsector(lba, buf)		{if(CFG_LED_DISK) DISK_LED_ON; SPI_FLASH_BufferRead(buf, lba, wSectorSize); \
											if(CFG_LED_DISK) DISK_LED_OFF;}
#define memory_writesector(lba, buf)	{if(CFG_LED_DISK) DISK_LED_ON; SPI_FLASH_SectorErase(lba); \
											SPI_FLASH_BufferWrite(buf, lba, wSectorSize); if(CFG_LED_DISK) DISK_LED_OFF;}


#ifndef NULL
#define NULL                    ((void *)0)
#endif

#define CBW_SIGNATURE           0x55534243
#define CSW_SIGNATURE           0x55534253

#define SCSI_PASSED 		    0
#define SCSI_FAILED 		    1
#define SCSI_PHASE_ERROR 	    2

#define SCSI_TESTUNITREADY      0x00
#define SCCI_STARTSTOPUNIT      0x1b
#define SCSI_REQUESTSENSE       0x03
#define SCSI_INQUIRY            0x12
#define SCSI_MODESENSE6         0x1a
#define SCSI_MODESENSE10        0x5a
#define SCSI_MEDIAREMOVAL       0x1e
#define SCSI_FORMATCAPACITY     0x23
#define SCSI_READCAPACITY10     0x25
#define SCSI_READ10             0x28
#define SCSI_WRITE10            0x2a
#define SCSI_VERIFY10           0x2f

#define SENSE_NOSENSE           0x00
#define SENSE_RECOVEREDERROR    0x01
#define SENSE_NOTREADY          0x02
#define SENSE_MEDIUMERROR       0x03
#define SENSE_HARDWAREERROR     0x04
#define SENSE_ILLEGALREQUEST    0x05
#define SENSE_UNITATTENTION     0x06
#define SENSE_DATAPROTECT       0x07
#define SENSE_BLANKCHECK        0x08
#define SENSE_VENDORSPECIFIC    0x09
#define SENSE_COPYABORTED       0x0a
#define SENSE_ABORTE_COMMAND    0x0b
#define SENSE_VOLUMEOVERFLOW    0x0d
#define SENSE_MISCOMPARE        0x0e

#define ASC_INVALIDCOMMAND      0x20
#define ASC_MEDIUMNOTPRESENT    0x3a

typedef struct
{ 
    uint32_t   dCBWSignature; 
    uint32_t   dCBWTag; 
    uint32_t   dCBWDataLength;
    uint8_t    bmCBWFlags; 
    uint8_t    bCBWLUN; 
    uint8_t    bCBWCBLength; 
    uint8_t    bScsiOpcode;
    uint8_t    ScsiParam[15]; 
}CBW; 
 
typedef struct
{
	uint32_t   dCSWSignature; 
	uint32_t   dCSWTag; 
	uint32_t   dCSWDataResidue; 
	uint8_t    bCSWStatus; 
}CSW; 

void scsi_process();

void scsi_in(uint8_t *pData, int nSize);
void scsi_out(uint8_t *pData, int nSize);

uint8_t scsi_test_unit_ready();
void scsi_start_stop_unit();
void scsi_media_removal();
void scsi_inquiry();
void scsi_read_capacity10();
void scsi_format_capacity();
void scsi_request_sense();
void scsi_mode_sense6();
void scsi_mode_sense10();
void scsi_read10();
void scsi_write10();
void scsi_verify10();

#endif

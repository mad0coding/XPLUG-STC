#ifndef __W25QXX_H
#define __W25QXX_H

#include "DEBUG.H"
#include "STC8G_H_SPI.h"
#include <stdio.h>

//#define sFLASH_ID			0xEF3015	//W25X16
//#define sFLASH_ID			0xEF4015	//W25Q16
//#define sFLASH_ID			0XEF4017	//W25Q64
#define sFLASH_ID			0XEF4018	//W25Q128

#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag				0x01
#define Dummy_Byte				0xFF

/*命令定义-开头*******************************/
#define W25X_WriteEnable			0x06
#define W25X_WriteDisable			0x04
#define W25X_ReadStatusReg			0x05
#define W25X_WriteStatusReg			0x01
#define W25X_ReadData				0x03
#define W25X_FastReadData			0x0B
#define W25X_FastReadDual			0x3B
#define W25X_PageProgram			0x02
#define W25X_BlockErase				0xD8
#define W25X_SectorErase			0x20
#define W25X_ChipErase				0xC7
#define W25X_PowerDown				0xB9
#define W25X_ReleasePowerDown		0xAB
#define W25X_DeviceID				0xAB
#define W25X_ManufactDeviceID		0x90
#define W25X_JedecDeviceID			0x9F
/*命令定义-结尾*******************************/


/*SPI接口定义-开头****************************/
#define SPI_FLASH_CS_PIN			P54	//原理图中为CS
#define SPI_FLASH_CS_LOW()			(SPI_FLASH_CS_PIN = 0)
#define SPI_FLASH_CS_HIGH()			(SPI_FLASH_CS_PIN = 1)
/*SPI接口定义-结尾****************************/


/* 调用函数定义 */
#define SPI_FLASH_SendByte(Data)	SPI_WriteReadByte(Data)

uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

//void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);



#endif






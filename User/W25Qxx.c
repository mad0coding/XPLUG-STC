/**
  ******************************************************************************
  * @file    W25Qxx.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2024-2-23
  * @brief   spi flash 底层应用函数
  ******************************************************************************
  */
  
#include "W25Qxx.h"

//static uint32_t SPITimeout = SPIT_LONG_TIMEOUT;    
//static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/**
  * @brief  使用SPI发送两个字节的数据
  * @param  byte：要发送的数据
  * @retval 返回接收到的数据
  */
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord){
	uint8_t ans = SPI_FLASH_SendByte(HalfWord >> 8);			//先通信高字节
	return (ans << 8) | SPI_FLASH_SendByte(HalfWord & 0xFF);	//再通信低字节并组合
}

/**
  * @brief  唤醒
  * @param  none
  * @retval none
  */
void SPI_Flash_WAKEUP(void){
	SPI_FLASH_CS_LOW();							// 选择 FLASH: CS 低
	SPI_FLASH_SendByte(W25X_ReleasePowerDown);	// 发送 上电 命令
	SPI_FLASH_CS_HIGH();						// 停止信号 FLASH: CS 高
}

/**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void){
	uint8_t FLASH_Status = 0;
	SPI_FLASH_CS_LOW();									// 选择 FLASH: CS 低
	SPI_FLASH_SendByte(W25X_ReadStatusReg);				// 发送 读状态寄存器 命令
	do{													// 若FLASH忙碌，则等待
		FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	// 读取FLASH芯片的状态寄存器
	}
	while((FLASH_Status & WIP_Flag) == 1);				// 正在写入标志
	SPI_FLASH_CS_HIGH();								// 停止信号 FLASH: CS 高
}

/**
  * @brief  擦除FLASH扇区
  * @param  SectorAddr：要擦除的扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr){
	SPI_FLASH_WriteEnable();							// 发送FLASH写使能命令
	SPI_FLASH_WaitForWriteEnd();						// 等待FLASH内部数据写入完毕
	SPI_FLASH_CS_LOW();									// 选择FLASH: CS低电平
	SPI_FLASH_SendByte(W25X_SectorErase);				// 发送扇区擦除指令
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);	// 发送擦除扇区地址的高位
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);		// 发送擦除扇区地址的中位
	SPI_FLASH_SendByte(SectorAddr & 0xFF);				// 发送擦除扇区地址的低位
	SPI_FLASH_CS_HIGH();								// 停止信号 FLASH: CS 高电平
	SPI_FLASH_WaitForWriteEnd();						// 等待擦除完毕
}

/**
  * @brief  擦除FLASH扇区，整片擦除
  * @param  无
  * @retval 无
  */
void SPI_FLASH_BulkErase(void){
	SPI_FLASH_WriteEnable();			// 发送FLASH写使能命令
	SPI_FLASH_CS_LOW();					// 选择FLASH: CS低电平
	SPI_FLASH_SendByte(W25X_ChipErase);	// 发送整块擦除指令
	SPI_FLASH_CS_HIGH();				// 停止信号 FLASH: CS 高电平
	SPI_FLASH_WaitForWriteEnd();		// 等待擦除完毕
}

/**
  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * @retval 无
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	SPI_FLASH_WriteEnable();							// 发送FLASH写使能命令
	SPI_FLASH_CS_LOW();									// 选择FLASH: CS低电平
	SPI_FLASH_SendByte(W25X_PageProgram);				// 写页写指令
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);	// 发送写地址的高位
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);		// 发送写地址的中位
	SPI_FLASH_SendByte(WriteAddr & 0xFF);				// 发送写地址的低位

	if(NumByteToWrite > SPI_FLASH_PerWritePageSize){
		NumByteToWrite = SPI_FLASH_PerWritePageSize;
//		FLASH_ERROR("SPI_FLASH_PageWrite too large!"); 
	}
	
	while(NumByteToWrite--){	// 写入数据
		SPI_FLASH_SendByte(*pBuffer);	// 发送当前要写入的字节数据
		pBuffer++;						// 指向下一字节数据
	}
	SPI_FLASH_CS_HIGH();				// 停止信号 FLASH: CS 高电平
	SPI_FLASH_WaitForWriteEnd();		// 等待写入完毕
}

/**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PageSize;	//mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0
	count = SPI_FLASH_PageSize - Addr;		//差count个数据值，刚好可以对齐到页地址
	NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;	//计算出要写多少整数页
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;	//mod运算求余，计算出剩余不满一页的字节数

	if(Addr == 0){	// Addr=0,则WriteAddr 刚好按页对齐 aligned
		if(NumOfPage == 0){	// NumByteToWrite < SPI_FLASH_PageSize
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else{				// NumByteToWrite > SPI_FLASH_PageSize
			while(NumOfPage--){	//先把整数页都写了
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);	//若有多余的不满一页的数据，把它写完
		}
	}
	else{			// 若地址与 SPI_FLASH_PageSize 不对齐
		if(NumOfPage == 0){	// NumByteToWrite < SPI_FLASH_PageSize
			if(NumOfSingle > count){	//当前页剩余的count个位置比NumOfSingle小，一页写不完
				temp = NumOfSingle - count;
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);	//先写满当前页

				WriteAddr += count;
				pBuffer += count;
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);	//再写剩余的数据
			}
			else{						//当前页剩余的count个位置能写完NumOfSingle个数据
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else{				// NumByteToWrite > SPI_FLASH_PageSize
			NumByteToWrite -= count;	//地址不对齐多出的count分开处理，不加入这个运算
			NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);	// 先写完count个数据，为的是让下一次要写的地址对齐
			// 接下来就重复地址对齐的情况
			WriteAddr += count;
			pBuffer += count;
			while(NumOfPage--){	//把整数页都写了
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			if(NumOfSingle != 0){	//若有多余的不满一页的数据，把它写完
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

 /**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead){
	SPI_FLASH_CS_LOW();									// 选择FLASH: CS低电平
	SPI_FLASH_SendByte(W25X_ReadData);					// 发送 读 指令
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);	// 发送 读 地址高位
	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);		// 发送 读 地址中位
	SPI_FLASH_SendByte(ReadAddr & 0xFF);				// 发送 读 地址低位
	while(NumByteToRead--){	// 当有数据要读
		*pBuffer = SPI_FLASH_SendByte(Dummy_Byte);	// 读取一个字节
		pBuffer++;									// 指向下一个字节缓冲区
	}
	SPI_FLASH_CS_HIGH();	// 停止信号 FLASH: CS 高电平
}

/**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
uint32_t SPI_FLASH_ReadID(void){
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SPI_FLASH_CS_LOW();						// 开始通讯：CS低电平
	SPI_FLASH_SendByte(W25X_JedecDeviceID);	// 发送JEDEC指令，读取ID
	Temp0 = SPI_FLASH_SendByte(Dummy_Byte);	// 读取一个字节数据
	Temp1 = SPI_FLASH_SendByte(Dummy_Byte);	// 读取一个字节数据
	Temp2 = SPI_FLASH_SendByte(Dummy_Byte);	// 读取一个字节数据
	SPI_FLASH_CS_HIGH();					// 停止通讯：CS高电平
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;	//把数据组合起来，作为函数的返回值
	return Temp;
}
/**
  * @brief  读取FLASH Device ID
  * @param 	无
  * @retval FLASH Device ID
  */
uint32_t SPI_FLASH_ReadDeviceID(void){
	uint32_t Temp = 0;
	SPI_FLASH_CS_LOW();	// Select the FLASH: Chip Select low

	SPI_FLASH_SendByte(W25X_DeviceID);	// Send "RDID " instruction
	SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_SendByte(Dummy_Byte);

	Temp = SPI_FLASH_SendByte(Dummy_Byte);	// Read a byte from the FLASH
	SPI_FLASH_CS_HIGH();					// Deselect the FLASH: Chip Select high
	return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr){
	SPI_FLASH_CS_LOW();					// Select the FLASH: Chip Select low
	SPI_FLASH_SendByte(W25X_ReadData);	// Send "Read from Memory " instruction
	// Send the 24-bit address of the address to read from -----------------------
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);	// Send ReadAddr high nibble address byte
	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);		// Send ReadAddr medium nibble address byte
	SPI_FLASH_SendByte(ReadAddr & 0xFF);				// Send ReadAddr low nibble address byte
}

/**
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void){
	SPI_FLASH_CS_LOW();						// 通讯开始：CS低
	SPI_FLASH_SendByte(W25X_WriteEnable);	// 发送写使能命令
	SPI_FLASH_CS_HIGH();					// 通讯结束：CS高
}

/**
  * @brief  进入掉电模式
  * @param  none
  * @retval none
  */
void SPI_Flash_PowerDown(void){
	SPI_FLASH_CS_LOW();					// 通讯开始：CS低
	SPI_FLASH_SendByte(W25X_PowerDown);	// 发送 掉电 命令
	SPI_FLASH_CS_HIGH();				// 通讯结束：CS高
}




/******************************END OF FILE******************************/




/**
  ******************************************************************************
  * @file    W25Qxx.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2024-2-23
  * @brief   spi flash �ײ�Ӧ�ú���
  ******************************************************************************
  */
  
#include "W25Qxx.h"

//static uint32_t SPITimeout = SPIT_LONG_TIMEOUT;    
//static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/**
  * @brief  ʹ��SPI���������ֽڵ�����
  * @param  byte��Ҫ���͵�����
  * @retval ���ؽ��յ�������
  */
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord){
	uint8_t ans = SPI_FLASH_SendByte(HalfWord >> 8);			//��ͨ�Ÿ��ֽ�
	return (ans << 8) | SPI_FLASH_SendByte(HalfWord & 0xFF);	//��ͨ�ŵ��ֽڲ����
}

/**
  * @brief  ����
  * @param  none
  * @retval none
  */
void SPI_Flash_WAKEUP(void){
	SPI_FLASH_CS_LOW();							// ѡ�� FLASH: CS ��
	SPI_FLASH_SendByte(W25X_ReleasePowerDown);	// ���� �ϵ� ����
	SPI_FLASH_CS_HIGH();						// ֹͣ�ź� FLASH: CS ��
}

/**
  * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void){
	uint8_t FLASH_Status = 0;
	SPI_FLASH_CS_LOW();									// ѡ�� FLASH: CS ��
	SPI_FLASH_SendByte(W25X_ReadStatusReg);				// ���� ��״̬�Ĵ��� ����
	do{													// ��FLASHæµ����ȴ�
		FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	// ��ȡFLASHоƬ��״̬�Ĵ���
	}
	while((FLASH_Status & WIP_Flag) == 1);				// ����д���־
	SPI_FLASH_CS_HIGH();								// ֹͣ�ź� FLASH: CS ��
}

/**
  * @brief  ����FLASH����
  * @param  SectorAddr��Ҫ������������ַ
  * @retval ��
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr){
	SPI_FLASH_WriteEnable();							// ����FLASHдʹ������
	SPI_FLASH_WaitForWriteEnd();						// �ȴ�FLASH�ڲ�����д�����
	SPI_FLASH_CS_LOW();									// ѡ��FLASH: CS�͵�ƽ
	SPI_FLASH_SendByte(W25X_SectorErase);				// ������������ָ��
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);	// ���Ͳ���������ַ�ĸ�λ
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);		// ���Ͳ���������ַ����λ
	SPI_FLASH_SendByte(SectorAddr & 0xFF);				// ���Ͳ���������ַ�ĵ�λ
	SPI_FLASH_CS_HIGH();								// ֹͣ�ź� FLASH: CS �ߵ�ƽ
	SPI_FLASH_WaitForWriteEnd();						// �ȴ��������
}

/**
  * @brief  ����FLASH��������Ƭ����
  * @param  ��
  * @retval ��
  */
void SPI_FLASH_BulkErase(void){
	SPI_FLASH_WriteEnable();			// ����FLASHдʹ������
	SPI_FLASH_CS_LOW();					// ѡ��FLASH: CS�͵�ƽ
	SPI_FLASH_SendByte(W25X_ChipErase);	// �����������ָ��
	SPI_FLASH_CS_HIGH();				// ֹͣ�ź� FLASH: CS �ߵ�ƽ
	SPI_FLASH_WaitForWriteEnd();		// �ȴ��������
}

/**
  * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * @retval ��
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	SPI_FLASH_WriteEnable();							// ����FLASHдʹ������
	SPI_FLASH_CS_LOW();									// ѡ��FLASH: CS�͵�ƽ
	SPI_FLASH_SendByte(W25X_PageProgram);				// дҳдָ��
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);	// ����д��ַ�ĸ�λ
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);		// ����д��ַ����λ
	SPI_FLASH_SendByte(WriteAddr & 0xFF);				// ����д��ַ�ĵ�λ

	if(NumByteToWrite > SPI_FLASH_PerWritePageSize){
		NumByteToWrite = SPI_FLASH_PerWritePageSize;
//		FLASH_ERROR("SPI_FLASH_PageWrite too large!"); 
	}
	
	while(NumByteToWrite--){	// д������
		SPI_FLASH_SendByte(*pBuffer);	// ���͵�ǰҪд����ֽ�����
		pBuffer++;						// ָ����һ�ֽ�����
	}
	SPI_FLASH_CS_HIGH();				// ֹͣ�ź� FLASH: CS �ߵ�ƽ
	SPI_FLASH_WaitForWriteEnd();		// �ȴ�д�����
}

/**
  * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param  WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PageSize;	//mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0
	count = SPI_FLASH_PageSize - Addr;		//��count������ֵ���պÿ��Զ��뵽ҳ��ַ
	NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;	//�����Ҫд��������ҳ
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;	//mod�������࣬�����ʣ�಻��һҳ���ֽ���

	if(Addr == 0){	// Addr=0,��WriteAddr �պð�ҳ���� aligned
		if(NumOfPage == 0){	// NumByteToWrite < SPI_FLASH_PageSize
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else{				// NumByteToWrite > SPI_FLASH_PageSize
			while(NumOfPage--){	//�Ȱ�����ҳ��д��
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);	//���ж���Ĳ���һҳ�����ݣ�����д��
		}
	}
	else{			// ����ַ�� SPI_FLASH_PageSize ������
		if(NumOfPage == 0){	// NumByteToWrite < SPI_FLASH_PageSize
			if(NumOfSingle > count){	//��ǰҳʣ���count��λ�ñ�NumOfSingleС��һҳд����
				temp = NumOfSingle - count;
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);	//��д����ǰҳ

				WriteAddr += count;
				pBuffer += count;
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);	//��дʣ�������
			}
			else{						//��ǰҳʣ���count��λ����д��NumOfSingle������
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else{				// NumByteToWrite > SPI_FLASH_PageSize
			NumByteToWrite -= count;	//��ַ����������count�ֿ������������������
			NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);	// ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ����
			// ���������ظ���ַ��������
			WriteAddr += count;
			pBuffer += count;
			while(NumOfPage--){	//������ҳ��д��
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			if(NumOfSingle != 0){	//���ж���Ĳ���һҳ�����ݣ�����д��
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

 /**
  * @brief  ��ȡFLASH����
  * @param 	pBuffer���洢�������ݵ�ָ��
  * @param   ReadAddr����ȡ��ַ
  * @param   NumByteToRead����ȡ���ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead){
	SPI_FLASH_CS_LOW();									// ѡ��FLASH: CS�͵�ƽ
	SPI_FLASH_SendByte(W25X_ReadData);					// ���� �� ָ��
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);	// ���� �� ��ַ��λ
	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);		// ���� �� ��ַ��λ
	SPI_FLASH_SendByte(ReadAddr & 0xFF);				// ���� �� ��ַ��λ
	while(NumByteToRead--){	// ��������Ҫ��
		*pBuffer = SPI_FLASH_SendByte(Dummy_Byte);	// ��ȡһ���ֽ�
		pBuffer++;									// ָ����һ���ֽڻ�����
	}
	SPI_FLASH_CS_HIGH();	// ֹͣ�ź� FLASH: CS �ߵ�ƽ
}

/**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
uint32_t SPI_FLASH_ReadID(void){
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	SPI_FLASH_CS_LOW();						// ��ʼͨѶ��CS�͵�ƽ
	SPI_FLASH_SendByte(W25X_JedecDeviceID);	// ����JEDECָ���ȡID
	Temp0 = SPI_FLASH_SendByte(Dummy_Byte);	// ��ȡһ���ֽ�����
	Temp1 = SPI_FLASH_SendByte(Dummy_Byte);	// ��ȡһ���ֽ�����
	Temp2 = SPI_FLASH_SendByte(Dummy_Byte);	// ��ȡһ���ֽ�����
	SPI_FLASH_CS_HIGH();					// ֹͣͨѶ��CS�ߵ�ƽ
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;	//�����������������Ϊ�����ķ���ֵ
	return Temp;
}
/**
  * @brief  ��ȡFLASH Device ID
  * @param 	��
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
  * @brief  ��FLASH���� дʹ�� ����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void){
	SPI_FLASH_CS_LOW();						// ͨѶ��ʼ��CS��
	SPI_FLASH_SendByte(W25X_WriteEnable);	// ����дʹ������
	SPI_FLASH_CS_HIGH();					// ͨѶ������CS��
}

/**
  * @brief  �������ģʽ
  * @param  none
  * @retval none
  */
void SPI_Flash_PowerDown(void){
	SPI_FLASH_CS_LOW();					// ͨѶ��ʼ��CS��
	SPI_FLASH_SendByte(W25X_PowerDown);	// ���� ���� ����
	SPI_FLASH_CS_HIGH();				// ͨѶ������CS��
}




/******************************END OF FILE******************************/




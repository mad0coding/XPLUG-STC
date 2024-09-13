/********************************** (C) COPYRIGHT *******************************
* File Name          : DEFINES.H
* Version            : V1.0
* Date               : 2024/01/20
* Description        : 一些常量或类型定义
********************************************************************************/

#ifndef	__DEFINES_H__
#define __DEFINES_H__

//#include <stdio.h>
//#include <string.h>


//定义函数返回值(注意两种定义)
//#ifndef  SUCCESS
//#define  SUCCESS	1
//#endif
//#ifndef  FAIL
//#define  FAIL	0
//#endif
#ifndef  SUCCESS
#define  SUCCESS	0
#endif
#ifndef  FAIL
#define  FAIL	(-1)
#endif
//定义是否使能
#ifndef  ENABLE
#define  ENABLE		1
#endif
#ifndef  DISABLE
#define  DISABLE	0
#endif
//定义真假值
#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif
//定义空指针
#ifndef NULL
#define NULL	0
#endif
//定义(定时器)起停
#ifndef  START
#define  START	1
#endif
#ifndef  STOP
#define  STOP	0
#endif


//******************************自定义类型别名******************************//
#define u8d UINT8D
#define u8i UINT8I
#define u8p UINT8P
#define u8x UINT8X

#define u8		UINT8
#define u16		UINT16
#define u32		UINT32

#define uint8_t		UINT8
#define uint16_t	UINT16
#define uint32_t	UINT32
#define int8_t		char
#define int16_t		short
#define int32_t		long

#ifndef UINT8P
typedef unsigned char  pdata            UINT8P;
#endif
#ifndef UINT16P
typedef unsigned short pdata            UINT16P;
#endif
#ifndef UINT32P
typedef unsigned long  pdata            UINT32P;
#endif
#ifndef UINT16C
typedef const unsigned short code        UINT16C;
#endif
#ifndef PUINT16C
typedef const unsigned short code      *PUINT16C;
#endif
//**************************************************************************//

//******************************来自STC的类型别名******************************//
//typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

//typedef unsigned char u8;
//typedef unsigned int u16;
//typedef unsigned long u32;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned int ushort;
typedef unsigned long ulong;

//typedef unsigned char uint8_t;
//typedef unsigned int uint16_t;
//typedef unsigned long uint32_t;
//*****************************************************************************//

//******************************来自WCH的类型别名******************************//
#ifndef BOOL
typedef bit                             BOOL;
#endif
#ifndef UINT8
typedef unsigned char                   UINT8;
#endif
#ifndef UINT16
typedef unsigned short                  UINT16;
#endif
#ifndef UINT32
typedef unsigned long                   UINT32;
#endif
#ifndef UINT8D
typedef unsigned char  data             UINT8D;
#endif
#ifndef UINT16D
typedef unsigned short data             UINT16D;
#endif
#ifndef UINT32D
typedef unsigned long  data             UINT32D;
#endif
#ifndef UINT8I
typedef unsigned char  idata            UINT8I;
#endif
#ifndef UINT16I
typedef unsigned short idata            UINT16I;
#endif
#ifndef UINT32I
typedef unsigned long  idata            UINT32I;
#endif
#ifndef UINT8X
typedef unsigned char  xdata            UINT8X;
#endif
#ifndef UINT16X
typedef unsigned short xdata            UINT16X;
#endif
#ifndef UINT32X
typedef unsigned long  xdata            UINT32X;
#endif
#ifndef UINT8V
typedef unsigned char volatile          UINT8V;
#endif
#ifndef UINT8DV
typedef unsigned char volatile data     UINT8DV;
#endif
#ifndef UINT8XV
typedef unsigned char volatile xdata    UINT8XV;
#endif
#ifndef UINT8PV
typedef unsigned char volatile pdata    UINT8PV;
#endif
#ifndef UINT8C
typedef const unsigned char code        UINT8C;
#endif
#ifndef PUINT8
typedef unsigned char                 *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short                *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long                 *PUINT32;
#endif
#ifndef PUINT8I
typedef unsigned char  idata          *PUINT8I;
#endif
#ifndef PUINT16I
typedef unsigned short idata          *PUINT16I;
#endif
#ifndef PUINT32I
typedef unsigned long  idata          *PUINT32I;
#endif
#ifndef PUINT8X
typedef unsigned char  xdata          *PUINT8X;
#endif
#ifndef PUINT16X
typedef unsigned short xdata          *PUINT16X;
#endif
#ifndef PUINT32X
typedef unsigned long  xdata          *PUINT32X;
#endif
#ifndef PUINT8V
typedef unsigned char volatile        *PUINT8V;
#endif
#ifndef PUINT8DV
typedef unsigned char volatile data   *PUINT8DV;
#endif
#ifndef PUINT8XV
typedef unsigned char volatile xdata  *PUINT8XV;
#endif
#ifndef PUINT8PV
typedef unsigned char volatile pdata  *PUINT8PV;
#endif
#ifndef PUINT8C
typedef const unsigned char code      *PUINT8C;
#endif
//*****************************************************************************//

//******************************中断编号******************************//
#define INT_NO_INT0			0
#define INT_NO_Timer0		1
#define INT_NO_INT1			2
#define INT_NO_Timer1		3
#define INT_NO_UART1		4
#define INT_NO_ADC			5
#define INT_NO_LVD			6
#define INT_NO_PCA			7
#define INT_NO_UART2		8
#define INT_NO_SPI			9
#define INT_NO_INT2			10
#define INT_NO_INT3			11
#define INT_NO_Timer2		12
#define INT_NO_RSV13		13
#define INT_NO_INT4			16
#define INT_NO_UART3		17
#define INT_NO_UART4		18
#define INT_NO_Timer3		19
#define INT_NO_Timer4		20
#define INT_NO_CMP			21
#define INT_NO_I2C			24
#define INT_NO_USB			25
#define INT_NO_PWMA			26
#define INT_NO_PWMB			27
#define INT_NO_TKSU			35
#define INT_NO_RTC			36
#define INT_NO_P0			37
#define INT_NO_P1			38
#define INT_NO_P2			39
#define INT_NO_P3			40
#define INT_NO_P4			41
#define INT_NO_P5			42
#define INT_NO_P6			43
#define INT_NO_P7			44
#define INT_NO_DMA_M2M		47
#define INT_NO_DMA_ADC		48
#define INT_NO_DMA_SPI		49
#define INT_NO_DMA_UR1T		50
#define INT_NO_DMA_UR1R		51
#define INT_NO_DMA_UR2T		52
#define INT_NO_DMA_UR2R		53
#define INT_NO_DMA_UR3T		54
#define INT_NO_DMA_UR3R		55
#define INT_NO_DMA_UR4T		56
#define INT_NO_DMA_UR4R		57
#define INT_NO_DMA_LCM		58
#define INT_NO_LCM			59
#define INT_NO_Timer11		67
//********************************************************************//


#endif

/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.H
* Version            : V1.0
* Date               : 2024/01/20
* Description        : STC8H DEBUG
                     (1)、us\ms基本延时函数;
                     (2)、软件复位函数;
                     (3)、看门狗初始化和清零函数;
********************************************************************************/

#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string.h>

#include "stc8h.h"//包含此头文件后不需要再包含"reg51.h"头文件
#include "DEFINES.H"


#define MAIN_Fosc			35000000L	//定义主频

//****************************************常用宏函数****************************************//
#ifndef LIMIT
#define LIMIT(x,min,max)	(x)=(((x)<=(min))?(min):(((x)>=(max))?(max):(x)))//上下限
#endif
#ifndef MINI//为了不和MIN寄存器冲突
#define MINI(X, Y)	((X) < (Y) ? (X) : (Y))//最小值
#endif
#ifndef MAX
#define MAX(X, Y)	((X) > (Y) ? (X) : (Y))//最大值
#endif
#ifndef ABS
#define ABS(X)		((X) > 0 ? (X) : (-(X)))//绝对值
#endif
#ifndef SIGN
#define SIGN(X)		(((X)<0) ? (-1) : (((X)>0) ? 1 : 0))//取符号
#endif
//******************************************************************************************//

void delay_ms(UINT16D ms);			//us延时
void delay_us(UINT16D us);			//ms延时

void STC8H_SoftReset(uint8_t ifISP);							//STC8H软复位
void STC8H_WDT_Init(uint8_t en, uint8_t ps, uint8_t idleMode);	//STC8H看门狗初始化
void STC8H_WDT_Clear();											//喂狗




#endif









/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.C
* Version            : V1.0
* Date               : 2024/01/20
* Description        : STC8H DEBUG
                     (1)、us\ms基本延时函数;
                     (2)、软件复位函数;
                     (3)、看门狗初始化和清零函数;
********************************************************************************/

#include "DEBUG.H"

/*******************************************************************************
* Function Name  : delay_us(UINT16D us)
* Description    : us延时函数
* Input          : UINT16D us
* Output         : None
* Return         : None
*******************************************************************************/
void delay_us(UINT16D us)  // 以uS为单位延时
{
	static UINT8D DELAY_COUNT = 0;
	while(us){  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ DELAY_COUNT;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef	MAIN_Fosc
#if		MAIN_Fosc >= 14000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 16000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 18000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 20000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 22000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 24000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 26000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 28000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 30000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 32000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 34000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 36000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 38000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 40000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 42000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 44000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 46000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 48000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 50000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 52000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 54000000
		++ DELAY_COUNT;
#endif
#if		MAIN_Fosc >= 56000000
		++ DELAY_COUNT;
#endif
#endif
		-- us;
	}
}

/*******************************************************************************
* Function Name  : delay_ms(UINT16D ms)
* Description    : ms延时函数
* Input          : UINT16D ms
* Output         : None
* Return         : None
*******************************************************************************/
void delay_ms(UINT16D ms){
	unsigned int i;
	do{
		i = MAIN_Fosc / 10000;
		while(--i);
	}while(--ms);
}

 /*******************************************************************************
 * Function Name  : STC8H_SoftReset(uint8_t ifISP)
 * Description    : STC8H软复位
 * Input          : uint8_t ifISP
 * Output         : None
 * Return         : None
 *******************************************************************************/
void STC8H_SoftReset(uint8_t ifISP)
{
	if(ifISP) IAP_CONTR |= (1 << 6);//从ISP区重启,数据初始化
	else IAP_CONTR &= ~(1 << 6);//从APP区重启,数据不变
	IAP_CONTR |= (1 << 5);//启动软复位
}

#define D_WDT_FLAG			(1<<7)//看门狗溢出标志位
#define D_EN_WDT			(1<<5)//看门狗使能位
#define D_CLR_WDT			(1<<4)//清零看门狗操作位
#define D_IDLE_WDT			(1<<3)//IDLE模式看门狗是否计数
/*******************************************************************************
* Function Name  : STC8H_WDT_Init(uint8_t en, uint8_t ps, uint8_t idleMode)
* Description    : STC8H看门狗初始化
                   SCALE=2^(ps+1)
                   Timeout=(12*32768*SCALE)/MAIN_Fosc
* Input          : uint8_t en, uint8_t ps, uint8_t idleMode
                   是否启动,分频设置,IDLE模式时是否计数
* Output         : None
* Return         : None
*******************************************************************************/
void STC8H_WDT_Init(uint8_t en, uint8_t ps, uint8_t idleMode)
{
	if(en == ENABLE) WDT_CONTR = D_EN_WDT;				//使能看门狗(软件无法关闭看门狗)
	WDT_CONTR = (WDT_CONTR & ~0x07) | (ps & 0x07);		//看门狗定时器时钟分频系数(0~7代表2~256)
	if(idleMode == 0) WDT_CONTR &= ~0x08;				//IDLE模式停止计数
	else WDT_CONTR |= 0x08;								//IDLE模式继续计数
}

/*******************************************************************************
* Function Name  : STC8H_WDT_Clear()
* Description    : STC8H看门狗喂狗
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STC8H_WDT_Clear()
{
	WDT_CONTR |= D_CLR_WDT;//喂狗
}






//*****************************************
//***  ＡＤ入力関数
//*****************************************
//*****************************************
//*** includes
//***
#include <stdio.h>
#include "iodefine.h"
#include "r_cg_macrodriver.h"		//NOP()を使用するために必要

//**********************************************************************
//*** 関数宣言
//***
extern void		ad19_init(void);	//AD19 初期化
extern unsigned char	ad19_input(void);	//AD19 AD(8ﾋﾞｯﾄ)入力

//**************************************************
//** A/D変換制御
//** 初期化
//**
void ad19_init(void)
{

	PER0 = PER0 | 0x20;		//ADCENセット(1=ｸﾛｯｸ供給)
//ﾎﾟｰﾄ設定
	PMC12 = PMC12 | 0x01;		//ｱﾅﾛｸﾞ入力設定(=1)
	PM12 = PM12 | 0x01;		//ﾎﾟｰﾄ12_0を入力ﾎﾟｰﾄに設定(=1)
//AD変換ﾓｰﾄﾞ設定	
	ADM0 = 0x30;			//AD変換停止状態(ｽﾀｰﾄ信号待ち)、変換起動時間3CLK、ｾﾚｸﾄﾓｰﾄﾞ
	ADM1 = 0x20;			//ｿﾌﾄｳｪｱﾄﾘｶﾞｰ、ﾜﾝｼｮｯﾄ変換ﾓｰﾄﾞ
	ADM2 = 0x00;			//基準Vdd,Vss,,8ﾋﾞｯﾄ分解能
//変換結果上限、下限設定
	ADUL = 0xfe;			//上限(Vdd 20.1V相当)
	ADLL = 0x01;			//下限(Vdd 0.1V相当)
	ADTES = 0x00;			//ﾃｽﾄ用電圧--内部基準電圧1.45V(Vdd 8.8V相当)
	ADS = 0x13;			//AN19(P120端子)選択
}
//**************************************************
//** A/D変換制御
//** 1回読出し（戻数）
//**
unsigned char ad19_input(void)
{
	unsigned char n;
	unsigned char add;

//AD変換開始
	ADM0 = ADM0 | 0x01;		//ADCE=1(A/D変換待機状態）
	for(n=0;n < 10;n++) n++;	//5ループ待機(5microsec以上)
	ADM0 = ADM0 | 0x80;		//ADCS=1(AD変換開始)
	for(n=0;n < 6;n++) n++;		//3ループ待機(3CLK以上)
	add = 0x00;
//データ読出し1回目
	while (add != 0x01){
		add = IF1H;		//変換終了割込みﾌﾗｸﾞ読出し
	 	add = add & 0x01;	//ADIF抽出
		WDTE = 0xAC;		//ｳｫｯﾁﾄﾞｯｸﾞﾀｲﾏｰ_ｸﾘﾔ
	}
	add = ADCRH;
//
	for(n=0;n < 50;n++) n++;	//5ループ待機(5microsec以上)
//データ読出し2回目
	ADM0 = ADM0 | 0x80;		//ADCS=1(AD変換開始)
	add = 0x00;
	while (add != 0x01){
		add = IF1H;		//変換終了割込みﾌﾗｸﾞ読出し
	 	add = add & 0x01;	//ADIF抽出
		WDTE = 0xAC;		//ｳｫｯﾁﾄﾞｯｸﾞﾀｲﾏｰ_ｸﾘﾔ
	}
	add = ADCRH;
//AD変換待機停止	
	ADM0 = ADM0 & 0x7e;		//ADCE=1(A/D変換停止状態）
//		
	return(add);
}
//*************************************************************
//***	時計＆FRAM　制御
//***
#include <stdio.h>
#include <string.h>
#include "iodefine.h"
#include "rtc_fram.h"
//
//*** 変数

//*******************************************
//** SPI(CSI) 初期化
//** CSI11(SAU0,ﾁｬﾈﾙ3)使用
//**
void csi11_initialize(void)
{
//
	PER0 	|= 0x04;	//SAU0 ON
	SPS0	= 0x003;	//fmck0= fclk/8,fmck1= fclk/1
//	SPS0	= 0x007;	//fmck0= fclk/8,fmck1= fclk/1
	SMR03	= 0x0020;	//fmck0を選択,転送完了割込み
	SCR03	= 0xC007;	//送受,MSBﾌｧｰｽﾄ,8Bit(ﾀｲﾌﾟ1）
	SDR03	= 0x0000;	//sck11= fmck0/2
	SO0	&= 0x0F07;	//送受信開始時のﾚﾍﾞﾙSCK11=1,SO11=0
	SOE0	|= 0x0008;	//SO03を送信許可
	PM1	&= 0xFA;	//SCK11&SO11を出力に設定
	PM1	|= 0x02;	//SI011を入力設定
	P1	|= 0x05;	//SCK,SO11を"1"設定
	SS0	|= 0x0008;	//SCI11動作許可
}
//*********************************************
//** SPI(CSI)11 1ﾊﾞｲﾄ送信
//** 送信ﾃﾞｰﾀ 	= tr_data
//** 戻り数	= 0:正常終了、1:送信異常
//** 
unsigned char csi11_tx1b(unsigned char tr_data)
{
	unsigned int timeover_count= 1000;	//ﾀｲﾑｵｰﾊﾞｰ1000ｶｳﾝﾄ
	unsigned int ncnt;

	SDR03 = tr_data;			//1ﾃﾞｰﾀ送信
//** 送信チェック
	for(ncnt=0; ncnt<timeover_count; ncnt++){
		if ((SSR03 & 0x0040) == 0) return(0);	//TSF check
	}
	return(1);				//送信異常
}
//*********************************************
//** SPI(CSI)11 nﾊﾞｲﾄ送信
//** 送信ﾃﾞｰﾀ	= csi_txdata[]
//** 送信ﾃﾞｰﾀ数	= len
//** 戻り数	= 0:正常終了、1:送信異常
//**
unsigned char csi11_txnb(unsigned char len)
{
	unsigned char  Ret,n;

	for (n=0 ;n<=len ;n++){	
		Ret = csi11_tx1b(csi_txdata[n]);	//1ﾃﾞｰﾀ送信
		if (Ret != 0) return(1);		//送信異常？
	}
	return(0);
}
//*********************************************
//** SPI(CSI11) 1ﾊﾞｲﾄ受信
//** 受信ﾃﾞｰﾀ	= csi_rxdata
//** 戻り数	= 0:正常終了、1:受信異常
//**
unsigned char csi11_rx1b(void)
{
	unsigned int timeover_count= 1000;	//ﾀｲﾑｵｰﾊﾞｰ1000ｶｳﾝﾄ
	unsigned int ncnt;
	unsigned char tmp_data;

	SDR03 = 0xFF;				//ﾀﾞﾐｰ送信
	chk_data = 0;
	for(ncnt=0; ncnt<timeover_count; ncnt++){
		if ((SSR03 & 0x0040) == 0){	//TSF check
			chk_data = 1;
			tmp_data = SDR03;	//ﾃﾞｰﾀ読出し
			csi_rxdata = tmp_data;
			return(0);
		}
	}
	return(1);				//受信異常
}
//***********************************************************
//** RTCﾃﾞｰﾀ読出し
//** 読出しﾃﾞｰﾀ	= gYear,gMonth,gDay,gHour,gMin,gSec (binary)
//** 戻り数	= 0:正常終了、1:読出し異常
//***********************************************************
unsigned char rtc_data_read(void)
{
	unsigned char Ret;
	unsigned char i,n;
	
	P1 = P1 | 0x80;				// RTC CE ON
//
	for (i=0 ;i<150 ;i++) {
		n = n + i;			// 31microSEC以上　DELAY
	}
//RTC読出し
	Ret = csi11_tx1b(0xf4);			// address'f'からﾊﾞｰｽﾄ読出し指定
	if (Ret != 0){				// 
		P1 = P1 & 0x7F;			// RTC CE OFF
		return(1);			// 受信異常
	}

	for (i=0 ;i<8 ;i++){
		Ret = csi11_rx1b();		// データ読出し→ csi_rxdata
		if (Ret != 0){  		// 
			P1 = P1 & 0x7F;		// RTC CE OFF
			return(1);		// 受信異常
		}
		chk_buf[i] = csi_rxdata;
	}
	P1 = P1 & 0x7F;				// RTC CE OFF
//時計ﾃﾞｰﾀを格納
	if (chk_buf[7] != 0xff) rtc_read_year = chk_buf[7];
	if (chk_buf[6] != 0xff) rtc_read_month= chk_buf[6];
	if (chk_buf[5] != 0xff) rtc_read_day  = chk_buf[5];
	if (chk_buf[3] != 0xff) rtc_read_hour = chk_buf[3];
	if (chk_buf[2] != 0xff) rtc_read_min  = chk_buf[2];
	if (chk_buf[1] != 0xff) rtc_read_sec  = chk_buf[1];
//	rtc_read_sec  = 0x31;
return(0);
}
//**************************************************************	
//** 10進数ﾃﾞｰﾀ(bcd_data)　→　16進数ﾃﾞｰﾀ(bc_data)
//** 引数	:bcd_data(1ﾊﾞｲﾄ)
//** 戻り数	:bc_data(1ﾊﾞｲﾄ）
//**
unsigned char bcd2bc(unsigned char bcd_data)
{
	unsigned char  temp1,temp2,temp3;

	temp1 = bcd_data % 0x10;
	temp2 = bcd_data / 0x10;
	temp3 = temp2 * 0x0a + temp1;
	return (temp3);
}
//***********************************************************
//** 時計ﾃﾞｰﾀをRTCに書込み
//** 書込みﾃﾞｰﾀ rtc_write_year   rtc_write_[][][][]	(BCD)
//**  rtc_write_buff[]
//**	+0	+1	+2	+3	+4	+5	+6
//**	sec	min	hour	weekday	day	month	year
//** 戻り数	= 0:正常終了、1:読出し異常
//***********************************************************
unsigned char  rtc_data_write(void)
{
	unsigned char Ret;
	unsigned char n,i;

// 書込みﾃﾞｰﾀをﾊﾞｯﾌｧに移動	
	rtc_write_buff[0] = rtc_write_sec;
	rtc_write_buff[1] = rtc_write_min;
	rtc_write_buff[2] = rtc_write_hour;
	rtc_write_buff[3] = 0;
	rtc_write_buff[4] = rtc_write_day;
	rtc_write_buff[5] = rtc_write_month;
	rtc_write_buff[6] = rtc_write_year;
	rtc_write_buff[7] = 0;
//		
	P1 = P1 | 0x80;				// RTC CE ON
	for (i=0 ;i<150 ;i++) {
		n += i;				// 31microSEC以上 DELAY
	}
//RTC書込み
	Ret = csi11_tx1b(0x00);			// address'0'からﾊﾞｰｽﾄ書込み指定
	if (Ret != 0){				// 
		P1 = P1 & 0x7F;			// RTC CE OFF
		return(1);			// 受信異常
	}
	for (i=0 ;i<8 ;i++){
		Ret = csi11_tx1b(rtc_write_buff[i]);	// 0番地から順番に書込み
		if (Ret != 0){			// 
			P1 = P1 & 0x7F;		// RTC CE OFF
			return(1);		// 受信異常
		}
	}
	P1 = P1 & 0x7F;				//RTC CE OFF
//
	return(0);
}
//***********************************************************
//**   RTC 初期化	 RTC_init(void)
//** RTC(RX-4045)初期化	: 24時間制/FOUT端子OFF
//** pon,foutﾚｼﾞｽﾀのON/OFF(1=ON,0=OFF)
//** 戻り値	: non
//**
void rtc_init(unsigned char fout)
{
//	unsigned char Ret;
	unsigned char i,n;
	
	P1 = P1 | 0x80;				// RTC CE ON
	for (i=0 ;i<150 ;i++){
		n += i;				// DELAY
	}
//
	if(fout == 1) {
		csi11_tx1b(0xE0);		// address'E'からﾊﾞｰｽﾄ書込み指定
		csi11_tx1b(0x30);		// 制御ﾚｼﾞｽﾀ1(E):12/24=1,CLEN2=1
		csi11_tx1b(0x08);		// 制御ﾚｼﾞｽﾀ2(F):,CLEN1=1
	}
	else {
		csi11_tx1b(0xE0);		// address'E'からﾊﾞｰｽﾄ書込み指定
		csi11_tx1b(0x20);		// 制御ﾚｼﾞｽﾀ1(E):12/24=1,CLEN2=0
	}
	P1 = P1 & 0x7F;				// RTC CE OFF
}

//***********************************************************************
//***       RTC ﾊﾟﾜｰｵﾝ検出
//***  RTC(RX-4045)の電源初期化を検出する
//*** Return Value	: 0=normal,1=reset
//***
unsigned char rtc_powon_check(void)
{
//	unsigned char Ret;
	unsigned char i,n;
	
	P1 = P1 | 0x80;	;				// RTC CE ON
	for (i=0 ;i<150 ;i++){
		n += i;					// DELAY
	}
//
	csi11_tx1b(0xF4);				// address'F'からﾊﾞｰｽﾄ読出し指定
	csi11_rx1b();					// アドレスFのデータ読出し
//
	P1 = P1 & 0x7F;					// RTC CE OFF
//
	if ((csi_rxdata & 0x10) != 0) return(1);	// RTC ﾊﾟﾜｰoff有り、ﾘｾｯﾄ必要 
	return(0);					// ﾘｾｯﾄ不要
}

//***********************************************************************
//***	RTC 月日時分秒リセット
//***  RTC(RX-4045)ﾘｾｯﾄ時に時計を2010/01/01 00:00:00にｾｯﾄする
//***
void rtc_time_reset(void)
{		
	rtc_write_buff[6] = 0x10;	// year
	rtc_write_buff[5] = 0x01;	// Month
	rtc_write_buff[4] = 0x01;	// Dat
	rtc_write_buff[3] = 0;
	rtc_write_buff[2] = 0x00;	// Hour
	rtc_write_buff[1] = 0x00;	// Min
	rtc_write_buff[0] = 0x00;	// Sec
	rtc_data_write();		// rtc_wdata[n]をRTCに書込み
}
//**********************************************************************
//***  FRAM1 書込み(16ﾊﾞｲﾄ単位書込み)
//***  書込みﾃﾞｰﾀ：	fram_write_buff[16]
//***  書込みｱﾄﾞﾚｽ：	 fram_write_address	
//***
unsigned char fram1_write(void)
{
	unsigned char Ret,i,n;
	n=0;
	P0 = P0 & 0xBF;				// FRAM1 CS ON
	csi11_tx1b(0x06);			// WREN(書込み許可）
	P0 = P0 | 0x40;				// FRAM1 CS OFF	
//
	for (i=0 ;i<100 ;i++) n += i;		// DELAY
//
	P0 = P0 & 0xBF;				// FRAM1 CS ON
 	Ret = fram_write();
	P0 = P0 | 0x40;				// FRAM1 CS OFF	
//	
	for (i=0 ;i<100 ;i++) n += i;		// DELAY
//
	P0 = P0 & 0xBF;				// FRAM1 CS ON
	csi11_tx1b(0x04);			// WREN(書込み禁止）
	P0 = P0 | 0x40;				// FRAM1 CS OFF	
//
	return(Ret);
}	
//**********************************************************************
//***  FRAM2 書込み(16ﾊﾞｲﾄ単位書込み)
//***  書込みﾃﾞｰﾀ：	fram_write_buff[16]
//***  書込みｱﾄﾞﾚｽ：	fram_write_address	
//***
unsigned char fram2_write(void)
{
	unsigned char Ret,i,n;

	n = 0;
	P0 = P0 & 0xDF;				// FRAM2 CS ON
	csi11_tx1b(0x06);			// WREN(書込み許可）
	P0 = P0 | 0x20;				// FRAM2 CS OFF	
//
	for (i=0 ;i<10 ;i++) n += i;		// DELAY
//
	P0 = P0 & 0xDF;				// FRAM2 CS ON
 	Ret = fram_write();
	P0 = P0 | 0x20;				// FRAM2 CS OFF	
//
	for (i=0 ;i<10 ;i++) n += i;		// DELAY
//
	P0 = P0 & 0xDF;				// FRAM2 CS ON
	csi11_tx1b(0x04);			// WREN(書込み禁止）
	P0 = P0 | 0x20;				// FRAM2 CS OFF	
//
	return(Ret);
}	
//**********************************************************************
//*** FRAM 16ﾊﾞｲﾄ書込み（CS、書込み許可・禁止　含まず）
//***
unsigned char fram_write(void)
{
	unsigned char Ret,i;
	unsigned adr100,adr10,adr1;
	unsigned long adr_temp;
//
	adr_temp = fram_write_address % 0x1000;	// 先頭ｱﾄﾞﾚｽ3ﾊﾞｲﾄ作成
	adr100 = adr_temp /0x100;
	adr_temp = adr_temp % 0x100;
	adr10 = adr_temp /0x10;
	adr1 = adr_temp % 0x10;
//
	csi11_tx1b(0x02);			// 書込み指定(1ﾊﾞｲﾄ)
	csi11_tx1b(adr100);			// ｱﾄﾞﾚｽ指定(3ﾊﾞｲﾄ)
	csi11_tx1b(adr10);
	csi11_tx1b(adr1);
//
	for (i=0 ;i<16 ;i++){
		Ret = csi11_tx1b(fram_write_buff[i]);	// 0番地から順番に書込み
		if (Ret != 0){			// 
			return(1);		// 受信異常
		}
	}
//
	return(0);
}

//**********************************************************************
//***  FRAM1 読出し(16ﾊﾞｲﾄ単位書込み)
//***  読出しﾃﾞｰﾀ：	fram_read_buff[16]
//***  読出しｱﾄﾞﾚｽ：	fram_read_address	
//***
unsigned char fram1_read(void)
{
	unsigned char Ret;

	P0 = P0 & 0xBF;				// FRAM1 CS ON
 	Ret = fram_read();
	P0 = P0 | 0x40;				// FRAM1 CS OFF	
//	
	return(Ret);
}	

//**********************************************************************
//***  FRAM2 読出し(16ﾊﾞｲﾄ単位書込み)
//***  読出しﾃﾞｰﾀ：	fram_read_buff[16]
//***  読出しｱﾄﾞﾚｽ：	fram_read_address	
//***
unsigned char fram2_read(void)
{
	unsigned char Ret;

	P0 = P0 & 0xDF;				// FRAM2 CS ON
 	Ret = fram_read();
	P0 = P0 | 0x20;				// FRAM2 CS OFF	
//	
	return(Ret);
}	

//**********************************************************************
//*** FRAM 16ﾊﾞｲﾄ読出し（CS　含まず）
//***
unsigned char fram_read(void)
{
	unsigned char Ret,i;
	unsigned adr100,adr10,adr1;
	unsigned long adr_temp;

//
	adr_temp = fram_read_address % 0x1000;	// 先頭ｱﾄﾞﾚｽ3ﾊﾞｲﾄ作成
	adr100 = adr_temp /0x100;
	adr_temp = adr_temp % 0x100;
	adr10 = adr_temp /0x10;
	adr1 = adr_temp % 0x10;
//
	csi11_tx1b(0x03);			// 読出し指定(1ﾊﾞｲﾄ)
	csi11_tx1b(adr100);			// ｱﾄﾞﾚｽ指定(3ﾊﾞｲﾄ)
	csi11_tx1b(adr10);
	csi11_tx1b(adr1);
//
	for (i=0 ;i<16 ;i++){
		Ret = csi11_rx1b();		// 0番地から順番に読出し
		if (Ret != 0){			// 
			return(1);		// 受信異常
		}
		fram_read_buff[i] = csi_rxdata;	//
	}
//
	return(0);
}
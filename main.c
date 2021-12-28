/***********************************************************************/
/*                                                                     */
/*  FILE        : SLP10_VT_SIO_TEST                                    */
/*  DATE        : 2019/12/10                                           */
/*  DESCRIPTION : Main Program                                         */
/*  CPU TYPE    : R5F104MG(AFB) 80pin   ROM:128kB   RAM:16384B         */
/*                                                                     */
/*  NOTE:THIS IS A TEST EXAMPLE.                                       */
/*                                                                     */
/***********************************************************************/
//**********************************************************************
//*** includes
//***
#include <stdio.h>
#include <string.h>
#include "iodefine.h"
#include "led_switech.h"		//ﾃﾞｨｯﾌﾟｽｲｯﾁ処理、LED制御
#include "uart2.h"			//uart2関数
#include "ad19.h"			//Vdd測定用ADコン制御関数
#include "r_cg_macrodriver.h"		//HALT()を使用するために必要
#include "key_disp.h"			//ｷｰｽｲｯﾁ、LCD制御
#include "rtc_fram.h"			//時計、EEPROM制御
#include "uart3.h"			//uart3(USB)
//**********************************************************************
//*** function（関数宣言）
//***
void 		main(void);
void		IO_Init(void);			//IOﾎﾟｰﾄ　初期化
void 		clk_HOCOto_X1(void);		//HOCOクロックをX1振動子クロックへ変更
//void 		suii_level_adjust(void);	//suii_bcd + level_settei 
//unsigned int 	bcd_to_bc(unsigned int bcd_dat);//BCD4桁をBCに変換
//unsigned int 	bc_to_bcd(unsigned int bc_dat);	//BC(0~9999)をBCDに変換
void 		timer01_interval_sec(unsigned char dt);//ｲﾝﾀｰﾊﾞﾙﾀｲﾏｰunit0ch1の初期設定
void 		timer01_interval_irq (void);
//BCD入力
void 		bcd_ch1in(void);	//CH1 BCD入力、ﾋﾞｯﾄ配置変換
void 		bcd_ch2in(void);	//CH2 BCD入力、ﾋﾞｯﾄ配置変換
void 		bcd_convert(void);
void 		bcd4byte_in(void);


//**********************************************************************
//*** define　(文字定義）
//***            1234567890123456789
#define VER	"SLP10-VT_SIO V1.00 " //最大26Byteで指定のこと

//**********************************************************************
//*** bariables（変数宣言）
//***
//*  設定ｽｲｯﾁ
unsigned char	mode_settei;		//送信動作切替設定	
unsigned char	frequency_settei;	//送信周波数チャンネル(7～46CH)
unsigned char	length_settei;		//測定柱全長
unsigned int	level_settei;		//ﾚﾍﾞﾙ補正値
unsigned char 	sw_id[16];		//switch read data

//*  LED & ETC
unsigned char	tx1_flag;		//送信中ﾌﾗｸﾞ
unsigned char	rtx_flag;		//ﾘﾓｺﾝ送信中ﾌﾗｸﾞ
unsigned char	rrx_flag;		//ﾘﾓｺﾝ受信中ﾌﾗｸﾞ
unsigned char	sng_flag;		//ｾﾝｻｰ異常ﾌﾗグ-abnormal flagﾞ
unsigned char	bng_flag;		//電源異常ﾌﾗｸﾞ
unsigned char	sleep_flag;		//ｽﾘｰﾌﾟ開始ﾌﾗｸﾞ
unsigned int 	led_count;

//*  UART1(MU-3) & 2(外部ｼﾘｱﾙ)
unsigned char	ID[16];			//入力生データ
unsigned int	hbcd;			//水位
unsigned int	hbc;
unsigned char	herr;
unsigned char	uart1_buf[32];		//出力バッファー
unsigned char	uart1_buf_cnt;		//出力カウンター
unsigned char	uart2_buf[32];		//出力バッファー
unsigned char	uart2_buf_cnt;		//出力カウンター

//AD
unsigned char	ad_data;		//AD入力値(16進)
unsigned int	vdd_bc;			//Vdd電圧_BC[0.1V]
unsigned int	vdd_bcd;		//Vdd電圧BCD[0.1V]
//
unsigned int	iv_counter;		//ｲﾝﾀｰﾊﾞﾙ発生計数器0～9999
//チェック用
unsigned char 	chk_byt;
unsigned int 	chk_word;

//** RTC用変数
//** 時計読出し周期ｶｳﾝﾀ
unsigned int loop_counter;
//
unsigned char 	csi_txdata[128];
unsigned char 	csi_rxdata;
unsigned char	chk_data;
unsigned char	chk_buf[16];
unsigned char   rtc_write_buffata[8];	//RTC書込み用ﾊﾞｯﾌｧ(BCD)
//** RTC用変数
unsigned char rtc_read_year;		//RTC読出しﾃﾞｰﾀ
unsigned char rtc_read_month;
unsigned char rtc_read_day;
unsigned char rtc_read_hour;
unsigned char rtc_read_min;
unsigned char rtc_read_sec;
//
unsigned char rtc_write_year;		//RTC書込み用ﾃﾞｰﾀ
unsigned char rtc_write_month;
unsigned char rtc_write_day;
unsigned char rtc_write_hour;
unsigned char rtc_write_min;
unsigned char rtc_write_sec;
unsigned char rtc_write_buff[8];	//RTC書込みﾊﾞｯﾌｧ
unsigned char rtc_write_flag;		//RTC書込みﾌﾗｸﾞ(1：書込み)
//** FRAM用変数
unsigned char fram_read_buff[16];	//FRAM読出しﾊﾞｯﾌｧ(16ﾊﾞｲﾄ)
unsigned char fram_write_buff[16];	//FRAM書込みﾊﾞｯﾌｧ(16ﾊﾞｲﾄ)
unsigned long fram_read_address;      //FRAM読出し先頭ｱﾄﾞﾚｽ
unsigned long fram_write_address;	//FRAM書込み先頭ｱﾄﾞﾚｽ
unsigned char fram_read_flag;		//FRAM読出しﾌﾗｸﾞ(1:読出し)
unsigned char fram_write_flag;		//FRAM書込みﾌﾗｸﾞ(1:書込み)//
//雨量
unsigned char	rid;			//雨量ﾊﾟﾙｽ入力値P6_4
unsigned char	brid;
unsigned int r_counter;			//雨量ﾊﾟﾙｽ積算値9999mm
//
unsigned char 	bcd_id[4];		//入力ﾃﾞｰﾀ
unsigned char 	bcd_data[4];		//BCDﾃﾞｰﾀ
unsigned char	ch1_id[4];		//CH1入力ﾃﾞｰﾀ
unsigned char	ch1_bcd[4];		//CH1BCDﾃﾞｰﾀ
unsigned char	ch2_id[4];		//CH2入力ﾃﾞｰﾀ
unsigned char	ch2_bcd[4];		//CH2BCDﾃﾞｰﾀ
//UART2(GPS)
unsigned char	rx2_id_buff[32]="-- -- -- -- -- -- -- -";	//RX2入力ﾊﾞｯﾌｧ
unsigned char	rx2_id_point = 0;	//RX2入力ﾊﾞｯﾌｧﾎﾟｲﾝﾀ
unsigned char	rx2_id;			//RX2入力ﾃﾞｰﾀ（随時最新）
unsigned char	gps_power_flag = 0;	//0=OFF,1=ON
//UART3(USB)
unsigned char	rx3_id_buff[16]="------------";	//RX3入力ﾊﾞｯﾌｧ
unsigned char	rx3_id_point = 0;	//RX3入力ﾊﾞｯﾌｧﾎﾟｲﾝﾀ
unsigned char	rx3_id;			//RX3入力ﾃﾞｰﾀ（随時最新）
//************************************************************************
//* Function Name: main
//* Description  : This function implements main function.（メイン関数）
//* Arguments    : None
//* Return Value : None
//*************************************************************************
void main(void)
{
    	IO_Init();			//IO初期化

//Main clock 設定    
	clk_HOCOto_X1();		//CPU&IOｸﾛｯｸをX1に変更
//	HOCODIV = 0;			//HOCO  32MHz
//	HOCODIV = 0x01;			//HOCO  16MHz
//	HOCODIV = 0x02;			//HOCO  8MHz
//	HOCODIV = 0x05;			//HOCO  1MHz
//	CKC = CKC & 0xef;		//HOCOをﾒｲﾝｸﾛｯｸに切替
//SPI 初期化(RTC,FRAM)
	csi11_initialize();		//SPI初期化(RTC,FRAM)
	rtc_init(1);			//RTC=24時間ﾓｰﾄﾞ､周波数出力OFF
//
	if ((P10 & 0x01) == 0){		//[SET] ON ?
		rtc_write_year = 0x22;	//RTC書込みしﾃﾞｰﾀ
		rtc_write_month = 0x01;
		rtc_write_day = 0x01;
		rtc_write_hour = 0x00;
		rtc_write_min = 0x00;
		rtc_write_sec = 0x00;
		rtc_data_write();
	}
//LCD 初期化
	lcd_init();
	lcd_disp();
	
//UART 初期化	
	UART2_initial();
	UART3_initial();

//A/D 初期化　AD／255＝（Vdd／6.1)／3.3　=>　Vdd＝0.79 * AD　
	ad19_init();				//AD19 初期化
	ad_data = ad19_input();			//Vddデータ入力
//** ｲﾝﾀｰﾊﾞﾙﾀｲﾏｰ設定
	timer01_interval_sec(1);		//1秒ｲﾝﾀｰﾊﾞﾙﾀｲﾏｰ
//** 雨量ﾊﾟﾙｽ切替リレーON
	P6 |= 0x40;				//ﾗｯﾁﾘﾚｰON出力ｾｯﾄ
//
	P0 &= 0xDF;		//FRAM2 CS Enable
	csi11_tx1b(0x03);	//READ ｺﾏﾝﾄﾞ
	csi11_tx1b(0x01);	//ｱﾄﾞﾚｽ1FFFF(最後尾のアドレス)
	csi11_tx1b(0xFF);
	csi11_tx1b(0xFF);
	csi11_rx1b();		//ｶｳﾝﾀの下一桁を1FFFF番地に読出しみ
	P0 = P0 | 0x20;		//FRAM2 CS disable
	r_counter =csi_rxdata;
//
//**********************************************************************
//***		メインループ
//**********************************************************************
    	while (1U)
	{
//ｾﾝｻｰﾃﾞｰﾀ制御
//表示制御
		lcd_disp();		
//UART2 出力制御（外部ｼﾘｱﾙ出力）
//		UART2_txbuf_set();		//tx2_trans_flag = 1の時、送信ﾊﾞｯﾌｧへデータ格納
//		UART2_trans_1d();		//送信ﾊﾞｯﾌｧにﾃﾞｰﾀが有り、tx2 送信ﾚｼﾞｽﾀ空のとき、1byte出力
//電源電圧Vdd入力
		if (loop_counter == 0) {
			ad_data = ad19_input();		//Vddデータ入力
			vdd_bc = ad_data * 80;			
			vdd_bc = (vdd_bc + 50) / 100;	//0.1V以下四捨五入
//		vdd_bcd = bc_to_bcd(vdd_bc);	//ﾌｫｰﾏｯﾄ="00.0" V		
		}
//時計入力＆LED点滅
		if (loop_counter >= 3) {
			P6 &= 0xFE;	//LED1 ON
		}
		if (loop_counter >= 4){
			P6 |= 0x01;		//LED1(黄緑)OFF
			loop_counter = 0;
			rtc_data_read();	//時計入力（rtc_sec～rtc_year)
			P4 &= 0xDF;		//LCDﾊﾞｯｸﾗｲﾄ OFF
			P6 &= 0xBF;		//ﾗｯﾁﾘﾚｰON出力ﾘｾｯﾄ
		}
		else	loop_counter++;
//BCD入力
		if (loop_counter == 2) bcd_ch1in();	//BCD1入力
		if (loop_counter == 3) bcd_ch2in();	//BCD2入力
		
//雨量入力
		rid = P6 & 0x20;		//凸ﾊﾟﾙｽIN
		if (rid == 0 && brid != 0){
			r_counter++;
//雨量ｶｳﾝﾀ書込み
			P0 &= 0xDF;		//FRAM2 CS Enable
			chk_byt = csi11_tx1b(0x06);	//書込み許可ラッチ
			P0 |= 0x20;		//FRAM2 CS Disable
//
			P0 &= 0xDF;		//FRAM2 CS Enable
			csi11_tx1b(0x02);	//WRITE ｺﾏﾝﾄﾞ
			csi11_tx1b(0x01);	//ｱﾄﾞﾚｽ1FFFF(最後尾のアドレス)
			csi11_tx1b(0xFF);
			csi11_tx1b(0xFF);
			chk_data = r_counter % 0x100;
			csi11_tx1b(chk_data);	//ｶｳﾝﾀの下一桁を1FFFF番地に書込み
			P0 |= 0x20;		//FRAM2 CS Disable
//
			P0 &= 0xDF;		//FRAM2 CS Enable
			chk_byt = csi11_tx1b(0x04);	//書込み禁止ラッチ
			P0 |= 0x20;		//FRAM2 CS Disable
//			
		}
		brid = rid;

//ｽﾘｰﾌﾟ制御
		WDTE = 0xAC;			//WDT ｸﾘﾔ
//		P6 = P6 | 0x01;			//LED0<0>=1 OFF
//		P4 &= 0xDF;			//LCDﾊﾞｯｸﾗｲﾄ OFF
//		ADM0 = ADM0 & 0x7e;		//AD変換停止(ADCS,ADCE=0)
//		PER0 = PER0 & 0xdf;		//ADCEN(PER<5>)停止(0=ｸﾛｯｸ停止)
//		PER0 = PER0 & 0xf3;		//SAU0EN,SAU1EN停止(0=ｸﾛｯｸ停止)
//		PER0 = PER0 & 0xf0;		//SAU0EN,SAU1EN,TAU0EN,TAU1EN停止(0=ｸﾛｯｸ停止)
//		HALT();
//		P3 = 0x01;			//ｾﾝｻｰ電源 ON
		ad19_init();			//AD19 初期化
//		UART1_initial();		//UART1 初期化(MU)
//		UART2_initial();		

//UART2 初期化(AUX)
		NOP();
		NOP();
//		P6 = P6 & 0xfe;			//LED0<0>=0 ON
//ｳｫｯﾁﾄﾞｯｸﾞﾀｲﾏｰ制御
		WDTE = 0xAC;			//WDT ｸﾘﾔ

	}    
}
//***********************************************************************
//**	CH1 BCD 入力
//**
void 	bcd_ch1in(void)
{
	unsigned char i;
	
	P14 = P14 | 0x80;	//BCD INPUT Enable
	P11 = P11 | 0x01;	//CH1 SELECT ON
	bcd4byte_in();
	P11 = P11 & 0xFE;	//CH1 SELECT OFF
	P14 = P14 & 0x7F;	//BCD INPUT Disable
//
	bcd_convert();
//
	for (i=0; i<4;i++){
		ch1_id[i] = bcd_id[i];
		ch1_bcd[i] = bcd_data[i];
	}
//
}
//***********************************************************************
//**	CH2 BCD 入力
//**
void 	bcd_ch2in(void)
{
	unsigned char i;
	
	P14 = P14 | 0x80;	//BCD INPUT Enable
	P11 = P11 | 0x02;	//CH2 SELECT ON
	bcd4byte_in();
	P11 = P11 & 0xFD;	//CH2 SELECT OFF
	P14 = P14 & 0x7F;	//BCD INPUT Disable
//
	bcd_convert();
//
	for (i=0; i<4;i++){		//ﾃﾞｰﾀ移動
		ch2_id[i] = bcd_id[i];
		ch2_bcd[i] = bcd_data[i];
	}
//
}
//**********************************************************	
//ﾋﾞｯﾄ配置変換
void bcd_convert(void)
{
	unsigned char bcd1,bcd2;
	unsigned int iwork;
	
	bcd1 = bcd_id[0] ^ 0xFF;	//反転
	bcd1 = bcd1 >> 1;		//右シフト１
	bcd1 &= 0x0F;
	iwork = bcd_id[0] + bcd_id[1] * 0x100;
	iwork = iwork >> 2;		//右シフト2
	bcd2 = iwork % 0x100;
	bcd2 = bcd2 ^ 0xFF;		//反転
	bcd2 &= 0xF0;
	bcd_data[0] = bcd1 + bcd2;	//10位、1位<8421 8421>
//	
	bcd1 = bcd_id[1] ^ 0xFF;	//反転
	bcd1 = bcd1 >> 3;		//右シフト3
	bcd1 &= 0x0F;
	bcd2 = bcd_id[2];
	bcd2 = bcd2 << 4;		//左シフト4
	bcd2 = bcd2 ^ 0xFF;		//反転
	bcd2 &= 0xF0;
	bcd_data[1] = bcd1 + bcd2;	//1000位、100位<8421 8421>
//
	bcd1 = bcd_id[2] ^ 0xFF;	//反転
	bcd1 = bcd1 >> 5;		//右シフト5
	bcd1 &= 0x07;
	bcd2 = bcd_id[3] ^ 0xFF;	//反転
	bcd2 = bcd2 << 3;		//左シフト3
	bcd2 &= 0x18;	
	bcd_data[2] = bcd1 + bcd2;	//10000位<***- 8421>
//
	bcd1 = bcd_id[3] ^ 0xFF;	//反転
	bcd1 = bcd1 >> 2;		//右シフト2
	bcd_data[3] = bcd1 & 0x0F;	//ch1_bcd[3]=<****,CAL,PNG,SNG,BSY>
}
//*********************************************************************
void bcd4byte_in(void)
{
	P7 = P7 | 0x01;		//SEL0 ON
	delay_msec(3);
	bcd_id[0] = P2;
	P7 = P7 & 0xFE;		//SEL0 OFF
//
	P7 = P7 | 0x02;		//SEL1 ON
	delay_msec(3);
	bcd_id[1] = P2;
	P7 = P7 & 0xFD;		//SEL1 OFF
//
	P7 = P7 | 0x04;		//SEL2 ON
	delay_msec(3);
	bcd_id[2] = P2;
	P7 = P7 & 0xFB;		//SEL2 OFF
//
	P7 = P7 | 0x08;		//SEL3 ON
	delay_msec(3);
	bcd_id[3] = P2;
	P7 = P7 & 0xF7;		//SEL3 OFF
//

}
//*************************************************************************
//***  サブ関数領域
//***
void IO_Init(void)
{
    /* Start user code. Do not edit comment generated here */
    
    /* Start user code. Do not edit comment generated here */
//     int i;
//GPIO_Port 設定
	P0 = 0x62;		//SDC_CS,FRAM_CS disable
	P1 = 0x01;		//RTC disable
//	P2 = 0;			//入力
//
	P3 = 0x03;		//5V ON, 設定ｽｲｯﾁｺﾓﾝOFF
	P4 = 0;			//LCDﾊﾞｯｸﾗｲﾄ OFF
	P5 = 0;
//
	P6 = 0x03;		//LED1,2 OFF
	P7 = 0x10;		//GPS OFF	
	P10 = 0;
//
	P11 = 0;
	P13 = 0;
	P14 = 0;		//SDC POWER OFF	
//
	P15 = 0;
//Portモードレジスタ セット
	PM0  = 0x09;		//SDカード & FRAM_CS1,2
	PM1  = 0x78;		//SPI(RTC,FRAM) & 操作ｽｲｯﾁ(MODE,UP,DOWN)  
	PM2  = 0xFF;		//BCD input Port
//
	PM3  = 0x00;		//設定ｽｲｯﾁｺﾓﾝ、5V制御
	PM4  = 0x07;		//SDカードWP入力&LCD(RS,E,ﾊﾞｯｸﾗｲﾄ制御)
	PM5  = 0xF0;		//LCD(D4~D7)& 操作ｽｲｯﾁ(LEFT,RIGHT)
//
	PM6  = 0xB0;		//LED1,2& RainPulse& ﾗｯﾁﾘﾚｰ制御& 12V低下入力
	PM7  = 0x60;		//BCD入力SEL& S0~S7 output
	PM10 = 0xFF;		//操作ｽｲｯﾁ(SET)入力
//
	PM11 = 0xFC;		//BCD入力CH1,CH2出力
	PM12 = 0xFF;		//input(X,XT)& 電源電圧入力(A/D)
//	PM13 = 0xFE;		//NoUSE
//
	PM14 = 0x09;		//USB(UART3)& SDC電源& BCD入力(CALL,INPUT Enable)
	PM15 = 0xFF;		//S80,S90 input
//ﾌﾟﾙｱｯﾌﾟ設定
	PU1 = 0x38;		//MODE,UP,DOWN pullup
//	PU3 = PU3 | 0x02;	//NC P31 pullup
	PU4 = PU4 | 0x06;	//NC P41,P42 pullup
	PU5 = 0x30;		//LEFT,RIGHT pullup
	PU10 = 0X01;		//SET pullup
//	PU11 = PU11 | 0x03;	//NC P110,P111 pullup
//ｱﾅﾛｸﾞﾎﾟｰﾄ／ﾃﾞｼﾞﾀﾙﾎﾟｰﾄ切替設定
	ADPC = 0x01;		//P20-P27,P150-P156をﾃﾞｼﾞﾀﾙIOに設定
	PMC0 = 0XF3;		//ANI16,ANI17使用しない
//PMC1変更なし
	PMC10 = 0xfe;		//ANI20使用しない
	PMC12 = 0xff;		//ANI19使用する
	PMC14 = 0x7f;		//ANI18使用しない		
//UART2ピン配置変更
	PIOR0	|= 0x02U;	//PIOR01=1(UART2のピン配置を変更)

}
//********************************************
//*** システムクロック設定
//*** 高速オンチップクロック(HOCO)　→　外部高速クロック(X1)
//***
void clk_HOCOto_X1(void)
{
	unsigned char cd = 0;

	CMC = CMC | 0x40;	// X1発振子ﾓｰﾄﾞ設定 EXCLK=0 & OSCSEL=1
	OSTS = 0x05;		// 発振安定時間設定 6.55msec／5MHz
	CSC = CSC & 0x7f;	// X1発振開始 MSTOP=0
	while (cd < 0xfc){
		cd = OSTC;	// 発振安定時間待機
	}
	CKC = CKC | 0x10;	// fmxをﾒｲﾝｸﾛｯｸに切替	
//	chk_byt = 0x0002c;	// ｵﾌﾟｼｮﾝ2
	CSC = CSC | 0x01;	// HOCO を停止 
}


//****************************************
//** ｲﾝﾀｰﾊﾞﾙ ﾀｲﾏｰ
//** ﾀｲﾏｰｱﾚｲﾕﾆｯﾄ0、ﾁｬﾈﾙ1を使用
//** fclk = 5 [MHz]
//** 2^10分周してT=204.8 [microsec]をｶｳﾝﾄ
//** ｲﾝﾀｰﾊﾞﾙ時間＝引数 [sec]---引数max[12]--12sec
//** 243[(maicrosec]以下の誤差あり
//**
void timer01_interval_sec(unsigned char dt)
{
	unsigned int interval_count;

//ｸﾛｯｸ設定
	PER0 = PER0 | 0x01;		//TAU0EN enable
	TPS0 = 0x33a3;			//CK01 = fclk/10,他はfclk/3
//ﾁｬﾈﾙ01 初期設定
	TMR01 =0x8000;			//ｿﾌﾄｳｪｱﾄﾘｶﾞｰｽﾀｰﾄ,ｲﾝﾀｰﾊﾞﾙﾓｰﾄﾞ
	interval_count = dt % 13;
	interval_count = interval_count * 4883;// * 4883の補正	
	TDR01 = interval_count;		//ﾃﾞｨﾚｲｶｳﾝﾄ値ｾｯﾄ
	TOE0 = 0x00;			//外部出力無し
//割込み許可
	MK1L = MK1L & 0xdf;		//割込みﾏｽｸ TMMK01= 0(許可）
	PR01L = PR01L & 0xdf;		//TMPR001=0
	PR11L = PR11L | 0x20;		//TMPR101=1(割込み優先度ﾚﾍﾞﾙ2)
	EI();				//PSW割込み許可
//動作開始
	TS0 = 0x0002;			//ﾕﾆｯﾄ0,ﾁｬﾈﾙ1 開始(TE01 = 1)
//	TT0 = 0x0002;			//ﾕﾆｯﾄ0,ﾁｬﾈﾙ1 停止(TE01 = 0)
//	PER0 = PER0 & 0xfe;		//TAU0EN disable(省電力時）
	
}
//************************************
// ｲﾝﾀｰﾊﾞﾙﾀｲﾏｰ 	割り込み受信関数
//	iv_counter に　1加算
//	iv_counter > 10000 で 0 にﾘｾｯﾄ
//************************************
#pragma interrupt timer01_interval_irq	(vect=0x2E)
void timer01_interval_irq (void)
{
	iv_counter++;
	if (iv_counter > 10000){
		iv_counter = 0;
	}
}

/* End user code */

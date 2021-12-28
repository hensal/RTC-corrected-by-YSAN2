//**********************************************************************
//*** includes
//***
#include <stdio.h>
#include "iodefine.h"
//
#include "r_cg_macrodriver.h"		//HALT(),EI()を使用するために必要
#include "uart2.h"

//**********************************************************************
//*** function（関数宣言）
//***
extern void 	UART2_initial(void);
extern void 	UART2_txbuf_set(void);
extern void 	UART2_trans_1d(void);
//
unsigned char 	UART2_od_set(void);
void 		tx2_hex2asc3d(unsigned int hex_data,unsigned char buf_point);
unsigned char 	tx2_hex2asc_1d(unsigned char hexd);

//**********************************************************************
//*** bariables（変数宣言）
//***
unsigned char	tx2_po;			//送信中ﾃﾞｰﾀのﾎﾟｲﾝﾀ(ﾀﾞｳﾝｶｳﾝﾀ)
unsigned char	tx2_buf_vol;		//ﾊﾞｯﾌｧｰへｾｯﾄしたデータ数
unsigned char	tx2_trans_flag;		//送信開始ﾌﾗｸﾞ　1で開始
unsigned char	tx2_buf[128];		//送信ﾃﾞｰﾀﾊﾞｯﾌｧｰ
//
//歯抜け補正後測定柱ﾃﾞｰﾀ
unsigned int	s0ih;
unsigned int	s00ih;
unsigned int	s000ih;
unsigned int	s0000ih;


/*******************************************************************************
* Function Name: UART2初期化
* Description  : This function initializes the UART2 module.
* Arguments    : None
* Return Value : None
********************************************************************************/
void UART2_initial(void)
{
	PER0 |= 0x08;		//SAU1EN ON(1)--ｼﾘｱﾙ・ｱﾚｲ・ﾕﾆｯﾄ1 有効
//ｼﾘｱﾙ通信停止
	ST1	|= 0x03;	//UNIT1 CH0&1 STOP--ｼﾘｱﾙ・ﾁｬﾈﾙ停止ﾚｼﾞｽﾀ(SE1ﾚｼﾞｽﾀのON/OFF)
	SPS1	= 0x0046;	//CK10=CK11= fCLK/(2^6)--ｼﾘｱﾙｯｸﾛｯｸ設定78.125kHz
				//CK11= fCLK/(2^4)--ｼﾘｱﾙｯｸﾛｯｸ設定312.5kHz
//送信設定(UNIT1,CH0)
	SMR10	= 0x0022;	//送信CK10SEL,DOWN_EDGE START,UART SEL
	SCR10	= 0x8097;	//送信TX,ｸﾛｯｸ位相,noPARITY,LSB,STOP1,8BIT 
	SDR10	= 0x0600;	//送信ｸﾛｯｸ分周設定(1/8)--39060bps(約38400bps)
//受信設定(UNIT1,CH1)
	NFEN0	|= 0x10;	//SAU_RXD2_FILTER_ON(受信ﾌｨﾙﾀ-有効設定)
	SIR11	= 0x0007;	//受信ｴﾗｰﾌﾗｸﾞｸﾘﾔ
	SMR11	= 0x0122;	//受信CK10SEL,DOWN_EDGE START,UART SEL
//	SMR11	= 0x0162;	//受信CK10SEL,UP_EDGE START,UART SEL
//	SCR11	= 0x4017;	//受信RX,ｸﾛｯｸ位相,noPARITY,MSB,STOP1,8BIT
	SCR11	= 0x4097;	//受信RX,ｸﾛｯｸ位相,noPARITY,LSB,STOP1,8BIT
	SDR11	= 0x0600;	//受信ｸﾛｯｸ分周設定(1/8)
//UNIT1のCH停止解除設定
	SO1 	|= 0x0000;	//SAU1 出力端子ｸﾘﾔ
    	SOL1 	|= 0x0000;	//SAU1_CH0 出力ﾚﾍﾞﾙﾉｰﾏﾙ
    	SOE1	|= 0x0001;	//SAU1_CH0 出力許可
//I/Oポート設定
	PM7	|= 0x40U;	//Set RxD2 pin mode(p7_6 input)
	P7	|= 0x80U;	//Set TxD2 pin data(p7_7 = 1)
	PM7 	&= 0x7FU;	//Set TxD2 pin mode(p7_7 output)
//割込み許可
	MK0H 	= MK0H & 0xfd;		//割込みﾏｽｸ SRMK2= 0(許可）
	PR00H 	= PR00H & 0xfd;		//SRPR02=0
	PR10H 	= PR10H | 0x02;		//SRPR02=1(割込み優先度ﾚﾍﾞﾙ2)
//ｼﾘｱﾙ通信開始
	SS1	|= 0x03;	//SAU1_CH0&1 START--ｼﾘｱﾙ・ﾁｬﾈﾙ開始ﾚｼﾞｽﾀ(SE1ﾚｼﾞｽﾀのON/OFF)

}

//*****************************************************************************
//** 1ﾊﾞｲﾄﾃﾞｰﾀ送信
//** 送信ﾚｼﾞｽﾀが空の場合、1ﾊﾞｲﾄ送信  戻り数＝１（OK）
//** 送信ﾚｼﾞｽﾀが空かないで送信できなった場合　戻り数=0（NG）
//**　
unsigned char UART2_tx_1b(unsigned char tx_data)
{
	unsigned char status;
	unsigned int timeover_count= 1000;	//ﾀｲﾑｵｰﾊﾞｰ1000ｶｳﾝﾄ
	unsigned int ncnt;

	for ( ncnt=0; ncnt<timeover_count; ncnt++){
		status = SSR10;			//ｼﾘｱﾙｽﾃｰﾀｽﾚｼﾞｽﾀ（送信状態読出し）
		if ((status & 0x0020) == 0){	//BFF=0:送信ﾃﾞｰﾀﾚｼﾞｽﾀ（空）?
			SDR10 = tx_data;
			return (0);
		}
	}
	return(1);
}
/************************************************************************/
/* 割り込み受信関数							*/
/************************************************************************/
#pragma interrupt uart2_rx_int	(vect=0x16)
void uart2_rx_int (void)
{
	unsigned char err;

	err = SSR11 & 0x07;			// エラーデータの取り出し
	rx2_id = SDR11;				// 受信データの取り出し
//
	if (err == 0) 	rx2_id_buff[rx2_id_point] = rx2_id;	//受信データをﾊﾞｯﾌｧに保存
	else		rx2_id_buff[rx2_id_point] = err | 0xF0;
//
	rx2_id_point++;					//ﾎﾟｲﾝﾀ++して、20以上の場合は０リセット
	if (rx2_id_point > 19) rx2_id_point = 0;
}
//***************************************************************************




//****************************************************************************
//** 測定柱ﾃﾞｰﾀｾｯﾄ、送信
//** trans_flag = 1の時ﾊﾞｯﾌｧｰにｾｯﾄ
//** trans_flag ﾘｾｯﾄ
//**
void UART2_txbuf_set(void)
{

	if (tx2_trans_flag == 1 && tx2_po == 0){
		tx2_buf_vol = UART2_od_set();	//送信ﾃﾞｰﾀをﾊﾞｯﾌｧにｾｯﾄ
		tx2_po = tx2_buf_vol;
		tx2_po++;
		tx2_trans_flag = 0;
	}
}
//***************************************************************************
//** 測定柱ﾃﾞｰﾀ＞ｱｽｷｰ変換＞ﾊﾞｯﾌｧｰへ保存
//** 戻り値 = 保存ﾃﾞｰﾀ数
//**
unsigned char UART2_od_set(void)
{

	tx2_hex2asc3d(s0ih,0);
	tx2_buf[3] = ',';
	tx2_hex2asc3d(s00ih,4);
	tx2_buf[7] = ',';
	tx2_hex2asc3d(s000ih,8);
	tx2_buf[11] = ',';
	tx2_hex2asc3d(s0000ih,12);
	tx2_buf[15] = 0x0d;	//<CR>
//
	return (16);
}	
//**************************************************************************
//** hex_dataを3バイトのｱｽｷｰｺｰﾄﾞに変換してtx_buf[buf_point]に保存
//**

void tx2_hex2asc3d(unsigned int hex_data,unsigned char buf_point)
{
	unsigned char d1,d2;

	hex_data = hex_data & 0x0fff;
//
	d1 = hex_data / 0x100;
	d2 = hex_data % 0x100;
	tx2_buf[buf_point] = tx2_hex2asc_1d(d1);
	d1 = d2 / 0x10;
	d2 = d2 % 0x10;
	tx2_buf[buf_point+1] = tx2_hex2asc_1d(d1);
	tx2_buf[buf_point+2] = tx2_hex2asc_1d(d2);
}
//**************************************************************************
//** <****8421> を　0～Fまでのｱｽｷｰｺｰﾄﾞに変換
//**

unsigned char tx2_hex2asc_1d(unsigned char hexd)
{
	unsigned char ascd;

	hexd = hexd & 0x0f;
//
	if (hexd > 9) 	ascd = hexd + 0x30;
	else 		ascd = hexd + 0x37;
//
	return(ascd);

} 
//*****************************************************************************
//** ﾊﾞｯﾌｧｰに送信ﾃﾞｰﾀがあり、送信ﾚｼﾞｽﾀが空の場合、1ﾊﾞｲﾄ送信
//** tx_poを-1してから、ﾃﾞｰﾀを送信--tx_poは送信し終わったﾃﾞｰﾀを示している。
//**　
void UART2_trans_1d(void)
{
	unsigned char status;

	if (tx2_po != 0){
		status = SSR10;			//送信状態読出し
		if (status & 0x0020 == 0){	//送信ﾃﾞｰﾀﾚｼﾞｽﾀ（空）?
			tx2_po--;
			SDR10 = tx2_buf[tx2_buf_vol - tx2_po];
		}
	}
}
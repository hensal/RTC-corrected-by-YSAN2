
//**********************************************************************
//*** includes
//***
#include <stdio.h>
#include "iodefine.h"
//
#include "r_cg_macrodriver.h"		//HALT()を使用するために必要
#include "uart3.h"

/*******************************************************************************
* Function Name: UART3初期化
* Description  : This function initializes the UART3 module.
* Arguments    : None
* Return Value : None
********************************************************************************/
void UART3_initial(void)
{
	PER0 |= 0x08;		//SAU1EN ON(1)--ｼﾘｱﾙ・ｱﾚｲ・ﾕﾆｯﾄ1 有効
//ｼﾘｱﾙ通信停止
	ST1	|= 0x03;	//UNIT1 CH2&3 STOP--ｼﾘｱﾙ・ﾁｬﾈﾙ停止ﾚｼﾞｽﾀ(SE1ﾚｼﾞｽﾀのON/OFF)
	SPS1	= 0x0046;	//CK10= fCLK/(2^6)--ｼﾘｱﾙｯｸﾛｯｸ設定78.125kHz
				//CK11= fCLK/(2^4)--ｼﾘｱﾙｯｸﾛｯｸ設定312.5kHz
//送信設定(UNIT1,CH2)
	SMR12	= 0x0022;	//送信CK10SEL,DOWN_EDGE START,UART SEL
	SCR12	= 0x8017;	//送信TX,ｸﾛｯｸ位相,noPARITY,LSB,STOP1,8BIT 
	SDR12	= 0x0600;	//送信ｸﾛｯｸ分周設定(1/8)--9765bps(約9600bps)
//受信設定(UNIT1,CH3)
	NFEN0	|= 0x10;	//SAU_RXD2_FILTER_ON(受信ﾌｨﾙﾀ-有効設定)
	SIR13	= 0x0007;	//受信ｴﾗｰﾌﾗｸﾞｸﾘﾔ
	SMR13	= 0x0122;	//受信CK10SEL,DOWN_EDGE START,UART SEL
	SCR13	= 0x4017;	//受信RX,ｸﾛｯｸ位相,noPARITY,LSB,STOP1,8BIT
	SDR13	= 0x0600;	//受信ｸﾛｯｸ分周設定(1/8)
//UNIT1のCH停止解除設定
	SO1 	|= 0x0000;	//SAU1 出力端子ｸﾘﾔ
    	SOL1 	|= 0x0000;	//SAU1_CH2 出力ﾚﾍﾞﾙﾉｰﾏﾙ
    	SOE1	|= 0x0004;	//SAU1_CH2 出力許可
//I/Oポート設定
	PM14	|= 0x08U;	//Set RxD3 pin mode(p14_3 input)
	P14	|= 0x12U;	//Set TxD3 pin data(p14_4 = 1),RTS=1
	PM14 	&= 0xEFU;	//Set TxD3 pin mode(p14_4 output)
//割込み許可
	MK1H = MK1H & 0xdf;		//割込みﾏｽｸ SRMK3= 0(許可）
	PR01H = PR01H & 0xdf;		//SRPR03=0
	PR11H = PR11H | 0x20;		//SRPR03=1(割込み優先度ﾚﾍﾞﾙ2)
	EI();				//PSW割込み許可
//ｼﾘｱﾙ通信開始
	SS1	|= 0x03;	//SAU1_CH2&3 START--ｼﾘｱﾙ・ﾁｬﾈﾙ開始ﾚｼﾞｽﾀ(SE1ﾚｼﾞｽﾀのON/OFF)

}
//*****************************************************************************
//** 1ﾊﾞｲﾄﾃﾞｰﾀ送信
//** 送信ﾚｼﾞｽﾀが空の場合、1ﾊﾞｲﾄ送信  戻り数＝１（OK）
//** 送信ﾚｼﾞｽﾀが空かないで送信できなった場合　戻り数=0（NG）
//**　
unsigned char UART3_tx_1b(unsigned char tx_data)
{
	unsigned char status;
	unsigned int timeover_count= 1000;	//ﾀｲﾑｵｰﾊﾞｰ1000ｶｳﾝﾄ
	unsigned int ncnt;

	for ( ncnt=0; ncnt<timeover_count; ncnt++){
		status = SSR12;			//ｼﾘｱﾙｽﾃｰﾀｽﾚｼﾞｽﾀ（送信状態読出し）
		if ((status & 0x0020) == 0){	//BFF=0:送信ﾃﾞｰﾀﾚｼﾞｽﾀ（空）?
			SDR12 = tx_data;
			return (0);
		}
	}
	return(1);
}
/************************************************************************/
/* 割り込み受信関数														*/
/************************************************************************/
#pragma interrupt uart3_rx_int	(vect=0x3E)
void uart3_rx_int (void)
{
	unsigned char err;

	err = SSR13 & 0x07;			// エラーデータの取り出し
	rx3_id = SDR13;				// 受信データの取り出し
//	n = uart3_rxfifo.count;
//	if(n < sizeof(uart3_rxfifo.buff)) {
//		uart3_rxfifo.count = ++n;
//		i = uart3_rxfifo.idx_w;
//		uart3_rxfifo.buff[i++] = d;
//		if(i >= sizeof(uart3_rxfifo.buff))
//			i = 0;
//		uart3_rxfifo.idx_w = i;
//	}
	if (err == 0) rx3_id_buff[rx3_id_point] = rx3_id;	//受信データをﾊﾞｯﾌｧに保存
	else	rx3_id_buff[rx3_id_point] = err | 0xF0;
	rx3_id_point++;					//ﾎﾟｲﾝﾀ++して、10以上の場合は０リセット
	if (rx3_id_point > 9) rx3_id_point = 0;
}
//*************************************************************
//***	���v��FRAM�@����
//***
#include <stdio.h>
#include <string.h>
#include "iodefine.h"
#include "rtc_fram.h"
//
//*** �ϐ�

//*******************************************
//** SPI(CSI) ������
//** CSI11(SAU0,����3)�g�p
//**
void csi11_initialize(void)
{
//
	PER0 	|= 0x04;	//SAU0 ON
	SPS0	= 0x003;	//fmck0= fclk/8,fmck1= fclk/1
//	SPS0	= 0x007;	//fmck0= fclk/8,fmck1= fclk/1
	SMR03	= 0x0020;	//fmck0��I��,�]������������
	SCR03	= 0xC007;	//����,MSB̧���,8Bit(����1�j
	SDR03	= 0x0000;	//sck11= fmck0/2
	SO0	&= 0x0F07;	//����M�J�n��������SCK11=1,SO11=0
	SOE0	|= 0x0008;	//SO03�𑗐M����
	PM1	&= 0xFA;	//SCK11&SO11���o�͂ɐݒ�
	PM1	|= 0x02;	//SI011����͐ݒ�
	P1	|= 0x05;	//SCK,SO11��"1"�ݒ�
	SS0	|= 0x0008;	//SCI11���싖��
}
//*********************************************
//** SPI(CSI)11 1�޲đ��M
//** ���M�ް� 	= tr_data
//** �߂萔	= 0:����I���A1:���M�ُ�
//** 
unsigned char csi11_tx1b(unsigned char tr_data)
{
	unsigned int timeover_count= 1000;	//��ѵ��ް1000����
	unsigned int ncnt;

	SDR03 = tr_data;			//1�ް����M
//** ���M�`�F�b�N
	for(ncnt=0; ncnt<timeover_count; ncnt++){
		if ((SSR03 & 0x0040) == 0) return(0);	//TSF check
	}
	return(1);				//���M�ُ�
}
//*********************************************
//** SPI(CSI)11 n�޲đ��M
//** ���M�ް�	= csi_txdata[]
//** ���M�ް���	= len
//** �߂萔	= 0:����I���A1:���M�ُ�
//**
unsigned char csi11_txnb(unsigned char len)
{
	unsigned char  Ret,n;

	for (n=0 ;n<=len ;n++){	
		Ret = csi11_tx1b(csi_txdata[n]);	//1�ް����M
		if (Ret != 0) return(1);		//���M�ُ�H
	}
	return(0);
}
//*********************************************
//** SPI(CSI11) 1�޲Ď�M
//** ��M�ް�	= csi_rxdata
//** �߂萔	= 0:����I���A1:��M�ُ�
//**
unsigned char csi11_rx1b(void)
{
	unsigned int timeover_count= 1000;	//��ѵ��ް1000����
	unsigned int ncnt;
	unsigned char tmp_data;

	SDR03 = 0xFF;				//��а���M
	chk_data = 0;
	for(ncnt=0; ncnt<timeover_count; ncnt++){
		if ((SSR03 & 0x0040) == 0){	//TSF check
			chk_data = 1;
			tmp_data = SDR03;	//�ް��Ǐo��
			csi_rxdata = tmp_data;
			return(0);
		}
	}
	return(1);				//��M�ُ�
}
//***********************************************************
//** RTC�ް��Ǐo��
//** �Ǐo���ް�	= gYear,gMonth,gDay,gHour,gMin,gSec (binary)
//** �߂萔	= 0:����I���A1:�Ǐo���ُ�
//***********************************************************
unsigned char rtc_data_read(void)
{
	unsigned char Ret;
	unsigned char i,n;
	
	P1 = P1 | 0x80;				// RTC CE ON
//
	for (i=0 ;i<150 ;i++) {
		n = n + i;			// 31microSEC�ȏ�@DELAY
	}
//RTC�Ǐo��
	Ret = csi11_tx1b(0xf4);			// address'f'�����ް�ēǏo���w��
	if (Ret != 0){				// 
		P1 = P1 & 0x7F;			// RTC CE OFF
		return(1);			// ��M�ُ�
	}

	for (i=0 ;i<8 ;i++){
		Ret = csi11_rx1b();		// �f�[�^�Ǐo���� csi_rxdata
		if (Ret != 0){  		// 
			P1 = P1 & 0x7F;		// RTC CE OFF
			return(1);		// ��M�ُ�
		}
		chk_buf[i] = csi_rxdata;
	}
	P1 = P1 & 0x7F;				// RTC CE OFF
//���v�ް����i�[
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
//** 10�i���ް�(bcd_data)�@���@16�i���ް�(bc_data)
//** ����	:bcd_data(1�޲�)
//** �߂萔	:bc_data(1�޲āj
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
//** ���v�ް���RTC�ɏ�����
//** �������ް� rtc_write_year   rtc_write_[][][][]	(BCD)
//**  rtc_write_buff[]
//**	+0	+1	+2	+3	+4	+5	+6
//**	sec	min	hour	weekday	day	month	year
//** �߂萔	= 0:����I���A1:�Ǐo���ُ�
//***********************************************************
unsigned char  rtc_data_write(void)
{
	unsigned char Ret;
	unsigned char n,i;

// �������ް����ޯ̧�Ɉړ�	
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
		n += i;				// 31microSEC�ȏ� DELAY
	}
//RTC������
	Ret = csi11_tx1b(0x00);			// address'0'�����ް�ď����ݎw��
	if (Ret != 0){				// 
		P1 = P1 & 0x7F;			// RTC CE OFF
		return(1);			// ��M�ُ�
	}
	for (i=0 ;i<8 ;i++){
		Ret = csi11_tx1b(rtc_write_buff[i]);	// 0�Ԓn���珇�Ԃɏ�����
		if (Ret != 0){			// 
			P1 = P1 & 0x7F;		// RTC CE OFF
			return(1);		// ��M�ُ�
		}
	}
	P1 = P1 & 0x7F;				//RTC CE OFF
//
	return(0);
}
//***********************************************************
//**   RTC ������	 RTC_init(void)
//** RTC(RX-4045)������	: 24���Ԑ�/FOUT�[�qOFF
//** pon,foutڼ޽���ON/OFF(1=ON,0=OFF)
//** �߂�l	: non
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
		csi11_tx1b(0xE0);		// address'E'�����ް�ď����ݎw��
		csi11_tx1b(0x30);		// ����ڼ޽�1(E):12/24=1,CLEN2=1
		csi11_tx1b(0x08);		// ����ڼ޽�2(F):,CLEN1=1
	}
	else {
		csi11_tx1b(0xE0);		// address'E'�����ް�ď����ݎw��
		csi11_tx1b(0x20);		// ����ڼ޽�1(E):12/24=1,CLEN2=0
	}
	P1 = P1 & 0x7F;				// RTC CE OFF
}

//***********************************************************************
//***       RTC ��ܰ�݌��o
//***  RTC(RX-4045)�̓d�������������o����
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
	csi11_tx1b(0xF4);				// address'F'�����ް�ēǏo���w��
	csi11_rx1b();					// �A�h���XF�̃f�[�^�Ǐo��
//
	P1 = P1 & 0x7F;					// RTC CE OFF
//
	if ((csi_rxdata & 0x10) != 0) return(1);	// RTC ��ܰoff�L��Aؾ�ĕK�v 
	return(0);					// ؾ�ĕs�v
}

//***********************************************************************
//***	RTC ���������b���Z�b�g
//***  RTC(RX-4045)ؾ�Ď��Ɏ��v��2010/01/01 00:00:00�ɾ�Ă���
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
	rtc_data_write();		// rtc_wdata[n]��RTC�ɏ�����
}
//**********************************************************************
//***  FRAM1 ������(16�޲ĒP�ʏ�����)
//***  �������ް��F	fram_write_buff[16]
//***  �����ݱ��ڽ�F	 fram_write_address	
//***
unsigned char fram1_write(void)
{
	unsigned char Ret,i,n;
	n=0;
	P0 = P0 & 0xBF;				// FRAM1 CS ON
	csi11_tx1b(0x06);			// WREN(�����݋��j
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
	csi11_tx1b(0x04);			// WREN(�����݋֎~�j
	P0 = P0 | 0x40;				// FRAM1 CS OFF	
//
	return(Ret);
}	
//**********************************************************************
//***  FRAM2 ������(16�޲ĒP�ʏ�����)
//***  �������ް��F	fram_write_buff[16]
//***  �����ݱ��ڽ�F	fram_write_address	
//***
unsigned char fram2_write(void)
{
	unsigned char Ret,i,n;

	n = 0;
	P0 = P0 & 0xDF;				// FRAM2 CS ON
	csi11_tx1b(0x06);			// WREN(�����݋��j
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
	csi11_tx1b(0x04);			// WREN(�����݋֎~�j
	P0 = P0 | 0x20;				// FRAM2 CS OFF	
//
	return(Ret);
}	
//**********************************************************************
//*** FRAM 16�޲ď����݁iCS�A�����݋��E�֎~�@�܂܂��j
//***
unsigned char fram_write(void)
{
	unsigned char Ret,i;
	unsigned adr100,adr10,adr1;
	unsigned long adr_temp;
//
	adr_temp = fram_write_address % 0x1000;	// �擪���ڽ3�޲č쐬
	adr100 = adr_temp /0x100;
	adr_temp = adr_temp % 0x100;
	adr10 = adr_temp /0x10;
	adr1 = adr_temp % 0x10;
//
	csi11_tx1b(0x02);			// �����ݎw��(1�޲�)
	csi11_tx1b(adr100);			// ���ڽ�w��(3�޲�)
	csi11_tx1b(adr10);
	csi11_tx1b(adr1);
//
	for (i=0 ;i<16 ;i++){
		Ret = csi11_tx1b(fram_write_buff[i]);	// 0�Ԓn���珇�Ԃɏ�����
		if (Ret != 0){			// 
			return(1);		// ��M�ُ�
		}
	}
//
	return(0);
}

//**********************************************************************
//***  FRAM1 �Ǐo��(16�޲ĒP�ʏ�����)
//***  �Ǐo���ް��F	fram_read_buff[16]
//***  �Ǐo�����ڽ�F	fram_read_address	
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
//***  FRAM2 �Ǐo��(16�޲ĒP�ʏ�����)
//***  �Ǐo���ް��F	fram_read_buff[16]
//***  �Ǐo�����ڽ�F	fram_read_address	
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
//*** FRAM 16�޲ēǏo���iCS�@�܂܂��j
//***
unsigned char fram_read(void)
{
	unsigned char Ret,i;
	unsigned adr100,adr10,adr1;
	unsigned long adr_temp;

//
	adr_temp = fram_read_address % 0x1000;	// �擪���ڽ3�޲č쐬
	adr100 = adr_temp /0x100;
	adr_temp = adr_temp % 0x100;
	adr10 = adr_temp /0x10;
	adr1 = adr_temp % 0x10;
//
	csi11_tx1b(0x03);			// �Ǐo���w��(1�޲�)
	csi11_tx1b(adr100);			// ���ڽ�w��(3�޲�)
	csi11_tx1b(adr10);
	csi11_tx1b(adr1);
//
	for (i=0 ;i<16 ;i++){
		Ret = csi11_rx1b();		// 0�Ԓn���珇�ԂɓǏo��
		if (Ret != 0){			// 
			return(1);		// ��M�ُ�
		}
		fram_read_buff[i] = csi_rxdata;	//
	}
//
	return(0);
}
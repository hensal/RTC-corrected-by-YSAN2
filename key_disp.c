//*************************************************************
//***	�t���\����@����
//***
#include <stdio.h>
#include <string.h>
#include "iodefine.h"
#include "key_disp.h"
#include "rtc_fram.h"
#include "uart2.h"
#include "uart3.h"

//*** �ϐ�
unsigned char	dsp_buff1[24];
unsigned char	dsp_buff2[24];
unsigned char 	dsp_data1[24];
unsigned char 	dsp_data2[24];
unsigned char	lcd_page;
unsigned char	key_code;
unsigned char	key_cursol;
//
unsigned char	moded;
unsigned char	upd;
unsigned char	downd;
unsigned char	leftd;
unsigned char	rightd;
unsigned char	setd;
//
unsigned char	bmoded;
unsigned char	bupd;
unsigned char	bdownd;
unsigned char	bleftd;
unsigned char	brightd;
unsigned char	bsetd;
//
unsigned char pos;
unsigned char uart_txd;
unsigned char uart_ret;
//

/**************************************************
 �t���\������-Liquid crystal display control 
**************************************************/
void lcd_disp(void)
{
	key_code = key_switch_in();
                               // ����������(�\��Ӱ�ސؑ�-Key input check (display mode switching)
	if (key_code == KEY_MODE) {		//MODE����(-Press) 
		if (lcd_page >= 0x80) 	lcd_page = 0;
		else			lcd_page += 0x10;
		key_code = 0;
	}
//
	switch(lcd_page) {
		//���ѱ��߉�ʕ\��
		case 0x1000:	dsp_warmup(0x00);	break;	// ���(*10) ���ѱ��߉��
//		
		//�ް��\��(*0x)
		case 0x0000:	dsp_main(0x00);		break;	// ���(*00) Ҳ��ް����
		case 0x0010:	dsp_bcdin(0x10);	break;	// ���(*10) BCD���͉��
		case 0x0020:	dsp_rainin(0x20);	break;	// ���(*20) �ώZ�J�ʉ��
		case 0x0030:	dsp_voltage(0x30);	break;	// ���(*30) DC�d��
		case 0x0040:	dsp_clock(0x40);	break;	// ���(*40) ���v�ݒ���
		case 0x0041:	dsp_clock_adjust(0x41);	break;	// ���(*41) ���v�ݒ���
		case 0x0050:	dsp_gps(0x50);		break;	// ���(*50) GPS���͉��
		case 0x0060:	dsp_usb(0x60);		break;	// ���(*60) USB�o�͉��
		case 0x0070:	dsp_fram(0x70);		break;	// ���(*70) FRAM���
		case 0x0080:	dsp_sdc(0x80);		break;	// ���(*80) SD�J�[�h���
//
		default:	lcd_page = 0;		break;
	}
}
//****************************************
//***	�E�H�[���A�b�v�\��
//***
void dsp_warmup(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "TMC-4000 Warm Up    ";
	unsigned char titl_l2[24] = "                    ";	
//
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
	dsp_buff_disp();
//���ă_�E��
	for (n=60; n>=2; n--){
		lcdLocate(1,15);
		pageno_disp(n);
		delay_msec(500);
	}
	
}
/******************************************
 *00 Ҳ݉�ʕ\��
   page_num = ���No
******************************************/
void dsp_main(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "TMC-4000 Logger  *  ";
	unsigned char titl_l2[24] = "                    ";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//�y�[�W�ԍ�
	bcd2d_disp(1,18,page_num);
//
	bcd2d_disp(2,11,rtc_read_hour);		//hour
	bcd2d_disp(2,14,rtc_read_min);		//minuts
	bcd2d_disp(2,17,rtc_read_sec);		//second
//
	dsp_buff_disp();
}

/******************************************
 *01 BCD���͉�ʕ\��
******************************************/
void dsp_bcdin(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "BCD INPUT DATA   *  ";
	unsigned char titl_l2[24] = "1+00000 0  2+00000 0";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(1,18,page_num);
//CH1�\��
	if ((ch1_bcd[2] & 0x10) != 0) dsp_buff2[1] = '-';
	else dsp_buff2[1] = '+';
	dsp_buff2[2] = ch1_bcd[2] | 0x30;	//10000
	bcd2d_disp(2,3,ch1_bcd[1]);		//1000,100
	bcd2d_disp(2,5,ch1_bcd[0]);		//10,1
	if (ch1_bcd[3] > 9) dsp_buff2[8] = ch1_bcd[3] + 0x37;
	else dsp_buff2[8] = ch1_bcd[3] + 0x30;
//
//CH2�\��
	if ((ch2_bcd[2] & 0x10) != 0) dsp_buff2[12] = '-';
	else dsp_buff2[12] = '+';
	dsp_buff2[13] = ch2_bcd[2] | 0x30;	//10000
	bcd2d_disp(2,14,ch2_bcd[1]);		//1000,100
	bcd2d_disp(2,16,ch2_bcd[0]);		//10,1
	if (ch2_bcd[3] > 9) dsp_buff2[8] = ch2_bcd[3] + 0x37;
	else dsp_buff2[19] = ch2_bcd[3] + 0x30;
//	
	dsp_buff_disp();
}

/******************************************
 *02 �ώZ�J�ʉ�ʕ\��
******************************************/
void dsp_rainin(unsigned char page_num)
{
	unsigned char n,d1,d2;
	unsigned int dint;
	unsigned char titl_l1[24] = "RAIN INPUT DATA  *  ";
	unsigned char titl_l2[24] = "     9999           ";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(1,18,page_num);
//�ώZ�l
	d1 = r_counter / 1000;
	dint = r_counter % 1000;
	d1 = d1*0x10 + dint / 100;
	d2 = dint / 10;
	d2 = d2*0x10 + dint % 10;
	bcd2d_disp(2,5,d1);
	bcd2d_disp(2,7,d2);	
//
	dsp_buff_disp();
}
/******************************************
 *03 DC�d����ʕ\��
******************************************/
void dsp_voltage(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "DC12V VOLTAGE    *  ";
	unsigned char titl_l2[24] = "      11.5 V        ";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//�y�[�W�ԍ�	
	bcd2d_disp(1,18,page_num);

//vdd_bc:Vdd�d��_BC[0.1V]
	dsp_buff2[6] = (vdd_bc / 100) + 0x30;
	n = vdd_bc % 100;
	dsp_buff2[7] = (n / 10) + 0x30;
	dsp_buff2[9] = (n % 10) + 0x30;

//
	dsp_buff_disp();
}
/******************************************
* 40 ���v�\�����
******************************************/
void dsp_clock(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "TIME & DATE      *  ";
	unsigned char titl_l2[24] = "2021/12/25 11:30:45 ";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(2,2,rtc_read_year);		//year��2�s�ڂ�2�����ڂɕ\��
	bcd2d_disp(2,5,rtc_read_month);		//month
	bcd2d_disp(2,8,rtc_read_day);		//day
	bcd2d_disp(2,11,rtc_read_hour);		//hour
	bcd2d_disp(2,14,rtc_read_min);		//minuts
	bcd2d_disp(2,17,rtc_read_sec);		//second
//
	bcd2d_disp(1,18,page_num);
//
	if (key_code == KEY_SET){
		rtc_write_year = rtc_read_year;
		rtc_write_month = rtc_read_month;
		rtc_write_day = rtc_read_day;
		rtc_write_hour = rtc_read_hour;
		rtc_write_min = rtc_read_min;
		rtc_write_sec = rtc_read_sec;
		lcd_cout(0x0F);			//����ON
		key_cursol = 18;		//���وʒu�ݒ�
		lcd_cout(0xc0 + key_cursol);	//���وʒu��LCD2�s�ڏ�����
		lcd_page = 0x0041;
//
		key_code = 0;			//�����͸��
	}
//
	dsp_buff_disp();
}
/******************************************
 *41 ���v�Z����ʕ\��
******************************************/
void dsp_clock_adjust(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "CLOCK ADUST      *  ";
	unsigned char titl_l2[24] = "2021/12/25 11:30:45 ";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(1,18,page_num);
//RTC�����ݗp�ް��\��
	bcd2d_disp(2,2,rtc_write_year);		//year��2�s�ڂ�2�����ڂɕ\��
	bcd2d_disp(2,5,rtc_write_month);	//month
	bcd2d_disp(2,8,rtc_write_day);		//day
	bcd2d_disp(2,11,rtc_write_hour);	//hour
	bcd2d_disp(2,14,rtc_write_min);		//minuts
	bcd2d_disp(2,17,rtc_write_sec);		//second
//
	dsp_buff_disp();
//
	lcdLocate(1,key_cursol);	//���وʒu��LCD2�s�ڏ�����
	lcd_cout(0x0E);			//����ON

//���ݽ����ɂ���ް��ύX
// ���{�^��
	if (key_code == KEY_LEFT){
		if (key_cursol <= 3) key_cursol = 18;
		else key_cursol = key_cursol - 3;
		lcd_cout(0xc0 + key_cursol);	//���وʒu��LCD2�s�ڏ�����
		key_code = 0;
	}
//  ���{�^��
	if (key_code == KEY_RIGHT){
		if (key_cursol >= 18) key_cursol = 3;
		else key_cursol = key_cursol + 3;
		lcd_cout(0xc0 + key_cursol);	//���وʒu��LCD2�s�ڏ�����
		key_code = 0;
	}
// UP�{�^��
	if (key_code == KEY_UP){
		switch( key_cursol) {
			case 3:	 rtc_write_year++;	break;	// �N+1
			case 6:  rtc_write_month++;	break;	//
			case 9:  rtc_write_day++;	break;	//
			case 12: rtc_write_hour++;	break;	//
			case 15: rtc_write_min++;	break;	//
			case 18: rtc_write_sec++;	break;	//
			default: key_cursol = 18;	break;	//
		}

		key_code = 0;
	}
// DOWN�{�^��
	if (key_code == KEY_DOWN){
		
		switch( key_cursol) {
			case 3:	 rtc_write_year--;	break;	// �N+1
			case 6:  rtc_write_month--;	break;	//
			case 9:  rtc_write_day--;	break;	//
			case 12: rtc_write_hour--;	break;	//
			case 15: rtc_write_min--;	break;	//
			case 18: rtc_write_sec--;	break;	//
			default: key_cursol = 15;	break;	//
		}

		key_code = 0;
	}
//���v�ް��␳
	if (rtc_write_year > 0x99) rtc_write_year = 0;
	if (rtc_write_month > 0x12) rtc_write_month = 0;
	if (rtc_write_month == 0x02){
		if(((rtc_write_year*10/0x10 + rtc_write_year%0x10) % 4) == 0){
			if(rtc_write_day > 0x29) rtc_write_day = 0;
		}
		else{
			if(rtc_write_day > 0x28) rtc_write_day = 0;
		}
	}
	else if ((rtc_write_month ==4) ||(rtc_write_month ==6) ||(rtc_write_month ==9) ||(rtc_write_month ==0x11)){
		if(rtc_write_day > 0x30) rtc_write_day = 0;
	}
	else {
		if(rtc_write_day > 0x31) rtc_write_day = 0;
	}
	if (rtc_write_hour > 0x23) rtc_write_hour = 0;
	if (rtc_write_min > 0x59) rtc_write_min = 0;
	if (rtc_write_sec > 0x59) rtc_write_sec = 0;

//[SET]�{�^���`�F�b�N
	if (key_code == KEY_SET){
		rtc_data_write();		//write to RTC
		lcd_cout(0x0C);			//����OFF
		lcd_page = 0x0040;
		key_code = 0;
	}
}


/******************************************
 *05 GPS���̓f�[�^�\��
******************************************/
void dsp_gps(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "GPS              *  ";
	unsigned char titl_l2[24] = "00,00,00,11,30,45.00";
//���ٕ\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//�߰�ޔԍ�
	bcd2d_disp(1,18,page_num);
//RX2�ް��\��
	for (n=0; n<20; n++){
		dsp_buff2[n] = rx2_id_buff[n];
	}
//[SET]��������
	if (key_code == KEY_UP) gps_power_flag = 1;
	if (key_code == KEY_DOWN) gps_power_flag = 0;
	if (gps_power_flag == 0){
		dsp_buff1[4] = 'O';
		dsp_buff1[5] = 'F';
		dsp_buff1[6] = 'F';
		P7 |= 0x10;		//GPS OFF(Hi OFF)
	}
	else {
		dsp_buff1[4] = 'O';
		dsp_buff1[5] = 'N';
		dsp_buff1[6] = ' ';
		P7 &= 0xEF;		//GPS ON(Lo ON)
	}
//
	dsp_buff_disp();
}

/******************************************
 *06 USB�o�͐����ʕ\��
******************************************/
void dsp_usb(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "USB              *  ";
	unsigned char titl_l2[24] = "TR=[SET]            ";
	unsigned char tx_buff[36] = "2021/12/31 12:30:00 +0000 111.5     ";

//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(1,18,page_num);
//RX3�ް��\��
	for (n=0; n<10; n++){
			dsp_buff2[n+10] = rx3_id_buff[n];
	}
//
//	dsp_buff2[15] = rx3_id;			//RX3�����ް��i�����ŐV�j
//
	dsp_buff_disp();
//[SET]�{�^���`�F�b�N
	if (key_code == KEY_SET){
		tx_buff[32] = 0x0d;	//<CR>
		tx_buff[33] = 0x0a;	//<LF>
		for (n=0; n<34; n++){
			uart_txd = tx_buff[n];
			uart_ret = UART3_tx_1b(uart_txd);
			if (uart_ret != 0) n--;
		}
		key_code = 0;
	}
}
/******************************************
 *07 FRAM��ʕ\��
******************************************/
void dsp_fram(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "MEMORY 21/12/15  *  ";
	unsigned char titl_l2[24] = "11:30 +0123.45 012.5";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(1,18,page_num);
//
	dsp_buff_disp();
}
/******************************************
 *08 SD�J�[�h��ʕ\��
******************************************/
void dsp_sdc(unsigned char page_num)
{
	unsigned char n;
	unsigned char titl_l1[24] = "SDCard 21/12/15  *  ";
	unsigned char titl_l2[24] = "11:30 +0123.45 012.5";
//�^�C�g���\��
	for (n=0 ;n<20 ;n++){
		dsp_buff1[n] = titl_l1[n];
		dsp_buff2[n] = titl_l2[n];
	}
//
	bcd2d_disp(1,18,page_num);
//
	dsp_buff_disp();

}

//*********************************************************
//**  ����X�C�b�`�@����
//**
unsigned char key_switch_in(void)
{
	unsigned char keyid1,keyid2,keyid3;
//������
	keyid1	= P1;			//<3>MODE,<4>UP,<5>DOWN
	keyid2	= P5;			//<4>LEFT,<5>RIGHT
	keyid3	= P10;			//<0>SET
	moded 	= keyid1 & 0x08;
	upd   	= keyid1 & 0x10;
	downd 	= keyid1 & 0x20;
	leftd 	= keyid2 & 0x10;
	rightd 	= keyid2 & 0x20;
	setd	= keyid3 & 0x01;
//�����ލ쐬
	key_code = 0;
	if (moded == 0 && bmoded != 0) {
		key_code = KEY_MODE;
	}
	bmoded = moded;
	if (upd == 0 && bupd != 0) {
		key_code = KEY_UP;
	}
	bupd  = upd;
	if (downd == 0 && bdownd != 0) {
		key_code = KEY_DOWN;
	}
	bdownd  = downd;
	if (leftd == 0 && bleftd != 0) {
		key_code = KEY_LEFT;
	}
	bleftd  = leftd;
	if (rightd == 0 && brightd != 0) {
		key_code = KEY_RIGHT;
	}
	brightd = rightd;
	if (setd == 0 && bsetd != 0) {
		key_code = KEY_SET;
	}
	bsetd  = setd;

	return(key_code);
}
//*******************************************************
//***	��ʔԍ��i2�������j�\��
//***  1�s�ډE���dsp_buff1[]���ް����Z�b�g
//***
void	pageno_disp(unsigned char page_num)
{
	unsigned char	d1,d2;
	
	d1 = page_num / 0x10;	
	d2 = page_num % 0x10;
	dsp_buff1[18] = d1 + 0x30;
	dsp_buff1[19] = d2 + 0x30;
}
//*******************************************************
//***	2������ �\��
//***  dsp_buff1[]���ް����Z�b�g
//***
void	bcd2d_disp(unsigned char line,unsigned char pos,unsigned char bcd_2d)
{
	unsigned char	d1,d2;
	
	d1 = bcd_2d / 0x10;	
	d2 = bcd_2d % 0x10;
	if (line == 1){
		dsp_buff1[pos] = d1 + 0x30;
		dsp_buff1[pos+1] = d2 + 0x30;
	}
	else{
		dsp_buff2[pos] = d1 + 0x30;
		dsp_buff2[pos+1] = d2 + 0x30;
	}
}//*******************************************************
// 1�������M�֐�(�߰�ݸޔ�)											//
//char lcd_putc(char s)
//{
//	lcd_dout(s);
//	return(0);
//}

//*****************************************
void dsp_buff_disp(void)
{
	unsigned char n;
	unsigned char dsp_1b;
	unsigned char l1_write_flag = 0;	//1�s�ڕω��L���t���O
	unsigned char l2_write_flag = 0;	//2�s�ڕω��L���t���O
	
//�\�����e�ω��L���`�F�b�N
	for (n=0 ;n<20 ;n++){
		if (dsp_buff1[n] != dsp_data1[n] ) l1_write_flag = 1;
		if (dsp_buff2[n] != dsp_data2[n] ) l2_write_flag = 1;
		 dsp_data1[n] = dsp_buff1[n];
		 dsp_data2[n] = dsp_buff2[n];
	}		
//�ω��L�����Ƃ�1�s�ڕ\��
	if (l1_write_flag == 1){
		lcd_l1(0x00);				//1�s�ڐ擪�A�h���X�Z�b�g
		for (n=0 ;n<20 ;n++) {
			dsp_1b = dsp_buff1[n];		//line 1
			lcd_dout(dsp_1b);		//1data write
		}
	}
//�ω��L�����Ƃ�2�s�ڕ\��
	if (l2_write_flag == 1){
		lcd_l2(0x00);				//2�s�ڐ擪�A�h���X�Z�b�g
		for (n=0 ;n<20 ;n++) {
			dsp_1b = dsp_buff2[n];		//line 2
			lcd_dout(dsp_1b);		//1data write
		}
	}
}
//********************************************************
//**�@LCD������(STKW-186)
//
void lcd_init(void)
{
	P3 |=0x02;		//5V ON
	P4 |=0x20;		//�ޯ�ײ�ON
	delay_msec(100);
//
	P4 &= 0xE7;		//E=0,RS=0
	P5 = 0x03;
//	
	P4 |= 0x10;		//E=1
	delay_micro(100);
	P4 &= 0xEF;		//E=0
	delay_msec(5);
//
	P4 |= 0x10;		//E=1
	delay_micro(100);
	P4 &= 0xEF;		//E=0
	delay_msec(5);
//
	P4 |= 0x10;		//E=1
	delay_micro(100);
	P4 &= 0xEF;		//E=0
	delay_msec(5);
//
	P5 = 0x02;		//4bit�ް����ݒ�
	P4 |= 0x10;		//E=1
	delay_micro(100);
	P4 &= 0xEF;		//E=0
	delay_msec(5);
//** ��������4�ޯ��ް���
//	lcd_cout(0x08);		//�\����	
//	delay_msec(5);
//	lcd_cout(0x01);		//�\����԰	
//	delay_msec(5);
//	lcd_cout(0x06);		//���ذӰ�޾��	
//	delay_msec(5);
//	lcd_cout(0x0E);		//�\��on�A����on
//	delay_msec(5);
//
	lcd_cout(0x02);		                          //for 4-bit lcd intialization
	delay_msec(5);
	lcd_cout(0x28);		
	delay_msec(5);
	lcd_cout(0x0C);                                    
	delay_msec(5);    
	lcd_cout(0x06);                                       //auto-increment
	delay_msec(5); 
	lcd_cout(0x01);
	delay_msec(5); 		
}

//*****************************************
//** ���وʒu�ݒ�
//** line 	0 :1�s�ځA0�ȊO�F2�s��
//** column	:��(0�͍��[)
//
void lcdLocate(unsigned char line,unsigned char column)
{
	if (line == 0) 	lcd_l1(column);
	else 		lcd_l2(column);
}

//*****************************************
//** 1�s�ڱ��ڽ���
//** column(��)
//
void lcd_l1(unsigned char column)
{
//	unsigned char pos;

	pos = column & 0x3F;	//��̍ő�l��64
	pos = pos | 0x80;	//1�s���ޯĒǉ�
	lcd_cout(pos);
}
//*****************************************
//** 2�s�ڱ��ڽ���
//
void lcd_l2(unsigned char column)
{
//	unsigned char pos;

	pos = column & 0x3F;	//��̍ő�l��64
	pos = pos | 0xC0;	//2�s���ޯĒǉ�
	lcd_cout(pos);
}
//*****************************************
//** ��׸�LCD�@���۰ٺ��ޏo��(STKW-186)
//
void lcd_cout(unsigned char code)
{
	unsigned char code_msb,code_lsb;

	code_msb = code / 0x10;
	code_lsb = code & 0x0F;
	P4 &= 0x0E7;		//E=0,RS=0
//
	P5 = code_msb;
	P4 |= 0x10;		//E=1
	delay_micro(10);
	P4 &= 0xEF;		//E=0
//
	delay_micro(10);
//
	P5 = code_lsb;
	P4 |= 0x10;		//E=1
	delay_micro(10);
	P4 &= 0xEF;		//E=0
	delay_micro(100);
//
}
//****************************************
//** ��׸�LCD�@�ް����ޏo��(STKW-186)
//
void lcd_dout(unsigned char code)
{
	unsigned char code_msb,code_lsb;

	code_msb = code / 0x10;
	code_lsb = code & 0x0F;
	P4 &= 0xEF;		//E=0
	P4 |= 0x08;		//RS=1
//
	P5 = code_msb;
	P4 |= 0x10;		//E=1
	delay_micro(10);
	P4 &= 0xEF;		//E=0
//
	delay_micro(10);
//
	P5 = code_lsb;
	P4 |= 0x10;		//E=1
	delay_micro(10);
	P4 &= 0xEF;		//E=0
	delay_micro(100);
//
}
//*****************************************
//** ϲ�ەb�x��
void delay_micro(unsigned int cnt)
{
	unsigned int del_cnt;
	unsigned char dcnt,dm;

	for (del_cnt=0;del_cnt<cnt;del_cnt++){
		for (dcnt=0; dcnt<6; dcnt++) dm=dm+2;
//			NOP();
//			NOP();
//			NOP();
//			NOP();
//			NOP();
	}
}
//******************************************
//** �ؕb�x��
void delay_msec(unsigned int icnt)
{
	unsigned int del_cnt;

	for (del_cnt=0;del_cnt<icnt;del_cnt++){
//		wdt_rst();
		delay_micro(1000);
	}
}

//*******************************************
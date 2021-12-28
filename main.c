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
#include "led_switech.h"		//�ި��߽��������ALED����
#include "uart2.h"			//uart2�֐�
#include "ad19.h"			//Vdd����pAD�R������֐�
#include "r_cg_macrodriver.h"		//HALT()���g�p���邽�߂ɕK�v
#include "key_disp.h"			//�������ALCD����
#include "rtc_fram.h"			//���v�AEEPROM����
#include "uart3.h"			//uart3(USB)
//**********************************************************************
//*** function�i�֐��錾�j
//***
void 		main(void);
void		IO_Init(void);			//IO�߰ā@������
void 		clk_HOCOto_X1(void);		//HOCO�N���b�N��X1�U���q�N���b�N�֕ύX
//void 		suii_level_adjust(void);	//suii_bcd + level_settei 
//unsigned int 	bcd_to_bc(unsigned int bcd_dat);//BCD4����BC�ɕϊ�
//unsigned int 	bc_to_bcd(unsigned int bc_dat);	//BC(0~9999)��BCD�ɕϊ�
void 		timer01_interval_sec(unsigned char dt);//���������ϰunit0ch1�̏����ݒ�
void 		timer01_interval_irq (void);
//BCD����
void 		bcd_ch1in(void);	//CH1 BCD���́A�ޯĔz�u�ϊ�
void 		bcd_ch2in(void);	//CH2 BCD���́A�ޯĔz�u�ϊ�
void 		bcd_convert(void);
void 		bcd4byte_in(void);


//**********************************************************************
//*** define�@(������`�j
//***            1234567890123456789
#define VER	"SLP10-VT_SIO V1.00 " //�ő�26Byte�Ŏw��̂���

//**********************************************************************
//*** bariables�i�ϐ��錾�j
//***
//*  �ݒ轲��
unsigned char	mode_settei;		//���M����ؑ֐ݒ�	
unsigned char	frequency_settei;	//���M���g���`�����l��(7�`46CH)
unsigned char	length_settei;		//���蒌�S��
unsigned int	level_settei;		//���ٕ␳�l
unsigned char 	sw_id[16];		//switch read data

//*  LED & ETC
unsigned char	tx1_flag;		//���M���׸�
unsigned char	rtx_flag;		//�Ӻݑ��M���׸�
unsigned char	rrx_flag;		//�Ӻݎ�M���׸�
unsigned char	sng_flag;		//�ݻ��ُ��׃O-abnormal flag�
unsigned char	bng_flag;		//�d���ُ��׸�
unsigned char	sleep_flag;		//�ذ�ߊJ�n�׸�
unsigned int 	led_count;

//*  UART1(MU-3) & 2(�O���ر�)
unsigned char	ID[16];			//���͐��f�[�^
unsigned int	hbcd;			//����
unsigned int	hbc;
unsigned char	herr;
unsigned char	uart1_buf[32];		//�o�̓o�b�t�@�[
unsigned char	uart1_buf_cnt;		//�o�̓J�E���^�[
unsigned char	uart2_buf[32];		//�o�̓o�b�t�@�[
unsigned char	uart2_buf_cnt;		//�o�̓J�E���^�[

//AD
unsigned char	ad_data;		//AD���͒l(16�i)
unsigned int	vdd_bc;			//Vdd�d��_BC[0.1V]
unsigned int	vdd_bcd;		//Vdd�d��BCD[0.1V]
//
unsigned int	iv_counter;		//������ٔ����v����0�`9999
//�`�F�b�N�p
unsigned char 	chk_byt;
unsigned int 	chk_word;

//** RTC�p�ϐ�
//** ���v�Ǐo����������
unsigned int loop_counter;
//
unsigned char 	csi_txdata[128];
unsigned char 	csi_rxdata;
unsigned char	chk_data;
unsigned char	chk_buf[16];
unsigned char   rtc_write_buffata[8];	//RTC�����ݗp�ޯ̧(BCD)
//** RTC�p�ϐ�
unsigned char rtc_read_year;		//RTC�Ǐo���ް�
unsigned char rtc_read_month;
unsigned char rtc_read_day;
unsigned char rtc_read_hour;
unsigned char rtc_read_min;
unsigned char rtc_read_sec;
//
unsigned char rtc_write_year;		//RTC�����ݗp�ް�
unsigned char rtc_write_month;
unsigned char rtc_write_day;
unsigned char rtc_write_hour;
unsigned char rtc_write_min;
unsigned char rtc_write_sec;
unsigned char rtc_write_buff[8];	//RTC�������ޯ̧
unsigned char rtc_write_flag;		//RTC�������׸�(1�F������)
//** FRAM�p�ϐ�
unsigned char fram_read_buff[16];	//FRAM�Ǐo���ޯ̧(16�޲�)
unsigned char fram_write_buff[16];	//FRAM�������ޯ̧(16�޲�)
unsigned long fram_read_address;      //FRAM�Ǐo���擪���ڽ
unsigned long fram_write_address;	//FRAM�����ݐ擪���ڽ
unsigned char fram_read_flag;		//FRAM�Ǐo���׸�(1:�Ǐo��)
unsigned char fram_write_flag;		//FRAM�������׸�(1:������)//
//�J��
unsigned char	rid;			//�J����ٽ���͒lP6_4
unsigned char	brid;
unsigned int r_counter;			//�J����ٽ�ώZ�l9999mm
//
unsigned char 	bcd_id[4];		//�����ް�
unsigned char 	bcd_data[4];		//BCD�ް�
unsigned char	ch1_id[4];		//CH1�����ް�
unsigned char	ch1_bcd[4];		//CH1BCD�ް�
unsigned char	ch2_id[4];		//CH2�����ް�
unsigned char	ch2_bcd[4];		//CH2BCD�ް�
//UART2(GPS)
unsigned char	rx2_id_buff[32]="-- -- -- -- -- -- -- -";	//RX2�����ޯ̧
unsigned char	rx2_id_point = 0;	//RX2�����ޯ̧�߲��
unsigned char	rx2_id;			//RX2�����ް��i�����ŐV�j
unsigned char	gps_power_flag = 0;	//0=OFF,1=ON
//UART3(USB)
unsigned char	rx3_id_buff[16]="------------";	//RX3�����ޯ̧
unsigned char	rx3_id_point = 0;	//RX3�����ޯ̧�߲��
unsigned char	rx3_id;			//RX3�����ް��i�����ŐV�j
//************************************************************************
//* Function Name: main
//* Description  : This function implements main function.�i���C���֐��j
//* Arguments    : None
//* Return Value : None
//*************************************************************************
void main(void)
{
    	IO_Init();			//IO������

//Main clock �ݒ�    
	clk_HOCOto_X1();		//CPU&IO�ۯ���X1�ɕύX
//	HOCODIV = 0;			//HOCO  32MHz
//	HOCODIV = 0x01;			//HOCO  16MHz
//	HOCODIV = 0x02;			//HOCO  8MHz
//	HOCODIV = 0x05;			//HOCO  1MHz
//	CKC = CKC & 0xef;		//HOCO��Ҳݸۯ��ɐؑ�
//SPI ������(RTC,FRAM)
	csi11_initialize();		//SPI������(RTC,FRAM)
	rtc_init(1);			//RTC=24����Ӱ�ޤ���g���o��OFF
//
	if ((P10 & 0x01) == 0){		//[SET] ON ?
		rtc_write_year = 0x22;	//RTC�����݂��ް�
		rtc_write_month = 0x01;
		rtc_write_day = 0x01;
		rtc_write_hour = 0x00;
		rtc_write_min = 0x00;
		rtc_write_sec = 0x00;
		rtc_data_write();
	}
//LCD ������
	lcd_init();
	lcd_disp();
	
//UART ������	
	UART2_initial();
	UART3_initial();

//A/D �������@AD�^255���iVdd�^6.1)�^3.3�@=>�@Vdd��0.79 * AD�@
	ad19_init();				//AD19 ������
	ad_data = ad19_input();			//Vdd�f�[�^����
//** ���������ϰ�ݒ�
	timer01_interval_sec(1);		//1�b���������ϰ
//** �J����ٽ�ؑփ����[ON
	P6 |= 0x40;				//ׯ��ڰON�o�;��
//
	P0 &= 0xDF;		//FRAM2 CS Enable
	csi11_tx1b(0x03);	//READ �����
	csi11_tx1b(0x01);	//���ڽ1FFFF(�Ō���̃A�h���X)
	csi11_tx1b(0xFF);
	csi11_tx1b(0xFF);
	csi11_rx1b();		//�����̉��ꌅ��1FFFF�Ԓn�ɓǏo����
	P0 = P0 | 0x20;		//FRAM2 CS disable
	r_counter =csi_rxdata;
//
//**********************************************************************
//***		���C�����[�v
//**********************************************************************
    	while (1U)
	{
//�ݻ��ް�����
//�\������
		lcd_disp();		
//UART2 �o�͐���i�O���رُo�́j
//		UART2_txbuf_set();		//tx2_trans_flag = 1�̎��A���M�ޯ̧�փf�[�^�i�[
//		UART2_trans_1d();		//���M�ޯ̧���ް����L��Atx2 ���Mڼ޽���̂Ƃ��A1byte�o��
//�d���d��Vdd����
		if (loop_counter == 0) {
			ad_data = ad19_input();		//Vdd�f�[�^����
			vdd_bc = ad_data * 80;			
			vdd_bc = (vdd_bc + 50) / 100;	//0.1V�ȉ��l�̌ܓ�
//		vdd_bcd = bc_to_bcd(vdd_bc);	//̫�ϯ�="00.0" V		
		}
//���v���́�LED�_��
		if (loop_counter >= 3) {
			P6 &= 0xFE;	//LED1 ON
		}
		if (loop_counter >= 4){
			P6 |= 0x01;		//LED1(����)OFF
			loop_counter = 0;
			rtc_data_read();	//���v���́irtc_sec�`rtc_year)
			P4 &= 0xDF;		//LCD�ޯ�ײ� OFF
			P6 &= 0xBF;		//ׯ��ڰON�o��ؾ��
		}
		else	loop_counter++;
//BCD����
		if (loop_counter == 2) bcd_ch1in();	//BCD1����
		if (loop_counter == 3) bcd_ch2in();	//BCD2����
		
//�J�ʓ���
		rid = P6 & 0x20;		//����ٽIN
		if (rid == 0 && brid != 0){
			r_counter++;
//�J�ʶ���������
			P0 &= 0xDF;		//FRAM2 CS Enable
			chk_byt = csi11_tx1b(0x06);	//�����݋����b�`
			P0 |= 0x20;		//FRAM2 CS Disable
//
			P0 &= 0xDF;		//FRAM2 CS Enable
			csi11_tx1b(0x02);	//WRITE �����
			csi11_tx1b(0x01);	//���ڽ1FFFF(�Ō���̃A�h���X)
			csi11_tx1b(0xFF);
			csi11_tx1b(0xFF);
			chk_data = r_counter % 0x100;
			csi11_tx1b(chk_data);	//�����̉��ꌅ��1FFFF�Ԓn�ɏ�����
			P0 |= 0x20;		//FRAM2 CS Disable
//
			P0 &= 0xDF;		//FRAM2 CS Enable
			chk_byt = csi11_tx1b(0x04);	//�����݋֎~���b�`
			P0 |= 0x20;		//FRAM2 CS Disable
//			
		}
		brid = rid;

//�ذ�ߐ���
		WDTE = 0xAC;			//WDT ���
//		P6 = P6 | 0x01;			//LED0<0>=1 OFF
//		P4 &= 0xDF;			//LCD�ޯ�ײ� OFF
//		ADM0 = ADM0 & 0x7e;		//AD�ϊ���~(ADCS,ADCE=0)
//		PER0 = PER0 & 0xdf;		//ADCEN(PER<5>)��~(0=�ۯ���~)
//		PER0 = PER0 & 0xf3;		//SAU0EN,SAU1EN��~(0=�ۯ���~)
//		PER0 = PER0 & 0xf0;		//SAU0EN,SAU1EN,TAU0EN,TAU1EN��~(0=�ۯ���~)
//		HALT();
//		P3 = 0x01;			//�ݻ��d�� ON
		ad19_init();			//AD19 ������
//		UART1_initial();		//UART1 ������(MU)
//		UART2_initial();		

//UART2 ������(AUX)
		NOP();
		NOP();
//		P6 = P6 & 0xfe;			//LED0<0>=0 ON
//�����ޯ����ϰ����
		WDTE = 0xAC;			//WDT ���

	}    
}
//***********************************************************************
//**	CH1 BCD ����
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
//**	CH2 BCD ����
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
	for (i=0; i<4;i++){		//�ް��ړ�
		ch2_id[i] = bcd_id[i];
		ch2_bcd[i] = bcd_data[i];
	}
//
}
//**********************************************************	
//�ޯĔz�u�ϊ�
void bcd_convert(void)
{
	unsigned char bcd1,bcd2;
	unsigned int iwork;
	
	bcd1 = bcd_id[0] ^ 0xFF;	//���]
	bcd1 = bcd1 >> 1;		//�E�V�t�g�P
	bcd1 &= 0x0F;
	iwork = bcd_id[0] + bcd_id[1] * 0x100;
	iwork = iwork >> 2;		//�E�V�t�g2
	bcd2 = iwork % 0x100;
	bcd2 = bcd2 ^ 0xFF;		//���]
	bcd2 &= 0xF0;
	bcd_data[0] = bcd1 + bcd2;	//10�ʁA1��<8421 8421>
//	
	bcd1 = bcd_id[1] ^ 0xFF;	//���]
	bcd1 = bcd1 >> 3;		//�E�V�t�g3
	bcd1 &= 0x0F;
	bcd2 = bcd_id[2];
	bcd2 = bcd2 << 4;		//���V�t�g4
	bcd2 = bcd2 ^ 0xFF;		//���]
	bcd2 &= 0xF0;
	bcd_data[1] = bcd1 + bcd2;	//1000�ʁA100��<8421 8421>
//
	bcd1 = bcd_id[2] ^ 0xFF;	//���]
	bcd1 = bcd1 >> 5;		//�E�V�t�g5
	bcd1 &= 0x07;
	bcd2 = bcd_id[3] ^ 0xFF;	//���]
	bcd2 = bcd2 << 3;		//���V�t�g3
	bcd2 &= 0x18;	
	bcd_data[2] = bcd1 + bcd2;	//10000��<***- 8421>
//
	bcd1 = bcd_id[3] ^ 0xFF;	//���]
	bcd1 = bcd1 >> 2;		//�E�V�t�g2
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
//***  �T�u�֐��̈�
//***
void IO_Init(void)
{
    /* Start user code. Do not edit comment generated here */
    
    /* Start user code. Do not edit comment generated here */
//     int i;
//GPIO_Port �ݒ�
	P0 = 0x62;		//SDC_CS,FRAM_CS disable
	P1 = 0x01;		//RTC disable
//	P2 = 0;			//����
//
	P3 = 0x03;		//5V ON, �ݒ轲�����OFF
	P4 = 0;			//LCD�ޯ�ײ� OFF
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
//Port���[�h���W�X�^ �Z�b�g
	PM0  = 0x09;		//SD�J�[�h & FRAM_CS1,2
	PM1  = 0x78;		//SPI(RTC,FRAM) & ���콲��(MODE,UP,DOWN)  
	PM2  = 0xFF;		//BCD input Port
//
	PM3  = 0x00;		//�ݒ轲����݁A5V����
	PM4  = 0x07;		//SD�J�[�hWP����&LCD(RS,E,�ޯ�ײĐ���)
	PM5  = 0xF0;		//LCD(D4~D7)& ���콲��(LEFT,RIGHT)
//
	PM6  = 0xB0;		//LED1,2& RainPulse& ׯ��ڰ����& 12V�ቺ����
	PM7  = 0x60;		//BCD����SEL& S0~S7 output
	PM10 = 0xFF;		//���콲��(SET)����
//
	PM11 = 0xFC;		//BCD����CH1,CH2�o��
	PM12 = 0xFF;		//input(X,XT)& �d���d������(A/D)
//	PM13 = 0xFE;		//NoUSE
//
	PM14 = 0x09;		//USB(UART3)& SDC�d��& BCD����(CALL,INPUT Enable)
	PM15 = 0xFF;		//S80,S90 input
//��ٱ��ߐݒ�
	PU1 = 0x38;		//MODE,UP,DOWN pullup
//	PU3 = PU3 | 0x02;	//NC P31 pullup
	PU4 = PU4 | 0x06;	//NC P41,P42 pullup
	PU5 = 0x30;		//LEFT,RIGHT pullup
	PU10 = 0X01;		//SET pullup
//	PU11 = PU11 | 0x03;	//NC P110,P111 pullup
//��۸��߰ā^�޼����߰Đؑ֐ݒ�
	ADPC = 0x01;		//P20-P27,P150-P156���޼���IO�ɐݒ�
	PMC0 = 0XF3;		//ANI16,ANI17�g�p���Ȃ�
//PMC1�ύX�Ȃ�
	PMC10 = 0xfe;		//ANI20�g�p���Ȃ�
	PMC12 = 0xff;		//ANI19�g�p����
	PMC14 = 0x7f;		//ANI18�g�p���Ȃ�		
//UART2�s���z�u�ύX
	PIOR0	|= 0x02U;	//PIOR01=1(UART2�̃s���z�u��ύX)

}
//********************************************
//*** �V�X�e���N���b�N�ݒ�
//*** �����I���`�b�v�N���b�N(HOCO)�@���@�O�������N���b�N(X1)
//***
void clk_HOCOto_X1(void)
{
	unsigned char cd = 0;

	CMC = CMC | 0x40;	// X1���U�qӰ�ސݒ� EXCLK=0 & OSCSEL=1
	OSTS = 0x05;		// ���U���莞�Ԑݒ� 6.55msec�^5MHz
	CSC = CSC & 0x7f;	// X1���U�J�n MSTOP=0
	while (cd < 0xfc){
		cd = OSTC;	// ���U���莞�ԑҋ@
	}
	CKC = CKC | 0x10;	// fmx��Ҳݸۯ��ɐؑ�	
//	chk_byt = 0x0002c;	// ��߼��2
	CSC = CSC | 0x01;	// HOCO ���~ 
}


//****************************************
//** ������� ��ϰ
//** ��ϰ�ڲ�Ư�0�A����1���g�p
//** fclk = 5 [MHz]
//** 2^10��������T=204.8 [microsec]����
//** ������َ��ԁ����� [sec]---����max[12]--12sec
//** 243[(maicrosec]�ȉ��̌덷����
//**
void timer01_interval_sec(unsigned char dt)
{
	unsigned int interval_count;

//�ۯ��ݒ�
	PER0 = PER0 | 0x01;		//TAU0EN enable
	TPS0 = 0x33a3;			//CK01 = fclk/10,����fclk/3
//����01 �����ݒ�
	TMR01 =0x8000;			//��ĳ���ضް����,�������Ӱ��
	interval_count = dt % 13;
	interval_count = interval_count * 4883;// * 4883�̕␳	
	TDR01 = interval_count;		//�ިڲ���Ēl���
	TOE0 = 0x00;			//�O���o�͖���
//�����݋���
	MK1L = MK1L & 0xdf;		//������Ͻ� TMMK01= 0(���j
	PR01L = PR01L & 0xdf;		//TMPR001=0
	PR11L = PR11L | 0x20;		//TMPR101=1(�����ݗD��x����2)
	EI();				//PSW�����݋���
//����J�n
	TS0 = 0x0002;			//�Ư�0,����1 �J�n(TE01 = 1)
//	TT0 = 0x0002;			//�Ư�0,����1 ��~(TE01 = 0)
//	PER0 = PER0 & 0xfe;		//TAU0EN disable(�ȓd�͎��j
	
}
//************************************
// ���������ϰ 	���荞�ݎ�M�֐�
//	iv_counter �Ɂ@1���Z
//	iv_counter > 10000 �� 0 ��ؾ��
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

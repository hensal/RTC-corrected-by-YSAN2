//**************************************************************
//***	  RTC & FRAM (CSI 11)
//**
//**  �֐�
//*�@CSI����
void            csi11_initialize(void);			//CSI11(SAU0,����3) ������
unsigned char 	csi11_tx1b(unsigned char tr_data);	//CSI11 1�޲đ��M
unsigned char	csi11_txnb(unsigned char len);		//CSI11 n�޲đ��M
unsigned char 	csi11_rx1b(void);			//CSI11 1�޲Ď�M
//*  RTC read & write
unsigned char 	rtc_data_read(void);			//RTC�Ǐo��,gYear,gMonth,gDay,gHour,gMin,gSec(BC)
unsigned char	bcd2bc(unsigned char bcd_data);		//bcd�@���@bc(1byte)
unsigned char  	rtc_data_write(void);			//���v�ް�(rtc_wdata[8])��RTC�ɏ�����
void 		rtc_init(unsigned char fout);		//RTC(RX-4045)������(24���Ԑ�/FOUT�[�qOFF)
unsigned char 	rtc_powon_check(void);			//RTC ��ܰ�݌��o(�߂�l0=normal,1=reset)
void 		rtc_time_reset(void);
//* FRAM read & write
unsigned char fram1_write(void);		//FRAM1 ������(16�޲ĒP�ʏ�����)
unsigned char fram2_write(void);		//FRAM2 ������(16�޲ĒP�ʏ�����)
unsigned char fram_write(void);			//FRAM 16�޲ď����݁iCS�A�����݋��E�֎~�@�܂܂��j
unsigned char fram1_read(void);			//FRAM1 �Ǐo��(16�޲ĒP�ʏ�����)
unsigned char fram2_read(void);			//FRAM2 �Ǐo��(16�޲ĒP�ʏ�����)
unsigned char fram_read(void);			//FRAM 16�޲ēǏo���iCS�@�܂܂��j
//*** �ϐ�
//** RTC�p�ϐ�
extern unsigned char rtc_read_year;		//RTC�Ǐo���ް�
extern unsigned char rtc_read_month;
extern unsigned char rtc_read_day;
extern unsigned char rtc_read_hour;
extern unsigned char rtc_read_min;
extern unsigned char rtc_read_sec;
//
extern unsigned char rtc_write_year;		//RTC�����ݗp�ް�
extern unsigned char rtc_write_month;
extern unsigned char rtc_write_day;
extern unsigned char rtc_write_hour;
extern unsigned char rtc_write_min;
extern unsigned char rtc_write_sec;
extern unsigned char rtc_write_buff[8];		//RTC�������ޯ̧
extern unsigned char rtc_write_flag;		//RTC�������׸�(1�F������)
//** FRAM�p�ϐ�
extern unsigned char fram_read_buff[16];	//FRAM�Ǐo���ޯ̧(16�޲�)
extern unsigned char fram_write_buff[16];	//FRAM�������ޯ̧(16�޲�)
extern unsigned long fram_read_address;		//FRAM�Ǐo���擪���ڽ
extern unsigned long fram_write_address;	//FRAM�����ݐ擪���ڽ
extern unsigned char fram_read_flag;		//FRAM�Ǐo���׸�(1:�Ǐo��)
extern unsigned char fram_write_flag;		//FRAM�������׸�(1:������)//
//
//*** �ϐ�
extern unsigned char 	csi_txdata[128];
extern unsigned char 	csi_rxdata;
extern unsigned char	chk_data;
extern unsigned char	chk_buf[16];

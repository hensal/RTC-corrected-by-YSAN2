//**************************************************************
//***	  RTC & FRAM (CSI 11)
//**
//**  関数
//*　CSI制御
void            csi11_initialize(void);			//CSI11(SAU0,ﾁｬﾈﾙ3) 初期化
unsigned char 	csi11_tx1b(unsigned char tr_data);	//CSI11 1ﾊﾞｲﾄ送信
unsigned char	csi11_txnb(unsigned char len);		//CSI11 nﾊﾞｲﾄ送信
unsigned char 	csi11_rx1b(void);			//CSI11 1ﾊﾞｲﾄ受信
//*  RTC read & write
unsigned char 	rtc_data_read(void);			//RTC読出し,gYear,gMonth,gDay,gHour,gMin,gSec(BC)
unsigned char	bcd2bc(unsigned char bcd_data);		//bcd　→　bc(1byte)
unsigned char  	rtc_data_write(void);			//時計ﾃﾞｰﾀ(rtc_wdata[8])をRTCに書込み
void 		rtc_init(unsigned char fout);		//RTC(RX-4045)初期化(24時間制/FOUT端子OFF)
unsigned char 	rtc_powon_check(void);			//RTC ﾊﾟﾜｰｵﾝ検出(戻り値0=normal,1=reset)
void 		rtc_time_reset(void);
//* FRAM read & write
unsigned char fram1_write(void);		//FRAM1 書込み(16ﾊﾞｲﾄ単位書込み)
unsigned char fram2_write(void);		//FRAM2 書込み(16ﾊﾞｲﾄ単位書込み)
unsigned char fram_write(void);			//FRAM 16ﾊﾞｲﾄ書込み（CS、書込み許可・禁止　含まず）
unsigned char fram1_read(void);			//FRAM1 読出し(16ﾊﾞｲﾄ単位書込み)
unsigned char fram2_read(void);			//FRAM2 読出し(16ﾊﾞｲﾄ単位書込み)
unsigned char fram_read(void);			//FRAM 16ﾊﾞｲﾄ読出し（CS　含まず）
//*** 変数
//** RTC用変数
extern unsigned char rtc_read_year;		//RTC読出しﾃﾞｰﾀ
extern unsigned char rtc_read_month;
extern unsigned char rtc_read_day;
extern unsigned char rtc_read_hour;
extern unsigned char rtc_read_min;
extern unsigned char rtc_read_sec;
//
extern unsigned char rtc_write_year;		//RTC書込み用ﾃﾞｰﾀ
extern unsigned char rtc_write_month;
extern unsigned char rtc_write_day;
extern unsigned char rtc_write_hour;
extern unsigned char rtc_write_min;
extern unsigned char rtc_write_sec;
extern unsigned char rtc_write_buff[8];		//RTC書込みﾊﾞｯﾌｧ
extern unsigned char rtc_write_flag;		//RTC書込みﾌﾗｸﾞ(1：書込み)
//** FRAM用変数
extern unsigned char fram_read_buff[16];	//FRAM読出しﾊﾞｯﾌｧ(16ﾊﾞｲﾄ)
extern unsigned char fram_write_buff[16];	//FRAM書込みﾊﾞｯﾌｧ(16ﾊﾞｲﾄ)
extern unsigned long fram_read_address;		//FRAM読出し先頭ｱﾄﾞﾚｽ
extern unsigned long fram_write_address;	//FRAM書込み先頭ｱﾄﾞﾚｽ
extern unsigned char fram_read_flag;		//FRAM読出しﾌﾗｸﾞ(1:読出し)
extern unsigned char fram_write_flag;		//FRAM書込みﾌﾗｸﾞ(1:書込み)//
//
//*** 変数
extern unsigned char 	csi_txdata[128];
extern unsigned char 	csi_rxdata;
extern unsigned char	chk_data;
extern unsigned char	chk_buf[16];

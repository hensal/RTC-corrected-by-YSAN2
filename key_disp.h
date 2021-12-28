//*** スイッチ入力、表示制御関数
void lcd_disp(void);				//表示切替
void dsp_warmup(unsigned char page_num);
void dsp_main(unsigned char page_num);
void dsp_bcdin(unsigned char page_num);
void dsp_rainin(unsigned char page_num);
void dsp_voltage(unsigned char page_num);
void dsp_clock(unsigned char page_num);
void dsp_clock_adjust(unsigned char page_num);
void dsp_gps(unsigned char page_num);
void dsp_usb(unsigned char page_num);
void dsp_fram(unsigned char page_num);
void dsp_sdc(unsigned char page_num);
unsigned char key_switch_in(void);		// 操作ｽｲｯﾁ 入力
void pageno_disp(unsigned char page_pno);	// 1ﾊﾞｲﾄ数を2桁文字として表示
void bcd2d_disp(unsigned char line,unsigned char pos,unsigned char bcd_2d);//1ﾊﾞｲﾄ数を2桁文字として表示ﾊﾞｯﾌｧに格納
//char lcd_putc(char s);			// 1文字送信関数(ﾎﾟｰﾘﾝｸﾞ版)
//char lcd_puts(char *s);			// 文字列送信関数(ﾎﾟｰﾘﾝｸﾞ版)
void lcd_init(void);
void dsp_buff_disp(void);			// dsp_buff0[],dsp_buff1[]を表示
void lcdLocate(unsigned char line,unsigned char column);	//** ｶｰｿﾙ位置設定
void lcd_l1(unsigned char column);		// 1行目ｱﾄﾞﾚｽｾｯﾄ
void lcd_l2(unsigned char column);		// 2行目ｱﾄﾞﾚｽｾｯﾄ
void lcd_cout(unsigned char code);		//ｷｬﾗｸﾀLCD ｺﾝﾄﾛｰﾙｺｰﾄﾞ出力(STKW-186)
void lcd_dout(unsigned char code);		//ｷｬﾗｸﾀLCD ﾃﾞｰﾀｺｰﾄﾞ出力(STKW-186)
void delay_micro(unsigned int cnt);		//** ﾏｲｸﾛ秒遅延
void delay_msec(unsigned int icnt);		//** ﾐﾘ秒遅延

//*** 定数設定
#define	KEY_MODE	0x01
#define	KEY_UP		0x02
#define	KEY_DOWN	0x03
#define	KEY_LEFT	0x04
#define	KEY_RIGHT	0x05
#define	KEY_SET	0x06
//****
//AD
extern unsigned char	ad_data;		//AD入力値(16進)
extern unsigned int	vdd_bc;			//Vdd電圧_BC[0.1V]
extern unsigned int	vdd_bcd;		//Vdd電圧BCD[0.1V]
//雨量pulse
extern unsigned int 	r_counter;		//雨量ﾊﾟﾙｽ積算値
//
extern unsigned char	ch1_id[4];		//CH1入力ﾃﾞｰﾀ
extern unsigned char	ch1_bcd[4];		//CH1BCDﾃﾞｰﾀ
extern unsigned char	ch2_id[4];		//CH2入力ﾃﾞｰﾀ
extern unsigned char	ch2_bcd[4];		//CH2BCDﾃﾞｰﾀ

//*** �X�C�b�`���́A�\������֐�
void lcd_disp(void);				//�\���ؑ�
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
unsigned char key_switch_in(void);		// ���콲�� ����
void pageno_disp(unsigned char page_pno);	// 1�޲Đ���2�������Ƃ��ĕ\��
void bcd2d_disp(unsigned char line,unsigned char pos,unsigned char bcd_2d);//1�޲Đ���2�������Ƃ��ĕ\���ޯ̧�Ɋi�[
//char lcd_putc(char s);			// 1�������M�֐�(�߰�ݸޔ�)
//char lcd_puts(char *s);			// �����񑗐M�֐�(�߰�ݸޔ�)
void lcd_init(void);
void dsp_buff_disp(void);			// dsp_buff0[],dsp_buff1[]��\��
void lcdLocate(unsigned char line,unsigned char column);	//** ���وʒu�ݒ�
void lcd_l1(unsigned char column);		// 1�s�ڱ��ڽ���
void lcd_l2(unsigned char column);		// 2�s�ڱ��ڽ���
void lcd_cout(unsigned char code);		//��׸�LCD ���۰ٺ��ޏo��(STKW-186)
void lcd_dout(unsigned char code);		//��׸�LCD �ް����ޏo��(STKW-186)
void delay_micro(unsigned int cnt);		//** ϲ�ەb�x��
void delay_msec(unsigned int icnt);		//** �ؕb�x��

//*** �萔�ݒ�
#define	KEY_MODE	0x01
#define	KEY_UP		0x02
#define	KEY_DOWN	0x03
#define	KEY_LEFT	0x04
#define	KEY_RIGHT	0x05
#define	KEY_SET	0x06
//****
//AD
extern unsigned char	ad_data;		//AD���͒l(16�i)
extern unsigned int	vdd_bc;			//Vdd�d��_BC[0.1V]
extern unsigned int	vdd_bcd;		//Vdd�d��BCD[0.1V]
//�J��pulse
extern unsigned int 	r_counter;		//�J����ٽ�ώZ�l
//
extern unsigned char	ch1_id[4];		//CH1�����ް�
extern unsigned char	ch1_bcd[4];		//CH1BCD�ް�
extern unsigned char	ch2_id[4];		//CH2�����ް�
extern unsigned char	ch2_bcd[4];		//CH2BCD�ް�

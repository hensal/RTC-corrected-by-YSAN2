//**********************************************************************
//*** includes
//***
#include <stdio.h>
#include "iodefine.h"
//
#include "r_cg_macrodriver.h"		//HALT(),EI()���g�p���邽�߂ɕK�v
#include "uart2.h"

//**********************************************************************
//*** function�i�֐��錾�j
//***
extern void 	UART2_initial(void);
extern void 	UART2_txbuf_set(void);
extern void 	UART2_trans_1d(void);
//
unsigned char 	UART2_od_set(void);
void 		tx2_hex2asc3d(unsigned int hex_data,unsigned char buf_point);
unsigned char 	tx2_hex2asc_1d(unsigned char hexd);

//**********************************************************************
//*** bariables�i�ϐ��錾�j
//***
unsigned char	tx2_po;			//���M���ް����߲��(�޳ݶ���)
unsigned char	tx2_buf_vol;		//�ޯ̧��־�Ă����f�[�^��
unsigned char	tx2_trans_flag;		//���M�J�n�׸ށ@1�ŊJ�n
unsigned char	tx2_buf[128];		//���M�ް��ޯ̧�
//
//�������␳�㑪�蒌�ް�
unsigned int	s0ih;
unsigned int	s00ih;
unsigned int	s000ih;
unsigned int	s0000ih;


/*******************************************************************************
* Function Name: UART2������
* Description  : This function initializes the UART2 module.
* Arguments    : None
* Return Value : None
********************************************************************************/
void UART2_initial(void)
{
	PER0 |= 0x08;		//SAU1EN ON(1)--�رفE�ڲ�E�Ư�1 �L��
//�رْʐM��~
	ST1	|= 0x03;	//UNIT1 CH0&1 STOP--�رفE���ْ�~ڼ޽�(SE1ڼ޽���ON/OFF)
	SPS1	= 0x0046;	//CK10=CK11= fCLK/(2^6)--�رٯ�ۯ��ݒ�78.125kHz
				//CK11= fCLK/(2^4)--�رٯ�ۯ��ݒ�312.5kHz
//���M�ݒ�(UNIT1,CH0)
	SMR10	= 0x0022;	//���MCK10SEL,DOWN_EDGE START,UART SEL
	SCR10	= 0x8097;	//���MTX,�ۯ��ʑ�,noPARITY,LSB,STOP1,8BIT 
	SDR10	= 0x0600;	//���M�ۯ������ݒ�(1/8)--39060bps(��38400bps)
//��M�ݒ�(UNIT1,CH1)
	NFEN0	|= 0x10;	//SAU_RXD2_FILTER_ON(��M̨��-�L���ݒ�)
	SIR11	= 0x0007;	//��M�װ�׸޸��
	SMR11	= 0x0122;	//��MCK10SEL,DOWN_EDGE START,UART SEL
//	SMR11	= 0x0162;	//��MCK10SEL,UP_EDGE START,UART SEL
//	SCR11	= 0x4017;	//��MRX,�ۯ��ʑ�,noPARITY,MSB,STOP1,8BIT
	SCR11	= 0x4097;	//��MRX,�ۯ��ʑ�,noPARITY,LSB,STOP1,8BIT
	SDR11	= 0x0600;	//��M�ۯ������ݒ�(1/8)
//UNIT1��CH��~�����ݒ�
	SO1 	|= 0x0000;	//SAU1 �o�͒[�q���
    	SOL1 	|= 0x0000;	//SAU1_CH0 �o������ɰ��
    	SOE1	|= 0x0001;	//SAU1_CH0 �o�͋���
//I/O�|�[�g�ݒ�
	PM7	|= 0x40U;	//Set RxD2 pin mode(p7_6 input)
	P7	|= 0x80U;	//Set TxD2 pin data(p7_7 = 1)
	PM7 	&= 0x7FU;	//Set TxD2 pin mode(p7_7 output)
//�����݋���
	MK0H 	= MK0H & 0xfd;		//������Ͻ� SRMK2= 0(���j
	PR00H 	= PR00H & 0xfd;		//SRPR02=0
	PR10H 	= PR10H | 0x02;		//SRPR02=1(�����ݗD��x����2)
//�رْʐM�J�n
	SS1	|= 0x03;	//SAU1_CH0&1 START--�رفE���يJ�nڼ޽�(SE1ڼ޽���ON/OFF)

}

//*****************************************************************************
//** 1�޲��ް����M
//** ���Mڼ޽�����̏ꍇ�A1�޲đ��M  �߂萔���P�iOK�j
//** ���Mڼ޽����󂩂Ȃ��ő��M�ł��Ȃ����ꍇ�@�߂萔=0�iNG�j
//**�@
unsigned char UART2_tx_1b(unsigned char tx_data)
{
	unsigned char status;
	unsigned int timeover_count= 1000;	//��ѵ��ް1000����
	unsigned int ncnt;

	for ( ncnt=0; ncnt<timeover_count; ncnt++){
		status = SSR10;			//�رٽð��ڼ޽��i���M��ԓǏo���j
		if ((status & 0x0020) == 0){	//BFF=0:���M�ް�ڼ޽��i��j?
			SDR10 = tx_data;
			return (0);
		}
	}
	return(1);
}
/************************************************************************/
/* ���荞�ݎ�M�֐�							*/
/************************************************************************/
#pragma interrupt uart2_rx_int	(vect=0x16)
void uart2_rx_int (void)
{
	unsigned char err;

	err = SSR11 & 0x07;			// �G���[�f�[�^�̎��o��
	rx2_id = SDR11;				// ��M�f�[�^�̎��o��
//
	if (err == 0) 	rx2_id_buff[rx2_id_point] = rx2_id;	//��M�f�[�^���ޯ̧�ɕۑ�
	else		rx2_id_buff[rx2_id_point] = err | 0xF0;
//
	rx2_id_point++;					//�߲��++���āA20�ȏ�̏ꍇ�͂O���Z�b�g
	if (rx2_id_point > 19) rx2_id_point = 0;
}
//***************************************************************************




//****************************************************************************
//** ���蒌�ް���āA���M
//** trans_flag = 1�̎��ޯ̧��ɾ��
//** trans_flag ؾ��
//**
void UART2_txbuf_set(void)
{

	if (tx2_trans_flag == 1 && tx2_po == 0){
		tx2_buf_vol = UART2_od_set();	//���M�ް����ޯ̧�ɾ��
		tx2_po = tx2_buf_vol;
		tx2_po++;
		tx2_trans_flag = 0;
	}
}
//***************************************************************************
//** ���蒌�ް��������ϊ����ޯ̧��֕ۑ�
//** �߂�l = �ۑ��ް���
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
//** hex_data��3�o�C�g�̱������ނɕϊ�����tx_buf[buf_point]�ɕۑ�
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
//** <****8421> ���@0�`F�܂ł̱������ނɕϊ�
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
//** �ޯ̧��ɑ��M�ް�������A���Mڼ޽�����̏ꍇ�A1�޲đ��M
//** tx_po��-1���Ă���A�ް��𑗐M--tx_po�͑��M���I������ް��������Ă���B
//**�@
void UART2_trans_1d(void)
{
	unsigned char status;

	if (tx2_po != 0){
		status = SSR10;			//���M��ԓǏo��
		if (status & 0x0020 == 0){	//���M�ް�ڼ޽��i��j?
			tx2_po--;
			SDR10 = tx2_buf[tx2_buf_vol - tx2_po];
		}
	}
}

//**********************************************************************
//*** includes
//***
#include <stdio.h>
#include "iodefine.h"
//
#include "r_cg_macrodriver.h"		//HALT()���g�p���邽�߂ɕK�v
#include "uart3.h"

/*******************************************************************************
* Function Name: UART3������
* Description  : This function initializes the UART3 module.
* Arguments    : None
* Return Value : None
********************************************************************************/
void UART3_initial(void)
{
	PER0 |= 0x08;		//SAU1EN ON(1)--�رفE�ڲ�E�Ư�1 �L��
//�رْʐM��~
	ST1	|= 0x03;	//UNIT1 CH2&3 STOP--�رفE���ْ�~ڼ޽�(SE1ڼ޽���ON/OFF)
	SPS1	= 0x0046;	//CK10= fCLK/(2^6)--�رٯ�ۯ��ݒ�78.125kHz
				//CK11= fCLK/(2^4)--�رٯ�ۯ��ݒ�312.5kHz
//���M�ݒ�(UNIT1,CH2)
	SMR12	= 0x0022;	//���MCK10SEL,DOWN_EDGE START,UART SEL
	SCR12	= 0x8017;	//���MTX,�ۯ��ʑ�,noPARITY,LSB,STOP1,8BIT 
	SDR12	= 0x0600;	//���M�ۯ������ݒ�(1/8)--9765bps(��9600bps)
//��M�ݒ�(UNIT1,CH3)
	NFEN0	|= 0x10;	//SAU_RXD2_FILTER_ON(��M̨��-�L���ݒ�)
	SIR13	= 0x0007;	//��M�װ�׸޸��
	SMR13	= 0x0122;	//��MCK10SEL,DOWN_EDGE START,UART SEL
	SCR13	= 0x4017;	//��MRX,�ۯ��ʑ�,noPARITY,LSB,STOP1,8BIT
	SDR13	= 0x0600;	//��M�ۯ������ݒ�(1/8)
//UNIT1��CH��~�����ݒ�
	SO1 	|= 0x0000;	//SAU1 �o�͒[�q���
    	SOL1 	|= 0x0000;	//SAU1_CH2 �o������ɰ��
    	SOE1	|= 0x0004;	//SAU1_CH2 �o�͋���
//I/O�|�[�g�ݒ�
	PM14	|= 0x08U;	//Set RxD3 pin mode(p14_3 input)
	P14	|= 0x12U;	//Set TxD3 pin data(p14_4 = 1),RTS=1
	PM14 	&= 0xEFU;	//Set TxD3 pin mode(p14_4 output)
//�����݋���
	MK1H = MK1H & 0xdf;		//������Ͻ� SRMK3= 0(���j
	PR01H = PR01H & 0xdf;		//SRPR03=0
	PR11H = PR11H | 0x20;		//SRPR03=1(�����ݗD��x����2)
	EI();				//PSW�����݋���
//�رْʐM�J�n
	SS1	|= 0x03;	//SAU1_CH2&3 START--�رفE���يJ�nڼ޽�(SE1ڼ޽���ON/OFF)

}
//*****************************************************************************
//** 1�޲��ް����M
//** ���Mڼ޽�����̏ꍇ�A1�޲đ��M  �߂萔���P�iOK�j
//** ���Mڼ޽����󂩂Ȃ��ő��M�ł��Ȃ����ꍇ�@�߂萔=0�iNG�j
//**�@
unsigned char UART3_tx_1b(unsigned char tx_data)
{
	unsigned char status;
	unsigned int timeover_count= 1000;	//��ѵ��ް1000����
	unsigned int ncnt;

	for ( ncnt=0; ncnt<timeover_count; ncnt++){
		status = SSR12;			//�رٽð��ڼ޽��i���M��ԓǏo���j
		if ((status & 0x0020) == 0){	//BFF=0:���M�ް�ڼ޽��i��j?
			SDR12 = tx_data;
			return (0);
		}
	}
	return(1);
}
/************************************************************************/
/* ���荞�ݎ�M�֐�														*/
/************************************************************************/
#pragma interrupt uart3_rx_int	(vect=0x3E)
void uart3_rx_int (void)
{
	unsigned char err;

	err = SSR13 & 0x07;			// �G���[�f�[�^�̎��o��
	rx3_id = SDR13;				// ��M�f�[�^�̎��o��
//	n = uart3_rxfifo.count;
//	if(n < sizeof(uart3_rxfifo.buff)) {
//		uart3_rxfifo.count = ++n;
//		i = uart3_rxfifo.idx_w;
//		uart3_rxfifo.buff[i++] = d;
//		if(i >= sizeof(uart3_rxfifo.buff))
//			i = 0;
//		uart3_rxfifo.idx_w = i;
//	}
	if (err == 0) rx3_id_buff[rx3_id_point] = rx3_id;	//��M�f�[�^���ޯ̧�ɕۑ�
	else	rx3_id_buff[rx3_id_point] = err | 0xF0;
	rx3_id_point++;					//�߲��++���āA10�ȏ�̏ꍇ�͂O���Z�b�g
	if (rx3_id_point > 9) rx3_id_point = 0;
}
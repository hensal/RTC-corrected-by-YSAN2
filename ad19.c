//*****************************************
//***  �`�c���͊֐�
//*****************************************
//*****************************************
//*** includes
//***
#include <stdio.h>
#include "iodefine.h"
#include "r_cg_macrodriver.h"		//NOP()���g�p���邽�߂ɕK�v

//**********************************************************************
//*** �֐��錾
//***
extern void		ad19_init(void);	//AD19 ������
extern unsigned char	ad19_input(void);	//AD19 AD(8�ޯ�)����

//**************************************************
//** A/D�ϊ�����
//** ������
//**
void ad19_init(void)
{

	PER0 = PER0 | 0x20;		//ADCEN�Z�b�g(1=�ۯ�����)
//�߰Đݒ�
	PMC12 = PMC12 | 0x01;		//��۸ޓ��͐ݒ�(=1)
	PM12 = PM12 | 0x01;		//�߰�12_0������߰Ăɐݒ�(=1)
//AD�ϊ�Ӱ�ސݒ�	
	ADM0 = 0x30;			//AD�ϊ���~���(���ĐM���҂�)�A�ϊ��N������3CLK�A�ڸ�Ӱ��
	ADM1 = 0x20;			//��ĳ���ضް�A�ݼ��ĕϊ�Ӱ��
	ADM2 = 0x00;			//�Vdd,Vss,,8�ޯĕ���\
//�ϊ����ʏ���A�����ݒ�
	ADUL = 0xfe;			//���(Vdd 20.1V����)
	ADLL = 0x01;			//����(Vdd 0.1V����)
	ADTES = 0x00;			//ýėp�d��--������d��1.45V(Vdd 8.8V����)
	ADS = 0x13;			//AN19(P120�[�q)�I��
}
//**************************************************
//** A/D�ϊ�����
//** 1��Ǐo���i�ߐ��j
//**
unsigned char ad19_input(void)
{
	unsigned char n;
	unsigned char add;

//AD�ϊ��J�n
	ADM0 = ADM0 | 0x01;		//ADCE=1(A/D�ϊ��ҋ@��ԁj
	for(n=0;n < 10;n++) n++;	//5���[�v�ҋ@(5microsec�ȏ�)
	ADM0 = ADM0 | 0x80;		//ADCS=1(AD�ϊ��J�n)
	for(n=0;n < 6;n++) n++;		//3���[�v�ҋ@(3CLK�ȏ�)
	add = 0x00;
//�f�[�^�Ǐo��1���
	while (add != 0x01){
		add = IF1H;		//�ϊ��I���������׸ޓǏo��
	 	add = add & 0x01;	//ADIF���o
		WDTE = 0xAC;		//�����ޯ����ϰ_���
	}
	add = ADCRH;
//
	for(n=0;n < 50;n++) n++;	//5���[�v�ҋ@(5microsec�ȏ�)
//�f�[�^�Ǐo��2���
	ADM0 = ADM0 | 0x80;		//ADCS=1(AD�ϊ��J�n)
	add = 0x00;
	while (add != 0x01){
		add = IF1H;		//�ϊ��I���������׸ޓǏo��
	 	add = add & 0x01;	//ADIF���o
		WDTE = 0xAC;		//�����ޯ����ϰ_���
	}
	add = ADCRH;
//AD�ϊ��ҋ@��~	
	ADM0 = ADM0 & 0x7e;		//ADCE=1(A/D�ϊ���~��ԁj
//		
	return(add);
}
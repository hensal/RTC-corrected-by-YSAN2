void		UART3_initial(void);
unsigned char	UART3_tx_1b(unsigned char tx_data);
void 		uart3_rx_int (void);		//UART3 ���͊�����

//UART3(USB)�����ޯ̧
extern unsigned char	rx3_id_buff[16];	//RX3�����ޯ̧
extern unsigned char	rx3_id_point;		//RX3�����ޯ̧�߲��
extern unsigned char	rx3_id;			//RX3�����ް��i�����ŐV�j

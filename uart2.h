extern void	UART2_initial(void);
unsigned char	UART2_tx_1b(unsigned char tx_data);
void 		uart2_rx_int (void);		//UART2 ���͊�����
//
extern void	UART2_txbuf_set(void);
extern void	UART2_trans_1d(void);

//UART2(GPS)
extern unsigned char	rx2_id_buff[32];	//RX2�����ޯ̧
extern unsigned char	rx2_id_point;		//RX2�����ޯ̧�߲��
extern unsigned char	rx2_id;			//RX2�����ް��i�����ŐV�j
extern unsigned char	gps_power_flag;		//0=OFF,1=ON

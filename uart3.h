void		UART3_initial(void);
unsigned char	UART3_tx_1b(unsigned char tx_data);
void 		uart3_rx_int (void);		//UART3 入力割込み

//UART3(USB)入力ﾊﾞｯﾌｧ
extern unsigned char	rx3_id_buff[16];	//RX3入力ﾊﾞｯﾌｧ
extern unsigned char	rx3_id_point;		//RX3入力ﾊﾞｯﾌｧﾎﾟｲﾝﾀ
extern unsigned char	rx3_id;			//RX3入力ﾃﾞｰﾀ（随時最新）

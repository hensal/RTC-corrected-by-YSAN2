extern void	UART2_initial(void);
unsigned char	UART2_tx_1b(unsigned char tx_data);
void 		uart2_rx_int (void);		//UART2 “ü—ÍŠ„‚İ
//
extern void	UART2_txbuf_set(void);
extern void	UART2_trans_1d(void);

//UART2(GPS)
extern unsigned char	rx2_id_buff[32];	//RX2“ü—ÍÊŞ¯Ì§
extern unsigned char	rx2_id_point;		//RX2“ü—ÍÊŞ¯Ì§Îß²İÀ
extern unsigned char	rx2_id;			//RX2“ü—ÍÃŞ°ÀiÅVj
extern unsigned char	gps_power_flag;		//0=OFF,1=ON

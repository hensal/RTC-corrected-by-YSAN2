void		UART3_initial(void);
unsigned char	UART3_tx_1b(unsigned char tx_data);
void 		uart3_rx_int (void);		//UART3 “ü—ÍŠ„‚İ

//UART3(USB)“ü—ÍÊŞ¯Ì§
extern unsigned char	rx3_id_buff[16];	//RX3“ü—ÍÊŞ¯Ì§
extern unsigned char	rx3_id_point;		//RX3“ü—ÍÊŞ¯Ì§Îß²İÀ
extern unsigned char	rx3_id;			//RX3“ü—ÍÃŞ°ÀiÅVj

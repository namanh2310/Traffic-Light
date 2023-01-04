void UART_Init()
{
	//set baud 9600 equivalent to f=8Mhz
	UBRRH=0;
	UBRRL=51;
	UCSRA=0x00;
	UCSRC=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);	//Set up data frame 1 bit start - 1 bit stop - no parity - 8 bit frame
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);	//Enable rx - tx, and enable interrupt when received data

}

unsigned char usart_getchar()
{
	while ( !(UCSRA & (1<<RXC)) ) ; //check bit RXC on UCSRA register
	return UDR; //return data
}

void usart_putchar(char data)
{
	//Wait for empty transmit buffer
	while ( !( UCSRA & (1<<UDRE)) )//check UDRE on USCRA register 
	;
	
	UDR = data; //Put data into buffer, sends the data
}

void usart_write(char *string)
{
	int i=0;
	for (i=0;i<255;i++)
	{
		if(string[i] != 0 )
		{
			usart_putchar(string[i]);
		}
		else
		{
			break;
		}
	}
}

void usart_read(char string)
	{
		char ch;
		int i=0;
		for (i=0;i<9;i++)
		{
			ch = usart_getchar();
			string = ch;
			string++;
			
		}
	}	

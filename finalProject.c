#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

#define D4 eS_PORTC4
#define D5 eS_PORTC5
#define D6 eS_PORTC6
#define D7 eS_PORTC7

#define RS eS_PORTC0
#define EN eS_PORTC2

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "external_interrupt_init.h"
#include "uart.h"
#include "spi_execute.h"
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

int motor_speed=2500;
int count_speed=1000;
int speed_count;
int color=1;
int mode=0;
char lcd_mode[16]="                ";
char lcd_message[16]="                ";
char speed_display[20];

void lcd_display() {
	switch(mode){
		case 0:
			sprintf(lcd_mode,"Mode: Walker");
			Lcd4_Set_Cursor(1,1);
			Lcd4_Write_String(lcd_mode);
			switch(color){
				case 1: 
					
					sprintf(lcd_message,"Stop!     ");
					Lcd4_Set_Cursor(2,1);
					Lcd4_Write_String(lcd_message);
					break;
				case 2: 
				
					sprintf(lcd_message,"Ready!    ");
					Lcd4_Set_Cursor(2,1);
					Lcd4_Write_String(lcd_message);
					break;
				case 3: 
					
					sprintf(lcd_message,"Go ahead! ");
					Lcd4_Set_Cursor(2,1);
					Lcd4_Write_String(lcd_message);
					break;
			}
			break;
		case 1:
			sprintf(lcd_mode,"Mode: Driver");
			Lcd4_Set_Cursor(1,1);
			Lcd4_Write_String(lcd_mode);
			switch(color){
				case 1: 
				
					sprintf(lcd_message,"Go(%dkm|h) ", speed_count/2);
					Lcd4_Set_Cursor(2,1);
					Lcd4_Write_String(lcd_message);
					break;
				case 2: 
				
					sprintf(lcd_message,"Slow down!");
					Lcd4_Set_Cursor(2,1);
					Lcd4_Write_String(lcd_message);
					break;
				case 3: 
				
					sprintf(lcd_message,"Stop(0km|h)    ");
					Lcd4_Set_Cursor(2,1);
					Lcd4_Write_String(lcd_message);
					break;
			}
			break;
	}
}

void activate_motor(){
  	DDRD=0xFF;
	TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
	TCCR1B |= (1<<WGM12)|(1<<WGM13)|(1<<CS11);
	ICR1=20000;
}

int main (void){
	// MOSI, SCK and SS are output
	unsigned char count=rand()%30+30;
	DDRA = 0xFF;
	PORTA &= ~(1<<0);
	DDRB = (1<<MOSI) | (1<<SCK) | (1<<SS) ;
	ADCSRA = 0x87;
	ADMUX = 0xE5;
	Lcd4_Init();
	activate_motor();
	SPCR = (1<<SPE)  | (1<<MSTR)| (1<<SPR0);  
	execute(0x09,0x03); // decode only digit 0 and 1
	execute(0x0B,0x02); // Set Scan Limit (digit0 to digit 2)
	execute(0x0C,0x01); // Turn On Display
	execute(0x0A,0x09);
	external_interrupt_init();
	UART_Init();
	sei();
	while(1) {
		sprintf(speed_display,"%d\nkm/h\r",(int)speed_count/2);
		usart_write("Your speed: ");
		usart_write(speed_display);
		ADCSRA |= (1<<ADSC);
		while ( (ADCSRA & (1<<ADIF) )== 0 );
		lcd_display();
		switch(color) {
			case 1:
			execute(0x01,count/10);
			execute(0x02,count%10);
			PORTA |= (1<<0);
			PORTA |= (1<<4);
			PORTA &= ~(1<<2);
			PORTA &= ~(1<<3); 
			_delay_ms(count_speed);
			speed_count=(ADC>>6);
			OCR1A=speed_count*100;
			OCR1B=speed_count*100;
			count-=1;
			if(count<=3 && count>0){
				PORTA |= (1<<3);
				_delay_ms(100);
				PORTA &= ~(1<<3);
				_delay_ms(100);
			}
			
			if(count<=0) {
				count=3;
				color+=1;
			}

			break;

			case 2:
			execute(0x01,count/10);
			execute(0x02,count%10);
			PORTA |= (1<<1);
			PORTA &= ~(1<<0);
			_delay_ms(count_speed);
			PORTA &= ~(1<<4);
			PORTA |= (1<<3);
			_delay_ms(100);
			PORTA &= ~(1<<3);
			_delay_ms(100);
			speed_count=0;
			OCR1A=0;
			OCR1B=0;
			count-=1;
			if(count<=0) {
				count=rand()%30+30;
				color+=1;
			}
			break;

			case 3:
			execute(0x01,count/10);
			execute(0x02,count%10);
			PORTA |= (1<<2);
			PORTA |= (1<<3);
			PORTA &= ~(1<<1);
			PORTA &= ~(1<<4);
			speed_count=0;
			_delay_ms(count_speed);
			if(count<=3 && count>0){
				PORTA |= (1<<4);
				_delay_ms(100);
				PORTA &= ~(1<<4);
				_delay_ms(100);
			}
			count-=1;
			if(count<=0) {
				count=rand()%30+30;
				color=1;
			}
			break;
		}
		
	}
}

ISR (INT0_vect)
{	
	mode++;
	if(mode>1){
		mode=0;
	}
}




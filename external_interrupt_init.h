void external_interrupt_init() 
{
	DDRC  = 0xFF;  	
  	//DDRD  &= ~((1<<2)|(1<<3)); 
  	PORTD |= (1<<2) | (1<<3);  
  	MCUCR = 0b00001010;    
  	GICR  = (1<<INT0)|(1<<INT1);
}
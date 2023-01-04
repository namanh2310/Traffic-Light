#define MOSI 5 
#define SCK  7 
#define SS   4

void execute (unsigned char cmd, unsigned char data){
	// initializing the packet by pulling SS low
	PORTB &= ~(1<<SS) ;  
	SPDR = cmd; 	// start CMD transmission
	// wait cmd transfer to finish
	while (!(SPSR & (1<<SPIF) ) );
	// start DATA transmission
	SPDR = data; 		
	// wait data transfer to finish
	while (!(SPSR & (1<<SPIF) ) ) ;
	// terminate the packet by pulling SS high
	PORTB |= 1<<SS;
	}

#include<reg51.h>
#include <string.h>

void uart_init();				//Initialize UART
void timer_init(); 			// Timer 2 in mode 2
void uart_tx(char x);		//Transmission function
char uart_rx();					//Receiving function
void uart_msg(char *p);   			//String transmission
void newLine();
void wait4Letter(char x);
void startTimer();
void endTimer();
void trainUserX(bit user);
void Timer0_ISR(void);

int overFlows = 0x0000;
unsigned long avg0[7] ; //store average for user 1
unsigned long avg1[7] ;//store average for user 2
unsigned char testingTimes[7]; //store testing time
unsigned char password  [7] = "aaaaaaa";  //to store the password
char test = '0';  // for now it's zero, but later on it will be from an input pin
char user = '0';   //for now it's zero, but later on it will be from an input pin

void main(void){
	
	// Initialize UART settings
	
	uart_init();
	
	while(1){
			
			if(test == '0'){
				
				uart_msg("Now training  ");
				
				while(test == '0'){
					
					uart_msg("pick user 0 or 1 to train  ");
					
					user = uart_rx() ;
					
					if(user == '0'){
						trainUserX(0);
					}
					else if(user == '1'){
						trainUserX(1);
					}
					
					uart_msg("press 0 to train again  or 1 to test   ");
					test = uart_rx();
					uart_tx(test);
				}
			
			}
			
			if(test== '1'){
			
				// CODE FOR TESTING
				
				/////////////////////////////////
				// RONY YOUR CODE STARTS HERE////
				/////////////////////////////////
				
				
			}
		
		
	}
}


void trainUserX (bit user){
	
		unsigned char j;
		unsigned char i;
		// CODE FOR TRAINING 
				
		// FOR EACH TRIAL
		for(j = '0'; j<'5' ; j++){
			
			uart_msg("trial number ");
			uart_tx(j);
			uart_msg("      ");
							
				// FOR EACH LETTER
				for( i = 0; i<7;i++){
								
					startTimer();
					wait4Letter(password[i]);
					endTimer();
								
					if(!user){
						avg0[i] += (overFlows << 16) | (TH0 << 8) | TL0;
					}
					else{
						avg1[i] += (overFlows << 16) | (TH0 << 8) | TL0;
					}
					
					overFlows = 0x00;
				
				}
					
		}
				
		for(i = 0 ; i<7 ; i++){
			
		avg0[i] /= 5 ;
			
		}
}

void wait4Letter(char x){

	while(1){
		char y = uart_rx();
		
		if(y == x){
			uart_msg("recieved   ");
			uart_tx(x);
			return;
		}
		else{
			uart_msg("Wrong character !   ");
		}
	}
}

void uart_msg(char *p){
	
	while(*p != 0){
		uart_tx(*p);
		p++;
	}
	
	return;
	
}

void startTimer(){
	TH0 = 0x00;
	TL0 = 0x00;
	TR0 = 1;
	ET0 = 1;
	EA = 1;
}

void endTimer(){
	TR0 = 0;
	ET0 = 0;
	EA = 0;
}

void uart_init(){
	 SCON = 0x50;
	 timer_init();
}

void timer_init(){
	TMOD = 0x21;
	TH1 = 0xFD;
	TR1 = 1;
}

void uart_tx(char x){
	SBUF = x;
	
	while(!TI);
	
	TI = 0;
}

char uart_rx(){
	char x;
	//REN = 1;
	while(!RI);
	x = SBUF;
	RI = 0;
	return x;
}

void Timer0_ISR(void) interrupt 1 {
	TR0 = 0;
	//uart_msg("OverFlow Detected");
	overFlows ++;
	TH0 = 0x00;
	TL0 = 0x00;
	TF0 = 0;
	TR0 = 1;
	
}



void newLine(){
	uart_tx(0x0D);
}


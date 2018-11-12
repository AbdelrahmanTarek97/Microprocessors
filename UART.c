#include<reg51.h>

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
void testingPhase();
int getEuclideanDistance(bit user);
//int sqroot(double square);
void Timer0_ISR(void);

char overFlows = 0x0000;
unsigned int avg0[7] ; //store average for user 1
unsigned int avg1[7] ;//store average for user 2
unsigned int testingTimes[7]; //store testing time
unsigned char password  [7] = "aaaaaaa";  //to store the password
char test = '0';  // for now it's zero, but later on it will be from an input pin
char user = '0';   //for now it's zero, but later on it will be from an input pin
char correct = '0'; // To check that the user entered a correct expected value to the UART
int euclideanDistance0 = 0 ;
int euclideanDistance1 = 0 ;

int result = 1;

void main(void){
	
	// Initialize UART settings
	
	uart_init();
	
	while(1){
			
		correct = '0';
					
		while(correct == '0'){
			uart_msg("press 0 to train or 1 to test   \n");
			test = uart_rx();
			if((test == '0') || (test == '1')){
				correct = '1';
			}
		}
			
		if(test == '0'){
				
			uart_msg("Now training  \n");
									
			correct = '0';
					
			while(correct == '0'){
				uart_msg("pick user 0 or 1 to train  \n");
						
				user = uart_rx();
				if((user == '0') || (user == '1')){
					correct = '1';
				}
			}
					
			if(user == '0'){
				trainUserX(0);
			}
			else if(user == '1'){
				trainUserX(1);
			}
			
		}
			
			if(test == '1'){
				
				testingPhase();
				euclideanDistance0 = getEuclideanDistance(0);
				euclideanDistance1 = getEuclideanDistance(1);
			
				//uart_msg(euclideanDistance0);
				//uart_msg("\n");
				
				if(euclideanDistance0 > euclideanDistance1){
					uart_msg("User 1 \n");
				} else if(euclideanDistance0 < euclideanDistance1){
					uart_msg("User 0 \n");
				} else {
					uart_msg("A Tie \n");
				}
				
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
			uart_msg("      \n");
							
				// FOR EACH LETTER
				for( i = 0; i<7;i++){
								
					startTimer();
					wait4Letter(password[i]);
					endTimer();
								
					if(i == 6){
						uart_msg("      \n");
					}
					
					if(!user){ 
						avg0[i] += overFlows;
					} else if(user){
						avg1[i] += overFlows;
					}
					
					overFlows = 0x00;
				
				}
					
		}
				
		for(i = 0 ; i<7 ; i++){
			if(!user){ 
				avg0[i] /= 5;
			} else {
				avg1[i] /= 5;
			}
		}
}

void testingPhase(){
	
	unsigned char i;

	// CODE FOR Testing 

	uart_msg("Testing! \n");
	
	// FOR EACH LETTER
	for( i = 0; i<7;i++){
								
		startTimer();
		wait4Letter(password[i]);
		endTimer();
								
		if(i == 6){
			uart_msg("      \n");
		}
		
		testingTimes[i] += overFlows;
					
		overFlows = 0x00;
				
	}				
}


void wait4Letter(char x){

	while(1){
		char y = uart_rx();
		
		if(y == x){
			uart_msg("recieved   ");
			uart_tx(x);
			uart_msg(" / ");
			return;
		}
		else{
			uart_msg(" Wrong character ! / ");
		}
	}
}

int getEuclideanDistance(bit user){
	unsigned char i;

	int euclideanDistance = 0;
	
	for(i=0;i<7;i++){
		
		if(!user){
				euclideanDistance += (testingTimes[i] - avg0[i])*(testingTimes[i] - avg0[i]);
		} else {
				euclideanDistance += (testingTimes[i] - avg1[i])*(testingTimes[i] - avg1[i]);
		}
		
	}
	
	//euclideanDistance = sqroot(euclideanDistance);		
	
	return euclideanDistance;
	
}
/*
int sqroot(int square){
    int root=square/3;
    int i;
    if (square <= 0) return 0;
    for (i=0; i<16; i++)
        root = (root + square / root) / 2;
    return root;
}
*/

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


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
long getEuclideanDistance(bit user);
void Timer0_ISR(void);

unsigned int overFlows = 0;
unsigned int avg0[10] ; //store average for user 1
unsigned int avg1[10] ;//store average for user 2
unsigned int testingTimes[10]; //store testing time
unsigned char password  [10] = ".tie5Ronal";  //to store the password
char test = '0';  // for now it's zero, but later on it will be from an input pin
char user = '0';   //for now it's zero, but later on it will be from an input pin
char correct = '0'; // To check that the user entered a correct expected value to the UART
long euclideanDistance0 = 0 ;
long euclideanDistance1 = 0 ;

sbit switch1 = P0^0;
sbit switch2 = P0^1;
sbit led = P0^3;

void main(void){
	
	// Initialize UART settings

	led = 0;
	//P0 = 0X04; // Set P0^0 and P0^1 as Input and P0^2 as Output
		
	uart_init();
	
	uart_msg("Please set the Switch 1 to Train(0) and Switch 2 to User 0 \n");
					
	while(switch1 != 0 || switch2 != 0);
			
	uart_msg("Now training User 0 \n");
			
	uart_msg("Press any button to start \n");		
	
	uart_rx();
	
	trainUserX(0);
		
	uart_msg("Please set the Switch 1 to Train(0) and Switch 2 to User 1 \n");
					
	while(switch1 != 0 || switch2 != 1);

	uart_msg("Now training User 1 \n");	
		
	uart_msg("Press any button to start \n");		
	
	uart_rx();
			
	trainUserX(1);
			
	uart_msg("Please set the Switch 1 to Test(1)  \n");

	while(switch1 != 1);
	
	uart_msg("Now testing \n");	
		
	uart_msg("Press any button to start \n");		
	
	uart_rx();

	testingPhase();
	
	euclideanDistance0 = getEuclideanDistance(0);
	euclideanDistance1 = getEuclideanDistance(1);

	if(euclideanDistance1 > euclideanDistance0){
		uart_msg("You are User 0 \n");

		led = 1;

		startTimer();
		while(overFlows < 50);
		endTimer();
	
		led = 0;
	
	} else if(euclideanDistance1 < euclideanDistance0){
		uart_msg("You are User 1 \n");
	
		led = 1;
		
		startTimer();
		while(overFlows < 50);
		endTimer();
		
		led = 0;
		
		startTimer();
		while(overFlows < 50);
		endTimer();
		
		led = 1;
		
		startTimer();
		while(overFlows < 50);
		endTimer();
		
		led = 0;
		
	} else {
		uart_msg("I do not know! \n");
	}

}


void trainUserX (bit user){

	unsigned char j;
	unsigned char i;
	
	for( i = 0; i<10; i++){
		if(!user){
			avg0[i] = 0;		
		} else{
			avg1[i] = 0;
		}
	}

	// CODE FOR TRAINING 
				
	// FOR EACH TRIAL
	for(j = '0'; j<'5' ; j++){
			
		uart_msg("trial number ");
		uart_tx(j);
		uart_msg("      \n");
							
		// FOR EACH LETTER
		for( i = 0; i<10;i++){
								
			startTimer();
			wait4Letter(password[i]);
			endTimer();
								
			if(i == 4 || i == 9){
				uart_msg("      \n");
			}
					
			if(!user){ 
				avg0[i] += overFlows;
			} else if(user){
				avg1[i] += overFlows;
			}
					
			overFlows = 0;
				
		}
					
	}
				
	for(i = 0 ; i<10 ; i++){
		if(!user){ 
			avg0[i] /= 5;
		} else {
			avg1[i] /= 5;
		}
	}
}

void testingPhase(){
	
	unsigned char i;

	for( i = 0; i<10; i++){
		testingTimes[i] = 0;
	}
	
	// CODE FOR Testing 

	uart_msg("Testing! \n");
		
	overFlows = 0;

	// FOR EACH LETTER
	for( i = 0; i<10;i++){
								
		startTimer();
		wait4Letter(password[i]);
		endTimer();
								
		if(i == 4 || i == 9){
			uart_msg("      \n");
		}
		
		testingTimes[i] += overFlows;
					
		overFlows = 0;
				
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

long getEuclideanDistance(bit user){
	unsigned char i;

	long euclideanDistance = 0;
	
	for(i=0;i<10;i++){
		
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
	overFlows = 0;
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


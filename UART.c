#include<reg51.h>
#include <stdlib.h>

void uart_init();				//Initialize UART
void timer_init(); 			// Timer 2 in mode 2 used for initialization of UART
void uart_tx(char x);		//Transmission function
char uart_rx();					//Receiving function
void uart_msg(char *p); //String transmission
void wait4Letter(char x); // A method that waits until the desired letter is entered 
void startTimer(); // Start the timer so the overFlows are calculated by the interrupts
void endTimer(); // Stop the timer
void Timer0_ISR(void); // A method that detects the interrupts in the timer
void trainUserX(bit user); // A method that lets a certain user to enter .tie5Ronal five times
void testingPhase(); // A method that enables any user to enter .tie5Ronal for testing 
unsigned long getEuclideanDistance(bit user); // A method that gets the euclidean difference between a ceratin user and the tested user

unsigned overFlows = 0; // This is our unit time in the program. It is the difference between a key stroke and another
unsigned long xdata avg0[9] ; //store average keystrokes for user 0
unsigned long xdata avg1[9];//store average keystrokes for user 1
unsigned long xdata testingTimes[9]; //store testing time of random user
unsigned char password  [10] = ".tie5Ronal";  //The password the users will enter
unsigned long euclideanDistance0 = 0 ; // The euclidean distance between user 0 and the random user
unsigned long euclideanDistance1 = 0 ;// The euclidean distance between user 1 and the random user

sbit switch1 = P0^0; // Switch 1 which indicates whether we are in training(0) or testing(1) Phase
sbit switch2 = P0^1; // Switch 2 which indicates whether user0(0) or user1(1) is entering the password now (In Training phase)
sbit led = P0^2; // The Led which indicates which user is the random user. Lights up 1 time means User 0, 2 times means User 1

void main(void){
	

	led = 0; // The Led is off at the begining
	
	// Initialize UART settings	
	uart_init();
	
	uart_msg("Set Switch 1 to Train(0) and Switch 2 to User 0 \n");
					
	while(switch1 != 0 || switch2 != 0); // Wait until the Switch 1 is 0 (Training Phase) and Switch 2 is 0 (User 0)
			
	uart_msg("Training User 0 \n");
			
	trainUserX(0); // Train User 0
		
	uart_msg("Set Switch 1 to Train(0) and Switch 2 to User 1 \n");
					
	while(switch1 != 0 || switch2 != 1); // Wait until the Switch 1 is 0 (Training Phase) and Switch 2 is 0 (User 1)

	uart_msg("training User 1 \n");	
		
	trainUserX(1); // Train User 1
			
	uart_msg("Set Switch 1 to Test(1)  \n");

	while(switch1 != 1); // Wait until the Switch 1 is 1 (Testing Phase)
	
	
	while(1)
	{
		uart_msg("Testing, Enter the password \n");	
		
		testingPhase(); // Testing random user
		
		euclideanDistance0 = getEuclideanDistance(0); // Get the Euclidean Distance between User 0 and Testing User
		euclideanDistance1 = getEuclideanDistance(1); // Get the Euclidean Distance between User 1 and Testing User

		if(euclideanDistance1 > euclideanDistance0){ // If Euclidean Distance of User 1 is bigger than User 0 then the random user is User 0
			uart_msg("You are User 0 \n");

			// TURN THE LED ON ONCE AND THEN OFF //
			
			led = 1; // Turn the LED On

			startTimer();
			while(overFlows < 50); // Wait for 50 overflow units
			endTimer();
		
			led = 0; // Turn The Led Off
		
		} else if(euclideanDistance1 < euclideanDistance0){ // If Euclidean Distance of User 1 is smaller than User 0 then the random user is User 1
			uart_msg("You are User 1 \n");
		
			// TURN THE LED ON AND THEN OFF AND THEN ON AND THEN OFF //
			
			led = 1; // Turn the LED on
			
			startTimer();
			while(overFlows < 50); // Wait for 50 overflow units
			endTimer();
			
			led = 0; // Turn the LED off
			
			startTimer();
			while(overFlows < 50); // Wait for 50 overflow units
			endTimer();
			
			led = 1; // Turn the LED on
			
			startTimer();
			while(overFlows < 50); // Wait for 50 overflow units
			endTimer();
			
			led = 0; // Turn the LED off
			
		} else { // If both euclidean distance is the same then we cannot know the random user
			uart_msg("I do not know! \n");
		}
			
	}

	//	while(1);
		
}

// A method that enables any user to enter .tie5Ronal for testing 
void trainUserX (bit user){

	unsigned char j;
	unsigned char i;
	
	// First Check That there are no values in avg array by clearing it 
	for( i = 0; i<9; i++){
		if(!user){
			avg0[i] = 0;
		} else{
			avg1[i] = 0;
		}
	}
	// For each trial and here we have 5 trials
	for(j = '0'; j<'5' ; j++){
		uart_msg("trial ");
		uart_tx(j);
		uart_msg("\n");
		
		wait4Letter(password[0]); // Wait for '.'
							
		// For each letter and we have 9 letters
		for( i = 0; i<9;i++){
								
			startTimer(); // Let the timer start so we can count the over flows
			wait4Letter(password[i+1]); // Wait util the user enters the desired letter
			endTimer(); // End timer
			// Add all the overflows of every corresponding character in the 5 trials and we will later on divide them by 5(No of Trials)
			if(!user){ 
				avg0[i] += (unsigned long)((((unsigned long)overFlows) << 16 ) + (((unsigned long)TH0) << 8) + ((unsigned long)TL0))/5;
				//avg0[i] += overFlows/5;
			} else if(user){
				avg1[i] += (unsigned long)((((unsigned long)overFlows) << 16) + (((unsigned long)TH0) << 8) + ((unsigned long)TL0))/5;
				//avg1[i] += overFlows/5;
			}
			overFlows = 0; // Reset overFlows to 0
				
		}
			uart_msg("\n");					
	}
	
}

// A method that enables any user to enter .tie5Ronal for testing 
void testingPhase(){
	
	unsigned char i;

	// First Check That there are no values in testing array by clearing it 
	for( i = 0; i<9; i++){
		testingTimes[i] = 0;
	}

	wait4Letter(password[0]); // Wait for '.'
	
	// For each letter and we have 9 letters
	for( i = 0; i<9;i++){
								
		startTimer(); // Start timers so we can count the overflows
		wait4Letter(password[i+1]); // Wait until random user enter the desired letter 
		endTimer(); // Stop timer
								
		testingTimes[i] = (unsigned long)((((unsigned long)overFlows) << 16 ) + (((unsigned long)TH0) << 8) + ((unsigned long)TL0));
		
		//testingTimes[i] = overFlows;
		
		overFlows = 0;
				
	}
	uart_msg("\n");
	
}

// A method that waits until the desired letter is entered 
void wait4Letter(char x){

	while(1){
		char y = uart_rx(); // Get Entered Letter
		
		// If the entered letter is as the desired letter then return else then keep looping until desired letter is entered
		if(y == x){
			uart_tx(x);
			return;
		}
		else{
//			uart_msg(" Wrong character ! / ");
		}
	}
}

// A method that gets the euclidean difference between a ceratin user and the tested user
unsigned long getEuclideanDistance(bit user){
	unsigned char i;

	unsigned long euclideanDistance = 0;
	
	for(i=0;i<9;i++){
		
		if(!user){ // The Euclidean Distance is calculated by squaring the difference between two points
			//euclideanDistance += (testingTimes[i] - avg0[i])*(testingTimes[i] - avg0[i]);
			euclideanDistance += (testingTimes[i]>avg0[i])? (testingTimes[i] - avg0[i]) : (avg0[i] - testingTimes[i]) ;
		} else {
			//euclideanDistance += (testingTimes[i] - avg1[i]) * (testingTimes[i] - avg1[i]);
			euclideanDistance += (testingTimes[i]>avg1[i])? (testingTimes[i] - avg1[i]) : (avg1[i] - testingTimes[i]) ;
		}
		
	}

	return euclideanDistance;
	
}

//String transmission
void uart_msg(char *p){
	
	while(*p != 0){
		uart_tx(*p);
		p++;
	}
	
	return;
	
}

// Start the timer so the overFlows are calculated by the interrupts
void startTimer(){
	overFlows = 0; // Clearing the overFlows
	TH0 = 0x00; // Restarting the timer high bits of timer 0 
	TL0 = 0x00; // Restarting the timer low bits of timer 0
	TR0 = 1; // Start timer 0
	ET0 = 1; // Enable timer 0 interruprs
	EA = 1; // Enable Interrupts
}

// Stop the timer
void endTimer(){
	TR0 = 0; // Stop Timer 0 interrupts
	ET0 = 0; // Disable Timer 0 interrupts
	EA = 0; // Disable Interrupts
}

//Initialize UART
void uart_init(){
	 SCON = 0x50;
	 timer_init();
}

// Timer 2 in mode 2 used for initialization of UART
void timer_init(){
	TMOD = 0x21;
	TH1 = 0xFD;
	TR1 = 1;
}

//Transmission function
void uart_tx(char x){
	SBUF = x;
	
	while(!TI);
	
	TI = 0;
}

//Receiving function
char uart_rx(){
	char x;
	//REN = 1;
	while(!RI);
	x = SBUF;
	RI = 0;
	return x;
}

// A method that detects the interrupts in the timer
void Timer0_ISR(void) interrupt 1 {
	TR0 = 0;
	overFlows ++; // Incerement Overflows
	TH0 = 0x00;
	TL0 = 0x00;
	TF0 = 0;
	TR0 = 1;	
}


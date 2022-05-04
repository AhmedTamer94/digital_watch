#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
//----------------------------------------------------------------
/*these variables are for:
 * SW_TCK1: stop watch timer flag when the timer reaches 1 it will do a certain
 * function will be illustrated below in the remain code.
 *
 * SW_count_pos: an array of 6 variables to store in the value of each 7SEG in
 * the normal mode.
 * SW[0,1] contains the seconds, SW[2,3] contains the minutes and SW[4,5]
 * contains the hours.
 *
 * SW_count_down: an array of 6 variables to store in the value of each 7SEG in
 * count down mode.
 *
 * Display_loop: a global variable for looping through the two arrays
 */
unsigned char SW_TCK1=1,SW_Count_Pos[6]={0,0,0,0,0,0},Display_loop;
unsigned char SW_Count_Down[6]={0,0,0,0,0,0};
//-------------------------------------------------------------------------
//each function will be illustrated below, above each function definition
void PINS_Init(void);
void INTs_Init(void);
void TIMER1_Init(void);
void count_Down_mode(void);
void count_Down(void);
void counter_positive(void);
void counter_sec(void);
void counter_hrs(void);
void counter_min(void);
ISR(INT0_vect);
ISR(INT1_vect);
ISR(INT2_vect);
ISR(TIMER1_COMPA_vect);
//---------------------------------------------------------------------------
/*its the main function which display the 7SEG and make the microcontroller
 * acts as ordinary digital watch which every 1s increase the seconds till
 * the watch reaches 99hr,59min,59,59s then it resets.
 */
int main(void){
	PINS_Init();
	INTs_Init();
	TIMER1_Init();
	while(1){/*at this while loop it will enable only one 7SEG and sends to the
	PORTC the value of this 7SEG
	 */
		for(Display_loop=0;Display_loop<6;Display_loop++){
			PORTA=(1<<Display_loop);
			PORTC=SW_Count_Pos[Display_loop];
			_delay_ms(1);
		}
		if(SW_TCK1==0){/* when the timer count 1s it will set the SW_TCK1=0
		then this condition is true then set the SW_TCK1=1 so we didn't go to
		the below function only every 1s*/
			SW_TCK1=1;
			counter_positive();
		}
		if(PINB&(1<<0)){/*when the user push the button that is connected to
		PB0 it will call the count down function*/
			_delay_ms(20);
			if(PINB&(1<<0))
				count_Down_mode();
		}

	}
}
/*this function to initiate the pins
 * PORTA: will be output to control the signal of the enable and disable of
 * the 7SEG.
 *
 * PORTC: will be output for the first 4 pins to send the value of the 7SEG.
 *
 * PORTB: will be an input and initiate the pull up mode at PIN2(PB1).
 *
 * PORTD: pin 16 & 17 will be input and initiate the pull up mode at pin 16.
 */
void PINS_Init(void){
	DDRA=0x3f;
	PORTA=0;
	DDRC=0x0f;
	PORTC=0;
	DDRB=0;
	PORTB|=(1<<2);
	DDRD&=~(1<<2)&(1<<3);
	PORTD|=(1<<2);
}
/*initiate the interrupts:
 * INT0: when sensing a falling edge it will trigger the interrupt.
 * INT1: when sensing a raising edge it will trigger the interrupt.
 * INT2: when sensing a falling edge it will trigger the interrupt.
 */
void INTs_Init(void){
	GICR|=(1<<INT0)|(1<<INT1)|(1<<INT2);
	SREG|=(1<<7);
	MCUCSR&=~(1<<ISC2);
	MCUCR|=(1<<ISC01)|(1<<ISC11)|(1<<ISC10);
}
/*initiate the Timer1 on force compare mode
 * once the microcontroller frequency 1MHZ i set the prescaler for the timer 1024
 * means that every clock of the timer will be every 1ms so every 1000 clock
 * will be 1s.
 * set the compare register equal to 1000
 */
void TIMER1_Init(void){
	TCNT1=0;
	TIMSK|=(1<<OCIE1A);
	TCCR1A|=(1<<FOC1A);
	TCCR1B|=(1<<CS12)|(1<<CS10)|(1<<WGM12);
	OCR1A=1000;
}
/*when the INT0 happens the microcontroller will reset the watch and this means:
 * set all the 7SEG values equal to zero and if the timer clock source is stopped
 * by the user it will enable it again.
 *
 */
ISR(INT0_vect){
	for(Display_loop=0;Display_loop<6;Display_loop++){
		SW_Count_Pos[Display_loop]=0;
	}
	TCCR1B|=(1<<CS12)|(1<<CS10);
	TCNT1=0;
}
/*
 * when INT1 happens the microcontroller will pause the watch and this means
 * stop the timer clock source and set it equal to 0 set CS10 and CS12 bits to 0
 */
ISR(INT1_vect){
	TCCR1B&=~((1<<CS12)|(1<<CS10));
}
/*when INT2 happens the microcontroller will resume the watch and this means
 * set the timer prescaler equal to 1024 set CS10 and CS12 bits to 1
 *
 */
ISR(INT2_vect){
	TCCR1B|=(1<<CS12)|(1<<CS10);
}
/*when the compare register reaches 1000 this means the timer counts 1s
 * it sets the SW_TCK1 variable to 0
 */
ISR(TIMER1_COMPA_vect){
	SW_TCK1=0;
}
/*this function increment the variables in the SW_count_pos array
 * when the function is called it increment the var[0]by one till it reaches 9
 * then when it called again it set var[0]=0 and increment the var[1]by 1
 * and so on.
 */
void counter_positive(void){
	if(SW_Count_Pos[0]<9){/* check if var[0]<9 if true it increments*/
		SW_Count_Pos[0]++;
	}
	else{/*else it sets var[0]=0*/
		SW_Count_Pos[0]=0;
		if(SW_Count_Pos[1]<5){/* check if var[1]<5 if true it increments*/
			SW_Count_Pos[1]++;
		}
		else{/*else it sets var[1]=0*/
			SW_Count_Pos[1]=0;
			if(SW_Count_Pos[2]<9){/* check if var[2]<9 if true it increments*/
				SW_Count_Pos[2]++;
			}
			else{/*else it sets var[2]=0*/
				SW_Count_Pos[2]=0;
				if(SW_Count_Pos[3]<5){/* check if var[3]<5 if true it increments*/
					SW_Count_Pos[3]++;
				}
				else{/*else it sets var[3]=0*/
					SW_Count_Pos[3]=0;
					if(SW_Count_Pos[4]<9){/* check if var[4]<9 if true it increments*/
						SW_Count_Pos[4]++;
					}
					else{/*else it sets var[4]=0*/
						SW_Count_Pos[4]=0;
						if(SW_Count_Pos[5]<9){/* check if var[5]<9 if true it increments*/
							SW_Count_Pos[5]++;
						}
						else{/*else it sets var[5]=0*/
							SW_Count_Pos[5]=0;
						}
					}
				}
			}
		}
	}
}

/*button PB3 enter the count down mode and stops the timer clock source till
 * the user press the button that initiates the count down calculations.*/

void count_Down_mode(void){
	/*create variables to check on them every time the user press a button
	 * to do a certain function*/
	char sec_flag=1,min_flag=1,hr_flag=1;
	TCCR1B&=~((1<<CS12)|(1<<CS10));
	TCNT1=0;
	while(1){
		/*counter_DW does the calculation every 1s and if the timer is off
		 * then no calculations are done*/
		if(SW_TCK1==0){
			SW_TCK1=1;
			count_Down();
		}
		for(Display_loop=0;Display_loop<6;Display_loop++){/*at this for loop
		 it will enable only one 7SEG and sends to the PORTC
		 the value of this 7SEG */
			PORTA=(1<<Display_loop);
			PORTC=SW_Count_Down[Display_loop];
			_delay_ms(1);
		}
		if(!(TCCR1B&(1<<CS12))){/*when the user begins the mode and press
			button PB6 the microcontroller won't accept any inputs for
			the seconds, minutes and hours*/
			if(PINB&(1<<3)){//increase the seconds by 30 every press
				_delay_ms(20);
				if(PINB&(1<<3)){
					if(sec_flag==0){
						counter_sec();
						sec_flag=1;
					}
				}
			}
			else
				sec_flag=0;
			if(PINB&(1<<4)){//increase the minutes by 5 every press
				_delay_ms(20);
				if(PINB&(1<<4)){
					if(min_flag==0){
						counter_min();
						min_flag=1;
					}
				}
			}
			else
				min_flag=0;
			if(PINB&(1<<5)){//increase the hours by 1 every press
				_delay_ms(20);
				if(PINB&(1<<5)){
					if(hr_flag==0){
						counter_hrs();
						hr_flag=1;
					}
				}
			}
			else
				hr_flag=0;
			if(PINB&(1<<6)){/*initiate the mode (initiate the timer)*/
				_delay_ms(20);
				if(PINB&(1<<6)){
					TCCR1B|=(1<<CS12)|(1<<CS10);
				}
			}
		}
		if(PINB&(1<<7)){//exit the mode PB7 and reset the timer
			_delay_ms(20);
			if(PINB&(1<<7)){
				for(Display_loop=0;Display_loop<6;Display_loop++){/*
				we clear the array first*/
					SW_Count_Down[Display_loop]=0;
				}
				/*we set CS12&CS10 to 1 if the user didn't push button PB6
				 * and wants to return to main function.
				 */
				TCCR1B|=(1<<CS12)|(1<<CS10);
				TCNT1=0;
				break;
			}
		}
	}
}
/*this function does the following:
 * increment the seconds by 30 thus every two press increments the minutes by 1.
 * the first if condition is to check if the time is not 99hr,59min,30,s
 * if this condition is true it will return from the function and do nothing.
 */
void counter_sec(void){
	if(SW_Count_Down[4]==9&&SW_Count_Down[5]==9&&SW_Count_Down[3]==5&&SW_Count_Down[2]==9&&SW_Count_Down[1]==3)
		return;
	if(SW_Count_Down[1]<3){
		SW_Count_Down[1]=SW_Count_Down[1]+3;
	}
	else{
		SW_Count_Down[1]=0;
		if(SW_Count_Down[2]<9){
			SW_Count_Down[2]++;
		}
		else{
			SW_Count_Down[2]=0;
			if(SW_Count_Down[3]<5){
				SW_Count_Down[3]++;
			}
			else{
				SW_Count_Down[3]=0;
				if(SW_Count_Down[4]<9){
					SW_Count_Pos[4]++;
				}
				else{
					SW_Count_Down[4]=0;
					if(SW_Count_Down[5]<9){
						SW_Count_Down[5]++;
					}
					else{
						SW_Count_Down[5]=0;
					}
				}
			}
		}
	}
}
/*this function does the following:
 * increment the minutes by 5 thus every 12 press increments the hours by 1.
 * the first if condition is to check if the time is not 99hr,55min
 * if this condition is true it will return from the function and do nothing.
 */
void counter_min(void){
	if(SW_Count_Down[4]==9&&SW_Count_Down[5]==9&&SW_Count_Down[3]==5&&SW_Count_Down[2]==5)
		return;
	if(SW_Count_Down[2]<5){
		SW_Count_Down[2]=SW_Count_Down[2]+5;
	}
	else{
		SW_Count_Down[2]=0;
		if(SW_Count_Down[3]<5){
			SW_Count_Down[3]++;
		}
		else{
			SW_Count_Down[3]=0;
			if(SW_Count_Down[4]<9){
				SW_Count_Down[4]++;
			}
			else{
				SW_Count_Down[4]=0;
				if(SW_Count_Down[5]<9){
					SW_Count_Down[5]++;
				}
				else{
					SW_Count_Down[5]=0;
				}
			}
		}
	}
}
/*this function does the following:
 * increment the hours by 1.
 * the first if condition is to check if the time is not 99hr and
 * if this condition is true it will return from the function and do nothing.
 */
void counter_hrs(void){
	if(SW_Count_Down[4]==9&&SW_Count_Down[5]==9)
		return;
	if(SW_Count_Down[4]<9){
		SW_Count_Down[4]++;
	}
	else{
		SW_Count_Down[4]=0;
		if(SW_Count_Down[5]<9){
			SW_Count_Down[5]++;
		}
	}
}

/*this function does the following:
 * decrements the the array which we store in it the time we want to count down.
 *
 * first we check if the array variables equal to 0 it will do nothing
 * and waits the user to return to the main function.
 *
 * every 1 second it decrements var[0]by 1 and every 60sec decrements var[2]by 1
 * and so on.
 */
void count_Down(void){
	for(Display_loop=0;Display_loop<6;Display_loop++){
		if(SW_Count_Down[Display_loop]!=0)
			break;
	}

	/* if this condition is true it stops the timer as we finish count down*/
	if(Display_loop==6){
		TCCR1B&=~((1<<CS12)|(1<<CS10));
		return;
	}
	/*these if conditions decrement the variables in the SW_count_down array every 1s
	 * so every 60s decrement the minutes and so on.
	 */
	if(SW_Count_Down[0]>0){
		SW_Count_Down[0]--;
	}
	else{
		SW_Count_Down[0]=9;
		if(SW_Count_Down[1]>0){
			SW_Count_Down[1]--;
		}
		else{
			SW_Count_Down[1]=5;
			if(SW_Count_Down[2]>0){
				SW_Count_Down[2]--;
			}
			else{
				SW_Count_Down[2]=9;
				if(SW_Count_Down[3]>0){
					SW_Count_Down[3]--;
				}
				else{
					SW_Count_Down[3]=5;
					if(SW_Count_Down[4]>0){
						SW_Count_Down[4]--;
					}
					else{
						SW_Count_Down[4]=9;
						if(SW_Count_Down[5]>0){
							SW_Count_Down[5]--;
						}
					}
				}
			}
		}
	}

}

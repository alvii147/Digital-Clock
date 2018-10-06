#include <avr/io.h> //includes avr input output header file
#include <avr/interrupt.h> //includes avr interrupt header file
#include <stdio.h> //includes standard input output header file
#include "lcd.h" //includes lcd display header file

unsigned char Hour = 0; //declares unsigned character variable "Hour" and assigns 0 to it
unsigned char Minute = 0; //declares unsigned character variable "Minute" and assigns 0 to it
unsigned char Sec = 0; //declares unsigned character variable "Sec" and assigns 0 to it
unsigned int count = 0; //declares unsigned character variable "count" and assigns 0 to it

char line[16];
void timer0_setup(void); //declares timer0 setup function
void minute_interrupt(void); //declares interrupt function for minute increment
void hour_interrupt(void); //declares interrupt function for hour increment
void lcd_write(void); //declares function to write on lcd

void main(void) //calls main function with return type void and no parameters
{
	DDRB &= ~(1<<0); //clears pin 0 or port B as input
	
	LCDInit(); //calls lcd initialization function
	lcd_write(); //calls lcd write function
	
	minute_interrupt(); //calls minute interrupt function
	hour_interrupt(); //calls hour interrupt function
	
	while(1) //calls while loop that loops endlessly
	{
		lcd_write(); //calls lcd write function
		if(bit_is_clear(PINB, 0)) //checks if start button is pressed
		{
			while(bit_is_clear(PINB, 0)) //calls while loop that loops until start button is released
			{
				
			}
			break; //breaks out of while loop
		}
	}
	
	timer0_setup(); //calls timer setup function
	
	while(1) //calls while loop that loops endlessly
	{
		lcd_write(); //calls lcd write function
	}
}

void timer0_setup(void) //defines previously declared timer setup function
{
	TCCR0 |= (1<<CS00); //selects no prescaling
	TCCR0 &= ~(1<<CS01);
	TCCR0 &= ~(1<<CS02);
	
	sei(); //enables global interrupt
	
	TIMSK |= (1<<TOIE0); //enables timer overflow interrupt
	
	TCNT0 = 0; //clears TCNT0 timer/counter register
}

ISR(TIMER0_OVF_vect) //calls interrupt service routine for timer
{
	count++; //increments count by 1
	
	if(count == 31250) //checks if count is 31250
	{
		Sec++; //increments Sec
		
		if(Sec > 59) //checks if Sec is greater than 59
		{
			Sec = 0; //clears Sec to 0
			Minute++; //increments Minute by 1
			if(Minute > 59) //checks if Minute is greater than 59
			{
				Minute = 0; //clears Minute to 0
				Hour++; //increments Hour by 1
			}
		}
		count = 0; //clears count to 0
	}
}

void minute_interrupt(void) //defines previously declared interrupt function
{
	sei(); //enables global interrupt
	
	MCUCR &= ~(1<<ISC10); //selects falling edge interrupt
	MCUCR |= (1<<ISC11);
	
	GICR |= (1<<INT1); //selects INT0 as interrupt pin
}

ISR(INT1_vect) //defines interrupt service routine for minute increment interrupt
{
	Minute++; //increments Minute by 1
	if(Minute > 59) //checks if Minute is greater than 59
	{
		Minute = 0; //clears Minute to 0
	}
}

void hour_interrupt(void) //defines previously declared hour interrupt function
{
	sei(); //enables global interrupt
	
	MCUCR &= ~(1<<ISC00); //selects falling edge interrupt
	MCUCR |= (1<<ISC01);
	
	GICR |= (1<<INT0); //selects INT1 as interrupt pin
}

ISR(INT0_vect) //defines interrupt service routine for hour increment interrupt
{
	Hour++; //increments Hour by 1
	if(Hour > 24) //checks if Hour is greater than 24
	{
		Hour = 0; //clears Hour to 0
	}
}

void lcd_write(void) //defines previously declared lcd write function
{
	sprintf(line, "Digital Clock"); //prints text into array line
	LCDGotoXY(3, 1); //goes to column 3 row 1
	LCDString(line); //writes array line on lcd
	
	sprintf(line, "%0.2u : %0.2u : %0.2u", Hour, Minute, Sec); //prints text into array line
	LCDGotoXY(3, 2); //goes to column 3 row 2
	LCDString(line); //writes array line on lcd
}
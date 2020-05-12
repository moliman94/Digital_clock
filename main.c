/*
 * Clock.c
 *
 * Created: 23-Apr-20 1:19:45 AM
 * Author : Mohamed Soliman
 */ 

#include <avr/io.h>
#include "Header.h"
#include "timer.h"
#include "EEPROM.h"
#include "LCD.h"

//Push buttons pins
#define SET_PB PD2
#define UP_PB PD3
#define DOWN_PB PD4

//Locations of hh, mm, ss in EEPROM
#define HH_LOC 6
#define MM_LOC 7
#define SS_LOC 8

//Functions prototypes
uint8 get_key();
void LCD_print_time(uint8 hr, uint8 min, uint8 sec);

//global variables
sint8 hh, mm, ss;
uint8 key = 0;
uint8 set_flag = 0;

int main(void)
{
	//Initializations
	timer_0_normal_init();
	timer_2_normal_init();
	LCD_init();
	/*
	EEPROM_write(SS_LOC, 0);
	EEPROM_write(MM_LOC, 0);
	EEPROM_write(HH_LOC, 0);
	*/
	hh = EEPROM_read(HH_LOC);
	mm = EEPROM_read(MM_LOC);
	ss = EEPROM_read(SS_LOC);
	LCD_print_string("  :  :  ");
	LCD_print_time(hh, mm, ss);
	
	uint8 debounce_flag = 0;
	
    while (1) 
    {
		set_flag = 0;
		timer_0_start();
		timer_2_stop();
		/*
		 * Show the clock until set key is pressed
	   	 */
		do 
		{
			LCD_print_time(hh, mm, ss);
			key = get_key();
		} while (key != 1);
		
		timer_0_stop();
		
		while(get_key() != 0);
		
		timer_2_start();
		/*
		 * Enter Set hour mode
		 */
		set_flag = 1;
		do 
		{
			key = get_key();
			if(debounce_flag == 0 && key == 2)
			{
				_delay_ms(5);
				debounce_flag = 1;
				hh++;
			}
			else if(debounce_flag == 0 && key == 3)
			{
				_delay_ms(5);
				debounce_flag = 1;
				hh--;
			}
			else if(debounce_flag == 1 && key == 0)
			{
				_delay_ms(5);
				debounce_flag = 0;
			}
		} while (key != 1);
		
		set_flag = 2;
		LCD_goto_xy(1,1);
		LCD_print_num_2_dig(hh);
		while(get_key() != 0);
		
		/*
		 * Enter set minute mode
		 */
		do
		{
			key = get_key();
			if(debounce_flag == 0 && key == 2)
			{
				_delay_ms(5);
				debounce_flag = 1;
				mm++;
			}
			else if(debounce_flag == 0 && key == 3)
			{
				_delay_ms(5);
				debounce_flag = 1;
				mm--;
			}
			else if(debounce_flag == 1 && key == 0)
			{
				_delay_ms(5);
				debounce_flag = 0;
			}
		} while (key != 1);
		
		set_flag = 3;
		LCD_goto_xy(1,4);
		LCD_print_num_2_dig(mm);
		while(get_key() != 0);
		
		/*
		 * Enter set seconds mode
		 */
		do
		{
			key = get_key();
			if(debounce_flag == 0 && key == 2)
			{
				_delay_ms(5);
				debounce_flag = 1;
				ss++;
			}
			else if(debounce_flag == 0 && key == 3)
			{
				_delay_ms(5);
				debounce_flag = 1;
				ss--;
			}
			else if(debounce_flag == 1 && key == 0)
			{
				_delay_ms(5);
				debounce_flag = 0;
			}
		} while (key != 1);
		
		while(get_key() != 0);
		
		
    }
}

//make Timer0 interrupt to make delay 1 second
ISR(TIMER0_OVF_vect)
{
	static uint8 count = 0;
	count++;
	if(count == 61)
	{
		count = 0;
		ss++;
		if(ss > 59)
		{
			ss = 0;
			mm++;
			if(mm > 59)
			{
				mm = 0;
				hh++;
				if(hh > 23)
				{
					hh = 0;
					mm = 0;
					ss = 0;
				}
			}
		}
		EEPROM_write(HH_LOC, hh);
		EEPROM_write(MM_LOC, mm);
		EEPROM_write(SS_LOC, ss);
	}
}

ISR(TIMER2_OVF_vect)
{
	static uint8 count1 = 0;
	static uint8 flag = 0;
	count1++;
	if(count1 == 20)
	{
		count1 = 0;
		if(set_flag == 1)
		{
			if(hh>23) hh = 0;
			else if(hh<0) hh = 23;
			
			flag++;
			LCD_goto_xy(1,1);
			if(flag == 1)
			{
				LCD_print_string("  ");
			}
			else if(flag == 2)
			{
				LCD_print_num_2_dig(hh);
				flag = 0;
			}
		}
		
		if(set_flag == 2)
		{
			if(mm>59) mm = 0;
			else if(mm<0) mm = 59;
			
			flag++;
			LCD_goto_xy(1,4);
			if(flag == 1)
			{
				LCD_print_string("  ");
			}
			else if(flag == 2)
			{
				LCD_print_num_2_dig(mm);
				flag = 0;
			}
		}
		
		if(set_flag == 3)
		{
			if(ss>59) ss = 0;
			else if(ss<0) ss = 59;
			
			flag++;
			LCD_goto_xy(1,7);
			if(flag == 1)
			{
				LCD_print_string("  ");
			}
			else if(flag == 2)
			{
				LCD_print_num_2_dig(ss);
				flag = 0;
			}
		}
	}
}

void keys_init()
{
	DDRD |= (1<<SET_PB)|(1<<UP_PB)|(1<<DOWN_PB);
}

uint8 get_key()
{
	_delay_ms(1);
	if(READBIT(PIND, SET_PB) == 0) return 1;
	else if(READBIT(PIND, UP_PB) == 0) return 2;
	else if(READBIT(PIND, DOWN_PB) == 0) return 3;
	_delay_ms(1);
	
	return 0;
}

void LCD_print_time(uint8 hr, uint8 min, uint8 sec)
{
	LCD_goto_xy(1,1);
	LCD_print_num_2_dig(hr);
	LCD_goto_xy(1,4);
	LCD_print_num_2_dig(min);
	LCD_goto_xy(1,7);
	LCD_print_num_2_dig(sec);
}
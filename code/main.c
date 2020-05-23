#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#include "lcd_n.h"


char temp_mode = 0;
char mode = 100;
unsigned char led = 0xfe;

char mil_sec = 0, sec = 0, min = 0, hour = 0, day = 13, month = 5;
short year = 2020;

char temp_sec = 0, temp_min = 0, temp_hour = 0, temp_day = 1, temp_month = 1;
short temp_year = 2020;

int flag_stop_watch = -1;
int state_flag = -1;

int stop_mil_sec = 0, stop_sec = 0, stop_min = 0, stop_hour = 0;

char temp_alarm_sec = 0, temp_alarm_min = 0, temp_alarm_hour = 0;
char alarm_sec = 0, alarm_min = 0, alarm_hour = 0;

char month_max[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char AM[] = "AM";
char PM[] = "PM";

bool is_leap(short year)
{
	if (year % 4 == 0)
	if ( year % 100 == 0)
	if ( year % 400 == 0)
	return true;
	else
	return false;
	else
	return true;
	else
	return false;
}

char *get_weekday(short year, char day, char month)
{
	int days = (year-1) * 365+day;
	if(month > 1 && is_leap(year))
	{
		month_max[1] = 29;
	}
	else
	month_max[1] = 28;
	while(1)
	{
		if(year >= 400)
		{
			year -= 400;
			days += 97;
		}
		else if(year >= 100)
		{
			year -= 100;
			days += 24;
		}
		else
		{
			days += (int)((year)/4);
			break;
		}
	}
	for(int i = 0; i < month-1; i++)
	days += month_max[i];
	switch(days%7)
	{
		case 0:
		return "Sun       ";
		case 1:
		return "Mon       ";
		break;
		case 2:
		return "Tue       ";
		break;
		case 3:
		return "Wed       ";
		break;
		case 4:
		return "Thurs     ";
		break;
		case 5:
		return "Fri       ";
		break;
		case 6:
		return "Sat       ";
		break;
		default:
		return "          ";
		break;
	}
}


void Interrupt_Init(void) {
	EIMSK = 0x0F;
	EICRA = 0xFF;

	sei();
}


void InitTimer() {
	TCCR0 = 0x0F; // CTC Mode, Prescale 104
	OCR0 = 155; // count 0 to 99  //16 MHZ
	TIMSK = 0x02; // Output compare interrupt enable
}

ISR(TIMER0_COMP_vect) {

	if (flag_stop_watch % 3 == 1)
	{

		stop_mil_sec++;
		if (stop_mil_sec >= 100)
		{
			stop_sec++;
			stop_mil_sec = 0;
			if (stop_sec >= 60)
			{

				stop_min++;
				stop_sec = 0;
				if (stop_min >= 60)
				{
					stop_hour++;
					stop_min = 0;
				}
			}
		}
	}

	mil_sec++;  //each 10 mil seconds


	if (stop_mil_sec >= 100)
	{
		stop_mil_sec = 0;

	}

	if (mil_sec >= 100)
	{
		sec++;

		mil_sec = 0;
		if (sec >= 60)
		{

			min++;
			sec = 0;
			if (min >= 60)
			{
				hour++;
				min = 0;
				if (hour >= 24)
				hour = 0;
			}

		}
	}

}


ISR (INT3_vect) // MOVE BACK BUTTON
{
	PortInit();
	LCD_Init();
	LCD_Clear();
	LCD_pos(0, 0);
	LCD_STR("                         ");
	LCD_pos(1, 0);
	LCD_STR("                         ");
	temp_sec = 0;
	temp_min = 0;
	temp_hour = 0;
	temp_day = 1;
	temp_month = 1;

	flag_stop_watch = -1;
	state_flag = -1;

	temp_alarm_sec = 0;
	temp_alarm_min = 0;
	temp_alarm_hour = 0;

	SREG |= 0x80;
	mode = 100;
}




ISR (INT2_vect) // MOVE DOWN BUTTON
{
	if ( mode == 100) {
		temp_mode++;
		if (temp_mode > 4)
		temp_mode = 0;
	}
	else if (mode == 3)
	{
		if (state_flag == 0)
		{

			temp_year--;
			if (temp_year <= 0)
			temp_year = 1;
			if (is_leap(temp_year))
			{
				month_max[1] = 29;

			} else
			{
				month_max[1] = 28;

			}


		}
		else if (state_flag == 2)
		{
			if (temp_day > 1 )
			{
				temp_day--;
				} else {
				temp_day = month_max[temp_month - 1];
			}

		} else if (state_flag == 1)
		{
			if (temp_month > 1)
			{
				temp_month--;
				} else {
				temp_month = 12;
			}
		} else if (state_flag == 3)
		{
			if (temp_hour > 0)
			{
				temp_hour--;
				} else {
				temp_hour = 23;
			}


		}
		else if (state_flag == 4)
		{
			if (temp_min > 0)
			{
				temp_min--;
				} else {
				temp_min = 59;
			}

		} else if (state_flag == 5)
		{
			if (temp_sec > 0)
			{
				temp_sec--;
				} else {
				temp_sec = 59;
			}
		}
	}
	else if (mode == 4)
	{
		if (state_flag == 0)
		{
			if (temp_alarm_hour > 0)
			{
				temp_alarm_hour--;
				} else {
				temp_alarm_hour = 23;
			}


		}
		else if (state_flag == 1)
		{
			if (temp_alarm_min > 0)
			{
				temp_alarm_min--;
				} else {
				temp_alarm_min = 59;
			}

		} else if (state_flag == 2)
		{
			if (temp_alarm_sec > 0)
			{
				temp_alarm_sec--;
				} else {
				temp_alarm_sec = 59;
			}
		}
	}
}


ISR (INT1_vect) // MOVE UP BUTTON
{
	if ( mode == 100) {
		temp_mode--;
		if (temp_mode < 0)
		temp_mode = 4;
	}
	else if (mode == 3)
	{
		if (state_flag == 0)
		{

			temp_year++;

			if (is_leap(temp_year))
			{
				month_max[1] = 29;

			} else
			{
				month_max[1] = 28;

			}


		}
		else if (state_flag == 2)
		{
			if (temp_day < month_max[temp_month - 1] )
			{
				temp_day++;
				} else {
				temp_day = 1;
			}

		} else if (state_flag == 1)
		{
			if (temp_month < 12)
			{
				temp_month++;
				} else {
				temp_month = 1;
			}
		} else if (state_flag == 3)
		{
			if (temp_hour < 23)
			{
				temp_hour++;
				} else {
				temp_hour = 0;
			}


		}
		else if (state_flag == 4)
		{
			if (temp_min < 59)
			{
				temp_min++;
				} else {
				temp_min = 0;
			}

		} else if (state_flag == 5)
		{
			if (temp_sec < 59)
			{
				temp_sec++;
				} else {
				temp_sec = 0;
			}
		}
	}
	else if (mode == 4)
	{
		if (state_flag == 0)
		{
			if (temp_alarm_hour < 23)
			{
				temp_alarm_hour++;
				} else {
				temp_alarm_hour = 0;
			}

		}
		else if (state_flag == 1)
		{
			if (temp_alarm_min < 59)
			{
				temp_alarm_min++;
				} else {
				temp_alarm_min = 0;
			}

		} else if (state_flag == 2)
		{
			if (temp_alarm_sec < 59)
			{
				temp_alarm_sec++;
				} else {
				temp_alarm_sec = 0;
			}
		}
	}
}


ISR (INT0_vect) // OK BUTTON
{
	mode = temp_mode;
	
	PortInit();
	LCD_Init();
	LCD_Clear();
	LCD_pos(0, 0);
	LCD_STR("                         ");
	LCD_pos(1, 0);
	LCD_STR("                         ");

	if (mode == 0)
	{
		SREG |= 0x80;
		mode = 0;
	}
	else if (mode == 1)
	{

		SREG |= 0x80;
		mode = 1;
	}
	else if (mode == 2)
	{
		
		

		flag_stop_watch++;
		if (flag_stop_watch % 3 != 2)
		{
			stop_sec = 0;
			stop_min = 0;
			stop_hour = 0;
			stop_mil_sec = 0;
		}

		SREG |= 0x80;
		mode = 2;
	}
	else if (mode == 3)
	{
		if (state_flag == -1)
		{
			state_flag++;

		} else if (state_flag == 0)
		{
			year = temp_year;
			state_flag++;
		}
		else if (state_flag == 2)
		{
			day = temp_day;
			state_flag++;
		} else if (state_flag == 1)
		{
			month = temp_month;
			state_flag++;
		} else if (state_flag == 3)
		{
			hour = temp_hour;
			state_flag++;
		}
		else if (state_flag == 4)
		{
			min = temp_min;
			state_flag++;
		} else if (state_flag == 5)
		{
			sec = temp_sec;
			state_flag++;
		} else if (state_flag == 6)
		{

			temp_sec = 0;
			temp_min = 0;
			temp_hour = 0;
			temp_day = 1;
			temp_month = 1;

			state_flag = -1;
			mode = 100;
		}
	}
	else if (mode == 4)
	{
		if (state_flag == -1)
		{
			state_flag++;

		}
		else if (state_flag == 0)
		{
			alarm_hour = temp_alarm_hour;
			state_flag++;
		}
		else if (state_flag == 1)
		{
			alarm_min = temp_alarm_min;
			state_flag++;
		} else if (state_flag == 2)
		{
			alarm_sec = temp_alarm_sec;
			state_flag++;
		} else if (state_flag == 3)
		{
			temp_alarm_sec = 0;
			temp_alarm_min = 0;
			temp_alarm_hour = 0;

			state_flag = -1;
			mode = 100;
		}
	}
}


int main(void)
{
	char *str = "";

	PortInit();
	LCD_Init();
	LCD_Clear();
	InitTimer();

	while (1)
	{

		Interrupt_Init();
		DDRB = 0xFF;
		DDRD = 0x00;

		switch (mode)
		{
			case 0:
			str = "";
			sprintf(str, "%04d %02d/%02d %s", year, day, month, get_weekday(year, day, month));
			LCD_pos(0, 0);
			LCD_STR(str);
			LCD_pos(1, 0);

			if (hour > 12) {
				LCD_pos(1, 0);
				LCD_STR(PM);
				LCD_pos(1, 2);
				LCD_CHAR(' ');
				LCD_pos(1, 3);
				LCD_CHAR((hour - 12) / 10 + '0');
				LCD_CHAR((hour - 12) % 10 + '0');
				LCD_CHAR(':');
				} else {
				LCD_pos(1, 0);
				LCD_STR(AM);
				LCD_pos(1, 2);
				LCD_CHAR(' ');
				LCD_pos(1, 3);
				LCD_CHAR(hour / 10 + '0');
				LCD_CHAR(hour % 10 + '0');
				LCD_CHAR(':');
			}

			LCD_pos(1, 6);
			LCD_CHAR((min / 10) + '0');
			LCD_CHAR((min % 10) + '0');
			LCD_CHAR(':');
			LCD_pos(1, 9);
			LCD_CHAR((sec / 10) + '0');
			LCD_CHAR((sec % 10) + '0');
			break;
			case 1:
			sprintf(str, "Time:%s %02d:%02d:%02d", (alarm_hour < 12) ? AM : PM, alarm_hour%12, alarm_min, alarm_sec);
			LCD_pos(0, 0);
			LCD_STR(str);
			LCD_pos(1, 0);
			if (hour > 12) {
				LCD_pos(1, 0);
				LCD_STR(PM);
				LCD_pos(1, 2);
				LCD_CHAR(' ');
				LCD_pos(1, 3);
				LCD_CHAR((hour - 12) / 10 + '0');
				LCD_CHAR((hour - 12) % 10 + '0');
				LCD_CHAR(':');
				} else {
				LCD_pos(1, 0);
				LCD_STR(AM);
				LCD_pos(1, 2);
				LCD_CHAR(' ');
				LCD_pos(1, 3);
				LCD_CHAR(hour / 10 + '0');
				LCD_CHAR(hour % 10 + '0');
				LCD_CHAR(':');
			}

			LCD_pos(1, 6);
			LCD_CHAR((min / 10) + '0');
			LCD_CHAR((min % 10) + '0');
			LCD_CHAR(':');
			LCD_pos(1, 9);
			LCD_CHAR((sec / 10) + '0');
			LCD_CHAR((sec % 10) + '0');
			break;
			case 2:

			 str = "Stopwatch:               ";
			 LCD_pos(0, 0);
			 for (int i = 0; i < strlen(str); ++i)
			 {
				 LCD_pos(0,i);
				 LCD_CHAR(str[i]);
			 }

			LCD_pos(1, 0);
			LCD_CHAR((stop_hour / 10) + '0');
			LCD_CHAR((stop_hour % 10) + '0');
			LCD_CHAR(':');
			LCD_pos(1, 3);
			LCD_CHAR((stop_min / 10) + '0');
			LCD_CHAR((stop_min % 10) + '0');
			LCD_CHAR(':');
			LCD_pos(1, 6);
			LCD_CHAR((stop_sec / 10) + '0');
			LCD_CHAR((stop_sec % 10) + '0');
			LCD_CHAR('.');
			LCD_pos(1, 9);
			LCD_CHAR((stop_mil_sec / 10) + '0');
			LCD_CHAR((stop_mil_sec % 10) + '0');


			break;
			case 3:
			str = "";
			sprintf(str, "%04d %02d/%02d %s", temp_year, temp_day, temp_month, get_weekday(temp_year, temp_day, temp_month));
			LCD_pos(0, 0);
			LCD_STR(str);
			LCD_pos(1, 0);

			if (temp_hour > 12) {
				LCD_pos(1, 0);
				LCD_STR(PM);
				LCD_pos(1, 2);
				LCD_CHAR(' ');
				LCD_pos(1, 3);
				LCD_CHAR((temp_hour - 12) / 10 + '0');
				LCD_CHAR((temp_hour - 12) % 10 + '0');
				LCD_CHAR(':');
				} else {
				LCD_pos(1, 0);
				LCD_STR(AM);
				LCD_pos(1, 2);
				LCD_CHAR(' ');
				LCD_pos(1, 3);
				LCD_CHAR(temp_hour / 10 + '0');
				LCD_CHAR(temp_hour % 10 + '0');
				LCD_CHAR(':');
			}

			LCD_pos(1, 6);
			LCD_CHAR((temp_min / 10) + '0');
			LCD_CHAR((temp_min % 10) + '0');
			LCD_CHAR(':');
			LCD_pos(1, 9);
			LCD_CHAR((temp_sec / 10) + '0');
			LCD_CHAR((temp_sec % 10) + '0');
			break;
			case 4:
			str = "";
			sprintf(str, "Time:%s %02d:%02d:%02d", (alarm_hour < 12) ? AM : PM, temp_alarm_hour%12, temp_alarm_min, temp_alarm_sec);
			LCD_pos(0, 0);
			LCD_STR(str);
			LCD_pos(1, 0);
			LCD_STR("                         ");
			break;
			case 100:
			LCD_pos(0, 0);
			LCD_STR("Mode:                        ");
			if (temp_mode == 0) {
				LCD_pos(1, 0);
				LCD_STR("Time                     ");
			}
			else if (temp_mode == 1) {
				LCD_pos(1, 0);
				LCD_STR("Alarm                    ");
			}
			else if (temp_mode == 2) {
				LCD_pos(1, 0);
				LCD_STR("Stopwatch                ");
			}
			else if (temp_mode == 3) {
				LCD_pos(1, 0);
				LCD_STR("Set Time                 ");
			}
			else if (temp_mode == 4) {
				LCD_pos(1, 0);
				LCD_STR("Set Alarm                ");
			}

			break;
			default:
			LCD_Clear();
			break;
		}
		if (hour == alarm_hour && min == alarm_min && sec == alarm_sec)
		{
			int i = 0;
			for (; i < 5; i++)
			{
				char LED;
				LED = 0xff; // 1111 1111
				PORTB = LED;
				_delay_ms(300);
				LED = 0x00; //0000 0000
				PORTB = LED;
				_delay_ms(300);
			}
		}
	}
}
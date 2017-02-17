/*
*Created: January 2017
*Author: Michal Malujdy
*Description: Program is designed for microcontroled ADuC831. The microcontroler guards correct fluid level in a tank using hysteresis.
With a start of the program there is a need to pass three informations via UART: a height of a tank, an upper desired fluid level and a lower desired fluid level.
Then the main loop starts. It checks the current fluid level via analog-digital converter. If the current fluid level falls below the lower desired
fluid level, a pump is powered by the PWM from the ADuC831 until the current fluid level reaches the upper desired fluid level. The current fluid level
is shown on LCD display.
*/
#include "initialization.h"

struct tank_parameters{
	float height;
	float level_min;
	float level_max;
	float current_level;
};

struct tank_parameters tank;

#define ADC_V_REF 2.5
#define ADC_REGISTER_VALUE_MAX 4095

void start_pumping_fluid()	
{
	//Turns PWM on by setting bits MD2 MD1 MD0 to respectively 0 0 1. Because MD2 and MD1 does not change its values, changing MD0 to 1 is sufficient
	PWMCON |= (1 << 4); 
}

void stop_pumping_fluid()
{
	//Turns PWM on by setting bits MD2 MD1 MD0 to respectively 0 0 0. Because MD2 and MD1 does not change its values, changing MD0 to 0 is sufficient
	PWMCON &= ~(1 << 4);
}

//Function called in ADC interrupt set. It checks current fluid level and turns PWM on or off if necessery
void manage_PWM()
{
		if(tank.current_level >= tank.level_max)
		{
			stop_pumping_fluid();
		}
		else if(tank.current_level <= tank.level_min)
		{
			start_pumping_fluid();
		}
}

void handle_ADC() interrupt 6
{
	float current_voltage_level;
	int ADC_register_value;
	char current_fluid_level_char [8];
	
	//Get the current voltage level from 2 8bit registers 
	int ADC_lower_register = ADCDATAL;
	int ADC_upper_register = ADCDATAH & 0x0F; //The first 4 bits are set to 0 because they does not //Kasowane sa pierwsze 4 bity, gdyz nie przechowuja one wartosci napiecia, a nr kanalu
	
	//Two separate 8bit values form ADC register is merged into one 16bit value
	ADC_register_value = ADC_upper_register << 8;
	ADC_register_value += ADC_lower_register;
	
	//Calculates input voltage based on register values
	current_voltage_level = ADC_V_REF * (float) ((float) ADC_register_value / (float) ADC_REGISTER_VALUE_MAX);
	
	tank.current_level = tank.height / 2.5f * current_voltage_level;
	
	//Conversion fluid level from float to char so that it can be displayed
	sprintf (current_fluid_level_char, "%3f", tank.current_level);
	LCD_WyswietlSlowo(current_fluid_level_char,0);
	
	//Depending on currently calculated fluid level turn PWM on or off
	manage_PWM();
}

void main()
{
	//A bit more complex function, sets PWM, ADC, UART, send commands to user and recieve the size of the tank. The body is placed in initialization.c
	initialize_modules (&(tank.height), &(tank.level_min), &(tank.level_max));
	
	while(1);
}
#include "LCD.h"

void LCD_send(char text,char RS,char RW, int delay)
{
	LCD_RS=RS;
	LCD_RW=RW;
	LCD_Dane=text;
	LCD_E=1;
	LCD_E=0;
	delay_us(delay);
}

void LCD_start()
{
	I2CM=1;
	MDE=1;
	
	//There is a need to send a few times some data to ensure whether the connection is correct
	delay_us(1500);
	LCD_send(0x38,0,0,40);
	delay_us(4000);
	LCD_send(0x38,0,0,40);
	delay_us(1000);
	//End of initialization
	
	LCD_send(0x38,0,0,40);
	LCD_send(0x0F,0,0,40);//Turn the display on
	LCD_send(0x01,0,0,1600);//Wipe the display
	LCD_send(0x02,0,0,1600);
	
}

void LCD_display_char(char single_char)
{
	LCD_Send(single_char,1,0,40);
}

//Set the cursor in the desired place
void LCD_adressDD(char address)
{
	//Going to a new line in 16 segment display
	if(address>16)
	{
		address+=0x32;
	}
	
	LCD_send(0x80+address,0,0,40);
}

void LCD_display_word(char* word,char start)
{
	int i;
	int length;
	
	//Check for the length of the word
	length = strlen(word);
	
	//Send every single char from the word separately
	for(i=0; i<length; i++)
	{
		LCD_adressDD(i + start);
		LCD_display_char(word[i]);
	}
}

//This function forces a short delay because speed of connection between ADuC and display is
//much slower than ADuC processor
void delay_us(int delay)
{
	int i;
	for(i=0;i<delay;i++);
}
#ifndef LCD_H
#define LCD_H

#include <aduc831.h>
#include <string.h>
#include <stdio.h>
#define LCD_RS MCO
#define LCD_RW MDO
#define LCD_Dane P0
sbit LCD_E=P2^0;

void delay_us(int delay);
void LCD_Send(char text,char RS,char RW, int delay);
void LCD_Start();
void LCD_Display_Char(char single_char);
void LCD_AdressDD(char address);
void LCD_WyswietlSlowo(char* word,char start);
	
#endif
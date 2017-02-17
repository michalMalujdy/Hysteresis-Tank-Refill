#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <stdlib.h>
#include <stdio.h>
#include "LCD.h"

struct UART_processing
{
	int is_char_sent; //Flag determining whether a single char has been sent
	int is_data_recieved; //Flag determining whether a single char has been recieved
	char recieved_data_char [10]; //Recieved value from user in char format
	int recieved_char_number; //It is used to place recieved char at a correct index in recieved_data_char
};

void enable_interruptions();
void set_PWM();
void set_Timer2();
void set_ADC();
void set_UART();
void handle_UART(); //Interrupt function from UART. Is fired every time when somthing is recievied or send by UART
float show_command_and_get_value(char command[], int elements_number);
void clear_array(char* array, int array_size);
void initialize_modules(float *height, float *level_min, float *level_max); //This function is called in main() to set PWM, ADC, UART, send commands to user and recieve the size of the tank
#endif
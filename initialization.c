#include "initialization.h"

struct UART_processing UART_param;

void enable_interruptions()
{	
	EA = 1; //Global permission
	ES = 1; //From UART
	EADC = 1; //From ADC
}

void set_PWM()
{
	//Sets PWM in a single mode in resolution 16bit without dividers. The PWM clock frequency equals oscillator frequency. At first the PWM is shut down so bits MD2 MD1 MD0 are set respectively to 0 0 0
	PWMCON = 0x03; //SNGL=0 MD2=0 MD1=0 MD0=0 CDIV1=0 CDIV2=0 CSEL1=1 CSEL0=1
	
	//Frequency of the PWM is set to 500Hz
	PWM1L = 0x64;
	PWM1H = 0x56;
	
	//The duty cycle is set to 75%
	PWM0L = 0xCB;
	PWM0H = 0x40;	
	
	CFG831 = 0x00; //No dividers
}

void set_Timer2()
{
	TMOD = 1;
	
	//Interrupt called every 20msc
	RCAP2H = 0xB8;
	RCAP2L = 0x00;
	
	//The timer does not start yet. First there is a need to get data from user about the tank
	TR2 = 0;
}

void set_ADC()
{
	ADCCON1 = 0xA6; //MD1=1; EXT_REF=0; CKx=0x10; AQx=0x01; T2C=1; EXC=0;
	ADCCON2 = (ADCCON2 & 0x0F) | 0x03; //The voltage will be measured on third channel
}

void set_UART()
{
	//Sets UART to mode 1 (8bit, variable baud rate) and allows for reception
	SCON = 0x50;// SM0=0 SM1=1 SM2=0 REN=1 TB8=0 RB8=0 TI=0 R1=0 
	
	//Set the baudrate on 115200 executing by Timer3
	T3CON = 0x81;
	T3FD = 0x20;
}

//Interrupt function from UART. Is fired every time when somthing is recievied or send by UART
void handle_UART() interrupt 4
{
	//RI is true when something is recieved by UART
	if(RI)
	{
		//SBUF is ADuC831 a register that contains recieved char
		char recieved_char = (char)SBUF;
		RI = 0;
		
		//If recievied Char is a ';' then it is an end of the sentence
		if(recieved_char == 59)
		{					
			UART_param.is_data_recieved = 1;
			UART_param.recieved_char_number = 0;
		}
		else
		{	
			UART_param.recieved_data_char[UART_param.recieved_char_number] = recieved_char;
			UART_param.recieved_char_number++;			
		}
	}
	
	//TI is true when something is sent by UART
	if(TI)
	{
		TI = 0;
		UART_param.is_char_sent = 1;
	}
	return;
}

//This function sends the command via UART and holds the whole program until the user sends back a value ended with ';'. Then it converts the value from char to float by atof function and returns it
float show_command_and_get_value(char command[], int elements_number)
{
	float user_value_float;
		
	//A Loop responsible for sending the whole command by sending single char from command[]. After that awaits the confirmation of sending it controlled by variable char_sent
	int i = 0;
	for(i; i < elements_number; i++)
	{
		UART_param.is_char_sent = 0;
		SBUF = command[i];
		
		while(!UART_param.is_char_sent);
	}	
	
	//Set the carriage in a new line in case of another command
	SBUF = (char) 13;	
	while(!UART_param.is_char_sent);
	
	//Holds the whole program until the user passes data finished with ';'
	while(!UART_param.is_data_recieved);
	UART_param.is_data_recieved = 0;	
	
	user_value_float = atof(UART_param.recieved_data_char); //Conversion from recieved_data_char[] to user_value_float	
	
	clear_array(UART_param.recieved_data_char, 10); //Clears the recieved_data_char[] so that when new data is recieved, it does not collide with its previous content
	
	return user_value_float;
}

void clear_array(char* array, int array_size)
{
	int i = 0;
	for(i; i < array_size; i++)
	{
		array[i] = 0;
	}
}

void initialize_modules(float *height, float *level_min, float *level_max)
{
	float any_key_to_start = 0;
	
	enable_interruptions();
	
	LCD_Start();	
	set_PWM();
	set_Timer2();
	set_ADC();
	set_UART();
	
	//This command holds the program until the user is ready, before sending actual commands requesting demensions
	while(!any_key_to_start)
	{
		any_key_to_start = show_command_and_get_value("Send any key", 12);
	}	
	
	//Sending three commands and recieving neccessary data
	*height = show_command_and_get_value("Set the height of the tank", 26);
	*level_max = show_command_and_get_value("Set the maximum level", 21);
	*level_min = show_command_and_get_value("Set the minimum level", 21);
	
	TR2 = 1;//Wlaczenie Timer2 a przez to uruchomienie dzialania przetwornika ADC
}
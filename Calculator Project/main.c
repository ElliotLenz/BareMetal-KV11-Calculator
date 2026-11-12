#include "MKV11Z7.h"
#include "lcd_hd44780.h"
#include "keypad_4x4.h"
#include <stdio.h>

//Maximum Digits for each number allowed on Calculator
#define MAX_DIGITS 3

int main(void){
		//Turn on clock for PORTC, PORTA, and PORTD
		SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;
		
		//Initialize Pins
		Keypad_Init();
		LCD_Init();
		
		//Clear screen
		LCD_Clear();
		
		char current_key = '\0';
		char last_key = '\0';
		
		//=================================================================
		//State Machine
		//=================================================================
		//Variables
		int num1 = 0;
		int num2 = 0;
		int digits1 = 0;
		int digits2 = 0;
		char math_operator = '\0';
		int calc_state = 1; // State1: Entering num1 | State2: Entering num2 | State3: Result shown
		int result = 0;
		char display_buffer[16];
		
		while(1){
						//Read keypad
						current_key = Keypad_Scan();
						
						//If a key is pressed AND it is a new key pressed
						if(current_key != '\0' && current_key != last_key){
								
								//=====Numbers=====
								if(current_key >= '0' && current_key <= '9') {
										
										//Auto Reset after equation finish
										if (calc_state == 3){
												LCD_Clear();
												num1 = 0;
												num2 = 0;
												digits1 = 0;
												digits2 = 0;
												math_operator = '\0';
												calc_state = 1;
												result = 0;
										}
										
										//Building first number		
										if(calc_state == 1 && digits1 < MAX_DIGITS){
														num1 = (num1 * 10) + (current_key - '0');
														digits1++;
														//Display numbers on screen
														LCD_WriteChar(current_key);
										} 
									
										//Building second number	
										else if (calc_state == 2 && digits2 < MAX_DIGITS){
														num2 = (num2 * 10) + (current_key - '0');
														digits2++;
														//Display numbers on screen
														LCD_WriteChar(current_key);
										}
								}
								//=====Operators=====	
								//Addition
								else if (current_key == 'A' && calc_state == 1 && digits1 > 0){
												math_operator = '+';
												calc_state = 2;
												LCD_WriteChar('+');
								}
							
								//Subtraction
								else if (current_key == 'B' && calc_state == 1 && digits1 > 0){
												math_operator = '-';
												calc_state = 2;
												LCD_WriteChar('-');
								}
							
								//Division
								else if (current_key == 'D' && calc_state == 1 && digits1 > 0){
												math_operator = '/';
												calc_state = 2;
												LCD_WriteChar('/');
								}
							
								//Multiplication
								else if (current_key == '*' && calc_state == 1 && digits1 > 0){
												math_operator = '*';
												calc_state = 2;
												LCD_WriteChar('*');
								}
							
								//=====Clear=====
								else if (current_key == 'C'){
												//Clear screen, return cursor to home, and resets all variables
												LCD_Clear();
												num1 = 0;
												num2 = 0;
												digits1 = 0;
												digits2 = 0;
												math_operator = '\0';
												calc_state = 1;
												result = 0;
								}
							
								//=====Equal=====
								else if (current_key == '#' && calc_state == 2 && digits2 > 0){
												//Create flag to track error in div
												int div_error = 0;
									
												//Calculate Result
												if (math_operator == '+'){
																result = num1 + num2;
												}
												else if (math_operator == '-'){
																result = num1 - num2;
												}
												else if (math_operator == '/'){
																if (num2 != 0) {
																		result = num1 / num2;
																} else {
																				//Trigger div flag
																				div_error = 1;
																}
												}
												else if (math_operator == '*'){
																result = num1 * num2;
												}
												// Send cursor to second row
												LCD_SetCursor(1, 0);
											
												//Check flag to decide what is displayed
												if (div_error == 1) {
																//Load error text
																sprintf(display_buffer, "ERROR"); 
												} else {
																//Load normal math result
																sprintf(display_buffer, "%d", result);
												}
												//Print results
												LCD_WriteString(display_buffer);
												
												//Tell state machine equation is done
												calc_state = 3;
								}
							
								//Debounce delay to prevent physical button bouncing
								for(volatile int i = 0; i < 50000; i++);
								}
							//Update last_key so we do not print 10000 times
							last_key = current_key;
						}
	}


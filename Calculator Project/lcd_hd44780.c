#include "MKV11Z7.h"
#include "lcd_hd44780.h"

//=================================================================
// LCD Hardware Definitions
//=================================================================
//LCD Port D Pins
#define RS 5
#define E 4
#define D4 0
#define D5 1
#define D6 2
#define D7 3

//LCD Port A Pins
#define RW 1

//=================================================================
//LCD Driver Functions
//=================================================================
//LCD_Write_Nibble
//Sends a 4-bit half-byte to the LCD data pins (D4-D7).
//Pulses the Enable (E) pin to create a falling edge, latching the data.
static void LCD_Write_Nibble(uint8_t nibble) {
	//Clears the old data from Pins D4-D7
	GPIOD->PCOR = 0x0F;
	
	//Writes the new 4-bit nibble to Pins D4-D7
	GPIOD->PSOR = (nibble & 0x0F);
	
	//Drive E pin to HIGH, to tell LCD to prepare for incoming data
	GPIOD->PSOR = (1 << E);
	
	//Delay, so screen registers signal
	for(volatile int i = 0; i < 1000; i++);
	
	//Drive E pin to LOW, creates a falling edge latching data into the LCD
	GPIOD->PCOR = (1 << E);
}

//LCD_Command
//Sends an 8-bit instruction to the LCD.
//Drives RS LOW to indicate an instruction, then splits the 8-bit command
//into two 4-bit nibbles for transmission.
static void LCD_Command(uint8_t cmd){
	//Drive RS pin to LOW, treat incoming data as a command
	GPIOD->PCOR = (1 << RS);
	
	//Drive RW pin to LOW, tells LCD we are writing data not reading
	GPIOA->PCOR = (1 << RW);
	
	//Splits 8-bit command into two 4-bits using Nibble
	LCD_Write_Nibble(cmd >> 4);
	LCD_Write_Nibble(cmd);
	
	//Wait for LCD to finish processing instruction
	for(volatile int i = 0; i < 100000; i++);
}

//LCD_Data
//Sends an 8-bit character to be printed on the LCD.
//Drives RS HIGH to indicate character data, then splits the 8-bit byte
//into two 4-bit nibbles for transmission.
static void LCD_Data(uint8_t data){
	//Drive RS pin to HIGH, treat outgoing data as a character to display
	GPIOD->PSOR = (1 << RS);
	
	//Drive RW pin to LOW, tells LCD we are writing data not reading
	GPIOA->PCOR = (1 << RW);
	
	//Splits 8-bit command into two 4-bits using Nibble
	LCD_Write_Nibble(data >> 4);
	LCD_Write_Nibble(data);
	
	//Wait for LCD to finish processing instruction
	for(volatile int i = 0; i < 100000; i++);
}

//LCD_Init
//Configures Port D and Port A pins for 4-bit LCD.
//Sets Data lines (D4-D7), Register Select (RS), Read/Write (RW),
//and Enable (E) as GPIO outputs.
void LCD_Init(void){
	//Configure LCD as GPIO
	PORTD->PCR[RS] = PORT_PCR_MUX(1);
	PORTD->PCR[E] = PORT_PCR_MUX(1);
	PORTD->PCR[D4] = PORT_PCR_MUX(1);
	PORTD->PCR[D5] = PORT_PCR_MUX(1);
	PORTD->PCR[D6] = PORT_PCR_MUX(1);
	PORTD->PCR[D7] = PORT_PCR_MUX(1);
	PORTA->PCR[RW] = PORT_PCR_MUX(1);
	
	//Set Data Direction for Port D Pins
	GPIOD->PDDR |= (1 << RS) | (1 << E) | (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7);
	
	//Set Data Direction for Port A Pins
	GPIOA->PDDR |= (1 << RW);
	
	//Drive RS and RW LOW so screen knows boot sequence is a command
	GPIOD->PCOR = (1 << RS);
	GPIOA->PCOR = (1 << RW);
	
	//Enable 4-bit mode
	LCD_Write_Nibble(0x03);
	for(volatile int i = 0; i <50000; i++);
	LCD_Write_Nibble(0x03);
	for(volatile int i = 0; i <50000; i++);
	LCD_Write_Nibble(0x03);
	for(volatile int i = 0; i <50000; i++);
	LCD_Write_Nibble(0x02);
	for(volatile int i = 0; i <50000; i++);
	
	//Boot-up Sequence
	LCD_Command(0x28); //Set 4-bit mode, 2 lines, 5x8 font
	LCD_Command(0x0C); //Display ON, Cursor OFF
	LCD_Command(0x01); //Clear Screen
	LCD_Command(0x06); //Entry mode
}
//Clears the LCD screen and returns the cursor to the home position.
void LCD_Clear(void){
	LCD_Command(0x01);
}

//Moves the cursor to a specific coordinate on the screen.
void LCD_SetCursor(uint8_t row, uint8_t col){
	uint8_t address = (row == 0) ? col : (0x40 + col);
	LCD_Command(0x80 | address);
}

//Prints a single ASCII character at the current cursor position.
void LCD_WriteChar(char c){
	LCD_Data((uint8_t)c);
}

//Prints a null-terminated string starting at the current cursor position.
void LCD_WriteString(const char* str){
	int i = 0;
	while (str[i] != '\0'){
		LCD_WriteChar(str[i]);
		i++;
	}
}
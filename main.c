#include "MKV11Z7.h"

//=================================================================
//Hardware Definitions
//=================================================================
//Keypad Port C Pins
#define ROW0 0
#define ROW1 1
#define ROW2 2
#define ROW3 3
#define COL0 4
#define COL1 5
#define COL2 6
#define COL3 7

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
//Function Prototypes
//=================================================================
void Keypad_Init(void);
void LCD_Init(void);
void LCD_Write_Nibble(uint8_t nibble);
void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
char Keypad_Scan(void);

//=================================================================
//Hardware Intialization
//=================================================================
/*
 *Keypad_Init
 *Configures Port C pins for a 4x4 matrix keypad
 *Sets Rows 0-3 as outputs to drive signals and Columns 0-3 as inputs
 *with internal pull-up resistors to read button presses.
 */
void Keypad_Init(void){
	//Configure Rows (Outputs) as GPIO
	PORTC->PCR[ROW0] = PORT_PCR_MUX(1);
	PORTC->PCR[ROW1] = PORT_PCR_MUX(1);
	PORTC->PCR[ROW2] = PORT_PCR_MUX(1);
	PORTC->PCR[ROW3] = PORT_PCR_MUX(1);
	
	//Configure Coloumns (Inputs) as GPIO
	//And enable internal pull-up resistors so inputs idle HIGH when unpressed
	PORTC->PCR[COL0] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[COL1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[COL2] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[COL3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	
	//Set Data Direction
	//Rows set to Outputs
	GPIOC->PDDR |= (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
	
	//Columns set to Inputs
	GPIOC->PDDR &= ~(1 << COL0) | (1 << COL1) | (1 << COL2) | (1 << COL3);
}

/*
 *LCD_Init
 *Configures Port D and Port A pins for 4-bit LCD.
 *Sets Data lines (D4-D7), Register Select (RS), Read/Write (RW),
 *and Enable (E) as GPIO ouputs.
 */
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
	GPIOA->PCOR = (1 << RS);
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

//=================================================================
//LCD Driver Functions
//=================================================================
/*
 *LCD_Write_Nibble
 *Sends a 4-bit half-byte to the LCD data pins (D4-D7).
 *Pulses the Enable (E) pin to creaate a falling edge, latching the data.
 */
void LCD_Write_Nibble(uint8_t nibble) {
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

/*
 *LCD_Command
 *Sends an 8-bit instruction to the LCD.
 *Drives RS LOW to indicate an instruction, then splits the 8-bit command
 *into two 4-bit nibbles for transmission.
 */
void LCD_Command(uint8_t cmd){
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

/*
 *LCD_Data
 *Sends an 8-bit character to be printed on the LCD.
 *Drives RS HIGH to indicate character data, then splits the 8-bit byte
 *into two 4-bit nibbles for transmission.
 */
void LCD_Data(uint8_t data){
	//Drive RS pin to HIGH, treat outgoing data as a command
	GPIOD->PSOR = (1 << RS);
	
	//Drive RW pin to LOW, tells LCD we are writing data not reading
	GPIOA->PCOR = (1 << RW);
	
	//Splits 8-bit command into two 4-bits using Nibble
	LCD_Write_Nibble(data >> 4);
	LCD_Write_Nibble(data);
	
	//Wait for LCD to finish processing instruction
	for(volatile int i = 0; i < 100000; i++);
}

//=================================================================
//Keypad Functions
//=================================================================
/*
 *Keypad_Scan
 *Sequentially drives one row LOW at a time and reads the columns.
 *Returns the mapped character of the pressed button or 0xFF if none.
 */
char Keypad_Scan(void){
	//Define the pins and character map
	const uint8_t rows[4] = {ROW0, ROW1, ROW2, ROW3};
	const uint8_t cols[4] = {COL0, COL1, COL2, COL3};
	
	const char key_map[4][4] = {
	{'1','4','7','*'},
	{'2','5','8','0'},
	{'3','6','9','#'},
	{'A','B','C','D'}
	};
	
	for (int r = 0; r < 4; r++) {
		//Drives All Rows 0, 1, 2, 3 to HIGH
		GPIOC->PSOR = (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
		//Drives only current row to LOW
		GPIOC->PCOR = (1 << rows[r]);
		
		//Small delay, to let voltage drop stabalize
		for(volatile int d = 0; d < 100; d++);
		
		for(int c = 0; c < 4; c++) {
			//Check if the current column is pulled low by button press
			if ((GPIOC->PDIR & (1 << cols[c])) == 0) {
			return key_map[r][c];
			}
		}
		
	}
	
	return '\0';
}

int main(){
	//Turn on clock for PORTC, PORTA, and PORTD
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTD_MASK;
	
	//Intialize Pins
	Keypad_Init();
	LCD_Init();
	
	//Clear screen
	LCD_Command(0x01);
	
	char current_key = '\0';
	char last_key = '\0';
	
	while(1){
		//Read keypad
		current_key = Keypad_Scan();
		
		//If a key is pressed AND it is a new key pressed
		if(current_key != '\0' && current_key != last_key) {
			
			if(current_key == 'C') {
			//Clear screen when C pressed (Also returns cursor to home)
				LCD_Command(0x01);
			} 
			else if (current_key == 'D'){
				//Return cursor to beginning of second row when D pressed
				LCD_Command(0xC0);
			} 
			else {
			//Print to LCD
			LCD_Data(current_key);
			};
			//Debounce delay to prevent physical button bouncing
			for(volatile int i = 0; i < 50000; i++);
		}
		//Update last_key so we dont print 10000 times
		last_key = current_key;
	}
}


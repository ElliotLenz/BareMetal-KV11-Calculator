#include "MKV11Z7.h"
#include "keypad_4x4.h"

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

//=================================================================
//Keypad Functions
//=================================================================
//Keypad_Scan
//Sequentially drives one row LOW at a time and reads the columns.
//Returns the mapped character of the pressed button or '\0' if none.
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
		
		//Small delay, to let voltage drop stabilize
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

//Keypad_Init
//Configures Port C pins for a 4x4 matrix keypad
//Sets Rows 0-3 as outputs to drive signals and Columns 0-3 as inputs
//with internal pull-up resistors to read button presses.
void Keypad_Init(void){
	//Configure Rows (Outputs) as GPIO
	PORTC->PCR[ROW0] = PORT_PCR_MUX(1);
	PORTC->PCR[ROW1] = PORT_PCR_MUX(1);
	PORTC->PCR[ROW2] = PORT_PCR_MUX(1);
	PORTC->PCR[ROW3] = PORT_PCR_MUX(1);
	
	//Configure Columns (Inputs) as GPIO
	//And enable internal pull-up resistors so inputs idle HIGH when unpressed
	PORTC->PCR[COL0] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[COL1] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[COL2] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTC->PCR[COL3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	
	//Set Data Direction
	//Rows set to Outputs
	GPIOC->PDDR |= (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
	
	//Columns set to Inputs
	GPIOC->PDDR &= ~((1 << COL0) | (1 << COL1) | (1 << COL2) | (1 << COL3));
}
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
//Hardware Intialization
//=================================================================
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
}

void LCD_Command(uint8_t cmd){
	//Drive RS pin to LOW, treat incoming data as a command
	GPIOD->PCOR = (1 << RS);
	//Drive RW pin to LOW, tells LCD we are writing data not reading
	GPIOA->PCOR = (1 << RW);
}
void LCD_Data(void){

}

void Keypad_Scan(void){

}

int main(){
	//Turn on clock for PORTC
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	//Intialize Pins
	Keypad_Init();
	
	while(1){
	
	}
}


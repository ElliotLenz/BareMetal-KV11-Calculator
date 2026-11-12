#ifndef KEYPAD_4X4_H
#define KEYPAD_4X4_H

//Initializes the GPIO pins (rows as outputs, columns as inputs with pull-ups)
void Keypad_Init(void);

//Scans the matrix and returns the pressed character, or '\0' if nothing is pressed
char Keypad_Scan(void);

#endif
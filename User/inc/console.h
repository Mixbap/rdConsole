#ifndef CONSOLE_H
#define CONSOLE_H

#include "math.h"
#include "ini.h"

uint8_t transferLine[] = "\r\n";
uint8_t cursor[] = "\n>> ";

extern uint8_t flagConsole;

extern rdParam param;

void runConsole(void);
void printMenu(void);
uint8_t readData(void);
uint8_t interpret(uint8_t value);
uint8_t dataInterpret(uint8_t* data, uint8_t idx);

#endif


#ifndef CONSOLE_H
#define CONSOLE_H

#include "math.h"
#include "ini.h"

uint8_t buf0[20];
uint8_t buf1[20] = "DMA1\r\n";
uint8_t buf2[20] = "DMA2\r\n";
uint8_t buf3[20] = "\r\n";

extern uint8_t flagConsole;

void runConsole(void);
void printMenu(void);
uint8_t readData(void);

#endif


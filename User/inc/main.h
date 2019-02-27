#ifndef MAIN_H
#define MAIN_H

#include "ini.h"
#include "it.h"

uint16_t RD = 0;
uint8_t Flag_IRQ = 0;
uint32_t n = 0;

uint8_t firstIrq = 0;
uint8_t flagConsole = 0;

rdParam param;

#define DISTANCE_VALUE 3
uint16_t distanceArr[DISTANCE_VALUE];
uint8_t indDistance = 0;
int distance = 0;

void checkLimits(void);
void distanceMode(void);



#endif

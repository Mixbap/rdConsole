#ifndef MAIN_H
#define MAIN_H

#include "ini.h"
#include "it.h"

uint32_t RD = 0;
uint8_t Flag_IRQ = 0;
uint32_t n = 0;

uint8_t buf0[20];
uint8_t buf1[20] = "DMA1\r\n";
uint8_t buf2[20] = "DMA2\r\n";

uint8_t buf3[20] = "\r\n";

#endif

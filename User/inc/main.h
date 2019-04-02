// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: main.h 
// Назначение: Анализ сигнала биений радиолокационного датчика и формирование модулирующего сигнала  
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************
#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#define DISTANCE_VALUE 3

extern uint32_t n;
extern uint32_t countStartLCD;

extern uint8_t flagConsole;

extern uint16_t distanceArr[DISTANCE_VALUE];
extern uint8_t indDistance;
extern int distance;

extern void checkLimits(void);
extern void distanceMode(void);
#endif

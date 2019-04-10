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
void checkLimits(int localRD, uint32_t* n_ptr);
int distanceMode(uint16_t localRD);
#endif

// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: it.h 
// Назначение: Обработчик прерываний 
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************

#ifndef IT_H
#define IT_H
#include <stdint.h>

extern volatile uint16_t RD;
extern uint8_t Flag_IRQ;
extern uint8_t flagConsole;

extern void DMA_IRQHandler(void);
extern void EXT_INT2_IRQHandler(void);

#endif

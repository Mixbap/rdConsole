// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: console.h 
// Назначение: Терминал для настройка конфигурации и параметров блока 
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************

#ifndef CONSOLE_H
#define CONSOLE_H
#include <stdint.h>
enum typeError
{
	incorInp = -1, // Некорректный ввод данных
	typeModeError = -2, // Неверный тип модулирующего напряжения
	freqModError = -3, // Неверный ввод частоты модулирующего напряжения
	bufModeError = -4, // Неверный ввод размера буфера модулирующего напряжения
	amplModError = -5, // Неверный ввод амплитуды модулирующего напряжения
	freqBwError = -6, // Неверный ввод полосы пропускания
	coefAdjError = -7, // Неверный ввод коэффициента корректировки
	constModeError = -8, // Неверный ввод постоянной состовляющей модулирующего напряжения
	selectModeError = -9, // Неверный ввод режима работы
};

void runConsole(void);
void printMenu(void);
uint32_t readData(void);
uint8_t interpret(uint8_t value);
uint8_t deinterpret(uint8_t value);
uint32_t dataInterpret(uint8_t* data, uint8_t idx);
uint8_t dataDeinterpret(uint8_t* data, uint32_t value);

void selectMode(void);
void typeModHandler(void);
void freqModHandler(void);
void bufModeHandler(void);
void amplModHandler(void);
void freqBwHandler(void);
void limitAccHandler(void);
void coefAdjHandler(void);
void constModeHandler(void);

void getConfig(void);

void errorHandler(void);

#endif


#ifndef CONSOLE_H
#define CONSOLE_H

#include "math.h"
#include "ini.h"

uint8_t transferLine[] = "\r\n";
uint8_t cursor[] = "\n>> ";
uint8_t unsupCommand[] = "Unsupported command!\r\n\n";

extern uint8_t flagConsole;
extern rdParam param;

int error;

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
};

void runConsole(void);
void printMenu(void);
uint32_t readData(void);
uint8_t interpret(uint8_t value);
uint32_t dataInterpret(uint8_t* data, uint8_t idx);

void typeModHandler(void);
void freqModHandler(void);
void bufModeHandler(void);
void amplModHandler(void);
void freqBwHandler(void);
void limitAccHandler(void);
void coefAdjHandler(void);
void constModeHandler(void);

void errorHandler(void);

#endif


// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: console.c 
// Назначение: Терминал для настройка конфигурации и параметров блока 
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************
#include <stdlib.h>
#include <stdio.h>
#include "console.h"
#include "math.h"
#include "ini.h"
#include "main.h"

char transferLine[] = "\r\n";
char cursor[] = "\n>> ";

//--------------------------------------------------------------
// Запуск консоли
//--------------------------------------------------------------
void runConsole(rdParam* localParam)
{
	uint8_t active = 1;
	uint32_t menuItem;
	int isNumber;
	char defConf[] = "Set default configuraton succesfull!\r\n";
	char exit[] = "Exit terminal succesfull!\r\n";
	char unsupCommand[] = "Unsupported command!\r\n\n";
	DMA_ini(); 							// Инициализация DMA
	UART1_DMA_ini(); 	// Инициализация UART1+DMA TX RX

	while (active)
	{
		printMenu();

		menuItem = readData(&isNumber);
		if (isNumber == 0)
		{
			printError(incorInp);		
		}
		else
		{
			// Обработчик команд меню
			switch (menuItem)
			{
			case 1:
				selectMode(&(localParam->mode));
				break;
			case 2:
				typeModHandler(&(localParam->typeMod));
				break;
			case 3:
				freqModHandler(&(localParam->freqMod));
				break;
			case 4:
				bufModeHandler(&(localParam->bufMode));
				break;
			case 5:
				amplModHandler(&(localParam->amplMod), localParam->typeMod);
				break;
			case 6:
				constModeHandler(&(localParam->constMode));
				break;
			case 7:
				freqBwHandler(&(localParam->freqBw0), &(localParam->freqBw1));
				break;
			case 8:
				freqBwRealHandler(&(localParam->freqBw0), &(localParam->freqBw1), 
									localParam->freqMod, localParam->coefAdj);
				break;
			case 9:
				limitAccHandler(&(localParam->limitAcc));
				break;
			case 10:
				coefAdjHandler(&(localParam->coefAdj));
				break;
			case 11:
				rdParamDefIni(localParam);
				WriteStringDMA(defConf);
				printConfig(*localParam);
				break;
			case 12:
				printConfig(*localParam);
				break;
			case 13:
				WriteStringDMA(exit);
				active = 0;
				break;
			default:
				WriteStringDMA(unsupCommand);				
				break;
			}
		}
	}

	UART_Cmd(MDR_UART1, DISABLE); // для нормальной работы ЦАП и ДМА	
}

//--------------------------------------------------------------
// Выводит сообщение в порт без перевода строки
//  принимает:
//    Message - сообщение для вывода
//--------------------------------------------------------------
void WriteString(char Message[])
{
	int i = 0;
	char buf[100] = {0};
	while (Message[i] != '\0')
		{
			buf[i] = Message[i];	
			i++;
		}
	WriteStringDMA(buf);
}

//--------------------------------------------------------------
// Выводит сообщение в порт с завершающим знаком new_line
//  Принимает:
//    Message - сообщение для вывода, без завершающего знака
//--------------------------------------------------------------
void WriteLine(char Message[])
{
	int i = 0;
	char buf[100] = {0};
	while (Message[i] != '\0')
		{
			buf[i] = Message[i];	
			i++;
		}	
	WriteStringDMA(buf);
	WriteStringDMA(transferLine);
}

//-----------------------------------------------------------------------------
// Выводит целое число в порт
//-----------------------------------------------------------------------------
void WriteInt(uint32_t value)
{
	char data[17] = {0};
	sprintf(data, "%d", value);
	WriteStringDMA(data);
}

//-----------------------------------------------------------------------------
// Выводит целое число в порт с переводом строки в конце
//-----------------------------------------------------------------------------
void WriteIntNl(uint32_t value)
{
	WriteInt(value);
	WriteStringDMA(transferLine);
}

//--------------------------------------------------------------
// Считывает из порта целое число,
//  isNumber - флаг того было ли введено число, 
//    0 - присутствовали символы отличные от цефр
//    1 - во вводе присутствовали только цифры, число было корректно считано
// Возвращает:
//   Считанное из порта число
//--------------------------------------------------------------
uint32_t readData(int* isNumber)
{
	const int bufSize = 21;
	char data[bufSize] = {0};
	int i;
	uint32_t res = 0;
	
	ReadStringDMA(data, bufSize);
	WriteStringDMA(transferLine);	
	*isNumber = 1;
	for (i = 0 ; data[i] != '\0'; i++)
	{

		if ( (data[i] < '0' || data[i] > '9') && (data[i]!='\r' && data[i] != '\n') ) 
			{
				*isNumber = 0;
				break;
			}

	}
	res = atoi(data);				
	return res;
}

//--------------------------------------------------------------
// Считывает из порта вещественное число,
//  isNumber - флаг того было ли введено число, 
//    0 - присутствовали символы отличные от цефр
//    1 - во вводе присутствовали только цифры, число было корректно считано
// Возвращает:
//   Считанное из порта число
//--------------------------------------------------------------
float readDataFloat(int* isNumber)
{
	const int bufSize = 21;
	char data[bufSize] = {0};
	int i;
	float res = 0;
	
	ReadStringDMA(data, bufSize);
	WriteStringDMA(transferLine);	
	*isNumber = 1;
	for (i = 0 ; data[i] != '\0'; i++)
	{
		if (data[i] > '0' || data[i] < '9' || data[i] == '.' || data[i] == '\r' || data[i] == '\n')
		;
		else
		{
			*isNumber = 0;
			break;
		}				
	}
	res = atof(data);				
	return res;
}

//--------------------------------------------------------------
// Вывод меню
//--------------------------------------------------------------
void printMenu(void)
{	
	WriteLine("\n\tTerminal of the block of processing of the radio sensor.");
	WriteLine(" [1] Select mode");
	WriteLine(" [2] Set type of the modulating voltage");
	WriteLine(" [3] Set frequency of the modulating voltage");
	WriteLine(" [4] Set size of the buffer of counting");
	WriteLine(" [5] Set maximum amplitude of the modulating voltage");
	WriteLine(" [6] Set constant of the modulating voltage");
	WriteLine(" [7] Set bw freq");
	WriteLine(" [8] Set bw Real freq");
	WriteLine(" [9] Set limit accumulation");
	WriteLine("[10] Set adjustment coefficient");
	WriteLine("[11] Set default configuration");
	WriteLine("[12] Get configuration");
	WriteLine("[13] Exit terminal");
	WriteString("\r\n>> ");
}

//--------------------------------------------------------------
// Выбор режима работы
// 	изменяет режим работы mode при корректном выборе режима, при некорректном значении повторяет ввод
//  осуществляет вывод сообщений об ошибке в COM порт
// Возвращает :
//  выбранный режим работы (1 или 2)
//--------------------------------------------------------------
int selectMode(uint32_t* mode)
{
	uint32_t result;
	int isNumber;
	char selMode[] = "Select mode:\r\n[1] Radio senser\r\n[2] Distance measurement\r\n";
	char selModePrint[] = "Mode: ";
	char senser[] = "Radio senser\r\n";
	char dist[] = "Distance measurement\r\n";
	
	while(1)
	{
		WriteStringDMA(selMode);
		WriteStringDMA(cursor);		
		result = readData(&isNumber);
		
		// Обработка ошибки
		if (isNumber == 0)
		{
			printError(incorInp);			
		}
		else if ((result < 1) || (result > 2))
		{			
			printError(selectModeError);			
		}
		else
		{
			*mode = result;			
			// Вывод режима
			WriteStringDMA(selModePrint);
			//DMA_TX_start(selModePrint, sizeof(selModePrint));
			if (result == 1)
				WriteStringDMA(senser);				
			else if (result == 2)
				WriteStringDMA(dist);				
			return result;
		}
	}
}

//--------------------------------------------------------------
// Задание формы модулирующего напряжения
// 	изменяет параметр typeMod, при некорректном значении, 
//  повторный запрос ввода с сообщением об ошибке
// Возвращает:
//   Введенное значение
//--------------------------------------------------------------
int typeModHandler(uint32_t* typeMod)
{
	uint32_t result;
	int isNumber;
	char mode[] = "Type of the modulating voltage:\r\n[1] Sinus\r\n[2] Saw\r\n[3] Triangle\r\n";
	char modePrint[] = "Type of the modulating voltage: ";
	char sineMode[] = "[1] Sine";
	char sawMode[] = "[2] Saw";
	char triangleMode[] = "[3] Triangle";
	
	while (1)
	{
		WriteStringDMA(mode);		
		WriteStringDMA(cursor);

		result = readData(&isNumber);
		
		// Обработка ошибки
		if (isNumber == 0)
		{
			printError(incorInp);				
		}
		else if ((result > 3) || (result < 1))
		{			
		 	printError(typeModeError);			
		}
		else 
		{
			*typeMod = result;						
			// Вывод typeMod
			WriteStringDMA(modePrint);
			switch (result)
			{
				case 1: {
					WriteStringDMA(sineMode);
					break;
				}
				case 2: {
					WriteStringDMA(sawMode);
					break;
				}
				case 3: {
					WriteStringDMA(triangleMode);
					break;
				}
				default:{
					break;
				}
			}			
			WriteStringDMA(transferLine);		
			return result;
		}
	}
}

//--------------------------------------------------------------
// Задание частоты модулирующего напряжения
//--------------------------------------------------------------
int freqModHandler(uint32_t* freqMod)
{
	uint32_t result;
	int isNumber;	
	char freq[] = "Enter frequency (Hz):\r\n";
	char freqPrint[] = "Frequency of the modulating voltage: ";
	
	while (1)
	{
		WriteStringDMA(freq);
		WriteStringDMA(cursor);

		result = readData(&isNumber);
		
		// Обработка ошибки
		if ( isNumber == 0)
		{
			printError(incorInp);		
		}
		else if (result > 200000)
		{			
			printError(freqModError);		
		}
		else
		{
			*freqMod = result;
			
			// Вывод freqMod
			WriteStringDMA(freqPrint);
			WriteIntNl(result);				
			return result;
		}
	}
}

//--------------------------------------------------------------
// Задание размера буфера отсчетов модулирующего напряжения
//--------------------------------------------------------------
int bufModeHandler(uint32_t* bufMode)
{
	uint32_t result;	
	int isNumber;
	char modeBuf[] = "Enter the buffer size:\r\n";
	char modeBufPrint[] = "Size of the buffer of counting: ";
	
	while (1)
	{
		WriteStringDMA(modeBuf);
		WriteStringDMA(cursor);

		result = readData(&isNumber);
		
		// Обработка ошибки
		if (isNumber == 0)
		{
			printError(incorInp);			
		}
		else if (result > 255)
		{			
			printError(bufModeError);
		}
		else
		{
			*bufMode = result;
			
			// Вывод bufMode
			WriteStringDMA(modeBufPrint);
			WriteIntNl(result);					
			return result;
		}
	}
}


//--------------------------------------------------------------
// Задание амплитуды модулирующего напряжения
//--------------------------------------------------------------
int amplModHandler(uint32_t* amplMod, uint32_t typeMod)
{
	uint32_t result;
	int isNumber;
	char modeAmp[] = "Enter amplitude:\r\n";
	char modeAmpPrint[] = "Maximum amplitude of the modulating voltage: ";
	
	while (1)
	{
		WriteStringDMA(modeAmp);
		WriteStringDMA(cursor);

		result = readData(&isNumber);
		
		// Обработка ошибки
		if (isNumber == 0)
		{
			printError(incorInp);		
		}
		else if (result > (typeMod == 1? 2047 : 4095) )
		{
			printError(amplModError);
		}
		else
		{
			*amplMod = result;
			// Вывод amplMod
			WriteStringDMA(modeAmpPrint);		
			WriteIntNl(result);		
			return result;
		}			
	}
}

//--------------------------------------------------------------
// Задание постоянной состовляющей модулирующего напряжения
//--------------------------------------------------------------
int constModeHandler(uint32_t* constModeParam)
{
	uint32_t result;
	int isNumber;
	char constMode[] = "Enter constant:\r\n";
	char constModePrint[] = "Constant of the modulating voltage: ";
	
	while (1)
	{
		WriteStringDMA(constMode);
		WriteStringDMA(cursor);
		
		result = readData(&isNumber);
		
		// Обработка ошибки
		if (isNumber < 0)
		{
			printError(incorInp);
		}
		else if (result > 4095)
		{			
			printError(constModeError);
		}
		else
		{
			*constModeParam = result;			
			// Вывод constMode
			WriteStringDMA(constModePrint);			
			WriteIntNl(result);			
			return result;
		}
	}
}

//--------------------------------------------------------------
// Задание полосы пропускания частоты биений
//--------------------------------------------------------------
int freqBwHandler(uint8_t* freqBw0Param, uint8_t* freqBw1Param)
{
	uint32_t result0, result1;
	int isNumber0, isNumber1;
	char freqBw0[] = "Set lower bound:\r\n";
	char freqBw1[] = "Set upper bound:\r\n";
	char freqBw[] = "Bandwidth of frequency of beats: [";
	char freqBwPoint[] = ",";
	char freqBwScob[] = "]";
	
	while (1)
	{
		WriteStringDMA(freqBw0);
		WriteStringDMA(cursor);		
		result0 = readData(&isNumber0);
		
		WriteStringDMA(freqBw1);
		WriteStringDMA(cursor);				
		result1 = readData(&isNumber1);

		// Обработка ошибки
		if (isNumber0 == 0 || isNumber1 == 0)
		{
			printError(incorInp);		
		}
		else if (result0 >= result1)
		{			
			printError(freqBwError);	
		}
		else if ((result0 > 255) || (result1 > 255))
		{			
		 	printError(freqBwError);
		}
		else
		{
			*freqBw0Param = result0;
			*freqBw1Param = result1;
			
			// Вывод freqBw
			WriteStringDMA(freqBw);
			WriteInt(result0);
			WriteStringDMA(freqBwPoint);	
			WriteInt(result1);
			WriteStringDMA(freqBwScob);
			WriteStringDMA(transferLine);			
			return result0;
		}
	}
}

uint8_t calcBwFreqFromReal(float freq, uint32_t freqMod, uint32_t coefAdj)
{
	return (uint8_t)(freq / freqMod * coefAdj / 100);
}

//--------------------------------------------------------------
// Задание полосы пропускания частоты биений
//--------------------------------------------------------------
int freqBwRealHandler(uint8_t* freqBw0Param, uint8_t* freqBw1Param,  uint32_t freqMod, uint32_t coefAdj)
{
	float result0, result1;
	int isNumber0, isNumber1;
	char freqBw0[] = "Set lower bound:\r\n";
	char freqBw1[] = "Set upper bound:\r\n";
	char freqBw[] = "Bandwidth of frequency of beats: [";
	char freqBwPoint[] = ",";
	char freqBwScob[] = "]";
	
	while (1)
	{
		WriteStringDMA(freqBw0);
		WriteStringDMA(cursor);		
		result0 = readDataFloat(&isNumber0);
		
		WriteStringDMA(freqBw1);
		WriteStringDMA(cursor);				
		result1 = readDataFloat(&isNumber1);

		// Обработка ошибки
		if (isNumber0 == 0 || isNumber1 == 0)
		{
			printError(incorInp);		
		}
		else if (result0 >= result1)
		{			
			printError(freqBwRealError);	
		}
		else if ((result0 > 255) || (result1 > 255))
		{			
		 	printError(freqBwRealError);
		}
		else
		{
			*freqBw0Param = calcBwFreqFromReal(result0, freqMod, coefAdj);
			*freqBw1Param = calcBwFreqFromReal(result1, freqMod, coefAdj);
			
			// Вывод freqBw
			WriteStringDMA(freqBw);
			WriteInt(*freqBw0Param);
			WriteStringDMA(freqBwPoint);	
			WriteInt(*freqBw1Param);
			WriteStringDMA(freqBwScob);
			WriteStringDMA(transferLine);			
			return *freqBw0Param;
		}
	}
}

//--------------------------------------------------------------
// Задание порога накопления
//--------------------------------------------------------------
int limitAccHandler(uint32_t* limitAcc)
{
	uint32_t result;
	int isNumber;
	char limit[] = "Enter limit accumulation:\r\n";
	char limitPrint[] = "Limit accumulation: ";
	
	while (1)
	{
		WriteStringDMA(limit);
		WriteStringDMA(cursor);

		result = readData(&isNumber);
		
		// Обработка ошибки
		if (isNumber == 0)
		{
			printError(incorInp);
		}
		*limitAcc = result;
		// Вывод limitAcc
		WriteStringDMA(limitPrint);	
		WriteIntNl(result);	
		return result;
	}
}

//--------------------------------------------------------------
// Задание коэффициента корректировки
//--------------------------------------------------------------
int coefAdjHandler(uint32_t* coefAdj)
{
	uint32_t result;
	int isNumber;
	char paramAdj[] = "Enter adjustment coefficient:\r\n";
	char coefPrint[] = "Adjustment coefficient: ";
	
	while (1)
	{
		WriteStringDMA(paramAdj);
		WriteStringDMA(cursor);

		result = readData(&isNumber);
		
		// Обработка ошибки
		if ( isNumber == 0)
		{
			printError(incorInp);
		}
		else if (result > 100)
		{			
			printError(coefAdjError);
		}
		else
		{
			*coefAdj = result;
			
			// Вывод coefAdj
			WriteStringDMA(coefPrint);
			WriteIntNl(result);
			return result;
		}
	}
}

//--------------------------------------------------------------
// Вывод параметров по умолчанию
//--------------------------------------------------------------
void printConfig(rdParam localParam)
{
	char mode[] = "Type of the modulating voltage:              ";
	char freq[] = "Frequency of the modulating voltage:         ";
	char modeBuf[] = "Size of the buffer of counting:              ";
	char modeAmp[] = "Maximum amplitude of the modulating voltage: ";
	char constMode[] = "Constant of the modulating voltage:          ";
	char freqBw[] = "Bandwidth of frequency of beats:             [";
	char freqBwPoint[] = ",";
	char freqBwScob[] = "]";
	char limit[] = "Limit accumulation:                          ";
	char coef[] = "Adjustment coefficient:                      ";

	// Вывод typeMod
	WriteStringDMA(mode);
	WriteIntNl(localParam.typeMod);

	// Вывод freqMod
	WriteStringDMA(freq);
	WriteIntNl(localParam.freqMod);
	
	// Вывод bufMode
	WriteStringDMA(modeBuf);
	WriteIntNl(localParam.bufMode);	
	
	// Вывод amplMod
	WriteStringDMA(modeAmp);
	WriteIntNl(localParam.amplMod);
	
	// Вывод constMode
	WriteStringDMA(constMode);
	WriteIntNl(localParam.constMode);
	
	// Вывод freqBw
	WriteStringDMA(freqBw);
	WriteInt(localParam.freqBw0);
	WriteStringDMA(freqBwPoint);
	WriteInt(localParam.freqBw1);
	WriteStringDMA(freqBwScob);
	WriteStringDMA(transferLine);
		
	// Вывод limitAcc
	WriteStringDMA(limit);	
	WriteIntNl(localParam.limitAcc);
	
	// Вывод coefAdj
	WriteStringDMA(coef);
	WriteIntNl(localParam.coefAdj);	
}

//--------------------------------------------------------------
// Вывод сообщения об ошибке
//--------------------------------------------------------------
void  printError(typeError localError)
{
	char selectModeErrorArr[] = "error selectModeError: Incorrect select mode\r\n\n";
	char incorInpArr[] = "error incorInp: Incorrect input\r\n\n";
	char typeModeErrorArr[] = "error typeModeError: Incorrect type mode(values in the range 1-3)\r\n\n";
	char freqModErrorArr[] = "error freqModError: Incorrect frequency(values less 200kHz)\r\n\n";
	char bufModeErrorArr[] = "error bufModeError: Incorrect length buffer(values less 256)\r\n\n";
	char amplModErrorArr[] = "error amplModError: Incorrect amplitude(sinus - less 2048, other - less 4096)\r\n\n";
	char freqBwErrorArr[] = "error freqBwError: Incorrect bandwidth\r\n\n";
	char freqBwRealErrorArr[] = "error freqBwError: Incorrect freq bandwidth\r\n\n";
	char coefAdjErrorArr[] = "error coefAdjError: Incorrect coefficient(values in the range 0-100)\r\n\n";
	char constModeErrorArr[] = "error constModeError: Incorrect constant(values less 4096)\r\n\n";
	
	switch (localError)
	{
		case incorInp:
			WriteStringDMA(incorInpArr);
			break;
		case typeModeError:
			WriteStringDMA(typeModeErrorArr);
			break;
		case freqModError:
			WriteStringDMA(freqModErrorArr);
			break;
		case bufModeError:
			WriteStringDMA(bufModeErrorArr);
			break;
		case amplModError:
			WriteStringDMA(amplModErrorArr);
			break;
		case freqBwError:
			WriteStringDMA(freqBwErrorArr);
			break;
		case freqBwRealError:
			WriteStringDMA(freqBwRealErrorArr);
			break;			
		case coefAdjError:
			WriteStringDMA(coefAdjErrorArr);
			break;
		case constModeError:
			WriteStringDMA(constModeErrorArr);
			break;
		case selectModeError:
			WriteStringDMA(selectModeErrorArr);
			break;
	}
}







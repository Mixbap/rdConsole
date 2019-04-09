// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: console.c 
// Назначение: Терминал для настройка конфигурации и параметров блока 
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************
#include <stdlib.h>
#include "console.h"
#include "math.h"
#include "ini.h"
#include "main.h"

char transferLine[] = "\r\n";
char cursor[] = "\n>> ";

typeError error;


//--------------------------------------------------------------
// Запуск консоли
//--------------------------------------------------------------
void runConsole(rdParam* localParam)
{
	uint8_t active = 1;
	char defConf[] = "Set default configuraton succesfull!\r\n";
	char exit[] = "Exit terminal succesfull!\r\n";
	char unsupCommand[] = "Unsupported command!\r\n\n";
	DMA_ini(); 							// Инициализация DMA
	UART1_DMA_ini(); 	// Инициализация UART1+DMA TX RX
	
	while (active)
	{
		printMenu();
		
		// Обработчик команд меню 
		switch (readData())
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
				limitAccHandler(&(localParam->limitAcc));
				break;
			case 9:
				coefAdjHandler(&(localParam->coefAdj));
				break;
			case 10:
				rdParamDefIni(localParam);
				WriteStringDMA(defConf);
				printConfig(*localParam);
				break;	
			case 11:
				printConfig(*localParam);
				break;	
			case 12:
				WriteStringDMA(exit);
				active = 0;
				break;		
			default:
				WriteStringDMA(unsupCommand);
				resetError();
				break;
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
  //DMA_TX_start(Message, sizeof(Message)); 
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
	WriteLine(" [7] Set constant of the modulating voltage");
	WriteLine(" [8] Set limit accumulation");
	WriteLine(" [9] Set adjustment coefficient");
	WriteLine("[10] Set default configuration");
	WriteLine("[11] Get configuration");
	WriteLine("[12] Exit terminal");
	WriteString("\r\n>> ");
}

//--------------------------------------------------------------
// Ввод данных
//--------------------------------------------------------------
uint32_t readData(void)
{
	const int bufSize = 21;
	char data[bufSize] = {0};
	uint32_t res = 0;
	
	ReadStringDMA(data, bufSize);
	WriteStringDMA(transferLine);

	res = atoi((char*)data);			
	//res = dataInterpret(data, idx);	
	return res;
}

//--------------------------------------------------------------
// Интерпретатор ASCII цифр
//--------------------------------------------------------------
uint8_t interpret(uint8_t value)
{

	if (value >= '0' && value <= '9')
		return value - '0';
	else
	{
		error = incorInp;
		return value;
	}
}

//--------------------------------------------------------------
// Деинтерпретатор ASCII цифр
//--------------------------------------------------------------
uint8_t deinterpret(uint8_t value)
{
	if (value < 10)
		return value + '0';
	else
		return value;
}

//--------------------------------------------------------------
// Деинтерпретатор данных
//--------------------------------------------------------------
uint8_t dataDeinterpret(uint8_t* data, uint32_t value)
{
	uint8_t idx = 0, i = 0;
	int ceil;
	
	// Размер данных
	while (!idx)
	{
		ceil = (int)(value/pow(10, i));
		if (ceil == 0)
			idx = i;
		
		i++;
	}
	
	// Получение массива данных
	for (i = 0; i < idx; i++)
	{
		ceil = (int)(value/pow(10, idx-1-i));
		data[i] = deinterpret((uint8_t)ceil);
		value = value - ((uint32_t)ceil*(uint32_t)pow(10, idx-1-i));
	}
	return idx;
}

//--------------------------------------------------------------
// Интерпретатор входных данных
//--------------------------------------------------------------
uint32_t dataInterpret(uint8_t* data, uint8_t idx)
{
	uint8_t i = 0;
	uint32_t result = 0;
	
	// Интрепретация массива данных по ASCII
	for (i = 0; i < idx; i++)
	{
		data[i] = interpret(data[i]);
	}

	// Интерпретация целой части
	for (i = 0; i < idx; i++)
	{
		result = result + (data[i] * (uint32_t)pow(10, idx - 1 - i));
	}

	return result;
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
	char selMode[] = "Select mode:\r\n[1] Radio senser\r\n[2] Distance measurement\r\n";
	char selModePrint[] = "Mode: ";
	char senser[] = "Radio senser\r\n";
	char dist[] = "Distance measurement\r\n";
	
	while(1)
	{
		WriteStringDMA(selMode);
		WriteStringDMA(cursor);		
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			printError(error);
			resetError();			
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
	char mode[] = "Type of the modulating voltage:\r\n[1] Sinus\r\n[2] Saw\r\n[3] Triangle\r\n";
	char modePrint[] = "Type of the modulating voltage: ";
	char sineMode[] = "[1] Sine";
	char sawMode[] = "[2] Saw";
	char triangleMode[] = "[3] Triangle";
	
	while (1)
	{
		WriteStringDMA(mode);		
		WriteStringDMA(cursor);

		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			processError();				
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
	uint8_t size;
	uint8_t data[5] = {0};
	char freq[] = "Enter frequency (Hz):\r\n";
	char freqPrint[] = "Frequency of the modulating voltage: ";
	
	while (1)
	{
		WriteStringDMA(freq);
		WriteStringDMA(cursor);

		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			processError();		
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
			// TODO переделать перевод числа в строку	
			size = dataDeinterpret(data, result);
			DMA_TX_start(data, size);

			WriteStringDMA(transferLine);			
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
	uint8_t size;
	uint8_t data[5] = {0};
	char modeBuf[] = "Enter the buffer size:\r\n";
	char modeBufPrint[] = "Size of the buffer of counting: ";
	
	while (1)
	{
		WriteStringDMA(modeBuf);
		WriteStringDMA(cursor);

		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			processError();			
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

			// TODO переделать перевод числа в строку
			size = dataDeinterpret(data, result);
			DMA_TX_start(data, size);

			WriteStringDMA(transferLine);			
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
	uint8_t size;
	uint8_t data[5] = {0};
	char modeAmp[] = "Enter amplitude:\r\n";
	char modeAmpPrint[] = "Maximum amplitude of the modulating voltage: ";
	
	while (1)
	{
		WriteStringDMA(modeAmp);
		WriteStringDMA(cursor);

		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			processError();		
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

			// TODO переделать перевод числа в строку
			size = dataDeinterpret(data, result);
			DMA_TX_start(data, size);

			WriteStringDMA(transferLine);			
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
	uint8_t size;
	uint8_t data[5] = {0};
	char constMode[] = "Enter constant:\r\n";
	char constModePrint[] = "Constant of the modulating voltage: ";
	
	while (1)
	{
		WriteStringDMA(constMode);
		WriteStringDMA(cursor);
		
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			processError();
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
			size = dataDeinterpret(data, result);
			DMA_TX_start(data, size);

			WriteStringDMA(transferLine);			
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
	uint8_t size;
	uint8_t data[5] = {0};
	char freqBw0[] = "Set lower bound:\r\n";
	char freqBw1[] = "Set upper bound:\r\n";
	char freqBw[] = "Bandwidth of frequency of beats: [";
	char freqBwPoint[] = ",";
	char freqBwScob[] = "]";
	
	while (1)
	{
		WriteStringDMA(freqBw0);
		WriteStringDMA(cursor);		
		result0 = readData();
		
		WriteStringDMA(freqBw1);
		WriteStringDMA(cursor);				
		result1 = readData();

		// Обработка ошибки
		if (error < 0)
		{
			processError();		
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

			size = dataDeinterpret(data, result0);
			DMA_TX_start(data, size);
			WriteStringDMA(freqBwPoint);	

			size = dataDeinterpret(data, result1);
			DMA_TX_start(data, size);

			WriteStringDMA(freqBwScob);

			WriteStringDMA(transferLine);			
			return result0;
		}
	}
}

//--------------------------------------------------------------
// Задание порога накопления
//--------------------------------------------------------------
int limitAccHandler(uint32_t* limitAcc)
{
	uint8_t size;
	uint32_t result;
	uint8_t data[5] = {0};
	char limit[] = "Enter limit accumulation:\r\n";
	char limitPrint[] = "Limit accumulation: ";
	
	while (1)
	{
		WriteStringDMA(limit);
		WriteStringDMA(cursor);

		result = readData();
		
		// Обработка ошибки
		if (error < 0)
			{
				processError();
			}
		*limitAcc = result;
		// Вывод limitAcc
		WriteStringDMA(limitPrint);	

		size = dataDeinterpret(data, result);
		DMA_TX_start(data, size);

		WriteStringDMA(transferLine);		
		return result;
	}
}

//--------------------------------------------------------------
// Задание коэффициента корректировки
//--------------------------------------------------------------
int coefAdjHandler(uint32_t* coefAdj)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	char paramAdj[] = "Enter adjustment coefficient:\r\n";
	char coefPrint[] = "Adjustment coefficient: ";
	
	while (1)
	{
		WriteStringDMA(paramAdj);
		WriteStringDMA(cursor);

		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			processError();
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

			size = dataDeinterpret(data, result);
			DMA_TX_start(data, size);

			WriteStringDMA(transferLine);
			return result;
		}
	}
}

//--------------------------------------------------------------
// Вывод параметров по умолчанию
//--------------------------------------------------------------
void printConfig(rdParam localParam)
{
	uint8_t data[5] = {0};
	uint8_t size;

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
	size = dataDeinterpret(data, (uint32_t)localParam.typeMod);
	DMA_TX_start(data, size);
	WriteStringDMA(transferLine);

	// Вывод freqMod
	WriteStringDMA(freq);

	size = dataDeinterpret(data, localParam.freqMod);
	DMA_TX_start(data, size);

	WriteStringDMA(transferLine);
	
	// Вывод bufMode
	WriteStringDMA(modeBuf);
	size = dataDeinterpret(data, localParam.bufMode);
	DMA_TX_start(data, size);
	WriteStringDMA(transferLine);
	
	// Вывод amplMod
	WriteStringDMA(modeAmp);
	size = dataDeinterpret(data, localParam.amplMod);
	DMA_TX_start(data, size);
	WriteStringDMA(transferLine);
	
	// Вывод constMode
	WriteStringDMA(constMode);
	size = dataDeinterpret(data, localParam.constMode);
	DMA_TX_start(data, size);
	WriteStringDMA(transferLine);
	
	// Вывод freqBw
	WriteStringDMA(freqBw);
	size = dataDeinterpret(data, localParam.freqBw0);
	DMA_TX_start(data, size);
	WriteStringDMA(freqBwPoint);
	size = dataDeinterpret(data, localParam.freqBw1);
	DMA_TX_start(data, size);
	WriteStringDMA(freqBwScob);
	WriteStringDMA(transferLine);
		
	// Вывод limitAcc
	WriteStringDMA(limit);	
	size = dataDeinterpret(data, localParam.limitAcc);
	DMA_TX_start(data, size);
	WriteStringDMA(transferLine);
	
	// Вывод coefAdj
	WriteStringDMA(coef);
	size = dataDeinterpret(data, localParam.coefAdj);
	DMA_TX_start(data, size);
	WriteStringDMA(transferLine);
	
}

//--------------------------------------------------------------
// Обработчик ошибок
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
	
	//error = 0;
}

void processError(void)
{
	printError(error);
	resetError();
}

void resetError(void)
{
	error = successNoError;
}







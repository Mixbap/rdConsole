#include "console.h"
//--------------------------------------------------------------
// Запуск консоли
//--------------------------------------------------------------
void runConsole(void)
{
	uint8_t act = 1;
	uint8_t defConf[] = "Set default configuraton succesfull!\r\n";
	uint8_t exit[] = "Exit terminal succesfull!\r\n";
	
	DMA_ini(); 							// Инициализация DMA
	UART1_DMA_ini(); 				// Инициализация UART1+DMA TX RX
	
	while (act)
	{
		printMenu();
		
		// Обработчик команд меню 
		switch (readData())
		{
		case 1:
			typeModHandler();
			break;
		case 2:
			freqModHandler();
			break;
		case 3:
			bufModeHandler();
			break;
		case 4:
			amplModHandler();
			break;
		case 5:
			constModeHandler();
			break;
		case 6:
			freqBwHandler();
			break;
		case 7:
			limitAccHandler();
			break;
		case 8:
			coefAdjHandler();
			break;
		case 9:
			rdParamDefIni();
			DMA_TX_start(defConf, sizeof(defConf));
			break;	
		case 10:
			getDefConfig();
			break;	
		case 11:
			DMA_TX_start(exit, sizeof(exit));
			act = 0;
			break;		
		default:
			DMA_TX_start(unsupCommand, sizeof(unsupCommand));
			error = 0;
			break;
		}
	}

	UART_Cmd(MDR_UART1, DISABLE); // для нормальной работы ЦАП и ДМА
	flagConsole = 1;
}

//--------------------------------------------------------------
// Вывод меню
//--------------------------------------------------------------
void printMenu(void)
{
	uint8_t hello[] = "\n\tTerminal of the block of processing of the radio sensor.\r\n";
	uint8_t mode[] = "[1] Type of the modulating voltage\r\n";
	uint8_t freq[] = "[2] Frequency of the modulating voltage\r\n";
	uint8_t modeBuf[] = "[3] Size of the buffer of counting\r\n";
	uint8_t modeAmp[] = "[4] Maximum amplitude of the modulating voltage\r\n";
	uint8_t constMode[] = "[5] Constant of the modulating voltage\r\n";
	uint8_t freqBw[] = "[6] Bandwidth of frequency of beats\r\n";
	uint8_t limit[] = "[7] Limit accumulation\r\n";
	uint8_t param[] = "[8] Adjustment coefficient\r\n";
	uint8_t defConf[] = "[9] Set default configuration\r\n";
	uint8_t getDefConf[] = "[10] Get default configuration\r\n";
	uint8_t exit[] = "[11] Exit terminal\r\n";
	
	DMA_TX_start(hello, sizeof(hello));
	DMA_TX_start(mode, sizeof(mode));
	DMA_TX_start(freq, sizeof(freq));
	DMA_TX_start(modeBuf, sizeof(modeBuf));
	DMA_TX_start(modeAmp, sizeof(modeAmp));
	DMA_TX_start(constMode, sizeof(constMode));
	DMA_TX_start(freqBw, sizeof(freqBw));
	DMA_TX_start(limit, sizeof(limit));
	DMA_TX_start(param, sizeof(param));
	DMA_TX_start(defConf, sizeof(defConf));
	DMA_TX_start(getDefConf, sizeof(getDefConf));
	DMA_TX_start(exit, sizeof(exit));
	DMA_TX_start(cursor, sizeof(cursor));
}

//--------------------------------------------------------------
// Ввод данных
//--------------------------------------------------------------
uint32_t readData(void)
{
	uint8_t data[20] = {0};
	uint8_t idx = 0;
	uint32_t res = 0;
	
	idx = DMA_RX_start(data, sizeof(data));
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	res = dataInterpret(data, idx);
	
	return res;
}
//--------------------------------------------------------------
// Интерпретатор ASCII цифр
//--------------------------------------------------------------
uint8_t interpret(uint8_t value)
{
	switch (value)
	{
		case 0x30: return 0;
		case 0x31: return 1;
		case 0x32: return 2;
		case 0x33: return 3;
		case 0x34: return 4;
		case 0x35: return 5;
		case 0x36: return 6;
		case 0x37: return 7;
		case 0x38: return 8;
		case 0x39: return 9;
		default:
			error = incorInp; // Некорректный ввод данных
			return value;
	}
}

//--------------------------------------------------------------
// Деинтерпретатор ASCII цифр
//--------------------------------------------------------------
uint8_t deinterpret(uint8_t value)
{
	switch (value)
	{
		case 0: return 0x30;
		case 1: return 0x31;
		case 2: return 0x32;
		case 3: return 0x33;
		case 4: return 0x34;
		case 5: return 0x35;
		case 6: return 0x36;
		case 7: return 0x37;
		case 8: return 0x38;
		case 9: return 0x39;
		default:
			return value;
	}
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
// Задание формы модулирующего напряжения
//--------------------------------------------------------------
void typeModHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t mode[] = "Type of the modulating voltage:\r\n[1] Sinus\r\n[2] Saw\r\n[3] Triangle\r\n";
	uint8_t modePrint[] = "Type of the modulating voltage:              ";
	
	while (1)
	{
		DMA_TX_start(mode, sizeof(mode));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if ((result > 3) | (result < 1))
		{
			error = typeModeError;
			errorHandler();
		}
		else 
		{
			param.typeMod = result;
			
			// Вывод typeMod
			DMA_TX_start(modePrint, sizeof(modePrint));
			size = dataDeinterpret(data, (uint32_t)param.typeMod);
			DMA_TX_start(data, size);
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// Задание частоты модулирующего напряжения
//--------------------------------------------------------------
void freqModHandler(void)
{
	uint32_t result;
	uint8_t freq[] = "Enter frequency (kHz):\r\n";
	
	while (1)
	{
		DMA_TX_start(freq, sizeof(freq));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if (result > 200)
		{
			error = freqModError;
			errorHandler();
		}
		else
		{
			param.freqMod = result;
			return;
		}
	}
}

//--------------------------------------------------------------
// Задание размера буфера отсчетов модулирующего напряжения
//--------------------------------------------------------------
void bufModeHandler(void)
{
	uint32_t result;
	uint8_t modeBuf[] = "Enter the buffer size:\r\n";
	
	while (1)
	{
		DMA_TX_start(modeBuf, sizeof(modeBuf));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if (result > 255)
		{
			error = bufModeError;
			errorHandler();
		}
		else
		{
			param.bufMode = result;
			return;
		}
	}
}

//--------------------------------------------------------------
// Задание амплитуды модулирующего напряжения
//--------------------------------------------------------------
void amplModHandler(void)
{
	uint32_t result;
	uint8_t modeAmp[] = "Enter amplitude:\r\n";
	
	while (1)
	{
		DMA_TX_start(modeAmp, sizeof(modeAmp));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if (param.typeMod == 1)
		{
			if (result > 2047)
			{
				error = amplModError;
				errorHandler();
			}
			else 
			{
				param.amplMod = result;
				return;
			}
		}
		else
		{
			if (result > 4095)
			{
				error = amplModError;
				errorHandler();
			}
			else 
			{
				param.amplMod = result;
				return;
			}
		}
	}
}

//--------------------------------------------------------------
// Задание постоянной состовляющей модулирующего напряжения
//--------------------------------------------------------------
void constModeHandler(void)
{
	uint32_t result;
	uint8_t constMode[] = "Enter constant:\r\n";
	
	while (1)
	{
		DMA_TX_start(constMode, sizeof(constMode));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if (result > 4095)
		{
			error = constModeError;
			errorHandler();
		}
		else
		{
			param.constMode = result;
			return;
		}
	}
}

//--------------------------------------------------------------
// Задание полосы пропускания частоты биений
//--------------------------------------------------------------
void freqBwHandler(void)
{
	uint32_t result0, result1;
	uint8_t freqBw0[] = "Set lower bound:\r\n";
	uint8_t freqBw1[] = "Set upper bound:\r\n";
	
	while (1)
	{
		DMA_TX_start(freqBw0, sizeof(freqBw0));
		DMA_TX_start(cursor, sizeof(cursor));
		result0 = readData();
		
		DMA_TX_start(freqBw1, sizeof(freqBw1));
		DMA_TX_start(cursor, sizeof(cursor));
		result1 = readData();

		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if (result0 >= result1)
		{
			error = freqBwError;
			errorHandler();
		}
		else if ((result0 > 255) || (result1 > 255))
		{
			error = freqBwError;
			errorHandler();
		}
		else
		{
			param.freqBw0 = result0;
			param.freqBw1 = result1;
			return;
		}
	}
}

//--------------------------------------------------------------
// Задание порога накопления
//--------------------------------------------------------------
void limitAccHandler(void)
{
	uint8_t limit[] = "Enter limit accumulation:\r\n";
	
	while (1)
	{
		DMA_TX_start(limit, sizeof(limit));
		DMA_TX_start(cursor, sizeof(cursor));
		param.limitAcc = readData();
		
		// Обработка ошибки
		if (error < 0)
			errorHandler();
		
		return;
	}
}

//--------------------------------------------------------------
// Задание коэффициента корректировки
//--------------------------------------------------------------
void coefAdjHandler(void)
{
	uint32_t result;
	uint8_t paramAdj[] = "Enter adjustment coefficient:\r\n";
	
	while (1)
	{
		DMA_TX_start(paramAdj, sizeof(paramAdj));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// Обработка ошибки
		if (error < 0)
		{
			errorHandler();
		}
		else if (result > 100)
		{
			error = coefAdjError;
			errorHandler();
		}
		else
		{
			param.coefAdj = result;
			return;
		}
	}
}

//--------------------------------------------------------------
// Вывод параметров по умолчанию
//--------------------------------------------------------------
void getDefConfig(void)
{
	uint8_t data[5] = {0};
	uint8_t size;

	uint8_t mode[] = "Type of the modulating voltage:              ";
	uint8_t freq[] = "Frequency of the modulating voltage:         ";
	uint8_t modeBuf[] = "Size of the buffer of counting:              ";
	uint8_t modeAmp[] = "Maximum amplitude of the modulating voltage: ";
	uint8_t constMode[] = "Constant of the modulating voltage:          ";
	uint8_t freqBw[] = "Bandwidth of frequency of beats:             [";
	uint8_t freqBwPoint[] = ",";
	uint8_t freqBwScob[] = "]";
	uint8_t limit[] = "Limit accumulation:                          ";
	uint8_t coef[] = "Adjustment coefficient:                      ";

	// Вывод typeMod
	DMA_TX_start(mode, sizeof(mode));
	size = dataDeinterpret(data, (uint32_t)param.typeMod);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));

	// Вывод freqMod
	DMA_TX_start(freq, sizeof(freq));
	size = dataDeinterpret(data, param.freqMod);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// Вывод bufMode
	DMA_TX_start(modeBuf, sizeof(modeBuf));
	size = dataDeinterpret(data, param.bufMode);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// Вывод amplMod
	DMA_TX_start(modeAmp, sizeof(modeAmp));
	size = dataDeinterpret(data, param.amplMod);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// Вывод constMode
	DMA_TX_start(constMode, sizeof(constMode));
	size = dataDeinterpret(data, param.constMode);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// Вывод freqBw
	DMA_TX_start(freqBw, sizeof(freqBw));
	size = dataDeinterpret(data, param.freqBw0);
	DMA_TX_start(data, size);
	DMA_TX_start(freqBwPoint, sizeof(freqBwPoint));
	size = dataDeinterpret(data, param.freqBw1);
	DMA_TX_start(data, size);
	DMA_TX_start(freqBwScob, sizeof(freqBwScob));
	DMA_TX_start(transferLine, sizeof(transferLine));
		
	// Вывод limitAcc
	DMA_TX_start(limit, sizeof(limit));	
	size = dataDeinterpret(data, param.limitAcc);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// Вывод coefAdj
	DMA_TX_start(coef, sizeof(coef));
	size = dataDeinterpret(data, param.coefAdj);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
}

//--------------------------------------------------------------
// Обработчик ошибок
//--------------------------------------------------------------
void errorHandler(void)
{
	uint8_t incorInpArr[] = "error incorInp: Incorrect input\r\n\n";
	uint8_t typeModeErrorArr[] = "error typeModeError: Incorrect type mode(values in the range 1-3)\r\n\n";
	uint8_t freqModErrorArr[] = "error freqModError: Incorrect frequency(values less 200kHz)\r\n\n";
	uint8_t bufModeErrorArr[] = "error bufModeError: Incorrect length buffer(values less 256)\r\n\n";
	uint8_t amplModErrorArr[] = "error amplModError: Incorrect amplitude(sinus - less 2048, other - less 4096)\r\n\n";
	uint8_t freqBwErrorArr[] = "error freqBwError: Incorrect bandwidth\r\n\n";
	uint8_t coefAdjErrorArr[] = "error coefAdjError: Incorrect coefficient(values in the range 0-100)\r\n\n";
	uint8_t constModeErrorArr[] = "error constModeError: Incorrect constant(values less 4096)\r\n\n";
	
	switch (error)
	{
		case incorInp:
			DMA_TX_start(incorInpArr, sizeof(incorInpArr));
			break;
		case typeModeError:
			DMA_TX_start(typeModeErrorArr, sizeof(typeModeErrorArr));
			break;
		case freqModError:
			DMA_TX_start(freqModErrorArr, sizeof(freqModErrorArr));
			break;
		case bufModeError:
			DMA_TX_start(bufModeErrorArr, sizeof(bufModeErrorArr));
			break;
		case amplModError:
			DMA_TX_start(amplModErrorArr, sizeof(amplModErrorArr));
			break;
		case freqBwError:
			DMA_TX_start(freqBwErrorArr, sizeof(freqBwErrorArr));
			break;
		case coefAdjError:
			DMA_TX_start(coefAdjErrorArr, sizeof(coefAdjErrorArr));
			break;
		case constModeError:
			DMA_TX_start(constModeErrorArr, sizeof(constModeErrorArr));
			break;
	}
	
	error = 0;
}







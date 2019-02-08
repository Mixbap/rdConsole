#include "console.h"
//--------------------------------------------------------------
// Запуск консоли
//--------------------------------------------------------------
void runConsole(void)
{
	uint8_t act = 1;
	uint8_t mode[] = "Type of the modulating voltage:\r\n1. Sinus\r\n2. Saw\r\n3. Triangle";
	uint8_t freq[] = "Enter frequency (kHz):\r\n";
	uint8_t modeBuf[] = "Enter the buffer size:\r\n";
	uint8_t modeAmp[] = "Enter amplitude:\r\n";
	uint8_t freqBw[] = "Set bandwidth:\r\n";
	uint8_t limit[] = "Enter limit accumulation:\r\n";
	uint8_t param[] = "Enter adjustment coefficient:\r\n";
	uint8_t exit[] = "Exit terminal succesfull!\r\n";
	uint8_t unsupCommand[] = "Unsupported command!\r\n";
	
	DMA_ini(); 							// Инициализация DMA
	UART1_DMA_ini(); 				// Инициализация UART1+DMA TX RX
	
	while (act)
	{
		printMenu();
		
		// Обработчик команд меню 
		switch (readData())
		{
		case 1:
			DMA_TX_start(mode, sizeof(mode));
			break;
		case 2:
			DMA_TX_start(freq, sizeof(freq));
			break;
		case 3:
			DMA_TX_start(modeBuf, sizeof(modeBuf));
			break;
		case 4:
			DMA_TX_start(modeAmp, sizeof(modeAmp));
			break;
		case 5:
			DMA_TX_start(freqBw, sizeof(freqBw));
			break;
		case 6:
			DMA_TX_start(limit, sizeof(limit));
			break;
		case 7:
			DMA_TX_start(param, sizeof(param));
			break;
		case 8:
			DMA_TX_start(exit, sizeof(exit));
			act = 0;
			break;	
		
		case 14:
			DMA_TX_start(param, sizeof(param));
			break;
		
		case 12:
			DMA_TX_start(param, sizeof(param));
			break;
		
		default:
			DMA_TX_start(unsupCommand, sizeof(unsupCommand));
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
	uint8_t mode[] = "1. Type of the modulating voltage\r\n";
	uint8_t freq[] = "2. Frequency of the modulating voltage\r\n";
	uint8_t modeBuf[] = "3. Size of the buffer of counting\r\n";
	uint8_t modeAmp[] = "4. Maximum amplitude of the modulating voltage\r\n";
	uint8_t freqBw[] = "5. Bandwidth of frequency of beats\r\n";
	uint8_t limit[] = "6. Limit accumulation\r\n";
	uint8_t param[] = "7. Adjustment coefficient\r\n";
	uint8_t exit[] = "8. Exit terminal\r\n";
	
	DMA_TX_start(hello, sizeof(hello));
	DMA_TX_start(mode, sizeof(mode));
	DMA_TX_start(freq, sizeof(freq));
	DMA_TX_start(modeBuf, sizeof(modeBuf));
	DMA_TX_start(modeAmp, sizeof(modeAmp));
	DMA_TX_start(freqBw, sizeof(freqBw));
	DMA_TX_start(limit, sizeof(limit));
	DMA_TX_start(param, sizeof(param));
	DMA_TX_start(exit, sizeof(exit));
	DMA_TX_start(cursor, sizeof(cursor));
}

//--------------------------------------------------------------
// Ввод данных
//--------------------------------------------------------------
uint8_t readData(void)
{
	uint8_t data[20] = {0};
	uint8_t idx = 0;
	uint8_t res = 0;
	//uint8_t i = 0;
	idx = DMA_RX_start(data, sizeof(data));
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	res = dataInterpret(data, idx);

	if (res > 10)
		LED0_ON;
	
	return res;//interpret(data[0]);
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
		default: return value;
	}
}

//--------------------------------------------------------------
// Интерпретатор входных данных
//--------------------------------------------------------------
uint8_t dataInterpret(uint8_t* data, uint8_t idx)
{
	uint8_t i = 0;
	uint8_t result = 0;
	uint8_t d[20];
	
	// Интрепретация массива данных по ASCII
	for (i = 0; i < idx; i++)
	{
		d[i] = interpret(data[i]);
	}

	// Интерпретация целой части
	for (i = 0; i < idx; i++)
	{
		result = result + (d[i] * (uint8_t)pow(10, idx - 1 - i));
	}

	return result;
}













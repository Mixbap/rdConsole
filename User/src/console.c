#include "console.h"

//--------------------------------------------------------------
// Запуск консоли
//--------------------------------------------------------------
void runConsole(void)
{
	uint8_t act = 1;
	uint8_t mode[] = "Type of the modulating voltage\r\n1. Sinis\r\n2. Saw\r\n3. Triangle";
	uint8_t m[] = "Exit terminal succesfull!\r\n";
	uint8_t unsupCommand[] = "Unsupported command!\r\n";
	
	DMA_ini(); 							// Инициализация DMA
	UART1_DMA_ini(); 				// Инициализация UART1+DMA TX RX
	
	while (act)
	{
		printMenu();

		// Обработчик команд меню 
		switch (readData())
		{
		case 0x31: //1
			DMA_TX_start(mode, sizeof(mode));
			break;
		case 0x38: //8
			DMA_TX_start(m, sizeof(m));
			act = 0;
			break;
		default:
			DMA_TX_start(unsupCommand, sizeof(unsupCommand));
			break;
		}
	}
/*	
	DMA_TX_start(buf1, 20);
	DMA_TX_start(buf2, 20);
	DMA_RX_start(buf0, 3); // Ввод 3 значений (не меньше не больше), можно меньше но добить пробелами
	DMA_TX_start(buf0, 20);
	DMA_TX_start(buf3, 20);
*/		
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
	uint8_t cursor[] = "\n>> ";
	
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
	DMA_RX_start(data, 2);
	DMA_TX_start(buf3, 20);
	return data[0];
}






#include "console.h"

//--------------------------------------------------------------
// ������ �������
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
	
	DMA_ini(); 							// ������������� DMA
	UART1_DMA_ini(); 				// ������������� UART1+DMA TX RX
	
	while (act)
	{
		printMenu();

		// ���������� ������ ���� 
		switch (readData())
		{
		case 0x31: //1
			DMA_TX_start(mode, sizeof(mode));
			break;
		case 0x32: //2
			DMA_TX_start(freq, sizeof(freq));
			break;
		case 0x33: //3
			DMA_TX_start(modeBuf, sizeof(modeBuf));
			break;
		case 0x34: //4
			DMA_TX_start(modeAmp, sizeof(modeAmp));
			break;
		case 0x35: //5
			DMA_TX_start(freqBw, sizeof(freqBw));
			break;
		case 0x36: //6
			DMA_TX_start(limit, sizeof(limit));
			break;
		case 0x37: //7
			DMA_TX_start(param, sizeof(param));
			break;
		case 0x38: //8
			DMA_TX_start(exit, sizeof(exit));
			act = 0;
			break;
		default:
			DMA_TX_start(unsupCommand, sizeof(unsupCommand));
			break;
		}
	}

	UART_Cmd(MDR_UART1, DISABLE); // ��� ���������� ������ ��� � ���
	flagConsole = 1;
}

//--------------------------------------------------------------
// ����� ����
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
	
	uint8_t data0[20] = {0};
	DMA_RX_start(data0, sizeof(data0)); // �������� ��� DMA Rx (�� ���������� ���������� ������� ������)
	
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
// ���� ������
//--------------------------------------------------------------
uint8_t readData(void)
{
	uint8_t data[2] = {0};
	DMA_RX_start(data, sizeof(data));	
	while ((DMA_GetFlagStatus(DMA_Channel_UART1_RX, DMA_FLAG_CHNL_ENA )));
	
	DMA_TX_start(transferLine, sizeof(transferLine));
	return data[0];
}






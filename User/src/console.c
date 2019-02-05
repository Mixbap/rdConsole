#include "console.h"

//--------------------------------------------------------------
// ������ �������
//--------------------------------------------------------------
void runConsole(void)
{
	uint8_t act = 1;
	uint8_t n[] = "Coefficient = \r\n";
	uint8_t m[] = "Exit terminal succesful!\r\n";
	uint8_t k[] = "FFFF\r\n";
	
	DMA_ini(); 							// ������������� DMA
	UART1_DMA_TX_PB5_ini(); // ������������� UART1+DMA TX (XS9 18 pin)
	
	while (act)
	{
		printMenu();

		// ���������� ������ ���� 
		switch (readData())
		{
		case 0x31: //1
			DMA_TX_start(n, sizeof(n));
			break;
		case 0x32: //2
			DMA_TX_start(m, sizeof(m));
			act = 0;
			break;
		default:
			DMA_TX_start(k, sizeof(k));
			break;
		}
	}
/*	
	DMA_TX_start(buf1, 20);
	DMA_TX_start(buf2, 20);
	DMA_RX_start(buf0, 3); // ���� 3 �������� (�� ������ �� ������), ����� ������ �� ������ ���������
	DMA_TX_start(buf0, 20);
	DMA_TX_start(buf3, 20);
*/		
	UART_Cmd(MDR_UART1, DISABLE); // ��� ���������� ������ ��� � ���
	flagConsole = 1;
}

//--------------------------------------------------------------
// ����� ����
//--------------------------------------------------------------
void printMenu(void)
{
	uint8_t hello[] = "\n\tTerminal of the block of processing of the radio sensor.\r\n";
	uint8_t param[] = "1. Adjustment coefficient\r\n";
	uint8_t exit[] = "2. Exit terminal\r\n";
	uint8_t cursor[] = "\n>> ";
	
	DMA_TX_start(hello, sizeof(hello));
	DMA_TX_start(param, sizeof(param));
	DMA_TX_start(exit, sizeof(exit));
	DMA_TX_start(cursor, sizeof(cursor));
}

//--------------------------------------------------------------
// ���� ������
//--------------------------------------------------------------
uint8_t readData(void)
{
	uint8_t data[20] = {0};
	DMA_RX_start(data, 2);
	DMA_TX_start(buf3, 20);
	return data[0];
}






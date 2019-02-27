#include "console.h"
//--------------------------------------------------------------
// ������ �������
//--------------------------------------------------------------
void runConsole(void)
{
	uint8_t act = 1;
	uint8_t defConf[] = "Set default configuraton succesfull!\r\n";
	uint8_t exit[] = "Exit terminal succesfull!\r\n";
	
	DMA_ini(); 							// ������������� DMA
	UART1_DMA_ini(); 				// ������������� UART1+DMA TX RX
	
	while (act)
	{
		printMenu();
		
		// ���������� ������ ���� 
		switch (readData())
		{
			case 1:
				selectMode();
				break;
			case 2:
				typeModHandler();
				break;
			case 3:
				freqModHandler();
				break;
			case 4:
				bufModeHandler();
				break;
			case 5:
				amplModHandler();
				break;
			case 6:
				constModeHandler();
				break;
			case 7:
				freqBwHandler();
				break;
			case 8:
				limitAccHandler();
				break;
			case 9:
				coefAdjHandler();
				break;
			case 10:
				selectCapturePort();
				break;
			case 11:
				rdParamDefIni();
				DMA_TX_start(defConf, sizeof(defConf));
				break;	
			case 12:
				getConfig();
				break;	
			case 13:
				DMA_TX_start(exit, sizeof(exit));
				act = 0;
				break;		
			default:
				DMA_TX_start(unsupCommand, sizeof(unsupCommand));
				error = 0;
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
	uint8_t selMode[] = "[1] Select mode\r\n";	
	uint8_t mode[] = "[2] Set type of the modulating voltage\r\n";
	uint8_t freq[] = "[3] Set frequency of the modulating voltage\r\n";
	uint8_t modeBuf[] = "[4] Set size of the buffer of counting\r\n";
	uint8_t modeAmp[] = "[5] Set maximum amplitude of the modulating voltage\r\n";
	uint8_t constMode[] = "[6] Set constant of the modulating voltage\r\n";
	uint8_t freqBw[] = "[7] Set bandwidth of frequency of beats\r\n";
	uint8_t limit[] = "[8] Set limit accumulation\r\n";
	uint8_t param[] = "[9] Set adjustment coefficient\r\n";	
	uint8_t selPort[] = "[10] Set capture port\r\n";
	uint8_t defConf[] = "[11] Set default configuration\r\n";
	uint8_t getConf[] = "[12] Get configuration\r\n";
	uint8_t exit[] = "[13] Exit terminal\r\n";
	
	DMA_TX_start(hello, sizeof(hello));
	DMA_TX_start(selMode, sizeof(selMode));
	DMA_TX_start(mode, sizeof(mode));
	DMA_TX_start(freq, sizeof(freq));
	DMA_TX_start(modeBuf, sizeof(modeBuf));
	DMA_TX_start(modeAmp, sizeof(modeAmp));
	DMA_TX_start(constMode, sizeof(constMode));
	DMA_TX_start(freqBw, sizeof(freqBw));
	DMA_TX_start(limit, sizeof(limit));
	DMA_TX_start(param, sizeof(param));
	DMA_TX_start(selPort, sizeof(selPort));
	DMA_TX_start(defConf, sizeof(defConf));
	DMA_TX_start(getConf, sizeof(getConf));
	DMA_TX_start(exit, sizeof(exit));
	DMA_TX_start(cursor, sizeof(cursor));
}

//--------------------------------------------------------------
// ���� ������
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
// ������������� ASCII ����
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
			error = incorInp; // ������������ ���� ������
			return value;
	}
}

//--------------------------------------------------------------
// ��������������� ASCII ����
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
// ��������������� ������
//--------------------------------------------------------------
uint8_t dataDeinterpret(uint8_t* data, uint32_t value)
{
	uint8_t idx = 0, i = 0;
	int ceil;
	
	// ������ ������
	while (!idx)
	{
		ceil = (int)(value/pow(10, i));
		if (ceil == 0)
			idx = i;
		
		i++;
	}
	
	// ��������� ������� ������
	for (i = 0; i < idx; i++)
	{
		ceil = (int)(value/pow(10, idx-1-i));
		data[i] = deinterpret((uint8_t)ceil);
		value = value - ((uint32_t)ceil*(uint32_t)pow(10, idx-1-i));
	}
	return idx;
}

//--------------------------------------------------------------
// ������������� ������� ������
//--------------------------------------------------------------
uint32_t dataInterpret(uint8_t* data, uint8_t idx)
{
	uint8_t i = 0;
	uint32_t result = 0;
	
	// ������������� ������� ������ �� ASCII
	for (i = 0; i < idx; i++)
	{
		data[i] = interpret(data[i]);
	}

	// ������������� ����� �����
	for (i = 0; i < idx; i++)
	{
		result = result + (data[i] * (uint32_t)pow(10, idx - 1 - i));
	}

	return result;
}

//--------------------------------------------------------------
// ����� ������ ������
//--------------------------------------------------------------
void selectMode(void)
{
	uint32_t result;
	uint8_t selMode[] = "Select mode:\r\n[1] Radio senser\r\n[2] Distance measurement\r\n";
	uint8_t selModePrint[] = "Mode: ";
	uint8_t senser[] = "Radio senser\r\n";
	uint8_t dist[] = "Distance measurement\r\n";
	
	while(1)
	{
		DMA_TX_start(selMode, sizeof(selMode));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
		if (error < 0)
		{
			errorHandler();
		}
		else if ((result < 1) | (result > 2))
		{
			error = selectModeError;
			errorHandler();
		}
		else
		{
			param.mode = result;
			
			// ����� ������
			DMA_TX_start(selModePrint, sizeof(selModePrint));
			if (result == 1)
				DMA_TX_start(senser, sizeof(senser));
			else if (result == 2)
				DMA_TX_start(dist, sizeof(dist));
			
			return;
		}
	}
}

//--------------------------------------------------------------
// ������� ����� ������������� ����������
//--------------------------------------------------------------
void typeModHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t mode[] = "Type of the modulating voltage:\r\n[1] Sinus\r\n[2] Saw\r\n[3] Triangle\r\n";
	uint8_t modePrint[] = "Type of the modulating voltage: ";
	
	while (1)
	{
		DMA_TX_start(mode, sizeof(mode));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
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
			
			// ����� typeMod
			DMA_TX_start(modePrint, sizeof(modePrint));
			size = dataDeinterpret(data, (uint32_t)param.typeMod);
			DMA_TX_start(data, size);
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// ������� ������� ������������� ����������
//--------------------------------------------------------------
void freqModHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t freq[] = "Enter frequency (Hz):\r\n";
	uint8_t freqPrint[] = "Frequency of the modulating voltage: ";
	
	while (1)
	{
		DMA_TX_start(freq, sizeof(freq));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
		if (error < 0)
		{
			errorHandler();
		}
		else if (result > 200000)
		{
			error = freqModError;
			errorHandler();
		}
		else
		{
			param.freqMod = result;
			
			// ����� freqMod
			DMA_TX_start(freqPrint, sizeof(freqPrint));
			size = dataDeinterpret(data, param.freqMod);
			DMA_TX_start(data, size);
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// ������� ������� ������ �������� ������������� ����������
//--------------------------------------------------------------
void bufModeHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t modeBuf[] = "Enter the buffer size:\r\n";
	uint8_t modeBufPrint[] = "Size of the buffer of counting: ";
	
	while (1)
	{
		DMA_TX_start(modeBuf, sizeof(modeBuf));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
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
			
			// ����� bufMode
			DMA_TX_start(modeBufPrint, sizeof(modeBufPrint));
			size = dataDeinterpret(data, param.bufMode);
			DMA_TX_start(data, size);
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// ������� ��������� ������������� ����������
//--------------------------------------------------------------
void amplModHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t modeAmp[] = "Enter amplitude:\r\n";
	uint8_t modeAmpPrint[] = "Maximum amplitude of the modulating voltage: ";
	
	while (1)
	{
		DMA_TX_start(modeAmp, sizeof(modeAmp));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
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
				
				// ����� amplMod
				DMA_TX_start(modeAmpPrint, sizeof(modeAmpPrint));
				size = dataDeinterpret(data, param.amplMod);
				DMA_TX_start(data, size);
				DMA_TX_start(transferLine, sizeof(transferLine));
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
				
				// ����� amplMod
				DMA_TX_start(modeAmpPrint, sizeof(modeAmpPrint));
				size = dataDeinterpret(data, param.amplMod);
				DMA_TX_start(data, size);
				DMA_TX_start(transferLine, sizeof(transferLine));
				return;
			}
		}
	}
}

//--------------------------------------------------------------
// ������� ���������� ������������ ������������� ����������
//--------------------------------------------------------------
void constModeHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t constMode[] = "Enter constant:\r\n";
	uint8_t constModePrint[] = "Constant of the modulating voltage: ";
	
	while (1)
	{
		DMA_TX_start(constMode, sizeof(constMode));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
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
			
			// ����� constMode
			DMA_TX_start(constModePrint, sizeof(constModePrint));
			size = dataDeinterpret(data, param.constMode);
			DMA_TX_start(data, size);
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// ������� ������ ����������� ������� ������
//--------------------------------------------------------------
void freqBwHandler(void)
{
	uint32_t result0, result1;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t freqBw0[] = "Set lower bound:\r\n";
	uint8_t freqBw1[] = "Set upper bound:\r\n";
	uint8_t freqBw[] = "Bandwidth of frequency of beats: [";
	uint8_t freqBwPoint[] = ",";
	uint8_t freqBwScob[] = "]";
	
	while (1)
	{
		DMA_TX_start(freqBw0, sizeof(freqBw0));
		DMA_TX_start(cursor, sizeof(cursor));
		result0 = readData();
		
		DMA_TX_start(freqBw1, sizeof(freqBw1));
		DMA_TX_start(cursor, sizeof(cursor));
		result1 = readData();

		// ��������� ������
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
			
			// ����� freqBw
			DMA_TX_start(freqBw, sizeof(freqBw));
			size = dataDeinterpret(data, param.freqBw0);
			DMA_TX_start(data, size);
			DMA_TX_start(freqBwPoint, sizeof(freqBwPoint));
			size = dataDeinterpret(data, param.freqBw1);
			DMA_TX_start(data, size);
			DMA_TX_start(freqBwScob, sizeof(freqBwScob));
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// ������� ������ ����������
//--------------------------------------------------------------
void limitAccHandler(void)
{
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t limit[] = "Enter limit accumulation:\r\n";
	uint8_t limitPrint[] = "Limit accumulation: ";
	
	while (1)
	{
		DMA_TX_start(limit, sizeof(limit));
		DMA_TX_start(cursor, sizeof(cursor));
		param.limitAcc = readData();
		
		// ��������� ������
		if (error < 0)
			errorHandler();
		
		// ����� limitAcc
		DMA_TX_start(limitPrint, sizeof(limitPrint));	
		size = dataDeinterpret(data, param.limitAcc);
		DMA_TX_start(data, size);
		DMA_TX_start(transferLine, sizeof(transferLine));
		return;
	}
}

//--------------------------------------------------------------
// ������� ������������ �������������
//--------------------------------------------------------------
void coefAdjHandler(void)
{
	uint32_t result;
	uint8_t size;
	uint8_t data[5] = {0};
	uint8_t paramAdj[] = "Enter adjustment coefficient:\r\n";
	uint8_t coefPrint[] = "Adjustment coefficient: ";
	
	while (1)
	{
		DMA_TX_start(paramAdj, sizeof(paramAdj));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
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
			
			// ����� coefAdj
			DMA_TX_start(coefPrint, sizeof(coefPrint));
			size = dataDeinterpret(data, param.coefAdj);
			DMA_TX_start(data, size);
			DMA_TX_start(transferLine, sizeof(transferLine));
			return;
		}
	}
}

//--------------------------------------------------------------
// ����� ������ �������
//--------------------------------------------------------------
void selectCapturePort(void)
{
	uint32_t result;
	uint8_t selPort[] = "Select capture port:\r\n[1] PC2\r\n[2] PE2\r\n";
	uint8_t selPortPrint[] = "Port: ";
	uint8_t portPc2[] = "PC2\r\n";
	uint8_t portPe2[] = "PE2\r\n";
	
	while(1)
	{
		DMA_TX_start(selPort, sizeof(selPort));
		DMA_TX_start(cursor, sizeof(cursor));
		result = readData();
		
		// ��������� ������
		if (error < 0)
		{
			errorHandler();
		}
		else if ((result < 1) | (result > 2))
		{
			error = selectPortError;
			errorHandler();
		}
		else
		{
			flagPort = result;
			
			// ����� ����� �������
			DMA_TX_start(selPortPrint, sizeof(selPortPrint));
			if (result == 1)
				DMA_TX_start(portPc2, sizeof(portPc2));
			else if (result == 2)
				DMA_TX_start(portPe2, sizeof(portPe2));
			
			return;
		}
	}
}

//--------------------------------------------------------------
// ����� ���������� �� ���������
//--------------------------------------------------------------
void getConfig(void)
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

	// ����� typeMod
	DMA_TX_start(mode, sizeof(mode));
	size = dataDeinterpret(data, (uint32_t)param.typeMod);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));

	// ����� freqMod
	DMA_TX_start(freq, sizeof(freq));
	size = dataDeinterpret(data, param.freqMod);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// ����� bufMode
	DMA_TX_start(modeBuf, sizeof(modeBuf));
	size = dataDeinterpret(data, param.bufMode);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// ����� amplMod
	DMA_TX_start(modeAmp, sizeof(modeAmp));
	size = dataDeinterpret(data, param.amplMod);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// ����� constMode
	DMA_TX_start(constMode, sizeof(constMode));
	size = dataDeinterpret(data, param.constMode);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// ����� freqBw
	DMA_TX_start(freqBw, sizeof(freqBw));
	size = dataDeinterpret(data, param.freqBw0);
	DMA_TX_start(data, size);
	DMA_TX_start(freqBwPoint, sizeof(freqBwPoint));
	size = dataDeinterpret(data, param.freqBw1);
	DMA_TX_start(data, size);
	DMA_TX_start(freqBwScob, sizeof(freqBwScob));
	DMA_TX_start(transferLine, sizeof(transferLine));
		
	// ����� limitAcc
	DMA_TX_start(limit, sizeof(limit));	
	size = dataDeinterpret(data, param.limitAcc);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
	// ����� coefAdj
	DMA_TX_start(coef, sizeof(coef));
	size = dataDeinterpret(data, param.coefAdj);
	DMA_TX_start(data, size);
	DMA_TX_start(transferLine, sizeof(transferLine));
	
}

//--------------------------------------------------------------
// ���������� ������
//--------------------------------------------------------------
void errorHandler(void)
{
	uint8_t selectModeErrorArr[] = "error selectModeError: Incorrect select mode\r\n\n";
	uint8_t incorInpArr[] = "error incorInp: Incorrect input\r\n\n";
	uint8_t typeModeErrorArr[] = "error typeModeError: Incorrect type mode(values in the range 1-3)\r\n\n";
	uint8_t freqModErrorArr[] = "error freqModError: Incorrect frequency(values less 200kHz)\r\n\n";
	uint8_t bufModeErrorArr[] = "error bufModeError: Incorrect length buffer(values less 256)\r\n\n";
	uint8_t amplModErrorArr[] = "error amplModError: Incorrect amplitude(sinus - less 2048, other - less 4096)\r\n\n";
	uint8_t freqBwErrorArr[] = "error freqBwError: Incorrect bandwidth\r\n\n";
	uint8_t coefAdjErrorArr[] = "error coefAdjError: Incorrect coefficient(values in the range 0-100)\r\n\n";
	uint8_t constModeErrorArr[] = "error constModeError: Incorrect constant(values less 4096)\r\n\n";
	uint8_t selectPortErrorArr[] = "error selectPortError: Incorrect capture port\r\n\n";
	
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
		case selectModeError:
			DMA_TX_start(selectModeErrorArr, sizeof(selectModeErrorArr));
			break;
		case selectPortError:
			DMA_TX_start(selectPortErrorArr, sizeof(selectPortErrorArr));
			break;
	}
	
	error = 0;
}







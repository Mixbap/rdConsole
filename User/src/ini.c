#include "ini.h"
#include <math.h>

DMA_CtrlDataInitTypeDef DMA_InitStructure;
DMA_ChannelInitTypeDef DMA_Channel_InitStructure;

uint16_t sin_mod[MODULATOR_BUFF] = {0};
uint16_t triangle_mod[MODULATOR_BUFF] = {0};
uint16_t pila_mod[MODULATOR_BUFF] = {0};

/*
// Отчеты синуса
uint16_t sin_gen[60] = {11,45,100,177,274,391,526,678,844,1024,1215,1415,
1622,1834,2048,2262,2474,2681,2881,3072,3252,3418,3570,3705,3822,3919,
3996,4051,4085,4095,4085,4051,3996,3919,3822,3705,3570,3418,3252,3072,
2881,2681,2474,2262,2048,1834,1622,1415,1215,1024,844,678,526,391,274,
177,100,45,11,0};
*/
//--------------------------------------------------------------
// Инициализация CLK = 80 MHz от HSE
//--------------------------------------------------------------
void CLK_80_ini(void)
{
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	if (RST_CLK_HSEstatus() == SUCCESS) 
	{
		RST_CLK_CPU_PLLconfig ( RST_CLK_CPU_PLLsrcHSEdiv1, 9 );

		RST_CLK_CPU_PLLcmd(ENABLE);
		if (RST_CLK_CPU_PLLstatus() == SUCCESS) 
		{
			RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1); 
			RST_CLK_CPU_PLLuse(ENABLE); 
			RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3); 
		}
		else 
		{	while(1);}
	}
	else 
	{	while(1);}
}
//------------------------------------------------------------
//Инициализация порта DAC2 (PE0 XS10 13pin)
//------------------------------------------------------------

void DAC2_PortE_ini(void)
{
	PORT_InitTypeDef PortE_Ini;
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	
	PortE_Ini.PORT_MODE = PORT_MODE_ANALOG;
	PortE_Ini.PORT_OE = PORT_OE_OUT;
	PortE_Ini.PORT_Pin = PORT_Pin_0;
	PortE_Ini.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortE_Ini.PORT_PULL_UP = PORT_PULL_UP_OFF;

	PORT_Init(MDR_PORTE, &PortE_Ini);
}
//------------------------------------------------------------
//Инициализация DAC2
//------------------------------------------------------------
void DAC2_ini(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC, ENABLE);
	
	DAC_DeInit();
	
	DAC2_Init(DAC2_AVCC);
	
	DAC2_Cmd(ENABLE);
}

//------------------------------------------------------------
//Инициализация LED (PB0, PB1, PB2, PB3)
//------------------------------------------------------------
void LED_PortB_ini(void)
{
	PORT_InitTypeDef PortB_Ini;
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	
	PortB_Ini.PORT_FUNC = PORT_FUNC_PORT;
	PortB_Ini.PORT_MODE = PORT_MODE_DIGITAL;
	PortB_Ini.PORT_OE = PORT_OE_OUT;
	PortB_Ini.PORT_Pin = PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3;
	PortB_Ini.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortB_Ini.PORT_PULL_UP = PORT_PULL_UP_ON;
	PortB_Ini.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTB, &PortB_Ini);
}
//------------------------------------------------------------
//Расчет массива отсчетов синуса
//------------------------------------------------------------
void Sin_massiv(void)
{
	uint32_t i;
	float w;
	
  w = 2 * 3.1415926535 / MODULATOR_BUFF; 
	
	for (i = 0; i < MODULATOR_BUFF; i++)
	{
     sin_mod[i] = MODULATOR_CONST + MODULATOR_AMPLITUDE * sin(w * i);
  }
}
//------------------------------------------------------------
// Расчет массива отсчетов пилы
//------------------------------------------------------------
void Pila_massiv(void)
{
	uint32_t i;
	float k;
	
	k = (float)(MODULATOR_AMPLITUDE_PILA) / MODULATOR_BUFF;
	
	for (i = 0; i < MODULATOR_BUFF-1; i++)
	{
		pila_mod[i] = k * i;
	}
	pila_mod[MODULATOR_BUFF-1] = pila_mod[0];
}

//------------------------------------------------------------
// Расчет массива отсчетов треугольника
//------------------------------------------------------------
void Triangle_massiv(void)
{
	uint32_t i;
	float k;
	
	k = (float)(MODULATOR_AMPLITUDE_TRIANGLE * 2) / MODULATOR_BUFF;
	
	for (i = 0; i < MODULATOR_BUFF/2; i++)
	{
		triangle_mod[i] = k * i;
	}
		for (i = MODULATOR_BUFF/2; i < MODULATOR_BUFF; i++)
	{
		triangle_mod[i] = k * (MODULATOR_BUFF-i);
	}
}

//------------------------------------------------------------
//Инициализация DAC2+DMA+TIM1
//------------------------------------------------------------
void DMA_DAC2_ini(void)
{
	RST_CLK_PCLKcmd (RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2, ENABLE);
	
	NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICER[0] = 0xFFFFFFFF;
	
	DMA_DeInit();
  DMA_StructInit (&DMA_Channel_InitStructure);
	
	DMA_InitStructure.DMA_SourceBaseAddr = (uint32_t) sin_mod;	              
  DMA_InitStructure.DMA_DestBaseAddr = (uint32_t)(&(MDR_DAC->DAC2_DATA));	
  DMA_InitStructure.DMA_CycleSize = MODULATOR_BUFF;							
  DMA_InitStructure.DMA_SourceIncSize = DMA_SourceIncHalfword;				    
  DMA_InitStructure.DMA_DestIncSize = DMA_DestIncNo;				              
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;     
  DMA_InitStructure.DMA_NumContinuous = DMA_Transfers_32;				          
  DMA_InitStructure.DMA_SourceProtCtrl = DMA_SourcePrivileged;			      
  DMA_InitStructure.DMA_DestProtCtrl = DMA_DestPrivileged;			          
  DMA_InitStructure.DMA_Mode = DMA_Mode_Basic;							              
  
  DMA_Channel_InitStructure.DMA_PriCtrlData = &DMA_InitStructure;
  DMA_Channel_InitStructure.DMA_Priority = DMA_Priority_Default;
  DMA_Channel_InitStructure.DMA_UseBurst = DMA_BurstClear;
  DMA_Channel_InitStructure.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;
  
  DMA_Init (DMA_Channel_TIM1, &DMA_Channel_InitStructure);						  
 
  MDR_DMA->CHNL_REQ_MASK_CLR = 1 << DMA_Channel_TIM1;
  MDR_DMA->CHNL_USEBURST_CLR = 1 << DMA_Channel_TIM1;
 
  DMA_Cmd (DMA_Channel_TIM1, ENABLE);

  NVIC_SetPriority (DMA_IRQn, 1); 
}
//------------------------------------------------------------
// Инициализация TIM1 для работы с DMA
//------------------------------------------------------------	
void TIMER1_ini(void)
{
	TIMER_CntInitTypeDef TIM_CntInit;	
	
  TIMER_DeInit (MDR_TIMER1);

  RST_CLK_PCLKcmd (RST_CLK_PCLK_TIMER1, ENABLE);  
	
  TIMER_BRGInit (MDR_TIMER1, TIMER_HCLKdiv1);
	
  TIM_CntInit.TIMER_Prescaler                = 0;                                  
  TIM_CntInit.TIMER_Period                   = MODULATOR_PERIOD (MODULATOR_FREQ); 
  TIM_CntInit.TIMER_CounterMode              = TIMER_CntMode_ClkFixedDir;          
  TIM_CntInit.TIMER_CounterDirection         = TIMER_CntDir_Up;                     
  TIM_CntInit.TIMER_EventSource              = TIMER_EvSrc_None;                    
  TIM_CntInit.TIMER_FilterSampling           = TIMER_FDTS_TIMER_CLK_div_1;         
  TIM_CntInit.TIMER_ARR_UpdateMode           = TIMER_ARR_Update_Immediately;			  
  TIM_CntInit.TIMER_ETR_FilterConf           = TIMER_Filter_1FF_at_TIMER_CLK;
  TIM_CntInit.TIMER_ETR_Prescaler            = TIMER_ETR_Prescaler_None;
  TIM_CntInit.TIMER_ETR_Polarity             = TIMER_ETRPolarity_NonInverted;
  TIM_CntInit.TIMER_BRK_Polarity             = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (MDR_TIMER1, &TIM_CntInit);                                        
 
  TIMER_DMACmd (MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);

  TIMER_Cmd (MDR_TIMER1, ENABLE);	

  NVIC_EnableIRQ(DMA_IRQn);
}
//------------------------------------------------------------
// Инициализация модулятора
//------------------------------------------------------------
void Modulator_ini(void)
{
	Sin_massiv();
	Pila_massiv();
	Triangle_massiv();
	
	DMA_DAC2_ini();
	
	DAC2_PortE_ini();
	DAC2_ini();
	
	TIMER1_ini();
}
//------------------------------------------------------------
// Инициализация таймера в режиме захвата (PC2 XS9 26 pin)
//------------------------------------------------------------
void TIMER_CAPTURE_ini(void)
{	
  PORT_InitTypeDef PortInitStructure;

  TIMER_CntInitTypeDef TimerInitStructure;

  TIMER_ChnInitTypeDef TimerChnInitStructure;
	
  TIMER_ChnOutInitTypeDef	TimerChnOutInitStructure;
	
	RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTC | RST_CLK_PCLK_TIMER3,	ENABLE);

  TIMER_BRGInit (RD_TIMER, TIMER_HCLKdiv1);     
	
	PORT_StructInit (&PortInitStructure);
	PortInitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
	PortInitStructure.PORT_OE    = PORT_OE_IN;
	PortInitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PortInitStructure.PORT_Pin   = RD_Pin; 
	PORT_Init (RD_PORT, &PortInitStructure);	

  TIMER_DeInit (RD_TIMER);

	TIMER_CntStructInit (&TimerInitStructure);
  TimerInitStructure.TIMER_Prescaler = 0;  										              
  TimerInitStructure.TIMER_Period = 65535;                                  
  TimerInitStructure.TIMER_CounterDirection = TIMER_CntDir_Up;		          
  TimerInitStructure.TIMER_CounterMode = TIMER_CntMode_EvtFixedDir;         
  TimerInitStructure.TIMER_EventSource = TIMER_EvSrc_CH1;			   					  
  TimerInitStructure.TIMER_ARR_UpdateMode = TIMER_ARR_Update_Immediately;   
  TimerInitStructure.TIMER_FilterSampling = TIMER_FDTS_TIMER_CLK_div_1;     
  TimerInitStructure.TIMER_ETR_FilterConf = TIMER_Filter_1FF_at_TIMER_CLK;   
  TimerInitStructure.TIMER_ETR_Prescaler = TIMER_ETR_Prescaler_None;        
  TimerInitStructure.TIMER_ETR_Polarity = TIMER_ETRPolarity_NonInverted;
  TimerInitStructure.TIMER_BRK_Polarity = TIMER_BRKPolarity_NonInverted;
  TIMER_CntInit (RD_TIMER, &TimerInitStructure);

  TIMER_ChnStructInit (&TimerChnInitStructure);
  TimerChnInitStructure.TIMER_CH_Number = RD_CHN; 											   
  TimerChnInitStructure.TIMER_CH_Mode = TIMER_CH_MODE_CAPTURE;							
  TimerChnInitStructure.TIMER_CH_EventSource = TIMER_CH_EvSrc_PE;            
	TIMER_ChnInit (RD_TIMER, &TimerChnInitStructure);

  TIMER_ChnOutStructInit (&TimerChnOutInitStructure);
  TimerChnOutInitStructure.TIMER_CH_Number = RD_CHN;																
  TimerChnOutInitStructure.TIMER_CH_DirOut_Polarity = TIMER_CHOPolarity_NonInverted;  
  TimerChnOutInitStructure.TIMER_CH_DirOut_Source = TIMER_CH_OutSrc_Only_0;						
  TimerChnOutInitStructure.TIMER_CH_DirOut_Mode = TIMER_CH_OutMode_Input;							
  TIMER_ChnOutInit (RD_TIMER, &TimerChnOutInitStructure);

  TIMER_Cmd (RD_TIMER, ENABLE);	

}

//------------------------------------------------------------------
// Инициализация UART1+DMA TX RX (Tx - XS9 18 pin; Rx - XS9 19 pin)
//------------------------------------------------------------------
void UART1_DMA_ini(void)
{
	PORT_InitTypeDef PortB_Ini;
	UART_InitTypeDef UART1_Ini;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	
	
  PortB_Ini.PORT_FUNC  = PORT_FUNC_ALTER;
  PortB_Ini.PORT_MODE  = PORT_MODE_DIGITAL;
  PortB_Ini.PORT_SPEED = PORT_SPEED_FAST;
  PortB_Ini.PORT_Pin   = PORT_Pin_5;
  PortB_Ini.PORT_OE    = PORT_OE_OUT;
  PORT_Init(MDR_PORTB, &PortB_Ini);
	
	PortB_Ini.PORT_Pin   = PORT_Pin_6;
  PortB_Ini.PORT_OE    = PORT_OE_IN;
	PORT_Init(MDR_PORTB, &PortB_Ini);

	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	
	UART_DeInit(MDR_UART1);
	UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
	UART_StructInit (&UART1_Ini);
	
	UART1_Ini.UART_BaudRate = 9600;
	UART1_Ini.UART_FIFOMode = UART_FIFO_OFF;
	UART1_Ini.UART_HardwareFlowControl = UART_HardwareFlowControl_TXE | UART_HardwareFlowControl_RXE;
	UART1_Ini.UART_Parity = UART_Parity_No;
	UART1_Ini.UART_StopBits = UART_StopBits1;
	UART1_Ini.UART_WordLength = UART_WordLength8b;
	UART_Init(MDR_UART1, &UART1_Ini);

	UART_DMAConfig(MDR_UART1,UART_IT_FIFO_LVL_8words,UART_IT_FIFO_LVL_8words);
	
	UART_DMACmd(MDR_UART1, UART_DMA_TXE, ENABLE);
	UART_DMACmd(MDR_UART1, UART_DMA_RXE, ENABLE);
	
	UART_Cmd(MDR_UART1, ENABLE);
}
//------------------------------------------------------------
// Инициализация DMA
//------------------------------------------------------------
void DMA_ini(void)
{
	DMA_ChannelInitTypeDef DMA_InitStr;

	//CLK ON (SSP1 and SSP2 ON --/--> error)
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DMA | RST_CLK_PCLK_SSP1 | RST_CLK_PCLK_SSP2, ENABLE);
	
	//Global Interruts OFF
	NVIC->ICPR[0] = 0xFFFFFFFF;
	NVIC->ICER[0] = 0xFFFFFFFF;

	DMA_DeInit();
	DMA_StructInit(&DMA_InitStr);

	MDR_DMA->CHNL_REQ_MASK_CLR = 0xFFFFFFFF;
  MDR_DMA->CHNL_USEBURST_CLR = 0xFFFFFFFF;

}

//------------------------------------------------------------
// Запуск DMA на передачу (UART1)
//------------------------------------------------------------
void DMA_TX_start(uint8_t* buf, uint32_t buf_size)
{
	DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
	DMA_ChannelInitTypeDef DMA_InitStr;
	
	DMA_DeInit();
	DMA_StructInit(&DMA_InitStr);

	DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)buf;
  DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)(&(MDR_UART1->DR));
  DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncByte;
  DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncNo;
  DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_PriCtrlStr.DMA_Mode = DMA_Mode_Basic;
  DMA_PriCtrlStr.DMA_CycleSize = buf_size;
  DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;
	
	DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
  DMA_InitStr.DMA_Priority = DMA_Priority_Default;
  DMA_InitStr.DMA_UseBurst = DMA_BurstClear;
  DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;

  DMA_Init(DMA_Channel_UART1_TX, &DMA_InitStr);
	
	while ((DMA_GetFlagStatus(DMA_Channel_UART1_TX, DMA_FLAG_CHNL_ENA ))); // проверка конца передачи
}

//------------------------------------------------------------
// Запуск DMA на прием(UART1)
//------------------------------------------------------------
void DMA_RX_start(uint8_t* buf, uint32_t buf_size)
{
	DMA_CtrlDataInitTypeDef DMA_PriCtrlStr;
	DMA_ChannelInitTypeDef DMA_InitStr;
	uint8_t idx = 0;
	
	DMA_DeInit();
	DMA_StructInit(&DMA_InitStr);
	
	DMA_PriCtrlStr.DMA_SourceBaseAddr = (uint32_t)(&(MDR_UART1->DR));
  DMA_PriCtrlStr.DMA_DestBaseAddr = (uint32_t)buf;
  DMA_PriCtrlStr.DMA_SourceIncSize = DMA_SourceIncNo;
  DMA_PriCtrlStr.DMA_DestIncSize = DMA_DestIncByte;
  DMA_PriCtrlStr.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_PriCtrlStr.DMA_Mode = DMA_Mode_Basic;
  DMA_PriCtrlStr.DMA_CycleSize = buf_size;
  DMA_PriCtrlStr.DMA_NumContinuous = DMA_Transfers_1;
  DMA_PriCtrlStr.DMA_SourceProtCtrl = DMA_SourcePrivileged;
  DMA_PriCtrlStr.DMA_DestProtCtrl = DMA_DestPrivileged;
	
	DMA_InitStr.DMA_PriCtrlData = &DMA_PriCtrlStr;
  DMA_InitStr.DMA_Priority = DMA_Priority_High;
  DMA_InitStr.DMA_UseBurst = DMA_BurstClear;
  DMA_InitStr.DMA_SelectDataStructure = DMA_CTRL_DATA_PRIMARY;

  DMA_Init(DMA_Channel_UART1_RX, &DMA_InitStr);
	
	// проверка конца приема нажатием enter
	while(1)
	{
		for (idx = 0; idx < buf_size; idx++)
		{
			if (buf[idx] == 13)  // enter
				return;
		}
	}
	
	//while ((DMA_GetFlagStatus(DMA_Channel_UART1_RX, DMA_FLAG_CHNL_ENA ))); // проверка конца приема
}

//------------------------------------------------------------
// Инициализация внешнего прерывания по PB10 (XS9 23 pin)
//------------------------------------------------------------
void PortB_EXT_ini(void)
{
	PORT_InitTypeDef PortB_Ini;
	//RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	
	// XS9 23 pin
	PortB_Ini.PORT_FUNC = PORT_FUNC_ALTER;
	PortB_Ini.PORT_MODE = PORT_MODE_DIGITAL;
	PortB_Ini.PORT_OE = PORT_OE_IN;
	PortB_Ini.PORT_Pin = PORT_Pin_10;
	PortB_Ini.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
	PortB_Ini.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortB_Ini.PORT_SPEED = PORT_SPEED_MAXFAST;

	PORT_Init(MDR_PORTB, &PortB_Ini);
}






#include "ini.h"
#include <math.h>

DMA_CtrlDataInitTypeDef DMA_InitStructure;
DMA_ChannelInitTypeDef DMA_Channel_InitStructure;

uint16_t sin_mod[MODULATOR_BUFF] = {0};

/*
uint16_t sin_gen[60] = {11,45,100,177,274,391,526,678,844,1024,1215,1415,
1622,1834,2048,2262,2474,2681,2881,3072,3252,3418,3570,3705,3822,3919,
3996,4051,4085,4095,4085,4051,3996,3919,3822,3705,3570,3418,3252,3072,
2881,2681,2474,2262,2048,1834,1622,1415,1215,1024,844,678,526,391,274,
177,100,45,11,0};
*/
//--------------------------------------------------------------
//CLK = 80 MHz
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
//Initialization port DAC2 (PE0 XS10 13pin)
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
//Initialization DAC2
//------------------------------------------------------------
void DAC2_ini(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC, ENABLE);
	
	DAC_DeInit();
	
	DAC2_Init(DAC2_AVCC);
	
	DAC2_Cmd(ENABLE);
}

//------------------------------------------------------------
//Initialization LED (PB0, PB1, PB2, PB3)
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
//Write massive sinus
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
//Initialization DMA
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
//Initialization Timer1 for triggered DMA
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
//Initialization modulator
//------------------------------------------------------------
void Modulator_ini(void)
{
	Sin_massiv();
	
	DMA_DAC2_ini();
	
	DAC2_PortE_ini();
	DAC2_ini();
	
	TIMER1_ini();
}
//------------------------------------------------------------
//Timer Capture Event (PC2 XS9 26 pin)
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
//--------------------------------------------------------------
//--------------------------------------------------------------




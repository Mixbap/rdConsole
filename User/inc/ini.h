// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: ini.c 
// Назначение: Инициализация периферии микроконтроллера  
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************

#ifndef INI_H
#define INI_H

#include <stdint.h>

#include "MDR32Fx.h"                    // Device header
#include "MDR32F9Qx_config.h"           // Keil::Device:Startup_MDR1986BE9x
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_dac.h"              // Keil::Drivers:DAC
#include "MDR32F9Qx_timer.h"            // Keil::Drivers:TIMER
#include "MDR32F9Qx_dma.h"              // Keil::Drivers:DMA
#include "MDR32F9Qx_eeprom.h"           // Keil::Drivers:EEMPROM
#include "MDR32F9Qx_uart.h"             // Keil::Drivers:UART

#include "config.h"

//------------------------------------------------------------
#ifdef LED
	#define LED0_ON()		PORT_SetBits(MDR_PORTB, PORT_Pin_0)
	#define LED1_ON()		PORT_SetBits(MDR_PORTB, PORT_Pin_1)
	#define LED2_ON()		PORT_SetBits(MDR_PORTB, PORT_Pin_2)
	#define LED3_ON()		PORT_SetBits(MDR_PORTB, PORT_Pin_3)

	#define LED0_OFF()	PORT_ResetBits(MDR_PORTB, PORT_Pin_0)
	#define LED1_OFF()	PORT_ResetBits(MDR_PORTB, PORT_Pin_1)
	#define LED2_OFF()	PORT_ResetBits(MDR_PORTB, PORT_Pin_2)
	#define LED3_OFF()	PORT_ResetBits(MDR_PORTB, PORT_Pin_3)
#endif
//------------------------------------------------------------

#ifdef IK_PORT
	#define IK_ON()		PORT_SetBits(MDR_PORTB, PORT_Pin_0)
	#define IK_OFF()	PORT_ResetBits(MDR_PORTB, PORT_Pin_0)
#endif

#define RD_PORT	    MDR_PORTC				
#define RD_Pin		  PORT_Pin_2        
#define RD_TIMER	MDR_TIMER3 				
#define RD_CHN	  TIMER_CHANNEL1

void delay(uint32_t n);

void DAC2_PortE_ini(void);
void LED_PortB_ini(void);
void IK_port_ini(void);
void CLK_80_ini(void);
void DAC2_ini(void);






void TIMER_CAPTURE_ini(void);

void UART1_DMA_ini(void);
void DMA_ini(void);

void WriteStringDMA(char* str);
int ReadStringDMA(char* buf, int bufSize);

void DMA_TX_start(uint8_t* buf, uint32_t buf_size);
uint8_t DMA_RX_start(uint8_t* buf, uint32_t buf_size);

void PortB_EXT_ini(void);



extern DMA_CtrlDataInitTypeDef DMA_InitStructure;
extern DMA_ChannelInitTypeDef DMA_Channel_InitStructure;


// Структура конфигурации блока
typedef struct
{
	uint32_t mode;
	uint32_t typeMod;
	uint32_t freqMod;
	uint32_t bufMode;
	uint32_t amplMod;
	uint8_t freqBw0;
	uint8_t freqBw1;
	uint32_t limitAcc;
	uint32_t coefAdj;
	uint32_t constMode;
}rdParam;

void rdParamDefIni(rdParam *localParam);
void Sin_massiv(rdParam *localParam);
void Pila_massiv(rdParam *localParam);
void Triangle_massiv(rdParam *localParam);
void Modulator_ini(rdParam *localParam);
void DMA_DAC2_ini(rdParam *localParam);
void TIMER1_ini(rdParam *localParam);


extern rdParam param;

#endif

#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "ADC.h"
#include "uart.h"

void initADC()
{
	LPC_SC->PCONP |= ((1U << 12) | (1U << 15));
	
	PIN_Configure(0, 24, 1, 2, 0); 
	//UART_Send("ADC config\n\r");
	LPC_ADC->ADCR = (1 << 21) | (1 << 1) | ((100 - 1) << 8);
	
	//UART_Send("ADC int config\n\r");
	LPC_ADC->ADINTEN = (1 << 1);
	
	NVIC_EnableIRQ(ADC_IRQn);
	
	//UART_Send("ADC conv start\n\r");
	ADC_StartConvertion();
}

void ADC_StartConvertion()
{
	LPC_ADC->ADCR |= (1 << 24); 
}

void ADC_IRQHandler()
{
	uint16_t val = LPC_ADC->ADDR1 >> 4;
	
	UART_SendNum(val);
	UART_Send("\n\r");
}

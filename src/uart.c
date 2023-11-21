#include "LPC17xx.h"
#include "UART_LPC17xx.h"
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "RTE_Device.h"                 // Keil::Device:Startup
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include <string.h>
#include <stdio.h>

void UART_Send(const char * txt) {
	const char* c = txt;
	while(*c != 0)
	{
		while (! ( LPC_UART0->LSR & (1 << 5) ) ); // THRE flag // delay
		//for (int i = 0; i < 15 && c != '\0'; i++)
		//{		
			LPC_UART0->THR = *c;
			c++;
		//}
	}
}

void UART_SendNum(int num) {
	char buff[32] = { 0 };
	sprintf(buff, "%d", num);
	UART_Send(buff);
}


void UART_IRQHandler(USART_RESOURCES* res)
{
	if(res->irq_num != UART0_IRQn) {
		return;
	}
	
	if ((LPC_UART0->IIR & 0b1110) == 0b0010)
	{
		
	}
}

void UART_Init() {
	//InilializeUsart(&USARTdrv2, &Driver_USART2);	
	//InilializeUsart(&USARTdrv0, &Driver_USART0);
	
	
	PIN_Configure(0, 3, 1, 0, 0);
	PIN_Configure(0, 2, 1, 2, 0);
	
	
	//LPC_UART0->THR = 'A';
	LPC_UART0->LCR =  (0b1  << 7)
					| (0b0  << 6)
					| (0b00 << 4)
					| (0b0  << 3)
					| (0b0  << 2)
					| (0b11 << 0);
	
	// B = Fpclk / (16 * DL * (1+ A/D)) = 115200
	// DL = Fpclk / (16 * B) = 27,12 ~ 27 | blad mniejszy niz 1%
	LPC_UART0->DLL = 27;
	LPC_UART0->DLM = 0;
	
	LPC_UART0->LCR =  (0b0  << 7)
					//| (0b0  << 6)
					//| (0b00 << 4)
					//| (0b0  << 3)
					//| (0b0  << 2)
					| (0b11 << 0);
	
	LPC_UART0->IER |= (1 << 1); // THRE Interrupt
	NVIC_EnableIRQ(UART0_IRQn);
	
	UART_Send("Hello\r\n");
}

void ARM_USART_eventFunc(uint32_t ev)
{
	
}

void InilializeUsart(ARM_DRIVER_USART** UsartWrapper, ARM_DRIVER_USART* Usart)
{
	(*UsartWrapper) = Usart;
	(*UsartWrapper)->Initialize(ARM_USART_eventFunc);
	(*UsartWrapper)->PowerControl(ARM_POWER_FULL);
	(*UsartWrapper)->Control(ARM_USART_MODE_ASYNCHRONOUS |
										ARM_USART_DATA_BITS_8 |
										ARM_USART_PARITY_NONE |
										ARM_USART_STOP_BITS_1 |
										ARM_USART_FLOW_CONTROL_NONE, 115200);
	(*UsartWrapper)->Control (ARM_USART_CONTROL_TX, 1);
	(*UsartWrapper)->Control (ARM_USART_CONTROL_RX, 1);
}


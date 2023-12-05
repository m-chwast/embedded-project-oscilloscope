#include "core.h"

#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "RTE_Device.h"                 // Keil::Device:Startup
#include <string.h>
#include "uart.h"
#include "console.h"
#include "lcd.h"
#include "buttons.h"
#include "LCD_ILI9325.h"
#include "ADC.h"

void Timer_Init();

void RTC_Init()
{
	//clock enable
	LPC_RTC->CCR = (1 << 0);
	LPC_RTC->CIIR = (1 << 0); //interrupt on seconds increment
	NVIC_EnableIRQ(RTC_IRQn);
}

void GPIO_Init()
{
	// Screen resolution 240x320
	PIN_Configure(0, 19, 0, 0, 0); //P0.19 as input, pull up
	//LPC_GPIOINT->IO0IntEnR |= (1 << 19);
	LPC_GPIOINT->IO0IntEnF = (1 << 19);
	NVIC_EnableIRQ(EINT3_IRQn);
}

void TIMER0_IRQHandler()
{
	LPC_TIM0->IR = (1 << 0);	//clear MR0 int
	{
		// Body
		ADC_StartConvertion();
	}
}

void RTC_IRQHandler()
{
	LPC_RTC->ILR = (1 << 0);	//reset interrupt
	static bool flag = false;
	
	{
		// Body
	}
	
	flag = !flag;
}

void EINT3_IRQHandler()
{
	LPC_GPIOINT->IO0IntClr |= (1 << 19);
	
	static uint32_t lastInt;
	if(sysTicks - lastInt < 50) {
		return;
	}
	lastInt = sysTicks;
	
	UART_Send("Tu GPIO co tam\r\n");
}

int main()
{
	UART_Init();
	
	ASSERT("Timer init\r\n");
	Timer_Init();

	ASSERT("RTC init\r\n");
	RTC_Init();
	ASSERT("GPIO init\r\n");
	GPIO_Init();
	// KEY2_Init();	
	ASSERT("Init lcd\r\n");
	initLCD();
	
	ASSERT("Init adc\r\n");
	initADC();
	
	while (1)
	{
		UART_ReadTask();
		Console_Task();
	}
}

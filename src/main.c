#include "LPC17xx.h"
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "RTE_Device.h"                 // Keil::Device:Startup
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include <string.h>
#include "uart.h"
#include "lcd.h"
#include "LCD_ILI9325.h"

#define SysTimerClock (1000 * 100)	// 1ms sysTick (for 100MHz sys clock)

void OnClikcTask(void);
void UART_ReadTask(char* buff, int* buffsize);

volatile uint32_t sysTicks = 0u;
void SysTick_Handler(void)
{
	sysTicks++;
}

void delay(int s)
{
	sysTicks = 0;
	while (sysTicks < s);
}

extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART2;
//static ARM_DRIVER_USART* USARTdrv0;
//static ARM_DRIVER_USART* USARTdrv2;

void TIMER0_IRQHandler() {
	UART_Send("Ping\r\n");
	LPC_TIM0->IR = (1 << 0);	//clear MR0 int
}


void Timer_Init() {
	//interrupt timer reset on match 0 
	LPC_TIM0->MCR = (1 << 0) | (1 << 1);
	
	LPC_TIM0->MR0 = 12500000 * 4;
	
	NVIC_EnableIRQ(TIMER0_IRQn);
	
	LPC_TIM0->TCR = (1 << 0);
}

void RTC_IRQHandler() {
	LPC_RTC->ILR = (1 << 0);	//reset interrupt
	
	static bool flag = false;
	
	if(flag) {
		UART_Send("Tik\r\n");
	}
	else {
		UART_Send("Tak\r\n");
	}
	flag = !flag;
}

void RTC_Init() {
	//clock enable
	LPC_RTC->CCR = (1 << 0);
	
	LPC_RTC->CIIR = (1 << 0); //interrupt on seconds increment
	
	NVIC_EnableIRQ(RTC_IRQn);
}

volatile bool buttonClicked = false;
void EINT0_IRQHandler() {
	LPC_SC->EXTINT = (1 << 0);	//clear int flag
	
	static uint32_t lastInt;
	if(sysTicks - lastInt < 25) {
		return;
	}
	lastInt = sysTicks;
	
	buttonClicked = true;
	
	UART_Send("Siema elo mordo co tam sie u ciebie dzieje\r\n");
}

void KEY2_Init() {
	
	PIN_Configure(2, 10, 1, 0, 0); //KEY2 as input, pull up

	PIN_Configure(0, 0, 0, 0, 0);
	LPC_GPIO0->FIODIR |= (1 << 0); //P0.0 as output

	LPC_SC->EXTMODE = (1 << 0);	//edge
	LPC_SC->EXTPOLAR = (0 << 0);	//falling
	LPC_SC->EXTINT = (1 << 0);	//clear int flag
	
	NVIC_EnableIRQ(EINT0_IRQn);
}

void EINT3_IRQHandler() {
	LPC_GPIOINT->IO0IntClr |= (1 << 19);
	
	static uint32_t lastInt;
	if(sysTicks - lastInt < 50) {
		return;
	}
	lastInt = sysTicks;
	
	UART_Send("Tu GPIO co tam\r\n");
}

void GPIO_Init() {
	// Screen resolution 240x320
	PIN_Configure(0, 19, 0, 0, 0); //P0.19 as input, pull up
	
	//LPC_GPIOINT->IO0IntEnR |= (1 << 19);
	LPC_GPIOINT->IO0IntEnF = (1 << 19);
	
	NVIC_EnableIRQ(EINT3_IRQn);
}

int main()
{
	SysTick_Config(SysTimerClock);

	UART_Init();
	int buffsize = 0;
	char buff[64] = { 0 };
	
	Timer_Init();
	RTC_Init();
	
	GPIO_Init();
	KEY2_Init();
	
	initLCD();

	Rect r = {40, 200, 20, 300, LCDCyan};
	drawRectangle(&r);
	
	drawText(30, 40, "Prosze dzialaj");
	drawTextNoBg(100, 100, "abc");
	
	drawLine(0, 0, 50, 300, LCDBlue);
	drawLine(100, 20, 50, 200, LCDBlue);

	while(1)
	{	
		UART_ReadTask(buff, &buffsize);
		OnClikcTask();
	}
}

void OnClikcTask()
{
	static int clickCoutn = 0;
	
	static uint32_t timeStart;
	if (buttonClicked && sysTicks - timeStart > 500)
	{
		timeStart = sysTicks;
		static bool flag = false;
		flag = !flag;
		if(flag) {
			LPC_GPIO0->FIOSET |= (1 << 0);	// set P0.0
		}
		else {
			LPC_GPIO0->FIOCLR |= (1 << 0);	// clear P0.0
		}
		clickCoutn++;
	}
	
	if (clickCoutn >= 8)
	{
		clickCoutn = 0;
		buttonClicked = false;
	}
}

void UART_ReadTask(char* buff, int* buffsize) {
	if((LPC_UART0->LSR & (0b1 << 0)) == 1) {
	char rbr = LPC_UART0->RBR;
	if(rbr == '\r') {
		buff[*buffsize] = '\r';
		buff[*buffsize + 1] = '\n';
		buff[*buffsize + 2] = 0;
		UART_Send(buff);
		*buffsize = 0;
	}
	else
	{
		buff[*buffsize] = rbr;
	}
		(*buffsize)++;
	}
}

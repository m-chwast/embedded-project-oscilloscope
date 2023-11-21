#include <stdint.h>
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
#include "uart.h"

extern uint32_t sysTicks;

void KEY2_Init()
{
	
	PIN_Configure(2, 10, 1, 0, 0); //KEY2 as input, pull up

	PIN_Configure(0, 0, 0, 0, 0);
	LPC_GPIO0->FIODIR |= (1 << 0); //P0.0 as output

	LPC_SC->EXTMODE = (1 << 0);	//edge
	LPC_SC->EXTPOLAR = (0 << 0);	//falling
	LPC_SC->EXTINT = (1 << 0);	//clear int flag
	
	NVIC_EnableIRQ(EINT0_IRQn);
}

volatile int8_t buttonClicked = 0;
void EINT0_IRQHandler()
{
	LPC_SC->EXTINT = (1 << 0);	//clear int flag
	
	static uint32_t lastInt;
	if(sysTicks - lastInt < 25) {
		return;
	}
	lastInt = sysTicks;
	
	buttonClicked = 1;
	
	UART_Send("Siema elo mordo co tam sie u ciebie dzieje\r\n");
}

void OnClikcTask()
{
	static int clickCoutn = 0;
	
	static uint32_t timeStart;
	if (buttonClicked && sysTicks - timeStart > 500)
	{
		timeStart = sysTicks;
		static int8_t flag = 0;
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
		buttonClicked = 0;
	}
}

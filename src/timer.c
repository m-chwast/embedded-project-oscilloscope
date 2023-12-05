#include <stdint.h>

#include "LPC17xx.h"

#define SysTimerClock (1000 * 100 * 1000)	// 1ms sysTick (for 100MHz sys clock)

void Timer_Init()
{
	SysTick_Config(SysTimerClock);
	
	//interrupt timer reset on match 0 
	LPC_TIM0->MCR = (1 << 0) | (1 << 1);
	LPC_TIM0->MR0 = 12500000 * 4;
	NVIC_EnableIRQ(TIMER0_IRQn);
	LPC_TIM0->TCR = (1 << 0);
}


volatile uint32_t sysTicks = 0u;

void SysTick_Handler(void)
{
	sysTicks++;
}

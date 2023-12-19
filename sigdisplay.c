#include "sigdisplay.h"
#include "lcd.h"
#include <stdlib.h>

#include "uart.h"

#define SIGNAL_WINDOW_H 150
#define SIGNAL_WINDOW_W 320

#define PX_PER_DIV 50


static uint32_t mvPerDiv = 1000;

static uint8_t displayBuff[SIGNAL_WINDOW_W];


static void DisplayStoredBuff(void)
{
	uint16_t color565 = RGBto565(0, 0, 255);
	for(uint32_t i = 0; i < SIGNAL_WINDOW_W; i++)
	{
		Coord c = OnSignal(i, displayBuff[i]);
		drawPixel(c.x, c.y, color565);
	}
}


void DisplaySignal(const uint16_t* data, uint32_t sampleNum) {
	uint8_t scaledData[300];
	//convert data to hold values in mv
	
	UART_Send("Data[0] = ");
	UART_SendNum(data[0]);
	UART_Send("\r\n");
	
	for(uint32_t i = 0; i < sampleNum; i++)
	{
		scaledData[i] = data[i] * 3300.0 / 0xFFFF;
	}
	
	for (int i = 0; i < 50; i++)
	{
		UART_SendNum(scaledData[i]);
		UART_Send(" | ");
	}
	
	const float pxPerMv = PX_PER_DIV / (mvPerDiv * 1.0f);
	
	//convert data to hold values in pixels
	for(uint32_t i = 0; i < sampleNum; i++)
	{
		scaledData[i] *= pxPerMv;
		if(scaledData[i] > SIGNAL_WINDOW_H)
		{
			scaledData[i] = SIGNAL_WINDOW_H;
		}
	}
	
	//TODO stuff
	int32_t buffSize = sampleNum < SIGNAL_WINDOW_W ? sampleNum : SIGNAL_WINDOW_W;
	
	for(uint32_t i = 0; i < buffSize; i++)
	{
		displayBuff[i] = scaledData[i] * 30;
	}
	for(uint32_t i = buffSize; i < SIGNAL_WINDOW_W; i++)
	{
		displayBuff[i] = 0;
	}
	
	for (int i = 0; i < 50; i++)
	{
		UART_SendNum(displayBuff[i]);
		UART_Send(" | ");
	}
	
	DisplayStoredBuff();
}
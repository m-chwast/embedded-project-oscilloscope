#include "sigdisplay.h"
#include "lcd.h"
#include <stdlib.h>


#define SIGNAL_WINDOW_H 150
#define SIGNAL_WINDOW_W 320

#define PX_PER_DIV 50


static uint32_t mvPerDiv = 1000;

static uint8_t displayBuff[SIGNAL_WINDOW_W];


static void DisplayStoredBuff(void) {
	uint16_t color565 = RGBto565(0, 0, 255);
	for(uint32_t i = 0; i < SIGNAL_WINDOW_W; i++) {
		Coord c = OnSignal(i, displayBuff[i]);
		drawPixel(c.x, c.y, color565);
	}
}


void Display_Signal(const uint16_t* data, uint32_t sampleNum, uint32_t sampleTrig) {
	uint8_t* scaledData = malloc(sampleNum);
	//convert data to hold values in mv
	for(uint32_t i = 0; i < sampleNum; i++) {
		scaledData[i] = data[i] * 3300 / 0xFFFF;
	}
	
	const float pxPerMv = PX_PER_DIV / mvPerDiv;
	
	//convert data to hold values in pixels
	for(uint32_t i = 0; i < sampleNum; i++) {
		scaledData[i] *= pxPerMv;
		if(scaledData[i] > SIGNAL_WINDOW_H) {
			scaledData[i] = SIGNAL_WINDOW_H;
		}
	}
	
	//TODO stuff
	
	for(uint32_t i = 0; i < sampleNum; i++) {
		uint32_t dataIndex = (sampleTrig + i) % sampleNum;
		displayBuff[i] = scaledData[dataIndex];
	}
	for(uint32_t i = sampleNum; i < SIGNAL_WINDOW_W; i++) {
		displayBuff[i] = 0;
	}
	
	free(scaledData);
	
	DisplayStoredBuff();
}
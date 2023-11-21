#ifndef __LCD__H_
#define __LCD__H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	int l, r;
	int t, b;
	uint16_t c;
} Rect;

void initLCD(void);
void displayTextire(const void* data);

void drawBackGround(uint16_t backgroundColor);
uint16_t RGBto565(uint8_t r, uint8_t g, uint8_t b);
void drawRectangle(const Rect* rect);
void drawText(int xPos, int yPos, const char* buff);
void drawTextNoBg(int xPos, int yPos, const char* buff);

void drawLine(int xs, int ys, int xe, int ye, uint16_t color);
	

#endif // __LCD__H_

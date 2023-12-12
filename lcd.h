#ifndef __LCD__H_
#define __LCD__H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_X 320
#define MAX_Y 240

#define SETTINGS_LINE_SIZE 16
#define SETTINGS_LETTER_SIZE 8

#define SET_MIN_X 0
#define SET_MAX_X MAX_X
#define SET_MIN_Y 0
#define SET_MAX_Y ((SETTINGS_LINE_SIZE + 1) * 4)

#define SIGNAL_MIN_X 0
#define SIGNAL_MAX_X MAX_X
#define SIGNAL_MIN_Y (SET_MAX_Y + 1)
#define SIGNAL_MAX_Y MAX_Y


typedef struct
{
	int x, y;
} Coord;

typedef struct
{
	int l, b;
	int r, t;
	uint16_t c;
} Rect;

void initLCD(void);
void displayTexture(const void* data);

void drawBackGround(uint16_t backgroundColor);

static void drawBorders(void);


uint16_t RGBto565(uint8_t r, uint8_t g, uint8_t b);
void drawPixel(int x, int y, uint16_t color);
void drawRectangle(const Rect* rect);
void drawText(int xPos, int yPos, const char* buff, bool drawBackGround);

void drawLine(int xs, int ys, int xe, int ye, uint16_t color);
void drawLineStrip(int n, Coord coords[n], uint16_t color);

Coord OnSignal(int x, int y);
static Coord LineCoord(int LineNumber, int pos);

#endif // __LCD__H_

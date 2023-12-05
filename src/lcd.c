#include "core.h"

#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"

#include "asciiLib.h"
#include "lcd.h"
#include "uart.h"
#include "math.h"
#include <stdio.h>

static Rect wafesDis = { SIGNAL_MIN_X, SIGNAL_MIN_Y, SIGNAL_MAX_X, SIGNAL_MAX_Y, LCDRed };
static Rect settingsDis = { SET_MIN_X, SET_MIN_Y, SET_MAX_X, SET_MAX_Y, LCDBlue };

static Coord OnSignal(int x, int y)
{
	return (Coord){ x + SIGNAL_MIN_X, y + SIGNAL_MIN_Y };
}

static Coord LineCoord(int LineNumber, int pos)
{
	return (Coord){ pos * SETTINGS_LETTER_SIZE, LineNumber * (SETTINGS_LINE_SIZE + 1) - 1 };
}

void initLCD()
{
	lcdConfiguration();
	uint16_t reg = lcdReadReg(OSCIL_ON);
	
	char buff[32] = { 0 };
	sprintf(buff, "0x%x\r\n", reg);
	
	UART_Send(buff);
	
	init_ILI9325();
	
	drawBorders();
}

static void drawBorders(void)
{
	drawLine(wafesDis.l, wafesDis.b, wafesDis.l, wafesDis.t, wafesDis.c);
	drawLine(wafesDis.l, wafesDis.t, wafesDis.r, wafesDis.t, wafesDis.c);
	drawLine(wafesDis.r, wafesDis.t, wafesDis.r, wafesDis.b, wafesDis.c);
	drawLine(wafesDis.r, wafesDis.b, wafesDis.l, wafesDis.b, wafesDis.c);
	
	drawLine(settingsDis.l, settingsDis.b, settingsDis.l, settingsDis.t, settingsDis.c);
	drawLine(settingsDis.l, settingsDis.t, settingsDis.r, settingsDis.t, settingsDis.c);
	drawLine(settingsDis.r, settingsDis.t, settingsDis.r, settingsDis.b, settingsDis.c);
	drawLine(settingsDis.r, settingsDis.b, settingsDis.l, settingsDis.b, settingsDis.c);
	

	Coord coords[] = {
		OnSignal(5, 5),
		OnSignal(50, 100),
		OnSignal(100, 75),
		OnSignal(250, 40),
		OnSignal(20, 60)
	};
	
	drawLineStrip(5, coords, LCDGreen);
	
	Coord textCoord;
	textCoord = LineCoord(1, 2);
	drawText(textCoord.x, textCoord.y, "WTF", false);
	textCoord = LineCoord(2, 2);
	drawText(textCoord.x, textCoord.y, "WTF", false);
	textCoord = LineCoord(3, 2);
	drawText(textCoord.x, textCoord.y, "WTF", false);
	textCoord = LineCoord(4, 2);
	drawText(textCoord.x, textCoord.y, "WTF", false);
}

void drawBackGround(uint16_t backgroundColor)
{
	lcdWriteReg(ADRY_RAM, 0);
	lcdWriteReg(ADRX_RAM, 0);
	lcdWriteIndex(DATA_RAM);
	for (int y = 0; y < LCD_MAX_Y; y++)
		for (int x = 0; x < LCD_MAX_X; x++)
			lcdWriteData(backgroundColor);
}

uint16_t RGBto565(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t color = ((r >> 3 ) << 11) | ((g >> 2) << 5) | (b >> 3);
	return color;
}

void drawPixel(int x, int y, uint16_t color)
{
	lcdWriteReg(ADRY_RAM, x);
	lcdWriteReg(ADRX_RAM, y);
	lcdWriteIndex(DATA_RAM);
	lcdWriteData(color);
}

void drawRectangle(const Rect* rect)
{
	lcdWriteReg(HADRPOS_RAM_START, rect->l);
	lcdWriteReg(HADRPOS_RAM_END, rect->r - 1);
	
	lcdWriteReg(VADRPOS_RAM_START, rect->t);
	lcdWriteReg(VADRPOS_RAM_END, rect->b);
	
	lcdWriteIndex(DATA_RAM);
	
	for (int i = 0; i < (rect->r - rect->l) * (rect->b - rect->t); i++)
	{	
		lcdWriteData(rect->c);
	}
	
	lcdWriteReg(HADRPOS_RAM_START, 0);
	lcdWriteReg(HADRPOS_RAM_END, LCD_MAX_X);
	lcdWriteReg(VADRPOS_RAM_START, 0);
	lcdWriteReg(VADRPOS_RAM_END, LCD_MAX_Y);
}

void drawLineLegacy(int xs, int ys, int xe, int ye, uint16_t color)
{
	if (xe < xs)
		swapInt(xs, xe);
	if (ye < ys)
		swapInt(ys, ye);
	
	float dy = ye - ys;
	float dx = xe - xs;
	if (dx == 0)
		dx = 1;
	if (dy == 0)
		dy = 1;
	
	float a = dy * 1.0 / dx;
	float y = 0;	
	for(int x = 0; x < dx; x++) 
	{
		lcdWriteReg(ADRY_RAM, x + xs);
		for(int i = 0; i < a; i++) 
		{
			lcdWriteReg(ADRX_RAM, ys + y + i);
			lcdWriteIndex(DATA_RAM);
			lcdWriteData(color);
		}
		y += a;
	}
}

void drawLine(int xs, int ys, int xe, int ye, uint16_t color)
 {
    int d, dx, dy, ai, bi, xi, yi;
    int x = xs, y = ys;
    if (xs < xe)
    {
        xi = 1;
        dx = xe - xs;
    }
    else
    {
        xi = -1;
        dx = xs - xe;
    }
    if (ys < ye)
    {
        yi = 1;
        dy = ye - ys;
    }
    else
    {
        yi = -1;
        dy = ys - ye;
    }
    drawPixel(x, y, color);
    if (dx > dy)
    {
        ai = (dy - dx) * 2;
        bi = dy * 2;
        d = bi - dx;
        while (x != xe)
        {
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                x += xi;
            }
            drawPixel(x, y, color);
        }
    }
    else
    {
        ai = ( dx - dy ) * 2;
        bi = dx * 2;
        d = bi - dy;
        while (y != ye)
        {
            if (d >= 0)
            {
                x += xi;
                y += yi;
                d += ai;
            }
            else
            {
                d += bi;
                y += yi;
            }
            drawPixel(x, y, color);
        }
    }
}

void drawLineStrip(int n, Coord coords[n], uint16_t color)
{
	for (int i = 0; i < n - 1; i++)
	{
		drawLine(coords[i].x, coords[i].y, coords[i + 1].x, coords[i + 1].y, color);
	}
}	

void drawText(int xPos, int yPos, const char* buff, bool drawBackGround)
{
	uint8_t letter[16] = { 0 };
	int ys = yPos + 0;
		
	for (const char* c = buff; *c != '\0'; c++)
	{
		GetASCIICode(0, letter, *c);
		int xs = xPos + (int)(c - buff) * 8;
		
		for(int8_t b = 0; b < 8; b++)
		{
			lcdWriteReg(ADRY_RAM, xs + 8 - b);
			for (int i = sizeof(letter) - 1; i >= 0; i--)
			{
				lcdWriteReg(ADRX_RAM, ys - i);
				lcdWriteIndex(DATA_RAM);
				if (drawBackGround)
					lcdWriteData((letter[i] >> b) & 1 ? LCDBlack : LCDWhite);
				else
					if ((letter[i] >> b) & 1)
						lcdWriteData(LCDWhite);
			}
		}
	}
}

void displayTexture(const void* data)
{
	uint32_t startTime = sysTicks;
	
	lcdWriteReg(ADRY_RAM, 0);
	lcdWriteReg(ADRX_RAM, 0);
	lcdWriteIndex(DATA_RAM);
	for (int y = 0; y < LCD_MAX_Y; y++)
	{
		for (int x = 0; x < LCD_MAX_X; x++)
		{
			//lcdWriteReg(DATA_RAM, LCDYellow);
			lcdWriteData(LCDYellow);
		}
	}
	
	uint32_t endTime = sysTicks;
	
	UART_Send("time: ");
	UART_SendNum(endTime - startTime);
	UART_Send("\r\n");
}

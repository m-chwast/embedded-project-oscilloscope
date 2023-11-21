#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"

#include "asciiLib.h"
#include "lcd.h"
#include "uart.h"
#include "math.h"
#include <stdio.h>

extern volatile uint32_t sysTicks;

void delay(int s);


void initLCD()
{
	lcdConfiguration();
	uint16_t reg = lcdReadReg(OSCIL_ON);
	
	char buff[32] = { 0 };
	sprintf(buff, "0x%x\r\n", reg);
	
	UART_Send(buff);
	
	init_ILI9325();
	
	displayTextire(NULL);
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

void drawLine(int xs, int ys, int xe, int ye, uint16_t color)
{
	if (xe < xs)
	{
		int temp = xs;
		xs = xe;
		xe = temp;
	}
	if (ye < ys)
	{
		int temp = ys;
		ys = ye;
		ye = temp;
	}
	
	//TODO vertical, horizontal lines
	
	float a = (ye - ys) * 1.0 / (xe - xs);
	
	float y = 0;
	
	for(int x = 0; x < xe - xs; x++) 
	{
		lcdWriteReg(ADRX_RAM, x + xs);
		
		for(int i = 0; i < a; i++) 
		{
			lcdWriteReg(ADRY_RAM, ys + y + i);
			lcdWriteIndex(DATA_RAM);
			lcdWriteData(color);
		}
		
		y += a;
	}
}

void drawText(int xPos, int yPos, const char* buff)
{
	uint8_t letter[16] = { 0 };
	//lcdWriteReg(ADRY_RAM, 0);
	//lcdWriteReg(ADRX_RAM, 0);
	
	int ys = yPos + 0;
	int ye = ys  + 14;
	//lcdWriteReg(VADRPOS_RAM_START, ys);
	//lcdWriteReg(VADRPOS_RAM_END, ye);
		
	for (const char* c = buff; *c != '\0'; c++)
	{
		/*char s[4] = {0};
		s[1] = '\r';
		s[2] = '\n';
		s[0] = *c;
		UART_Send(s);*/
		GetASCIICode(0, letter, *c);
		int xs = xPos + (int)(c - buff) * 8;
		int xe = xs + 7;
		
		int line = (xe + 1) / LCD_MAX_X;
		static int oldLine;
		
		/*if (line > oldLine)
		{
			oldLine = line;	
			ys = ys + 16;
			ye = ys + 14;			
			//lcdWriteReg(VADRPOS_RAM_START, ys);
			//lcdWriteReg(VADRPOS_RAM_END, ye);
		}*/
		
		//xs = line + xs % LCD_MAX_X;
		//xe = xs + 7;
		
		//lcdWriteReg(HADRPOS_RAM_START, xs);
		//lcdWriteReg(HADRPOS_RAM_END, xe);
		//lcdWriteIndex(DATA_RAM);
		for (int i = 0; i < sizeof(letter); i++)
		{
			lcdWriteReg(ADRX_RAM, xs);
			lcdWriteReg(ADRY_RAM, ys + i);
			lcdWriteIndex(DATA_RAM);
			for(int8_t b = 7; b >= 0; b--)
				lcdWriteData((letter[i] >> b) & 1 ? LCDBlack : LCDWhite);
		}
	}
	//lcdWriteReg(HADRPOS_RAM_START, 0);
	//lcdWriteReg(HADRPOS_RAM_END, LCD_MAX_X);
	//lcdWriteReg(VADRPOS_RAM_START, 0);
	//lcdWriteReg(VADRPOS_RAM_END, LCD_MAX_Y);
}

void drawTextNoBg(int xPos, int yPos, const char* buff)
{
	uint8_t letter[16] = { 0 };
	//lcdWriteReg(ADRY_RAM, 0);
	//lcdWriteReg(ADRX_RAM, 0);
	
	int ys = yPos + 0;
		
	for (const char* c = buff; *c != '\0'; c++)
	{
		GetASCIICode(0, letter, *c);
		int xs = xPos + (int)(c - buff) * 8;
		int xe = xs + 7;
		
		int line = (xe + 1) / LCD_MAX_X;
		static int oldLine;
		
		for (int i = 0; i < sizeof(letter); i++)
		{
			lcdWriteReg(ADRY_RAM, ys + i);
			
			for(int8_t b = 7; b >= 0; b--) {
				lcdWriteReg(ADRX_RAM, xs + (7 - b));
				lcdWriteIndex(DATA_RAM);
				if((letter[i] >> b) & 1)
					lcdWriteData(LCDBlack);
			}
		}
	}
}


void displayTextire(const void* data)
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

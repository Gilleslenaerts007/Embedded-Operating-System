
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "WS2812.h"
#include "xparameters.h"
#include "xil_io.h"
#include "sleep.h"
#include "math.h"
#include <stdint.h>
//#include "typedef.h"
//#include "xgpio.h"

#define WS2812ADR XPAR_WS2812_0_S00_AXI_BASEADDR

u16 result;

struct uint24_t { unsigned long v:24; };
typedef struct uint24_t __attribute__((packed)) uint24_t;

struct pixelColour
{
	uint8_t green, red, blue;
};

int GameArray[64];
struct pixelColour colourArray[8][8];

int color = 0x0f0000;
int arraypos=0;

void drawPixel(int X, int Y);
void drawLine(int x1, int y1, int x2, int y2);
void drawGame();

int main()
{
    init_platform();
    int readreg, Status;
    int itel;
    printf("Starting Pong.\n\r");

	for(;;){

		//WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xFF0000);
		//WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG1_OFFSET, 0x00FF00);
		//WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG2_OFFSET, 0x0000FF);
		drawLine(0, 0, 7, 7);
		drawGame();
		break;
	}

    cleanup_platform();
    return 0;
}

void drawPixel(int X, int Y){
	colourArray[Y][X].red = 1;
}
void drawGame()
{
	/*
	int rgbByte=0;
	for (int i=0; i<=252; i=i+4)
	{
		WS2812_mWriteReg(WS2812ADR,i,GameArray[rgbByte]);
		//printf("%d \n\r", i);
		//sleep(3);
		if (rgbByte < 63){
		rgbByte++;
		}
		else break;
		//printf("Stuck in loop");


	}
	 */
	for(int y = 0; y <= 8; ++y)
	{
		for(int x = 0; x <= 8; ++x)
		{
			int data = (colourArray[y][x].green << 16) + (colourArray[y][x].red << 8) + colourArray[y][x].blue;
			WS2812_mWriteReg(WS2812ADR,((x*4)+(y*32)), data);
		}
	}
}

void drawLine (int x1, int y1, int x2, int y2) {      //draw a line from x1,y1 to x2,y2
int dx, dy, sx, sy, err, e2;

dx = abs (x2-x1);
dy = abs (y2-y1);
if (x1<x2) sx = 1;
   else sx = -1;
if (y1<y2) sy = 1;
   else sy = -1;
err = dx-dy;
do {
   drawPixel (x1, y1);
   if ((x1 == x2) && (y1 == y2))
      break;
   e2 = 2*err;
   if (e2 > -dy) {
      err = err - dy;
		 x1 = x1+sx;
   }
   if (e2 < dx) {
      err = err + dx;
		 y1 = y1 + sy;
   }
} while (1);
	return;
}

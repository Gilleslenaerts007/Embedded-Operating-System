#include "PongHead.h"

void startPositions()
{
	//srand((unsigned) time(0));
    LokatieBalkL = 0;
    LokatieBalkR = 7;
    BalkRechts= 3;
    BalkLinks = 3;
    BallX=4;//rand(4)+2;		//X staat van links boven 0 naar Rechts 7
	BallY=4;//rand(7); 			//Y Staat van boven 0 naar beneden 7
	BallMoveX = 1;
	BallMoveY = 0;
}

void updateGame()
{
	hitDetect();
	BallX = BallX + BallMoveX;
    BallY = BallY + BallMoveY;
    drawGame();
    clearArray();
}

void getPlayer1Move()
{
	hitDetect();
	BallX = BallX + BallMoveX;
    BallY = BallY + BallMoveY;
    drawGame();
    clearArray();
}

void getPlayer2Move()
{
	hitDetect();
	BallX = BallX + BallMoveX;
    BallY = BallY + BallMoveY;
    drawGame();
    clearArray();
}

void drawGame()
{
	//color=GROEN
    selectColour = 'g';
	drawLine(LokatieBalkL, BalkLinks, LokatieBalkL, BalkLinks+2);
	//color=BLUE
    selectColour = 'b';
	drawLine(LokatieBalkR, BalkRechts, LokatieBalkR, BalkRechts+2);
	//Color=RED
    selectColour = 'r';
	drawPixel(BallX,BallY);
	
	for(int y = 0; y <= 7; ++y)
	{
		for(int x = 0; x <= 7; ++x)
		{
			int data = (colourArray[y][x].green << 16) + (colourArray[y][x].red << 8) + colourArray[y][x].blue;
			WS2812_mWriteReg(WS2812ADR,((x*4)+(y*32)), data);
		}
	}
}

void clearArray()		//Changed variable loop to 7, 0to7 = 8, met 8 clear je andere variables..
{
	for(int y = 0; y <= 7; ++y)
	{
		for(int x = 0; x <= 7; ++x)
		{
			colourArray[y][x].blue = 0x00;
			colourArray[y][x].green = 0x00;
			colourArray[y][x].red = 0x00;
		}
	}
}

//Add more colisions, like ball on edge contact from below to up
void hitDetect()
{
  if(	BallMoveX == 1 && BallX == ((LokatieBalkR-1))  )
	{
		if (BallY == BalkRechts)
		{
		BallMoveX = BallMoveX * -1;
		BallMoveY = -1;
            BalkHit++;
		}
		else if (BallY == BalkRechts+1)
		{
		BallMoveX = BallMoveX * -1;
		BallMoveY = 0;
            BalkHit++;
		}

		else if (BallY == BalkRechts+2)
		{
		BallMoveX = BallMoveX * -1;
		BallMoveY = 1;
             BalkHit++;
		}


	}
	else if ( BallMoveX == -1 && BallX == (LokatieBalkL+1))
	{
		if (BallY == BalkLinks)
		{
		BallMoveX = BallMoveX * -1;
		BallMoveY = -1;
             BalkHit++;
		}
		else if (BallY == BalkLinks+1)
		{
		BallMoveX = BallMoveX * -1;
		BallMoveY = 0;
             BalkHit++;
		}
		else if (BallY == BalkLinks+2)
		{
		BallMoveX = BallMoveX * -1;
		BallMoveY = 1;
             BalkHit++;}
	}

    if (BallY == 7 || BallY == 0)
    {
        BallMoveY = BallMoveY * -1;
    }

}

void drawPixel(int X, int Y)
{
	switch (selectColour)
	{
		case 'r' : colourArray[Y][X].red = COLOUR_INTENSITY;
				   break;
		case 'g' : colourArray[Y][X].green = COLOUR_INTENSITY;
				   break;
		case 'b' : colourArray[Y][X].blue = COLOUR_INTENSITY;
				   break;
		default  : colourArray[Y][X].red = COLOUR_INTENSITY;
			       colourArray[Y][X].green = COLOUR_INTENSITY;
				   colourArray[Y][X].blue = COLOUR_INTENSITY;
				   break;
	}
}

void drawLine (int x1, int y1, int x2, int y2) 
{      
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


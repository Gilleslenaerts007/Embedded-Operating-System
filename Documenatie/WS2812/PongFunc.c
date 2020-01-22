#include "PongHead.h"

void startPositions(gameData* game)
{
	//srand((unsigned) time(0));
    game->XBalkLinks = 0;
    game->XBalkRechts = 7;
    game->YBalkRechts= 3;
    game->YBalkLinks = 3;
    game->BallX=4;//rand(4)+2;		//X staat van links boven 0 naar Rechts 7
	game->BallY=4;//rand(7); 			//Y Staat van boven 0 naar beneden 7
	game->BallMoveX = 1;
	game->BallMoveY = 0;
	game->COLOUR_INTENSITY = 50;
}

void updateGame(gameData* game)
{
	hitDetect(game);
	game->BallX = game->BallX + game->BallMoveX;
    game->BallY = game->BallY + game->BallMoveY;
    drawGame(game);
    clearArray(game);

    //Check for score
	if (game->scoreFlag)
	{
		game->scoreFlag=0;
		printf("------------ SCORE -----------\n\rPlayer1: %d\n\rPlayer2: %d\n\r", game->scorePlayer1, game->scorePlayer2);
		printf("Resetting positions");
		clearArray(game);
		startPositions(game);
		drawGame(game);

		if ( (game->scorePlayer1>=10) || (game->scorePlayer2>=10) )
		{
			//Wait for ps input next start?
			game->scorePlayer1= 0;
			game->scorePlayer2= 0;
			sleep(5);
			//while( (GPIOSPS_ReadPin) = 0 )
		}

		sleep(1);
		clearArray(game);
	}
}

void getPlayer1Move(u32* Data, gameData* game)
{
	static int calculatedDistance = 0;
	int distance =  *Data;

	switch(distance)
	{
	case 0 ... 15:
		calculatedDistance = 5;
		break;
	case 16:
		calculatedDistance = 4;
		break;
	case 17:
		calculatedDistance = 3;
		break;
	case 18:
		calculatedDistance = 2;
		break;
	case 19:
		calculatedDistance = 1;
		break;
	case 20:
		calculatedDistance = 0;
		break;
	default:
		calculatedDistance = -1;
		break;
	}
	if(game->YBalkLinks < calculatedDistance)
	{
		game->YBalkLinks ++;
	}
	else if(game->YBalkLinks > calculatedDistance)
	{
		game->YBalkLinks --;
	}
	game->YBalkLinks = calculatedDistance;

}

void getPlayer2Move(char* Button, gameData* game)
{
	char inputbutton = *Button;

		if (!(inputbutton & 0b01) && game->YBalkRechts < 6) // DENNIS BUTTONS
		{
			game->YBalkRechts++;
		}
		else if (!(inputbutton & 0b10) && game->YBalkRechts > -1) // DENNIS BUTTONS
		{
			game->YBalkRechts--;
		}
}

void drawGame(gameData* game)
{
	//color=GROEN
    game->selectColour = 'g';
	drawLine(game->XBalkLinks, game->YBalkLinks, game->XBalkLinks, game->YBalkLinks+2, game);
	//color=BLUE
    game->selectColour = 'b';
	drawLine(game->XBalkRechts, game->YBalkRechts, game->XBalkRechts, game->YBalkRechts+2, game);
	//Color=RED
    game->selectColour = 'r';
	drawPixel(game->BallX,game->BallY, game);
	
	for(int y = 0; y <= 7; ++y)
	{
		for(int x = 0; x <= 7; ++x)
		{
			int data = (game->colourArray[y][x].green << 16) + (game->colourArray[y][x].red << 8) + game->colourArray[y][x].blue;
			WS2812_mWriteReg(WS2812ADR,((x*4)+(y*32)), data);
		}
	}
}

void clearArray(gameData* game)		//Changed variable loop to 7, 0to7 = 8, met 8 clear je andere variables..
{
	for(int y = 0; y <= 7; ++y)
	{
		for(int x = 0; x <= 7; ++x)
		{
			game->colourArray[y][x].blue = 0x00;
			game->colourArray[y][x].green = 0x00;
			game->colourArray[y][x].red = 0x00;
		}
	}
}

//Add more colisions, like ball on edge contact from below to up
void hitDetect(gameData* game)
{
	//actually detecting algorithm

	if(game->BallX == 1 || game->BallX == 6) //detecting collision with pads game->YBalkRechts game->YBalkLinks game->BallY
	{
		bool above 	= FALSE;
		bool center = FALSE;
		bool below 	= FALSE;
		//---detecting collision with pads
		for(int8_t count = -1; count <= 1; ++count)
		{
			if ((((game->YBalkRechts == game->BallY + count)	|| (game->YBalkRechts + 1 == game->BallY + count)	|| (game->YBalkRechts + 2 == game->BallY + count)) && game->BallX == 6)|| //center
				(((game->YBalkLinks  == game->BallY + count)	|| (game->YBalkLinks  + 1 == game->BallY + count)	|| (game->YBalkLinks  + 2 == game->BallY + count)) && game->BallX == 1))
			{
				if(count == -1)
					above = TRUE;
				if(count == 0)
					center = TRUE;
				if(count == 1)
					below = TRUE;
			}
		}
		//--------------------------------

		//--check which direction to go, dependent of y direction of ball and location hit on pad)
		if(above == FALSE && center == FALSE && below == FALSE)
		{
			//do nothing
		}
		if(above == FALSE && center == FALSE && below == TRUE) 		//hit top of pad (going diagonal) (1)
		{
			switch(game->BallMoveY)
			{
			case 1:
				//game->BallMoveY = 1;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			case 0:
				break;
			case -1:
				break;
			default:
				break;
			}
		}
		else if (above == FALSE && center == TRUE && below == TRUE)	//hit top of pad(2)
		{
			switch(game->BallMoveY)
			{
			case 1:
				//game->BallMoveY = 1;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			case 0:
				game->BallMoveY = -1;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			case -1:
				break;
			default:
				break;
			}
		}
		else if (above == TRUE && center == TRUE && below == TRUE)	//hit center(3)
		{
			switch(game->BallMoveY)
			{
			case 1:
				game->BallMoveY = 0;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			case 0:
				game->BallMoveX = game->BallMoveX * -1;
				break;
			case -1:
				game->BallMoveY = 0;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			default:
				xil_printf("Hit center error\r\n");
				break;
			}
		}
		else if (above == TRUE && center == TRUE && below == FALSE)	//hit bottom of pad(4)
		{
			switch(game->BallMoveY)
			{
			case 1:
				break;
			case 0:
				game->BallMoveY = 1;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			case -1:
				//game->BallMoveY = -1;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			default:
				break;
			}
		}
		else if (above == TRUE && center == FALSE && below == FALSE)//hit bottom of pad (going diagonal)(5)
		{
			switch(game->BallMoveY)
			{
			case 1:
				break;
			case 0:
				break;
			case -1:
				//game->BallMoveY = -1;
				game->BallMoveX = game->BallMoveX * -1;
				break;
			default:
				break;
			}
		}
	}
	//--------------------------
	if(game->BallY == 0 || game->BallY == 7)    //check if hit top or bottom
		game->BallMoveY = game->BallMoveY * -1;

	if(game->BallX == 7)	//check if p1 has scored
	{
		game->scorePlayer1 ++;
		xil_printf("P1 scored, score :%d\r\n", game->scorePlayer1);
		startPositions(game);
	}
	if(game->BallX == 0)	//check if p2 has scored
	{
		game->scorePlayer2 ++;
		xil_printf("P2 scored, score: %d\r\n", game->scorePlayer2);
		startPositions(game);
	}

	if(game->scorePlayer1 >= 5 || game->scorePlayer2 >= 5)
	{
		xil_printf(	"-----SCOREBOARD-----\r\n"
					"player 1: %d\r\n"
					"player 2: %d\r\n"
					,game->scorePlayer1, game->scorePlayer2);
		game->scorePlayer1 = 0;
		game->scorePlayer2 = 0;
		sleep(2);
	}
}

void drawPixel(int X, int Y, gameData* game)
{
	if(X > 7)
		X = 7;
	if(X < 0)
		X = 0;
	if(Y > 7)
		Y = 7;
	if(Y < 0)
		Y = 0;

	switch (game->selectColour)
	{
		case 'r' : game->colourArray[Y][X].red = game->COLOUR_INTENSITY;
				   break;
		case 'g' : game->colourArray[Y][X].green = game->COLOUR_INTENSITY;
				   break;
		case 'b' : game->colourArray[Y][X].blue = game->COLOUR_INTENSITY;
				   break;
		default  : game->colourArray[Y][X].red = game->COLOUR_INTENSITY;
			       game->colourArray[Y][X].green = game->COLOUR_INTENSITY;
				   game->colourArray[Y][X].blue = game->COLOUR_INTENSITY;
				   break;
	}
}

void drawLine (int x1, int y1, int x2, int y2, gameData* game)
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
	   drawPixel (x1, y1, game);
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

void startGPIO()
{
	int Status;

	//Init GPIOPL
	Status = XGpio_Initialize(&Gpio, GPIO_BUTTONS);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
	//Set data as input
	XGpio_SetDataDirection(&Gpio, BUTTONS_CHANNEL, BUTTONS);

	//Init PS
	XGpioPs_Config *GPIOPSConfigPtr;
	GPIOPSConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&GpioPS, GPIOPSConfigPtr,GPIOPSConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		printf("status error \n\r");
		return XST_FAILURE;
	}
    printf("Starting GPIO PS\n\r");
    Input_Pin = 0;
	XGpioPs_SetDirectionPin(&GpioPS,Input_Pin,0);


}


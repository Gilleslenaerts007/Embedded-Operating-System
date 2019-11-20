#ifndef _PongHead_H_
#define _PongHead_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "time.h"

/************************** Minized Includes *****************************/
#include "WS2812.h"
#include "xparameters.h"
#include "xil_io.h"

/************************** IP Block Definitions *****************************/
#define WS2812ADR XPAR_WS2812_0_S00_AXI_BASEADDR

/************************** Standard Game Definitions *****************************/
#define PLAYSPEEDFAST 50000 // SUPA SPEED
#define PLAYSPEEDNORMAL 120000 // SUPA NORMAL
#define true 1
#define false 0

/************************** Pong Variable Definitions *****************************/
int Speed;
int BalkLinks;
int BalkRechts ;
int BallX;
int BallY; 
int BallMoveX;
int BallMoveY;
int LokatieBalkL; 
int LokatieBalkR;
int BalkHit;

struct pixelColour
{
	uint8_t green, red, blue;
};
struct pixelColour colourArray[8][8];
char selectColour;
int COLOUR_INTENSITY;

/************************** Function Prototypes ******************************/

/****************************************************************************/
/**
* @brief	Initialize standard positions in 8x8 WS2812 NeoPixel Matrix.
*			Pong pallet width of 3, Ball position in random area in the middle.
*
* @note		Need to add random position between X3 - X6, Y1 - Y7. Maybe move
* 			ballX and ballY random too?
*
****************************************************************************/
void startPositions();

/****************************************************************************/
/**
* @brief	Updates the variables adjusted through the hitdetect function.
*			Calls mulitple functions to refresh the screen, works as tickrate.
*
* @note		
*
****************************************************************************/
void updateGame();

/****************************************************************************/
/**
* @brief	Reads player 1 inputs from HCRS04 Sound sensor.
* 			Max distance read is 30cm.
*
* @note
*
****************************************************************************/
void getPlayer1Move();

/****************************************************************************/
/**
* @brief	Read player 2 input from rotary encoder.
*
* @note
*
****************************************************************************/
void getPlayer2Move();

/****************************************************************************/
/**
* @brief	Checks collision with pong pallets and changes ball direction.
*			
* @note		Need to add more collision on pallet. op de hoek moet hij 
*			de richting inverteren en niet doodgaan. The more the better..
*
****************************************************************************/
void hitDetect();

/****************************************************************************/
/**
* @brief	Fills the databuffer with Drawline for pong pallets and 
*			DrawPixel for ball position.
*
*			Sorts the data send to WS2812 register in GRB format.
* @note		
*
****************************************************************************/
void drawGame();

/****************************************************************************/
/**
* @brief	Clears databuffer for next drawGame.
*			
* @note		
*
****************************************************************************/
void clearArray();

/****************************************************************************/
/**
* @brief	Based on the color selection in the game, the colour array gets
*			filled with the chosen colour intensity.
*			
* @note	
*
****************************************************************************/
void drawPixel(int X, int Y);

/****************************************************************************/
/**
* @brief	Draw a line from x1,y1 to x2,y2 with the drawPixel function.
*			
* @note		
*
****************************************************************************/
void drawLine(int x1, int y1, int x2, int y2);

#endif

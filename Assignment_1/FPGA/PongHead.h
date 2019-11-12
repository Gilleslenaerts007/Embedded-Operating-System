#ifndef _PongHead_H_
#define _PongHead_H_

#include "project.h"
#include "font.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "math.h"

#define true 1
#define false 0

#define bigX 10                  
#define bigY 5
#define NumberX bigX*bigY
#define tickspeed 10

int color = 0xff0000 ;
int BalkLinks = 2;
int BalkRechts = 2;
int BallX = 4;//1;
int BallY = 5; //4;
int BallMovex=1;
int BallMovey=0;
int Speed = tickspeed;
int LokatieBalkL=1; 
int LokatieBalkR=8;


void StartGame();
void ticker();
void hitdetect();
void drawgame();
void drawPixel(int X, int Y);
void drawline(int x1, int y1, int x2, int y2);
void startgame();

#endif
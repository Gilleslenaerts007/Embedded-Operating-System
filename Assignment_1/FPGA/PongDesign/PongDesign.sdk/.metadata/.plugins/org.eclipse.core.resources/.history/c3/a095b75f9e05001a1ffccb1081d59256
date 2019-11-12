#include PongHead.h

void StartGame()
{
    color = 4;
    //drawLine( 1, 6, 1, 11);
    drawLine( 1, 10, 1, 15); //Stel hier de breedte balk in
    color = 6;
    //drawLine( 80, 6, 80, 11);
    drawLine((bigX*8), 10, (bigX*8), 15);
    DrawScoreBoard();
    CyDelay(10);
    clearBORD(); 
}

void ticker()
{
    
    hitdetect();
    BallX = BallX + BallMovex;
    BallY = BallY + BallMovey;
  
}

void hitdetect()
{
  if(	BallMovex == 1 && BallX == ((LokatieBalkR-1))  )
	{
		if (BallY == BalkRechts || BallY == BalkRechts+1)
		{
		BallMovex = BallMovex * -1;
		BallMovey = -1;
            BalkHit++;
		}
		else if (BallY == BalkRechts+2 || BallY == BalkRechts+3)
		{
		BallMovex = BallMovex * -1;
		BallMovey = 0;
            BalkHit++;
		}
		else if (BallY == BalkRechts+4 || BallY == BalkRechts+5)
		{
		BallMovex = BallMovex * -1;
		BallMovey = 1;
             BalkHit++;
		}

	}
	else if ( BallMovex == -1 && BallX == (LokatieBalkL+1))
	{
		if (BallY == BalkLinks || BallY == BalkLinks+1)
		{
		BallMovex = BallMovex * -1;
		BallMovey = -1;
             BalkHit++;
		}
		else if (BallY == BalkLinks+2 || BallY == BalkLinks+3)
		{
		BallMovex = BallMovex * -1;
		BallMovey = 0;
             BalkHit++;
		}
		else if (BallY == BalkLinks+4 || BallY == BalkLinks+5)
		{
		BallMovex = BallMovex * -1;
		BallMovey = 1;
             BalkHit++;}
	}
    
    if (BallY == ((bigY-2)*8) || BallY == 1)
    {
        BallMovey = BallMovey * -1;
    }

    /*-------Score updates---------*/  
 else if (BallX <= LokatieBalkL)
    {

        if(ScoreP2[1]<'9'){
            player2score++;
             sprintf(&ScoreP2[1], "%d", player2score); //Zet integer rechts om in een string en plaats in array links
        }
        else{
            player2score = 1;
            sprintf(&ScoreP2[0], "%d", player2score);
            ScoreP1[1] = '0';
        }
        readController(1,1);
        LokatieBalkL = 1;
        LokatieBalkR = 80;
        BalkHit = 0;
        BallX = rand()%40+25;
        BallY = rand()%24;
        BallMovex = BallMovex*-1;
        ronde++;
        CyDelay(500);
        
        DrawScoreBoard();
        //DrawGame(BallX, BallY);
  
    }
    
  else if (BallX >=LokatieBalkR)
    {     
        if(ScoreP1[1]<'9'){
          player1score++;
          sprintf(&ScoreP1[1], "%d", player1score); //Zet integer rechts om in een string en plaats in array links
        }
        else{
            player1score = 1;
            sprintf(&ScoreP1[0], "%d", player1score);
            ScoreP1[1] = '0';
        }
        readController(2,1);
        LokatieBalkL = 1;
        LokatieBalkR = 80;
        BalkHit = 0;
        BallX = rand()%40+21;
        BallY = rand()%24;
        BallMovex = BallMovex*-1;
        ronde++;
        CyDelay(500);
        
        DrawScoreBoard();
        //DrawGame(BallX, BallY);
    }

}

void drawPixel(int X, int Y) {
    
   if (Y<=bigY*8 && X<=bigX*8 && X>0 && Y>0) {
      if (Y>8) X=X+(bigX*8)*((Y-1)/8);
      Y=Y%8;
      if (Y==0) Y=8;
      
      int p;
      p=NumberX-((X-1)/8)-1;

      dataArray[3*(Y-1)+24*p]&=~(1<<((X-1)%8));                  //turn off chosen drawPixel
      dataArray[3*(Y-1)+1+24*p]&=~(1<<((X-1)%8));
      dataArray[3*(Y-1)+2+24*p]&=~(1<<((X-1)%8));

      if (color&1) dataArray[3*(Y-1)+24*p]|=(1<<((X-1)%8));      //set color to the drawPixel
      if (color&2) dataArray[3*(Y-1)+1+24*p]|=(1<<((X-1)%8));
      if (color&4) dataArray[3*(Y-1)+2+24*p]|=(1<<((X-1)%8));
   }  
 }




void DrawGame(int BallX, int BallY)
{    
    if((BalkHit==1) && (BallX==40) && LokatieBalkL<32){
        LokatieBalkL = LokatieBalkL +8;
        LokatieBalkR = LokatieBalkR -8;
        BalkHit =0;
    }
    
    color = 4;
    drawLine( LokatieBalkL, BalkLinks, LokatieBalkL, BalkLinks+5);
    color = 5;
    drawLine( LokatieBalkR, BalkRechts, LokatieBalkR, BalkRechts+5);
    color = 2;
    drawPixel(BallX, BallY);
    CyDelay(10);
    display();
    clearBORD();
    
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
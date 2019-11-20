
#include "platform.h"
#include "xil_printf.h"
#include "PongHead.h"
#include "sleep.h"
//#include "typedef.h"
//#include "xgpio.h"


int main()
{
    init_platform();
    printf("Starting Pong.\n\r");
    startPositions();
	for(;;)
	{	/*
		updateGame();
		getPlayer1Move();
		usleep(PLAYSPEEDNORMAL);
		*/
		for(int i =0;i<60;i++){
			updateGame();
			usleep(PLAYSPEEDNORMAL);
		}
	}

    cleanup_platform();
    return 0;
}

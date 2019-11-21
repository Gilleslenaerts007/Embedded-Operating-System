
#include "platform.h"
#include "xil_printf.h"
#include "PongHead.h"
//#include "typedef.h"
//#include "xgpio.h"


int main()
{
    init_platform();
    startGPIO();
    printf("Starting Pong.\n\r");
    startPositions();
    COLOUR_INTENSITY = 10;

	for(;;)
	{
		updateGame();
		getPlayer1Move();
		getPlayer2Move();
		usleep(PLAYSPEEDNORMAL);
	}

    cleanup_platform();
    return 0;
}

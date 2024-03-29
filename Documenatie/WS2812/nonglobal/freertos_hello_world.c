/*
    Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
    Copyright (C) 2012 - 2018 Xilinx, Inc. All Rights Reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software. If you wish to use our Amazon
    FreeRTOS name, please do so in a fair use way that does not cause confusion.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    http://www.FreeRTOS.org
    http://aws.amazon.com/freertos


    1 tab == 4 spaces!
*/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
/*user includes */
#include "PongHead.h"

#define TIMER_ID	1
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define DELAY_500_MILISECOND		500UL
#define TIMER_CHECK_THRESHOLD	9
#define COLOURARRAYWIDTH 8
#define COLOURARRAYHEIGHT 8

/*-----------------------------------------------------------*/

/* The Tx and Rx tasks as described at the top of this file. */
static void prvTxTask( void *pvParameters );
static void prvRxTask( void *pvParameters );
static void vTimerCallback( TimerHandle_t pxTimer );
/*-----------------------------------------------------------*/

/* The queue used by the Tx and Rx tasks, as described at the top of this
file. */
static TaskHandle_t xgetPlayer1Input;
static TaskHandle_t xgetPlayer2Input;
static TaskHandle_t xgamePongTask;
static TaskHandle_t xprintDebugInfo;

static QueueHandle_t xQueuePlayer1 = NULL;
static QueueHandle_t xQueuePlayer2 = NULL;
static QueueHandle_t xQueueDebugInfo = NULL;

static TimerHandle_t xTimerTickRate = NULL;

static void vTimerCallback( TimerHandle_t pxTimer )
{
	long lTimerId;
	configASSERT( pxTimer );

	lTimerId = ( long ) pvTimerGetTimerID( pxTimer );
	tickFlag = 1;
	xTimerStart( xTimerTickRate, 0 );
}

int main( void )
{
	startGPIO();
	startPositions();
	updateGame();

	xil_printf( "Starting NeoPixel 8x8 Matrix: Pong Game by Gilles, Dennis and Jonas.\r\n" );

	xTaskCreate( 	xgetPlayer1Input, 					/* The function that implements the task. */
					( const char * ) "Player1Input", 		/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
					NULL, 						/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
					&xgetPlayer1Input );

	xTaskCreate( 	xgetPlayer2Input, 					/* The function that implements the task. */
					( const char * ) "Player2Input", 		/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
					NULL, 						/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
					&xgetPlayer2Input );

	xTaskCreate( 	xgamePongTask,
				    ( const char * ) "GamePongLogic",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + 1,
					&xgamePongTask );

	xTaskCreate(    xprintDebugInfo,
				    ( const char * ) "printDebugInfo",
					configMINIMAL_STACK_SIZE,
					NULL,
					tskIDLE_PRIORITY + 1,
					&xprintDebugInfo );

	xQueuePlayer1 = xQueueCreate(2, sizeof( u32) );
	xQueuePlayer2 = xQueueCreate(2, sizeof( u32 ) );
	xQueueDebugInfo = xQueueCreate(2, (COLOURARRAYHEIGHT*COLOURARRAYWIDTH)*3 ); //3 colour bytes * width and height

	/* Check the queue was created. */
	configASSERT( xQueuePlayer1 );
	configASSERT( xQueuePlayer2 );
	configASSERT( xQueueDebugInfo );

	const TickType_t x5miliseconds = pdMS_TO_TICKS( DELAY_500_MILISECOND );

	xTimerTickRate = xTimerCreate( (const char *) "tickRateTimer",
							x5miliseconds,
							pdFALSE,
							(void *) TIMER_ID,
							vTimerCallback);

	/* Check the timer was created. */
	configASSERT( xTimerTickRate );

	xTimerStart( xTimerTickRate, 0 );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for( ;; );
}

/*-----------------------------------------------------------*/
static void printDebugInfo (void *pvParameters)
{
	pixelColour debugArray[COLOURARRAYWIDTH][COLOURARRAYHEIGHT];

	for(;;)
	{
		xQueueReceive( 	xQueueDebugInfo,				/* The queue being read. */
						&debugArray,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

		for(int x = 0; x < COLOURARRAYHEIGHT; ++x)
		{
			for(int y = 0; y < COLOURARRAYWIDTH; ++y)
			{
				xil_printf("%d,%d,%d;", debugArray[COLOURARRAYHEIGHT][COLOURARRAYWIDTH].blue,
										debugArray[COLOURARRAYHEIGHT][COLOURARRAYWIDTH].green,
										debugArray[COLOURARRAYHEIGHT][COLOURARRAYWIDTH].red);
			}
		}
		xil_printf("\r\n");
	}

}
/*-----------------------------------------------------------*/
static void getPlayer1Input( void *pvParameters )
{
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );
u32 HCRS04Data;
	for( ;; )
	{
		HCRS04Data = HCSR04_SENSOR_mReadReg(XPAR_HCSR04_SENSOR_0_S00_AXI_BASEADDR,HCSR04_SENSOR_S00_AXI_SLV_REG3_OFFSET);

			xQueueSend( xQueuePlayer1,			/* The queue being written to. */
					&HCRS04Data, /* The address of the data being sent. */
					0UL );			/* The block time. */
	}
}

/*-----------------------------------------------------------*/
static void getPlayer2Input( void *pvParameters )
{
	char readinput;

	for( ;; )
	{
		readinput = 0x00;
		readinput = XGpio_DiscreteRead(&Gpio, BUTTONS_CHANNEL);

		/* Block to wait for data arriving on the queue. */
		xQueueSend( 	xQueuePlayer2,				/* The queue being read. */
						&readinput,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

	}
}

/*-----------------------------------------------------------*/
static void gamePongTask( void *pvParameters )
{

	//pixelColour colourArray[COLOURARRAYWIDTH][COLOURARRAYHEIGHT];

	u32 movePlayer1, movePlayer2;

	for( ;; )
	{
		/* Block to wait for data arriving on the queue. */
		xQueueReceive( 	xQueuePlayer1,				/* The queue being read. */
						&movePlayer1,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */
		xQueueReceive( 	xQueuePlayer2,				/* The queue being read. */
						&movePlayer2,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

		getPlayer1Move(&movePlayer1);
		getPlayer1Move(&movePlayer2);
		drawGame();
		clearArray();

		if(tickFlag)
		{
			updateGame();
		}

	}
}




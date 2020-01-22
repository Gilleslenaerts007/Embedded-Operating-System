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
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xgpiops.h"

#include "Hcsr04_Sensor.h"
#include "sleep.h"
#include "xil_io.h"

#define TIMER_ID	1
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define TIMER_CHECK_THRESHOLD	9
/*-----------------------------------------------------------*/

/* The Tx and Rx tasks as described at the top of this file. */
static void prvTxTask( void *pvParameters );
static void prvRxTask( void *pvParameters );
static void vTimerCallback( TimerHandle_t pxTimer );
/*-----------------------------------------------------------*/

/* The queue used by the Tx and Rx tasks, as described at the top of this
file. */
static TaskHandle_t xTxTask;
static TaskHandle_t xRxTask;
static QueueHandle_t xQueue = NULL;
static TimerHandle_t xTimer = NULL;
long RxtaskCntr = 0;

//GPIO Variables
#define GPIO_DOBBELSTEEN_DEVICE_ID XPAR_GPIO_0_DEVICE_ID
#define DOBBEL_CHANNEL 1

#define DOBBEL7 0b11111111

#define DOB0  0b00000000
#define DOB1  0b00010000
#define DOB2  0b10000001
#define DOB3  0b10010001
#define DOB4  0b10100101
#define DOB5  0b10110101
#define DOB6  0b11100111
#define DOB7  0b11110111

char segControl;
//u32 Data;
XGpio Gpio;
u32 Input_Pin; /* Switch button */
void startGPIO();
int dobbelSteenValue(int SegNumber);
u32 Data = 0;

static void vTimerCallback( TimerHandle_t pxTimer )
{
	long lTimerId;
	configASSERT( pxTimer );

	lTimerId = ( long ) pvTimerGetTimerID( pxTimer );

	if (lTimerId != TIMER_ID) {
		xil_printf("Timer Error");
	}

	xil_printf("\n\r FreeRTOS \n\r");
	sleep(1); // Sleep to see the text printed above..
}

int main( void )
{
	const TickType_t x10seconds = pdMS_TO_TICKS( DELAY_10_SECONDS );

	xil_printf( "Hello from Gilles. Gequoteerde Opdracht HCRS04 Timers, QUEUE, ..\r\n" );
	startGPIO();

	/* Create the two tasks.  The Tx task is given a lower priority than the
	Rx task, so the Rx task will leave the Blocked state and pre-empt the Tx
	task as soon as the Tx task places an item in the queue. */
	xTaskCreate( 	prvTxTask, 					/* The function that implements the task. */
					( const char * ) "Tx", 		/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
					NULL, 						/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
					&xTxTask );

	xTaskCreate( prvRxTask,
				 ( const char * ) "GB",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &xRxTask );

	xQueue = xQueueCreate( 	1,						/* There is only one space in the queue. */
							sizeof(Data ) );	/* Each space in the queue is large enough to hold a uint32_t. */

	/* Check the queue was created. */
	configASSERT( xQueue );

	//10sec timer.
	xTimer = xTimerCreate( (const char *) "Timer",
							x10seconds,
							pdFALSE,
							(void *) TIMER_ID,
							vTimerCallback);
	/* Check the timer was created. */
	configASSERT( xTimer );

	/* start the timer with a block time of 0 ticks. This means as soon
	   as the schedule starts the timer will start running and will expire after
	   10 seconds */
	xTimerStart( xTimer, 0 );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for( ;; );
}


/*-----------------------------------------------------------*/
static void prvTxTask( void *pvParameters )
{
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );
Data = 0;
	for( ;; )
	{
		/* Delay for 1 second. */
		//vTaskDelay( x1second );

		Data =  HCSR04_SENSOR_mReadReg(XPAR_HCSR04_SENSOR_0_S00_AXI_BASEADDR,HCSR04_SENSOR_S00_AXI_SLV_REG3_OFFSET);
		//xil_printf( "HSCD04 Raw data : %d.\n\r", Data );
		/* Send the next value on the queue.  The queue should always be
		empty at this point so a block time of 0 is used. */
		xQueueSend( xQueue,			/* The queue being written to. */
					&Data, /* The address of the data being sent. */
					0UL );			/* The block time. */
	}
}

/*-----------------------------------------------------------*/
static void prvRxTask( void *pvParameters )
{
u32 RecvData = 0;
int number = 0;
	for( ;; )
	{

		/* Block to wait for data arriving on the queue. */
		xQueueReceive( 	xQueue,				/* The queue being read. */
						&RecvData,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

		/* Print the received data. */
		xil_printf( "HSCD04 Measured : %d cm.\n\r", RecvData );
		number = (RecvData/10)%10;
		//xil_printf( "number : %d cm.\n\r", number ); //debug
		XGpio_DiscreteWrite(&Gpio,DOBBEL_CHANNEL,dobbelSteenValue(number));
		number = 0;
	}
}

int dobbelSteenValue(int SegNumber)
{
	static int value=0;
	switch (SegNumber)
	{
		case 0:
			value = 0;
			break;
		case 1:
			value = DOB1;
			break;
		case 2:
			value = DOB2;
			break;
		case 3:
			value = DOB3;
			break;
		case 4:
			value = DOB4;
			break;
		case 5:
			value = DOB5;
			break;
		case 6:
			value = DOB6;
			break;
		case 7:
			value = DOB7;
			break;
		default:
			value = 0;
			break;
	}

	return value;
}

/*-----------------------------------------------------------*/
void startGPIO()
{

	int status;

	xil_printf("Starting GPIO..\n\r");

	//Init PL
	XGpio_Config *GPIOConfigPtr;
	status = XGpio_Initialize(&Gpio, GPIO_DOBBELSTEEN_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&Gpio, DOBBEL_CHANNEL, ~DOBBEL7);

    return;
}


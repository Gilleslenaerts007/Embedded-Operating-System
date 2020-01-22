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

#define TIMER_ID	1
#define DELAY_60_SECONDS	60000UL
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
char HWstring[15] = "Hello World";
long RxtaskCntr = 0;

/*
 * Segment GPIO Defines
 */
#define GPIO_7SEGMENT_DEVICE_ID XPAR_GPIO_0_DEVICE_ID
#define SEGMENT_CHANNEL 1

#define SEGMENT7 0b111111111

#define A  0b001000000
#define B  0b010000000
#define C  0b000010000
#define D  0b000100000
#define E  0b000000100
#define F  0b000001000
#define G  0b000000001
#define DP 0b000000010

char segControl;
u32 Data;
XGpio Gpio;
XGpioPs GpioPS;
u32 Input_Pin; /* Switch button */
void startGPIOPS();
int segment7value(int SegNumber);

int main( void )
{
	//const TickType_t x10seconds = pdMS_TO_TICKS( DELAY_10_SECONDS );
	const TickType_t x60seconds = pdMS_TO_TICKS( DELAY_60_SECONDS );
	xil_printf( "Hello from Freertos example main\r\n" );
	startGPIOPS();
	XGpio_DiscreteWrite(&Gpio,SEGMENT_CHANNEL,0);

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

	/* Create the queue used by the tasks.  The Rx task has a higher priority
	than the Tx task, so will preempt the Tx task and remove values from the
	queue as soon as the Tx task writes to the queue - therefore the queue can
	never have more than one item in it. */
	xQueue = xQueueCreate( 	1,						/* There is only one space in the queue. */
							sizeof( Data ) );	/* Each space in the queue is large enough to hold a uint32_t. */

	/* Check the queue was created. */
	configASSERT( xQueue );

	/* Create a timer with a timer expiry of 10 seconds. The timer would expire
	 after 10 seconds and the timer call back would get called. In the timer call back
	 checks are done to ensure that the tasks have been running properly till then.
	 The tasks are deleted in the timer call back and a message is printed to convey that
	 the example has run successfully.
	 The timer expiry is set to 10 seconds and the timer set to not auto reload. */
	xTimer = xTimerCreate( (const char *) "Timer",
							x60seconds,
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

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}


/*-----------------------------------------------------------*/
static void prvTxTask( void *pvParameters )
{
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );

	for( ;; )
	{
		/* Delay for 1 second. */
		vTaskDelay( x1second );
		Data = XGpioPs_ReadPin(&GpioPS, Input_Pin);

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
char Recdstring[15] = "";
u32 RecData;
int number, value = 0;
	for( ;; )
	{
		/* Block to wait for data arriving on the queue. */
		xQueueReceive( 	xQueue,				/* The queue being read. */
						&RecData,	/* Data is read into this address. */
						portMAX_DELAY );	/* Wait without a timeout for data. */

		/* Print the received data. */
		//xil_printf( "Rx task received string from Tx task: %s\r\n", segControl );
		printf("Value of Switch: %d\r\n",(int) RecData);
		if(RecData) number++;
		if (number>=15) number =0;
		printf("Value of Switch: %d & number: %d\r\n",(int) RecData, number);
		XGpio_DiscreteWrite(&Gpio,SEGMENT_CHANNEL,segment7value(number));
		RecData = 0;
		RxtaskCntr++;
	}
}

/*-----------------------------------------------------------*/
static void vTimerCallback( TimerHandle_t pxTimer )
{
	long lTimerId;
	configASSERT( pxTimer );

	lTimerId = ( long ) pvTimerGetTimerID( pxTimer );

	if (lTimerId != TIMER_ID) {
		xil_printf("FreeRTOS Hello World Example FAILED");
	}

	/* If the RxtaskCntr is updated every time the Rx task is called. The
	 Rx task is called every time the Tx task sends a message. The Tx task
	 sends a message every 1 second.
	 The timer expires after 10 seconds. We expect the RxtaskCntr to at least
	 have a value of 9 (TIMER_CHECK_THRESHOLD) when the timer expires. */
	if (RxtaskCntr >= TIMER_CHECK_THRESHOLD) {
		xil_printf("FreeRTOS Hello World Example PASSED");
	} else {
		xil_printf("FreeRTOS Hello World Example FAILED");
	}

	vTaskDelete( xRxTask );
	vTaskDelete( xTxTask );
}
int segment7value(int SegNumber)
{
	int value=0;
	switch (SegNumber)
	{
		case 0:
			value = A + B + C + D + E + F;
			break;
		case 1:
			value = B + C;
			break;
		case 2:
			value = A + B + G + E + D;
			break;
		case 3:
			value = A + B + G + C + D;
			break;
		case 4:
			value = F + G + B + C;
			break;
		case 5:
			value = A + F + G + C + D;
			break;
		case 6:
			value = A + F + G + E + C + D;
			break;
		case 7:
			value = A + B + C;
			break;
		case 8:
			value = A + B + C + D + E + F + G;
			break;
		case 9:
			value = A + B + C + D + F + G;
			break;
		case 10:
			value = E + F + A + B + G + C;
			break;
		case 11:
			value = F + G + C + D + E;
			break;
		case 12:
			value = A + F + E + D;
			break;
		case 13:
			value = B + G + E + D + C;
			break;
		case 14:
			value = A + F + G + E + D;
			break;
		case 15:
			value = A + F + G + E;
			break;
		default:
			value = DP;
	}

	return value;
}
void startGPIOPS()
{
	volatile int Delay;

	int status;

	//Init PS
	XGpioPs_Config *GPIOPSConfigPtr;
	GPIOPSConfigPtr = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&GpioPS, GPIOPSConfigPtr,GPIOPSConfigPtr->BaseAddr);

	if (status != XST_SUCCESS) {
		printf("status error \n\r");
		return XST_FAILURE;
	}
    printf("Starting GPIO PS\n\r");
    Input_Pin = 0;
	XGpioPs_SetDirectionPin(&GpioPS,Input_Pin,0);

	//Init PL
	XGpio_Config *GPIOConfigPtr;
	status = XGpio_Initialize(&Gpio, GPIO_7SEGMENT_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&Gpio, SEGMENT_CHANNEL, ~SEGMENT7);

    return;
}


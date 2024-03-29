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
#include "xuartps_hw.h"
#include "xgpiops.h"
#include "xgpio.h"
#include "xstatus.h"

#define TIMER_ID30	30
#define TIMER_ID5	5
#define TIMER_ID2	2

#define DELAY_30_SECONDS	30000UL
#define DELAY_5_SECONDS	5000UL
#define DELAY_2_SECONDS		2000UL
#define DELAY_1_SECOND		1000UL
#define TIMER_CHECK_THRESHOLD	9
/*-----------------------------------------------------------*/

/* The Tx and Rx tasks as described at the top of this file. */
static void prvSerialTask( void *pvParameters );
static void prvPSButtonTask( void *pvParameters );
static void prvRxTask( void *pvParameters );
static void vTimerCallback( TimerHandle_t pxTimer );
/*-----------------------------------------------------------*/

/* The queue used by the Tx and Rx tasks, as described at the top of this
file. */
static TaskHandle_t xSerialTask;
static TaskHandle_t xPSButtonTask;
static TaskHandle_t xRxTask;

static QueueHandle_t xQueue = NULL;
static QueueHandle_t xQueue2 = NULL;

static TimerHandle_t xTimer30 = NULL;
static TimerHandle_t xTimer5 = NULL;
static TimerHandle_t xTimer2 = NULL;



/*User defines */
static void startGPIO();

//intr stuff
#include "xscugic.h"
#include "Xil_exception.h"
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INTERRUPT_ID	XPS_GPIO_INT_ID
static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId);
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status);
static XScuGic Intc;
static char intrflag = 0x00;
/* Semaphore handle */
#include "semphr.h"
xSemaphoreHandle xSemaphore_led = NULL;

//PS
XGpioPs GpioPS;
u32 Input_Pin; /* Switch button */
u32 Output_PinG, Output_PinR; /* LED button */

//PL
XGpio Gpio;
#define GPIO_EXAMPLE_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID
#define LED_DELAY     10000000

#define LEDG_CHANNEL 1
#define LEDR_CHANNEL 2
#define LEDG 0b00
#define LEDR 0b00

static void vTimerCallback( TimerHandle_t pxTimer )
{
	long lTimerId;
	static char toggleflag = 0x01;
	configASSERT( pxTimer );
	lTimerId = ( long ) pvTimerGetTimerID( pxTimer );

	if (lTimerId == TIMER_ID2)
	{
		xil_printf("2sec. Timer .. Led Toggle\r\n");
		if (toggleflag)
		{
			XGpio_DiscreteWrite(&Gpio, LEDG_CHANNEL, 0x01);
			toggleflag = 0x00;
		}
		else
		{
			XGpio_DiscreteClear(&Gpio, LEDG_CHANNEL, 0x01);
			toggleflag = 0x01;
		}

		configASSERT( xTimer2 );
		xTimerStart( xTimer2, 0 );
	}

	if (lTimerId == TIMER_ID5) {
		xil_printf("5sec. Timer .. Gilles Lenaerts.\r\n");
	}

	if (lTimerId == TIMER_ID30) {
		xil_printf("30sec. Timer .. Ending Program.\r\n");
		vTaskDelete( prvSerialTask );
		vTaskDelete( prvPSButtonTask );
		vTaskDelete( prvRxTask );
	}

}



int main( void )
{
	const TickType_t x2seconds = pdMS_TO_TICKS( DELAY_2_SECONDS );
	const TickType_t x5seconds = pdMS_TO_TICKS( DELAY_5_SECONDS );
	const TickType_t x30seconds = pdMS_TO_TICKS( DELAY_30_SECONDS );

	startGPIO();
	printf( "FreeRTOS with PS intr.\r\n" );

	 /* Create Binary Semaphore */
	 vSemaphoreCreateBinary(xSemaphore_led);
	 configASSERT( xSemaphore_led );

	/* Create the two tasks.  The Tx task is given a lower priority than the
	Rx task, so the Rx task will leave the Blocked state and pre-empt the Tx
	task as soon as the Tx task places an item in the queue. */
	xTaskCreate( 	prvSerialTask, 					/* The function that implements the task. */
					( const char * ) "A or B", 		/* Text name for the task, provided to assist debugging only. */
					configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
					NULL, 						/* The task parameter is not used, so set to NULL. */
					tskIDLE_PRIORITY,			/* The task runs at the idle priority. */
					&xSerialTask );

	xTaskCreate( prvPSButtonTask,
				 ( const char * ) "PsButton",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &xPSButtonTask );
	xTaskCreate( prvRxTask,
				 ( const char * ) "Quehandler",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &xRxTask );

	/* Create the queue used by the tasks.  The Rx task has a higher priority
	than the Tx task, so will preempt the Tx task and remove values from the
	queue as soon as the Tx task writes to the queue - therefore the queue can
	never have more than one item in it. */
	xQueue = xQueueCreate( 	1,						/* There is only one space in the queue. */
							sizeof( char ) );	/* Each space in the queue is large enough to hold a uint32_t. */
	xQueue2 = xQueueCreate( 	1,						/* There is only one space in the queue. */
							sizeof( u32 ) );	/* Each space in the queue is large enough to hold a uint32_t. */

	/* Check the queue was created. */
	configASSERT( xQueue );
	configASSERT( xQueue2 );

	/* Create a timer with a timer expiry of 10 seconds. The timer would expire
	 after 10 seconds and the timer call back would get called. In the timer call back
	 checks are done to ensure that the tasks have been running properly till then.
	 The tasks are deleted in the timer call back and a message is printed to convey that
	 the example has run successfully.
	 The timer expiry is set to 10 seconds and the timer set to not auto reload. */
	xTimer30 = xTimerCreate( (const char *) "Timer30",
							x30seconds,
							pdFALSE, 	//Auto reload or not when expire.
							(void *) TIMER_ID30,
							vTimerCallback);
	xTimer5 = xTimerCreate( (const char *) "Timer5",
							x5seconds,
							pdFALSE,
							(void *) TIMER_ID5,
							vTimerCallback);
	xTimer2 = xTimerCreate( (const char *) "Timer2",
							x2seconds,
							pdFALSE,
							(void *) TIMER_ID2,
							vTimerCallback);
	/* Check the timer was created. */
	configASSERT( xTimer30 );
	configASSERT( xTimer5 );
	configASSERT( xTimer2 );

	/* start the timer with a block time of 0 ticks. This means as soon
	   as the schedule starts the timer will start running and will expire after
	   10 seconds */
	xTimerStart( xTimer30, 0 );
	xTimerStart( xTimer5, 0 );
	xTimerStart( xTimer2, 0 );

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
static void prvSerialTask( void *pvParameters )
{
char input = 'b';

	for( ;; )
	{

		input = getchar();
		//check for new input if yes change old.
		if ( (input == 'A') | (input == 'a') | (input == 'B') | (input == 'b') )
		{
			xQueueSend( xQueue,			/* The queue being written to. */
						&input, /* The address of the data being sent. */
						0UL );			/* The block time. */
			xil_printf( "sending input : %c\r\n", input );
		}
		else input = 0x00;


	}
}

/*-----------------------------------------------------------*/
static void prvPSButtonTask( void *pvParameters )
{
const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );
static int psbutton=0;
	for( ;; )
	{
		/* Delay for 1 second. */
		//vTaskDelay( x1second );

		//psbutton = (XGpioPs_ReadPin(&GpioPS, Input_Pin));

		if ( xSemaphoreTake( xSemaphore_led,( portTickType ) portMAX_DELAY ) == pdTRUE )
		{
			psbutton = 1;
			xQueueSend( xQueue2,			/* The queue being written to. */
						&psbutton, /* The address of the data being sent. */
						0UL );			/* The block time. */
			psbutton = 0;
		}
		else
		{
			xil_printf("xSemaphore_led take fail\r\n");
			/* Call shutdown */
			//prvShutdown();
		}

	}
}

/*-----------------------------------------------------------*/
static void prvRxTask( void *pvParameters )
{
char recvCommandInput = 0x00;
int recvButtonInput = 0;
int i = 0;
char oldinput = 0x00;

//Quelay so queue's don't wait on data forever so we Still run code. No blocking for data to arrive..
TickType_t quelay = pdMS_TO_TICKS( 800UL );

	for( ;; )
	{
		/* Block to wait for data arriving on the queue. */
		xQueueReceive( 	xQueue,				/* The queue being read. */
						&recvCommandInput,	/* Data is read into this address. */
						quelay );	/* Wait without a timeout for data. */
		xQueueReceive( 	xQueue2,				/* The queue being read. */
						&recvButtonInput,	/* Data is read into this address. */
						quelay );	/* Wait without a timeout for data. */

		if (recvCommandInput != oldinput)
		{
			oldinput = recvCommandInput;

			if (recvCommandInput == 'a' )
			{
				xil_printf("System is on\r\n");
			}
			else
			{
				xil_printf("system if off \r\n");
			}
		}
		else recvCommandInput = oldinput;

		if (recvCommandInput == 'a' )
		{
			xil_printf("Recieved command: %c In RX Task...\r\n", recvCommandInput);
			xil_printf("Recieved button : %d In RX Task...\r\n", recvButtonInput);
			if (!recvButtonInput)
			{
				//xil_printf("I case is: %d", i);
				switch (i)
				{
					case 0: XGpioPs_WritePin(&GpioPS, Output_PinR,0b00);
							XGpioPs_WritePin(&GpioPS, Output_PinG,0b00);
							break;
					case 1: XGpioPs_WritePin(&GpioPS, Output_PinR,0b00);
							XGpioPs_WritePin(&GpioPS, Output_PinG,0b01);
							break;
					case 2: XGpioPs_WritePin(&GpioPS, Output_PinR,0b01);
							XGpioPs_WritePin(&GpioPS, Output_PinG,0b00);
							break;
					case 3: XGpioPs_WritePin(&GpioPS, Output_PinR,0b01);
							XGpioPs_WritePin(&GpioPS, Output_PinG,0b01);
							break;
					default: i = 0;
							break;
				}

				if (i>=3)
				{
					i =0;
				}
				else i++;
			}
		}

	}
}

/*-----------------------------------------------------------*/
void startGPIO()
{
	int Status;

	//Init GPIOPL
	Status = XGpio_Initialize(&Gpio, GPIO_EXAMPLE_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}
	//Set data as output
	XGpio_SetDataDirection(&Gpio, LEDG_CHANNEL, 0x00);
	XGpio_SetDataDirection(&Gpio, LEDR_CHANNEL, 0x00);

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
    Output_PinR = 52;
    Output_PinG = 53;

    //Set Led Pin
    XGpioPs_SetDirectionPin(&GpioPS, Output_PinR, 1);
	XGpioPs_SetOutputEnablePin(&GpioPS, Output_PinR, 1);
    XGpioPs_SetDirectionPin(&GpioPS, Output_PinG, 1);
	XGpioPs_SetOutputEnablePin(&GpioPS, Output_PinG, 1);

	//init leds out.
	XGpioPs_WritePin(&GpioPS, Output_PinR,0b00);
	XGpioPs_WritePin(&GpioPS, Output_PinG,0b00);

	// Set Input pin
	XGpioPs_SetDirectionPin(&GpioPS,Input_Pin,0);

	SetupInterruptSystem(&Intc, &GpioPS, GPIO_INTERRUPT_ID); //!!! gpio PS variable !!!
}

static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId)
{
	XScuGic_Config *IntcConfig;
	Xil_ExceptionInit();

	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);

	XScuGic_CfgInitialize(GicInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, GicInstancePtr);

	XScuGic_Connect(GicInstancePtr, GpioIntrId, (Xil_ExceptionHandler)XGpioPs_IntrHandler, (void *)Gpio);

	XGpioPs_SetIntrTypePin(Gpio, Input_Pin, XGPIOPS_IRQ_TYPE_EDGE_RISING);

	XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, IntrHandler);

	XGpioPs_IntrEnablePin(Gpio, Input_Pin);

	XGpioPs_IntrDisable(Gpio,0x01,0xFFFFFFFF);
	XScuGic_Enable(GicInstancePtr, GpioIntrId);

	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}

static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	xil_printf("Into interrupt. \r\n")
	XGpioPs *Gpio = (XGpioPs *)CallBackRef;

	if( Bank == 0x00) 				//Check from which bank intr. came...
	{
		printf("Got intr. from PS button. SW value: 1");
		intrflag = 0x01;
	}

	if ( xSemaphoreGive( xSemaphore_led ) != pdTRUE )
	{
		xil_printf("xSemaphore_led give fail\r\n");
	}

}



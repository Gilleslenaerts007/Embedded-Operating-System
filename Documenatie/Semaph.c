/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "xil_printf.h"

/* App includes. */
#include "xgpiops.h"
#define GPIO_DEVICE_ID	XPAR_XGPIOPS_0_DEVICE_ID
#define OUTPUT_PIN		52	/* Pin connected to LED/Output */
XGpioPs Gpio;	/* The driver instance for GPIO Device. */
XGpioPs_Config *ConfigPtr; /* The driver config instance for GPIO Device. */

/* Priorities at which the tasks are created. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* This example toggles the led for each TIMER_PERIOD.
 * Led_Task waits on the semaphore on each iteration, which is
 * signaled from the timer call back, each time the timer expires.
 * Configure timer period by changing the value against the
 * TIMER_PERIOD macro.
 * A safe shutdown function is provided which deletes all the
 * resources in case semaphore take/give fails or timer could not
 * be created.
 */
#define TIMER_PERIOD	100 /* No of ticks before timer expires */
#define TIMER_ID		123 /* Timer ID*/

/* Timer handle */
xTimerHandle xTimer;

/* Task handle */
xTaskHandle xTask;

/* Semaphore handle */
xSemaphoreHandle xSemaphore_led = NULL;

/*-----------------------------------------------------------*/
static void prvLed_Task( void *pvParameters );
void prvSetGpioHardware(void);
void prvShutdown(void);

/* Define timer callback function */
void vTimerCallback( xTimerHandle pxTimer )
{
	if ( xSemaphoreGive( xSemaphore_led ) != pdTRUE )
	{
		xil_printf("xSemaphore_led give fail\r\n");
		prvShutdown();
	}
}

int main( void )
{
	 xil_printf( "Hello from Gilles.. Oefening 10 SEMAPHORE. \r\n" );
	 //prvInitializeExceptions();

	 /* Create Binary Semaphore */
	 vSemaphoreCreateBinary(xSemaphore_led);
	 configASSERT( xSemaphore_led );

	 /* Setup the GPIO Hardware. */
	 prvSetGpioHardware();

	 /* Create the task */
     xTaskCreate( prvLed_Task, ( signed char * ) "LED_TASK",
     			configMINIMAL_STACK_SIZE, NULL,
    			mainLED_TASK_PRIORITY, &xTask );


	 /* Create timer.  Starting the timer before the scheduler
     has been started means the timer will start running immediately that
     the scheduler starts. */
     xTimer = xTimerCreate ( (const signed char *) "LedTimer",
        		 	 	 	 	 	   TIMER_PERIOD,
        		 	 	 	 	 	   pdTRUE, /* auto-reload when expires */
        		 	 	 	 	 	   (void *) TIMER_ID, /* unique id */
        		 	 	 	 	 	   vTimerCallback	/* Callback */
                           );

     if ( xTimer == NULL ) {
		 /* The timer was not created. */
		 xil_printf("Failed to create timer\n\r");
		 prvShutdown();
		 return 0;
	 } else {
		 /* Start the timer */
		 xTimerStart( xTimer, 0 );
	 }

     /* Starting the scheduler will start the timers running as it is already
     been set into the active state. */
     vTaskStartScheduler();

     /* Should not reach here. */
     for( ;; );
}

/*-----------------------------------------------------------*/
static void prvLed_Task( void *pvParameters )
{
  	unsigned int uiLedFlag = 0;

	for (;;)
		{
			if ( xSemaphoreTake( xSemaphore_led,
						( portTickType ) portMAX_DELAY ) == pdTRUE )
			{
				uiLedFlag ^= 1;
				XGpioPs_WritePin(&Gpio, OUTPUT_PIN, uiLedFlag);
			}
			else
			{
				xil_printf("xSemaphore_led take fail\r\n");
				/* Call shutdown */
				prvShutdown();
			}
		}
}

/*-----------------------------------------------------------*/
void prvSetGpioHardware( void )
{
 	int Status;
 	/*
 	 * Initialize the GPIO driver.
 	 */
 	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
 	Status = XGpioPs_CfgInitialize(&Gpio, ConfigPtr,
 					ConfigPtr->BaseAddr);
 	if (Status != XST_SUCCESS) {
 		xil_printf("GPIO Initialize failed\n");
 	}

 	/*
 	 * Set the direction for the pin to be output and
 	 * Enable the Output enable for the LED Pin.
 	 */
 	XGpioPs_SetDirectionPin(&Gpio, OUTPUT_PIN, 1);
 	XGpioPs_SetOutputEnablePin(&Gpio, OUTPUT_PIN, 1);

 	/*
 	 * Set the GPIO output to be low.
 	 */
 	XGpioPs_WritePin(&Gpio, OUTPUT_PIN, 0x0);
}

void prvShutdown( void )
{
	 /* Check if timer is created */
	 if (xTimer)
		 xTimerDelete(xTimer, 0);
	 vSemaphoreDelete( xSemaphore_led );
	 vTaskDelete( xTask );
}

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue or
	semaphore is created.  It is also called by various parts of the demo
	application.  If heap_1.c or heap_2.c are used, then the size of the heap
	available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* vApplicationStackOverflowHook() will only be called if
	configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2.  The handle and name
	of the offending task will be passed into the hook function via its
	parameters.  However, when a stack has overflowed, it is possible that the
	parameters will have been corrupted, in which case the pxCurrentTCB variable
	can be inspected directly. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationSetupHardware( void )
{
	/* Do nothing */
}

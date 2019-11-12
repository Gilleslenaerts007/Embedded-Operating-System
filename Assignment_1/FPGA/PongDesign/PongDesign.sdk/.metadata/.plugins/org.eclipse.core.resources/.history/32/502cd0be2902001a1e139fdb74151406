
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "WS2812.h"
#include "xparameters.h"
#include "xil_io.h"
#include "sleep.h"
#include "xgpio.h"

#define WS2812ADR XPAR_WS2812_0_S00_AXI_BASEADDR


#define GPIO_EXAMPLE_DEVICE_ID  XPAR_GPIO_0_DEVICE_ID
#define GPIO_CHANNEL 1

XGpio Gpio;

u16 result;

int main()
{
    init_platform();
    int readreg, Status;

	/* Initialize the GPIO driver */
	Status = XGpio_Initialize(&Gpio, GPIO_EXAMPLE_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization Failed\r\n");
		return XST_FAILURE;
	}

	/* Set the direction for all signals as inputs except the LED output */
		XGpio_SetDataDirection(&Gpio, GPIO_CHANNEL, 0);

    printf("Starting Pong.\n\r");

    WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xFF0000);
    XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x01);
    //XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x00);
   // WS2812_mWriteReg(WS2812ADR, 0, 1500000);

	for(;;){
		readreg= WS2812_mReadReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET);
		printf("Readreg0: %d\n\r", readreg);
		//XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x01);
		//XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x00);
	}

    cleanup_platform();
    return 0;
}

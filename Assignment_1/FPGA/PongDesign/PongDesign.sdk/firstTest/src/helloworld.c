
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "WS2812.h"
#include "xparameters.h"
#include "xil_io.h"
#include "sleep.h"
//#include "xgpio.h"

#define WS2812ADR XPAR_WS2812_0_S00_AXI_BASEADDR

u16 result;


int main()
{
    init_platform();
    int readreg, Status;
    int itel;
    printf("Starting Pong.\n\r");

    //WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xFFFFFF);
    //WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG64_OFFSET, 0xFF);
    //WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG64_OFFSET, 0x00);
    //XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x00);
   // WS2812_mWriteReg(WS2812ADR, 0, 1500000);

	for(;;){
    //WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xFFFFFF);
		//readreg= WS2812_mReadReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET);
		//printf("Readreg0: %d\n\r", readreg);
		//itel++;
		//XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x01);
		//XGpio_DiscreteWrite(&Gpio, GPIO_CHANNEL, 0x00);
		//WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xffffff);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xFF0000);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG2_OFFSET, 0xFF0000);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG63_OFFSET, 0xFF0000);
		sleep(2);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0x00FF00);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG2_OFFSET, 0x00FF00);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG63_OFFSET, 0x00FF00);
		sleep(2);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0xFFFFFF);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG2_OFFSET, 0xFFFFFF);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG63_OFFSET, 0xFFFFFF);
		sleep(2);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET, 0x000000);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG2_OFFSET, 0x000000);
		WS2812_mWriteReg(WS2812ADR,WS2812_S00_AXI_SLV_REG63_OFFSET, 0x000000);
		sleep(2);
		readreg = WS2812_mReadReg(WS2812ADR,WS2812_S00_AXI_SLV_REG0_OFFSET);
		printf("Readreg0: %d\n\r", readreg);
	}

    cleanup_platform();
    return 0;
}

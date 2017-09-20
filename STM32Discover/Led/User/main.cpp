#include "stm32f4xx.h"
#include "Periph/Gpio.h"



int main(void)
{
	using namespace Gpio;
	RCC->AHB1ENR |= 0x40;
	GpInit(GPIOG, Pin13 | Pin14);
	GPIOG << Pin_13;


	while (true) {
		
	}
}













#include "Periph/SDRAM.h"
#include "Periph/LTDC.h"
#include "Periph/Spi.h"


namespace LED {

	void Init(void)
	{
		RCC->AHB1ENR |= 0x40;
		
		GPIOG->MODER &= ~0x3C000000;
		GPIOG->MODER |= 0x14000000;
		GPIOG->OTYPER &= ~0x6000;
		GPIOG->PUPDR &= ~0x3C000000;
		GPIOG->OSPEEDR |= 0x3C000000;
		GPIOG->BSRRH = 0x6000;
	}
	
	void On(void)
	{
		GPIOG->BSRRL = 0x6000;
	}
	
	void Off(void)
	{
		GPIOG->BSRRH = 0x6000;
	}
	
	void toggle(void)
	{
		GPIOG->ODR ^= 0x6000;
	}

}

namespace System {

	void Init(void)
	{
		SysTick->LOAD = 22500 - 1;
		SysTick->VAL = 0x00;
		SysTick->CTRL = 0x01;
	}
	
	void delayMs(uint32_t ms)
	{
		uint32_t tickValue = ms * 22500;
		uint32_t oldValue = SysTick->VAL;
		uint32_t newValue = 0;
		uint32_t cntValue = 0;
		
		do {
		
			newValue = SysTick->VAL;
			if (newValue != oldValue) {
				if (newValue > oldValue) {
					cntValue += SysTick->LOAD + oldValue - newValue;
				} else {
					cntValue += oldValue - newValue;
				}
				oldValue = newValue;
			}
		
		} while (tickValue > cntValue);
	}

}


int main(void)
{
	SDRAM::Init();
	System::Init();
	LED::Init();
	Spi5::Init();
	Ltdc::Init();
	Ltdc::setColor(0xF800);
	Ltdc::drawRect(50, 50, 100, 100, 0x07FF);
	while (true)
	{
		LED::toggle();
		System::delayMs(100);
	}
}











#include "Periph/SDRAM.h"


namespace SDRAM {

/**				12 + 16 + 10 = 38
 *	PF0 <-> A0		PD14 <-> D0		PB5 <-> SDCKE1
 *	PF1 <-> A1		PD15 <-> D1		PB6 <-> SDNE1
 *	PF2 <-> A2		PD0 <-> D2		PC0 <-> SDNWE
 *	PF3 <-> A3		PD1 <-> D3		PE0 <-> NBL0
 *	PF4 <-> A4	 	PE7 <-> D4		PE1 <-> NBL1
 *	PF5 <-> A5		PE8 <-> D5		PF11 <-> SDNRAS
 *	PF12 <-> A6		PE9 <-> D6		PG4 <-> BA0
 *	PF13 <-> A7		PE10 <-> D7		PG5 <-> BA1
 *	PF14 <-> A8		PE11 <-> D8		PG8 <-> SDCLK
 *	PF15 <-> A9		PE12 <-> D9		PG15 <-> SDNCAS
 *	PG0 <-> A10		PE13 <-> D10	
 * 	PG1 <-> A11		PE14 <-> D11	
 *					PE15 <-> D12	
 *					PD8 <-> D13		
 *					PD9 <-> D14		
 *					PD10 <-> D15	
 *	GPIOB: 5, 6
 *	GPIOC: 0
 * 	GPIOD: 0, 1, 8, 9, 10, 14, 15
 *	GPIOE: 0, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15
 *	GPIOF: 0, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15
 * 	GPIOG: 0, 1, 4, 5, 8, 15
 */
	static void GpioInit(GPIO_TypeDef *port, uint32_t pinMask);
	static uint32_t constexpr Pin(uint32_t pin) {
		return 0x01 << pin;
	}
	static void WriteModeReg(uint32_t mode, uint32_t refresh, uint32_t value);
	
	void Init(void)
	{
		RCC->AHB1ENR |= 0x7E;
		RCC->AHB3ENR |= 0x01;
		// 5, 6
		GpioInit(GPIOB, Pin(5) | Pin(6));
		
		// 0
		GpioInit(GPIOC, Pin(0));
		
		// 0, 1, 8, 9, 10, 14, 15
		GpioInit(GPIOD, Pin(0) | Pin(1) | Pin(8) | Pin(9) | Pin(10) |
						Pin(14) | Pin(15));
		
		// 0, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15
		GpioInit(GPIOE, Pin(0) | Pin(1) | Pin(7) | Pin(8) |
						Pin(9) | Pin(10) | Pin(11) | Pin(12) |
						Pin(13) | Pin(14) | Pin(15));
		
		// 0, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15
		GpioInit(GPIOF, Pin(0) | Pin(1) | Pin(2) | Pin(3) |
						Pin(4) | Pin(5) | Pin(11) | Pin(12) |
						Pin(13) | Pin(14) | Pin(15));
		
		// 0, 1, 4, 5, 8, 15
		GpioInit(GPIOG, Pin(0) | Pin(1) |
						Pin(4) | Pin(5) | 
						Pin(8) | Pin(15));
		
		FMC_Bank5_6->SDCR[1] = (0 << 0) | // Col = 8
								(1 << 2) | // Row = 12
								(1 << 4) | // 16Bit
								(1 << 6) | // 4bank
								(2 << 7) | // 2 CAS Laency
								(0 << 9) | // Enable Write
								(2 << 10) | // CLK = 90MHz
								(0 << 12) | // Burst Read Disable
								(0 << 13); // Dis Rpipe
		/* 这几位需要在SDCR1寄存器中设置 **/
		FMC_Bank5_6->SDCR[0] = (2 << 10) | // CLK = 90MHz
								(1 << 12) | // Burst Read Enable
								(0 << 13); // Dis Rpipe
		
		FMC_Bank5_6->SDTR[1] = (1 << 0) | // tMRD = 2 Cycle
								(6 << 4) | // tXSR = 11.1 * 7 > 70ns
								(3 << 8) | // tRAS = 11.1 * 4 > 42ns
								(5 << 12) | // tRC = 11.1 * 6 > 63ns
								(1 << 16) | // tWR = 2 Cycle
								(1 << 20) | // tRP = 11.1 * 2 > 15ns
								(1 << 24); // tRCD = 11.1 * 2 > 15ns
		/* 这几位需要在SDTR1寄存器中设置 **/						
		FMC_Bank5_6->SDTR[0] = (1 << 20) | // tRP = 11.1 * 2 > 15ns
								(5 << 12); // tRC = 11.1 * 6 > 63ns
	
		WriteModeReg(0x01, 0, 0);
		for (uint32_t t = 0; t < 0x1FFF; t ++);
		WriteModeReg(0x02, 0, 0);
		WriteModeReg(0x03, 7, 0);
		WriteModeReg(0x04, 0, 0x222);
		FMC_Bank5_6->SDRTR = 1386 << 1;
	}
	
	static void WriteModeReg(uint32_t mode, uint32_t refresh, uint32_t value)
	{
		FMC_Bank5_6->SDCMR = mode |
							 0x08 |
							 (refresh << 5) |
							 (value << 9);
		while (0 != (FMC_Bank5_6->SDSR & 0x20));
	}
	
	static void GpioInit(GPIO_TypeDef *port, uint32_t pinMask)
	{
		uint32_t mask;
		for (uint32_t i = 0; i < 16; i ++) {
		
			mask = Pin(i);
			if ((pinMask & mask) != 0) {
				
				port->MODER &= ~(0x03 << i * 2);
				port->MODER |= 0x02 << i * 2;
				port->PUPDR &= ~(0x03 << i * 2);
				port->OSPEEDR |= (0x03 << i * 2);
				port->OTYPER &= ~(0x01 << i);
				if (i < 8) {
					port->AFR[0] &= ~(0x0F << 4 * i);
					port->AFR[0] |= 0x0C << 4 * i;
				} else {
					port->AFR[1] &= ~(0x0F << 4 * (i - 8));
					port->AFR[1] |= 0x0C << 4 * (i - 8);
				}
			}
		}
	}
	
	
}













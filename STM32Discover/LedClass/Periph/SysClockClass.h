#ifndef		__SYS_CLOCK_H
#define		__SYS_CLOCK_H

#include "stm32f4xx.h"

enum AHB1Periph {

	GpioA = 0x01,
	GpioB = 0x02,
	GpioC = 0x04,
	GpioD = 0x08,
	GpioE = 0x10,
	GpioF = 0x20,
	GpioG = 0x40,
	GpioH = 0x80,
	GpioI = 0x100,
	Crc = 0x1000,
	Dma1 = 0x200000,
	Dma2 = 0x400000,
	EthMac = 0x2000000,
	OtgHs = 0x20000000,
};

enum AHB2Periph {
	
	Dcmi = 0x01,
	Cryp = 0x10,
	Hash = 0x20,
	Rng = 0x40,
	OtgFs = 0x80,
};

enum AHB3Periph {
	
	Fsmc = 0x01,
};

enum APB1Periph {
	
	Tim2 = 0x01,
	Tim3 = 0x02,
	Tim4 = 0x04,
	Tim5 = 0x08,
	Tim6 = 0x10,
	Tim7 = 0x20,
	Tim12 = 0x40,
	Tim13 = 0x80,
	Tim14 = 0x100,
	Wwdg = 0x800,
	Spi2 = 0x4000,
	Spi3 = 0x8000,
	Usart2 = 0x20000,
	Usart3 = 0x40000,
	Uart4 = 0x80000,
	Uart5 = 0x100000,
	I2c1 = 0x200000,
	I2c2 = 0x400000,
	I2c3 = 0x800000,
	Can1 = 0x2000000,
	Can2 = 0x4000000,
	Pwr = 0x10000000,
	Dac = 0x20000000,
	Uart7 = 0x40000000,
	Uart8 = 0x80000000,
};

enum APB2Periph {

	Tim1 = 0x01,
	Tim8 = 0x02,
	Usart1 = 0x10,
	Usart6 = 0x20,
	Adc = 0x100,
	Sdio = 0x800,
	Spi1 = 0x1000,
	Spi4 = 0x2000,
	SysCfg = 0x4000,
	Tim9 = 0x10000,
	Tim10 = 0x20000,
	Tim11 = 0x40000,
	Spi5 = 0x100000,
	Spi6 = 0x200000,
};

class SysClock {

public:
	static inline void setAHB1Periph(AHB1Periph periph) {
		RCC->AHB1ENR |= periph;
	}
	
	static inline void setAHB2Periph(AHB2Periph periph) {
		RCC->AHB2ENR |= periph;
	}
	
	static inline void setAHB3Periph(AHB3Periph periph) {
		RCC->AHB3ENR |= periph;
	}
	
	static inline void setAPB1Periph(APB1Periph periph) {
		RCC->APB1ENR |= periph;
	}
	
	static inline void setAPB2Periph(APB2Periph periph) {
		RCC->APB2ENR |= periph;
	}
	
	static inline void resetAHB1Periph(AHB1Periph periph) {
		RCC->AHB1ENR &= ~periph;
	}
	
	static inline void resetAHB2Periph(AHB2Periph periph) {
		RCC->AHB2ENR &= ~periph;
	}
	
	static inline void resetAHB3Periph(AHB3Periph periph) {
		RCC->AHB3ENR &= ~periph;
	} 
	
	static inline void resetAPB1Periph(APB1Periph periph) {
		RCC->APB1ENR &= ~periph;
	}
	
	static inline void resetAPB2Periph(APB2Periph periph) {
		RCC->APB2ENR &= ~periph;
	}

private:
	SysClock(void)
	{
		
	}
};








#endif

#include "Periph/LTDC.h"



namespace Ltdc {
	
/**
 *	(PC10 <-> R2)	PA6 <-> G2		(PD6 <-> B2)
 *	PB0 <-> R3		PG10 <-> G3		PG11 <-> B3
 *	PA11 <-> R4		PB10 <-> G4		PG12 <-> B4
 *	PA12 <-> R5		PB11 <-> G5		PA3 <-> B5
 *	PB1 <-> R6		PC7 <-> G6		PB8 <-> B6
 *	PG6 <-> R7		PD3 <-> G7		PB9 <-> B7
 *	
 *	PA4 <-> VSYNC	PC6 <-> HSYNC	PF10 <-> DE
 *	PG7 <-> CLK
 *	
 *	GPIOA: 3, 4, 6, 11, 12
 *	GPIOB: 0, 1, 8, 9, 10, 11
 *	GPIOC: 6, 7
 *	GPIOD: 3
 *	GPIOF: 10
 *	GPIOG: 6, 7, 10, 11, 12 	
 */
	static void GpioInit(GPIO_TypeDef *port, uint32_t mask);
	static uint32_t constexpr Pin(uint32_t pin) {
		return 0x01 << pin;
	}
	static void setClk(uint32_t pllN, uint32_t pllR, uint32_t div);
	static void setParameter(void);
	static void setLayer1(uint32_t bufferAddr);
	void setBkColor(uint32_t bkColor) {
		LTDC->BCCR = bkColor;
	}
	volatile uint16_t (* const GRAM_POINT)[240] = (uint16_t (*)[240]) 0xD0000000;
	
	void Init(void)
	{
		RCC->AHB1ENR |= 0x5F;
		RCC->APB2ENR |= 0x4000000; // Enbale LTDC Clock
	
		// GPIOA: 3, 4, 6, 11, 12
		GpioInit(GPIOA, Pin(3) | Pin(4) | Pin(6) | Pin(11) |
						Pin(12));
		
		// GPIOB: 0, 1, 8, 9, 10, 11
		GpioInit(GPIOB, Pin(0) | Pin(1) | Pin(8) | Pin(9) |
						Pin(10) | Pin(11));
		
		// GPIOC: 6, 7
		GpioInit(GPIOC, Pin(6) | Pin(7));
		
		// GPIOD: 3
		GpioInit(GPIOD, Pin(3));
		
		// GPIOF: 10
		GpioInit(GPIOF, Pin(10));
		
		// GPIOG: 6, 7, 10, 11, 12 	
		GpioInit(GPIOG, Pin(6) | Pin(7) | Pin(10) | Pin(11)|
						Pin(12));

		setClk(192, 2, 16); // CLK = 6MHz
		setParameter();
		setLayer1(0xD0000000);
		LTDC->GCR = 0x01;
		LTDC->SRCR = 0x01;
	}
	
	void setColor(uint32_t color)
	{
		for (uint32_t y = 0; y < 320; y ++) {
			for (uint32_t x = 0; x < 240; x ++) {
				GRAM_POINT[y][x] = color;
			}
		}
	}
	
	void setPixel(uint32_t x0, uint32_t y0, uint32_t color)
	{
		GRAM_POINT[y0][x0] = color;
	}
	
	uint32_t getPixel(uint32_t x0, uint32_t y0)
	{
		return GRAM_POINT[y0][x0];
	}
	
	void drawRect(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint32_t color) 
	{
		uint32_t right = x0 + w;
		uint32_t bottom = y0 + h;
		for (uint32_t y = y0; y < bottom; y ++) {
			for (uint32_t x = x0; x < right; x ++) {
				GRAM_POINT[y][x] = color;
			}
		}		

	}
	
	static constexpr uint32_t Hsync = 10;
	static constexpr uint32_t HBP = 20;
	static constexpr uint32_t HAdr = 240;
	static constexpr uint32_t HFP = 10;
	static constexpr uint32_t Vsync = 2;
	static constexpr uint32_t VBP = 2;
	static constexpr uint32_t VAdr = 320;
	static constexpr uint32_t VFP = 4;
	
	static void setParameter(void)
	{
		LTDC->SSCR = ((Hsync - 1) << 16) |
					 (Vsync - 1);
		LTDC->BPCR = ((Hsync + HBP - 1) << 16) |
					 (Vsync + VBP - 1);
		LTDC->AWCR = ((Hsync + HBP + HAdr - 1) << 16) |
					 (Vsync + VBP + VAdr - 1);
		LTDC->TWCR = ((Hsync + HBP + HAdr + HFP - 1) << 16) |
					 (Vsync + VBP + VAdr + VFP - 1);
		LTDC->GCR = 0x00;
		LTDC->BCCR = 0x00;
	}
	
	static void setLayer1(uint32_t bufferAddr)
	{
		LTDC_Layer1->WHPCR = ((Hsync + HBP + HAdr-1) << 16) |
							 (Hsync + HBP);
		LTDC_Layer1->WVPCR = ((Vsync + VBP + VAdr-1) << 16) |
							 (Vsync + VBP);
		LTDC_Layer1->PFCR = 0x02;
		LTDC_Layer1->CACR = 0xFF;
		LTDC_Layer1->BFCR = 5 | (4 << 8);
		LTDC_Layer1->CFBAR = bufferAddr;
		LTDC_Layer1->CFBLR = (240 * 2 + 3) |
							 (240 * 2 << 16);
		LTDC_Layer1->CFBLNR = 320;
		LTDC_Layer1->CR = 0x01;
	}
	
	/** CLK = 1 * N / R / div 
		49 <= N <= 432
		2 <= R <= 7
		div = 2, 4, 8, 16
	**/
	static void setClk(uint32_t pllN, uint32_t pllR, uint32_t div)
	{
		RCC->CR &= ~(0x01 << 28);
		while (0 != (RCC->CR & (0x01 << 29)));
		RCC->PLLSAICFGR &= ~((7 << 28) | (0x1FF << 6));
		RCC->PLLSAICFGR |= (pllN << 6) | (pllR << 28);
		RCC->DCKCFGR &= ~(2 << 16);
		switch (div) {
		
			case 2: break;
			case 4: RCC->DCKCFGR |= 1 << 16; break;
			case 8: RCC->DCKCFGR |= 2 << 16; break;
			case 16: RCC->DCKCFGR |= 3 << 16; break;
		}
		RCC->CR |= 0x01 << 28;
		while (0 == (RCC->CR & (0X01 << 29)));
	}
	
	static void GpioInit(GPIO_TypeDef *port, uint32_t pinMask)
	{
		uint32_t mask;
		
		for (uint32_t i = 0; i < 16; i ++) {
		
			mask = Pin(i);
			if (0 != (mask & pinMask)) {
			
				port->MODER &= ~(0x03 << 2 * i);
				port->MODER |= 2 << 2 * i;
				port->PUPDR &= ~(0x03 << 2 * i);
				port->OSPEEDR |= 0x03 << 2 * i;
				port->OTYPER &= ~(0x01 << i);
				if (i < 8) {
					port->AFR[0] &= ~(0x0F << 4 * i);
					port->AFR[0] |= 0x0E << 4 * i;
				} else {
					port->AFR[1] &= ~(0x0F << 4 * (i - 8));
					port->AFR[1] |= 0x0E << 4 * (i - 8);
				}
			}
		}
	}
	
	
	

}












#include "stm32f4xx.h"

#define LCD_SLEEP_OUT            0x11   /* Sleep out register */
#define LCD_GAMMA                0x26   /* Gamma register */
#define LCD_DISPLAY_OFF          0x28   /* Display off register */
#define LCD_DISPLAY_ON           0x29   /* Display on register */
#define LCD_COLUMN_ADDR          0x2A   /* Colomn address register */ 
#define LCD_PAGE_ADDR            0x2B   /* Page address register */ 
#define LCD_GRAM                 0x2C   /* GRAM register */   
#define LCD_MAC                  0x36   /* Memory Access Control register*/
#define LCD_PIXEL_FORMAT         0x3A   /* Pixel Format register */
#define LCD_WDB                  0x51   /* Write Brightness Display register */
#define LCD_WCD                  0x53   /* Write Control Display register*/
#define LCD_RGB_INTERFACE        0xB0   /* RGB Interface Signal Control */
#define LCD_FRC                  0xB1   /* Frame Rate Control register */
#define LCD_BPC                  0xB5   /* Blanking Porch Control register*/
#define LCD_DFC                  0xB6   /* Display Function Control register*/
#define LCD_POWER1               0xC0   /* Power Control 1 register */
#define LCD_POWER2               0xC1   /* Power Control 2 register */
#define LCD_VCOM1                0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2                0xC7   /* VCOM Control 2 register */
#define LCD_POWERA               0xCB   /* Power control A register */
#define LCD_POWERB               0xCF   /* Power control B register */
#define LCD_PGAMMA               0xE0   /* Positive Gamma Correction register*/
#define LCD_NGAMMA               0xE1   /* Negative Gamma Correction register*/
#define LCD_DTCA                 0xE8   /* Driver timing control A */
#define LCD_DTCB                 0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ            0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define LCD_INTERFACE            0xF6   /* Interface control register */
#define LCD_PRC                  0xF7   /* Pump ratio control register */


#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

void delayMs(uint32_t ms)
{
	uint32_t oldValue = SysTick->VAL;
	uint32_t newValue = 0;
	const uint32_t tickValue = ms * 22500;
	uint32_t cntValue = 0;
	
	while (true) {
	
		newValue = SysTick->VAL;
		if (newValue != oldValue) {
			
			if (newValue < oldValue) {
				
				cntValue += oldValue - newValue;
			} else {
				
				cntValue += SysTick->LOAD + oldValue - newValue;
			}
			
			if (cntValue >= tickValue) {
				return;
			}
			oldValue = newValue;
		}
	}
}


namespace SDRAM {

	static void GpioInit(void);
	void Init(void);
	volatile uint16_t *const StartAddr = (volatile uint16_t *) 0xD0000000;
	volatile uint16_t *const EndAddr = (volatile uint16_t *) (0xD0000000 + 1024 * 1024 * 8);
}

namespace LCD {

	void Init();
	void Clear(uint16_t color);
}


int main(void)
{
	/**
	 * PG13 -> LED3 -> GND
	 * PG14 -> LED4 -> GND
	 */
	SysTick->LOAD = 22500 - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x01;
	
	RCC->AHB1ENR |= 0x40;
	uint32_t tmp = GPIOG->MODER;
	tmp &= ~0x3C000000;
	tmp |= 0x14000000;
	GPIOG->MODER = tmp;
	tmp = GPIOG->OSPEEDR;
	tmp |= 0x3C000000;
	GPIOG->OSPEEDR = tmp;
	tmp = GPIOG->PUPDR;
	tmp &= ~0x3C000000;
	tmp |= 0x14000000;
	GPIOG->PUPDR = tmp;
	tmp = GPIOG->OTYPER;
	tmp &= ~0x6000;
	GPIOG->OTYPER = tmp;
	GPIOG->BSRRH = 0x6000;

	SDRAM::Init();
	LCD::Init();
	LCD::Clear(LCD_COLOR_RED);
	volatile uint16_t (*pg)[240] = (uint16_t (*)[240]) 0xD0000000;
	for (uint32_t x = 0; x < 100; x ++) {
		pg[x][100] = 0x07FF;
	}
	while (true) {
	
		GPIOG->ODR ^= 0x6000;
		delayMs(50);
	} 
}


namespace LCD {

	/** LTDC 引脚定义
	 * PC10 <-> R2		PA6 <-> G2		PD6 <-> B2
	 * PB0 <-> R3		PG10 <-> G3		PG11 <-> B3
	 * PA11 <-> R4		PB10 <-> G4		PG12 <-> B4
	 * PA12 <-> R5		PB11 <-> G5		PA3 <-> B5
	 * PB1 <-> R6		PC7 <-> G6		PB8 <-> B6
	 * PG6 <-> R7		PD3 <-> G7		PB9 <-> B7
	 * 
	 * PA4 <-> VSYNC
	 * PC6 <-> HSYNC
	 * PF10 <-> DE
	 * PG7 <-> CLK
	 *
	 * GPIOA 3, 4, 6, 11, 12
	 * GPIOB 0, 1, 8, 9, 10, 11
	 * GPIOC 6, 7, 10
	 * GPIOD 3, 6,
	 * GPIOF 10
	 * GPIOG 6, 7, 10, 11, 12 
	 ** SPI5 引脚定义
	 * PC2 <-> CSX
	 * PD13 <-> WR/DC
	 * PF7 <-> SPI5_SCK
	 * PF9 <-> SPI5_MOSI
	 */
	 
	 inline void ChipSelect(void)
	 {
		GPIOC->BSRRH = 0x04;
	 }
	 
	 inline void ChipNoSelect(void)
	 {
		GPIOC->BSRRL = 0x04;
	 }
	 
	 inline void SelectCommand(void)
	 {
		GPIOD->BSRRH = 0x2000;
	 }
	 
	 inline void SeleceData(void)
	 {
		GPIOD->BSRRL = 0x2000;
	 }
	 
	 void Spi5WriteByte(uint8_t byte)
	 {
		SPI5->DR = byte;
		while (0 == (SPI5->SR & 0x02));
		while (0 != (SPI5->SR & 0x80));
	 }
	 
	 void LcdWriteData(uint8_t value)
	 {
		 SeleceData();
		 ChipSelect();
		 Spi5WriteByte(value);
		 ChipNoSelect();
	 }
	 
	 void LcdWriteCommand(uint8_t value)
	 {
		 SelectCommand();
		 ChipSelect();
		 Spi5WriteByte(value);
		 ChipNoSelect();
	 }
	 
	 void PowerOn(void)
	 {
		LcdWriteCommand(0xCA);
		LcdWriteData(0xC3);
		LcdWriteData(0x08);
		LcdWriteData(0x50);
		LcdWriteCommand(LCD_POWERB);
		LcdWriteData(0x00);
		LcdWriteData(0xC1);
		LcdWriteData(0x30);
		LcdWriteCommand(LCD_POWER_SEQ);
		LcdWriteData(0x64);
		LcdWriteData(0x03);
		LcdWriteData(0x12);
		LcdWriteData(0x81);
		LcdWriteCommand(LCD_DTCA);
		LcdWriteData(0x85);
		LcdWriteData(0x00);
		LcdWriteData(0x78);
		LcdWriteCommand(LCD_POWERA);
		LcdWriteData(0x39);
		LcdWriteData(0x2C);
		LcdWriteData(0x00);
		LcdWriteData(0x34);
		LcdWriteData(0x02);
		LcdWriteCommand(LCD_PRC);
		LcdWriteData(0x20);
		LcdWriteCommand(LCD_DTCB);
		LcdWriteData(0x00);
		LcdWriteData(0x00);
		LcdWriteCommand(LCD_FRC);
		LcdWriteData(0x00);
		LcdWriteData(0x1B);
		LcdWriteCommand(LCD_DFC);
		LcdWriteData(0x0A);
		LcdWriteData(0xA2);
		LcdWriteCommand(LCD_POWER1);
		LcdWriteData(0x10);
		LcdWriteCommand(LCD_POWER2);
		LcdWriteData(0x10);
		LcdWriteCommand(LCD_VCOM1);
		LcdWriteData(0x45);
		LcdWriteData(0x15);
		LcdWriteCommand(LCD_VCOM2);
		LcdWriteData(0x90);
		LcdWriteCommand(LCD_MAC);
		LcdWriteData(0xC8);
		LcdWriteCommand(LCD_3GAMMA_EN);
		LcdWriteData(0x00);
		LcdWriteCommand(LCD_RGB_INTERFACE);
		LcdWriteData(0xC2);
		LcdWriteCommand(LCD_DFC);
		LcdWriteData(0x0A);
		LcdWriteData(0xA7);
		LcdWriteData(0x27);
		LcdWriteData(0x04);

		  /* colomn address set */
		LcdWriteCommand(LCD_COLUMN_ADDR);
		LcdWriteData(0x00);
		LcdWriteData(0x00);
		LcdWriteData(0x00);
		LcdWriteData(0xEF);
		  /* Page Address Set */
		LcdWriteCommand(LCD_PAGE_ADDR);
		LcdWriteData(0x00);
		LcdWriteData(0x00);
		LcdWriteData(0x01);
		LcdWriteData(0x3F);
		LcdWriteCommand(LCD_INTERFACE);
		LcdWriteData(0x01);
		LcdWriteData(0x00);
		LcdWriteData(0x06);
		  
		LcdWriteCommand(LCD_GRAM);
		for (volatile uint32_t i = 0; i < 200; i ++);
		  
		LcdWriteCommand(LCD_GAMMA);
		LcdWriteData(0x01);
		  
		LcdWriteCommand(LCD_PGAMMA);
		LcdWriteData(0x0F);
		LcdWriteData(0x29);
		LcdWriteData(0x24);
		LcdWriteData(0x0C);
		LcdWriteData(0x0E);
		LcdWriteData(0x09);
		LcdWriteData(0x4E);
		LcdWriteData(0x78);
		LcdWriteData(0x3C);
		LcdWriteData(0x09);
		LcdWriteData(0x13);
		LcdWriteData(0x05);
		LcdWriteData(0x17);
		LcdWriteData(0x11);
		LcdWriteData(0x00);
		LcdWriteCommand(LCD_NGAMMA);
		LcdWriteData(0x00);
		LcdWriteData(0x16);
		LcdWriteData(0x1B);
		LcdWriteData(0x04);
		LcdWriteData(0x11);
		LcdWriteData(0x07);
		LcdWriteData(0x31);
		LcdWriteData(0x33);
		LcdWriteData(0x42);
		LcdWriteData(0x05);
		LcdWriteData(0x0C);
		LcdWriteData(0x0A);
		LcdWriteData(0x28);
		LcdWriteData(0x2F);
		LcdWriteData(0x0F);
		  
		LcdWriteCommand(LCD_SLEEP_OUT);
		for (volatile uint32_t i = 0; i < 200; i ++);
		LcdWriteCommand(LCD_DISPLAY_ON);
		  /* GRAM start writing */
		LcdWriteCommand(LCD_GRAM);
	 }
	 
	 void Init(void)
	 {
		auto LtdcGpioInit = [] {
		
			RCC->AHB1ENR |= 0x7F;
			
			// GPIOA 3, 4, 6, 11, 12
			GPIOA->MODER &= ~0x3C033C0;
			GPIOA->MODER |= 0x2802280;
			GPIOA->PUPDR &= ~0x3C033C0;
			GPIOA->OSPEEDR |= 0x3C033C0;
			GPIOA->OTYPER &= ~0x1858;
			GPIOA->AFR[0] &= ~0x0F0FF000;
			GPIOA->AFR[0] |= 0x0E0EE000;
			GPIOA->AFR[1] &= ~0x000FF000;
			GPIOA->AFR[1] |= 0x000EE000;
			
			// GPIOB 0, 1, 8, 9, 10, 11
			GPIOB->MODER &= ~0x00FF000F;
			GPIOB->MODER |= 0x00AA000A;
			GPIOB->PUPDR &= ~0x00FF000F;
			GPIOB->OSPEEDR |= 0x00FF000F;
			GPIOB->OTYPER &= ~0x0F03;
			GPIOB->AFR[0] &= ~0xFF;
			GPIOB->AFR[0] |= 0xEE;
			GPIOB->AFR[1] &= ~0xFFFF;
			GPIOB->AFR[1] |= 0xEEEE;
			
			// GPIOC 6, 7, 10
			GPIOC->MODER &= ~0x30F000;
			GPIOC->MODER |= 0x20A000;
			GPIOC->PUPDR &= ~0x30F000;
			GPIOC->OSPEEDR |= 0x30F000;
			GPIOC->OTYPER &= ~0x4C0;
			GPIOC->AFR[0] &= ~0xFF000000;
			GPIOC->AFR[0] |= 0xEE000000;
			GPIOC->AFR[1] &= ~0xF00;
			GPIOC->AFR[1] |= 0xE00;
			
			// GPIOD 3, 6,
			GPIOD->MODER &= ~0x30C0;
			GPIOD->MODER |= 0x2080;
			GPIOD->PUPDR &= ~0x30C0;
			GPIOD->OSPEEDR |= 0x30C0;
			GPIOD->OTYPER &= ~0x48;
			GPIOD->AFR[0] &= ~0x0F00F000;
			GPIOD->AFR[0] |= 0x0E00E000;
			
			// GPIOF 10
			GPIOF->MODER &= ~0x300000;
			GPIOF->MODER |= 0x200000;
			GPIOF->PUPDR &= ~0x300000;
			GPIOF->OSPEEDR |= 0x300000;
			GPIOF->OTYPER &= ~0x400;
			GPIOF->AFR[1] &= ~0xF00;
			GPIOF->AFR[1] |= 0xE00;
			
			// GPIOG 6, 7, 10, 11, 12 
			GPIOG->MODER &= ~0x3F0F000;
			GPIOG->MODER |= 0x2A0A000;
			GPIOG->PUPDR &= ~0x3F0F000;
			GPIOG->OSPEEDR |= 0x3F0F000;
			GPIOG->OTYPER &= ~0x1CC0;
			GPIOG->AFR[0] &= ~0xFF000000;
			GPIOG->AFR[0] |= 0xEE000000;
			GPIOG->AFR[1] &= ~0xFFF00;
			GPIOG->AFR[1] |= 0xEEE00;
			
		};
		
		auto Spi5GpioInit = [] {
		
		   /** PC2 <-> CSX
			* PD13 <-> WR/DC
			* PF7 <-> SPI5_SCK
			* PF9 <-> SPI5_MOSI
			*/
			RCC->AHB1ENR |= 0x2C;
			RCC->APB2ENR |= 0x100000; // Enable SPI5 Clock
			
			GPIOC->MODER &= ~0x30;
			GPIOC->MODER |= 0x10;
			GPIOC->PUPDR &= ~0x30;
			GPIOC->OSPEEDR |= 0x30;
			GPIOC->OTYPER &= ~0x04;
			
			/** Disable CS */
			GPIOC->BSRRL = 0x04;
			
			GPIOD->MODER &= ~0x0C000000;
			GPIOD->MODER |= 0x04000000;
			GPIOD->PUPDR &= ~0x0C000000;
			GPIOD->OSPEEDR |= 0x0C000000;
			GPIOD->OTYPER &= ~0x2000;
			
			GPIOF->MODER &= ~0xCC000;
			GPIOF->MODER |= 0x88000;
			GPIOF->PUPDR &= ~0xCC000;
			GPIOF->OSPEEDR |= 0xCC000;
			GPIOF->OTYPER &= ~0x280;
			GPIOF->AFR[0] &= ~0xF0000000;
			GPIOF->AFR[0] |= 0x50000000;
			GPIOF->AFR[1] &= ~0xF0;
			GPIOF->AFR[1] |= 0x50;
			
			SPI5->I2SCFGR = 0x00;
			SPI5->CR2 = 0x00;
			SPI5->CR1 = 0x364;
		};
		
		auto LtdcInit = [] {
			
			const uint32_t Hsync = 10; // 水平同步宽度
			const uint32_t HBP = 20; // 水平后沿宽度
			const uint32_t HAdr = 240; // 有效宽度
			const uint32_t HFP = 10; // 水平前沿
			const uint32_t Vsync = 2;
			const uint32_t VBP = 2;
			const uint32_t VAdr = 320;
			const uint32_t VFP = 4;
		
			RCC->APB2ENR |= 0x04000000; // Enable LTDC Clock
			
			RCC->CR &= ~(1 << 28);
			while (0 != (RCC->CR & (1 << 29)));
			RCC->PLLSAICFGR &= ~((7 << 28) | (0x1FF << 6));
			RCC->PLLSAICFGR |= (5 << 28) | (60 << 6);
			RCC->DCKCFGR &= ~(3 << 16);
			RCC->CR |= 1 << 28;
			while (0 == (RCC->CR & (1 << 29)));
			
			LTDC->SSCR = ((Hsync - 1) << 16) | (Vsync - 1);
			LTDC->BPCR = ( (Hsync + HBP - 1) << 16 ) | ( Vsync + VBP - 1 );
			LTDC->AWCR = ( (Hsync + HBP + HAdr - 1) << 16 ) |
						 (Vsync + VBP + VAdr - 1);
			LTDC->TWCR = ( (Hsync + HBP + HAdr + HFP - 1) << 16 ) |
						 (Vsync + VBP + VAdr + VFP - 1);
			LTDC->GCR = 0x0000;
			LTDC_Layer1->WHPCR = ( (Hsync + HBP + HAdr) << 16 ) | 
									(Hsync + HBP);
			LTDC_Layer1->WVPCR = ( (Vsync + VBP + VAdr) << 16 ) |
									(Vsync + VBP);
			LTDC_Layer1->PFCR = 0x02; // RGB565
			LTDC_Layer1->CACR = 240;
			LTDC_Layer1->CFBAR = (uint32_t) SDRAM::StartAddr;
			LTDC_Layer1->CFBLR = (240 * 2 + 3) | ( (240 * 2) << 16);
			LTDC_Layer1->CFBLNR = 160;
			LTDC_Layer1->BFCR = 0x07 | (0x04 << 8);
			
			LTDC_Layer1->CR = 0x01;
			LTDC->GCR |= 0x01;
			LTDC->SRCR = 0x01;
		};
		
		LtdcGpioInit();
		Spi5GpioInit();
		PowerOn();
		LtdcInit();
	 }
	 
	 void Clear(uint16_t color)
	 {
		volatile uint16_t (* GramAddr)[240] = (volatile uint16_t (*)[240]) SDRAM::StartAddr;
		
		for (uint32_t y = 0; y < 320; y ++) {
		
			for (uint32_t x = 0; x < 240; x ++) {
			
				GramAddr[y][x] = color;
			}
		}
	 }		 
}

namespace SDRAM {

	/**
	 * PF0 <-> A0		PD14 <-> D0
	 * PF1 <-> A1		PD15 <-> D1
	 * PF2 <-> A2		PD0 <-> D2
	 * PF3 <-> A3 		PD1 <-> D3
	 * PF4 <-> A4 		PE7 <-> D4
	 * PF5 <-> A5		PE8 <-> D5
	 * PF12 <-> A6		PE9 <-> D6
	 * PF13 <-> A7		PE10 <-> D7
	 * PF14 <-> A8		PE11 <-> D8
	 * PF15 <-> A9		PE12 <-> D9
	 * PG0 <-> A10		PE13 <-> D10
	 * PG1 <-> A11		PE14 <-> D11
	 * 					PE15 <-> D12
	 * PB5 <-> SDCKE1	PD8 <-> D13
	 * PB6 <-> SDNE1  	PD9 <-> D14
	 * PC0 <-> SDNWE	PD10 <-> D15
	 * PE0 <-> NBL0
	 * PE1 <-> NBL1
	 * PF11 <-> SDNRAS
	 * PG4 <-> BA0
	 * PG5 <-> BA1
	 * PG8 <-> SDCLK
	 * PG15 <-> SDNCAS
	 * @description PB: 5, 6, 
	 *				PC: 0,
	 *				PD: 0, 1, 8, 9, 10, 14, 15 
	 *				PE:	0, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15
	 *				PF: 0, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15,
	 *				PG: 0, 1, 4, 5, 8, 15,
	 */
	static void GpioInit(void) 
	{
		RCC->AHB1ENR |= 0x7E;
		
		// PB: 5, 6, 
		GPIOB->MODER &= ~0x3C00;
		GPIOB->MODER |= 0x2800;
		GPIOB->PUPDR &= ~0x3C00;
		GPIOB->OSPEEDR |= 0x3C00;
		GPIOB->OTYPER &= ~0x60;
		GPIOB->AFR[0] &= ~0xFF00000;
		GPIOB->AFR[0] |= 0xCC00000;
		
		// PC: 0,
		GPIOC->MODER &= ~0x03;
		GPIOC->MODER |= 0x02;
		GPIOC->PUPDR &= ~0x03;
		GPIOC->OSPEEDR |= 0x03;
		GPIOC->OTYPER &= ~0x01;
		GPIOC->AFR[0] &= ~0x0F;
		GPIOC->AFR[0] |= 0x0C;
		
		// PD: 0, 1, 8, 9, 10, 14, 15 
		GPIOD->MODER &= ~0xF03F000F;
		GPIOD->MODER |= 0xA02A000A;
		GPIOD->PUPDR &= ~0xF03F000F;
		GPIOD->OSPEEDR |= 0xF03F000F;
		GPIOD->OTYPER &= ~0xC703;
		GPIOD->AFR[0] &= ~0xFF;
		GPIOD->AFR[0] |= 0xCC;
		GPIOD->AFR[1] &= ~0xFF000FFF;
		GPIOD->AFR[1] |= 0xCC000CCC;
		
		// PE:	0, 1, 7, 8, 9, 10, 11, 12, 13, 14, 15
		GPIOE->MODER &= ~0xFFFFC00F;
		GPIOE->MODER |= 0xAAAA800A;
		GPIOE->PUPDR &= ~0xFFFFC00F;
		GPIOE->OSPEEDR |= 0xFFFFC00F;
		GPIOE->OTYPER &= ~0xFF83;
		GPIOE->AFR[0] &= ~0xF00000FF;
		GPIOE->AFR[0] |= 0xC00000CC;
		GPIOE->AFR[1] = 0xCCCCCCCC;
		
		// PF: 0, 1, 2, 3, 4, 5, 11, 12, 13, 14, 15,
		GPIOF->MODER &= ~0xFFC00FFF;
		GPIOF->MODER |= 0xAA800AAA;
		GPIOF->PUPDR &= ~0xFFC00FFF;
		GPIOF->OSPEEDR |= 0xFFC00FFF;
		GPIOF->OTYPER &= ~0xF81F;
		GPIOF->AFR[0] &= ~0x00FFFFFF;
		GPIOF->AFR[0] |= 0x00CCCCCC;
		GPIOF->AFR[1] &= ~0xFFFFF000;
		GPIOF->AFR[1] |= 0xCCCCC000;
		
		// PG: 0, 1, 4, 5, 8, 15,
		GPIOG->MODER &= ~0xC0030F0F;
		GPIOG->MODER |= 0x80020A0A;
		GPIOG->PUPDR &= ~0xC0030F0F;
		GPIOG->OSPEEDR |= 0xC0030F0F;
		GPIOG->OTYPER &= ~0x8133;
		GPIOG->AFR[0] &= ~0x00FF00FF;
		GPIOG->AFR[0] |= 0x00CC00CC;
		GPIOG->AFR[1] &= ~0xF000000F;
		GPIOG->AFR[1] |= 0xC000000C;
	}
	
	/**
	 * tMRD = 2 cycle
	 * tXSR = 70 ns
	 * tRAS = 42 ns
	 * tRC = 63 ns
	 * tWR = 2 CLK
	 * tRP = 15 ns
	 * tRCD = 15 ns
	 * Clock t = 1 / 90 = 11.1 ns
	 */
	void Init(void) 
	{
		GpioInit();
		RCC->AHB3ENR |= 0x01;
		
		FMC_Bank5_6->SDCR[1] = 0x00 | // 8位列地址
							   (0x01 << 2) | // 12位行地址
							   (0x01 << 4) | // 16位
							   (0x01 << 6) | // 4 bank
							   (0x02 << 7) | // CAS Latency = 2
							   (0x00 << 9) | // 允许写访问
							   (0x02 << 10) | // 时钟频率 = 180 / 2 = 90
							   (0x01 << 12) | // 读突发
							   (0x00 << 13);
		
		FMC_Bank5_6->SDCR[0] = (0x00 << 13) |
							   (0x00 << 12) |
							   (0x02 << 10);
		
		FMC_Bank5_6->SDTR[1] = 0x01 | // tMRD = 2 cycle
							   (0x06 << 4) | // tXSR = 11.1 * 7 ns
							   (0x03 << 8) | // tRAS = 11.1 * 4 ns
							   (0x05 << 12) | // tRC = 11.1 * 6 ns
							   (0x01 << 16) | // tWR = 2 CLK
							   (0x01 << 20) | // tRP = 11.1 * 2 ns
							   (0x01 << 24); // tRCD = 11.1 * 2 ns
		
		FMC_Bank5_6->SDTR[0] = (0x01 << 20) |
							   (0x05 << 12);
		
		auto SendCmd = [] ( uint8_t cmd, 
							uint8_t refersh, 
							uint16_t value ) -> uint8_t {
			FMC_Bank5_6->SDCMR = (cmd << 0) |
								 (1 << 3) |
								 (refersh << 5) |
								 (value << 9);
			uint32_t timeout = 0;
			while (FMC_Bank5_6->SDSR & 0x20) {
			
				timeout ++;
				if (timeout > 0x1FFFF) {
					return 1;
				}
			}
			return 0;
		};
		
		/** 时钟配置使能 **/
		SendCmd(1, 0, 0);
		for (volatile uint32_t index = 0; 
				index < 1000; 
				index ++);
		
		/** 预充电 **/
		SendCmd(2, 0, 0);
		
		/** 设置自刷新次数 **/
		SendCmd(3, 1, 0);
		
		/** SDRAM内部寄存器位定义
		 *  A9: 1
		 * (A8, A7): 0
		 * (A6, A5, A4): 2
		 *  A3: 0 
		 * (A2, A1, A0): 3 
		 */
		SendCmd(4, 0, 0x223);
		
		/**
		 * COUNT = 64 * 1000 * 90 / 4096 - 20 = 1386
		 */
		FMC_Bank5_6->SDRTR = 1386 << 1;
		
		for (volatile uint32_t index = 0; 
			index < 1000; 
			index ++);
	}
}






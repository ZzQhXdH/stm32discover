#include "Periph/Spi.h"

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


namespace Spi5 {
	
	static inline void ChipSelect(void) {
		GPIOC->BSRRH = 0x04;
	}
	
	static inline void ChipNoSelect(void) {
		GPIOC->BSRRL = 0x04;
	}
	
	static inline void SelectCommand(void) {
		GPIOD->BSRRH = 0x2000;
	}
	
	static inline void SelectData(void) {
		GPIOD->BSRRL = 0x2000;
	}
	
	void PowerOn(void);
/**
 *	PC2 <-> CSX
 *	PD13 <-> DCX
 *	PF9 <-> SPI5_MOSI
 *	PF7 <-> SPI5_SCK
 */	
	void Init(void)
	{
		RCC->AHB1ENR |= 0x04 | 0x08 | 0x20;
		RCC->APB2ENR |= 0x100000;
		
		GPIOC->MODER &= ~0x30;
		GPIOC->MODER |= 0x10;
		GPIOC->OTYPER &= ~0x04;
		GPIOC->PUPDR &= ~0x30;
		GPIOC->OSPEEDR |= 0x30;
		GPIOC->BSRRL = 0x04;
		
		GPIOD->MODER &= ~0xC000000;
		GPIOD->MODER |= 0x4000000;
		GPIOD->PUPDR &= ~0xC000000;
		GPIOD->OSPEEDR |= 0xC000000;
		GPIOD->OTYPER &= ~0x2000;
		
		GPIOF->MODER &= ~0xCC000;
		GPIOF->MODER |= 0x88000;
		GPIOF->OTYPER &= ~0x280;
		GPIOF->OSPEEDR |= 0xCC000;
		GPIOF->PUPDR &= ~0xCC000;
		GPIOF->AFR[0] &= ~0xF0000000;
		GPIOF->AFR[0] |= 0x50000000;
		GPIOF->AFR[1] &= ~0xF0;
		GPIOF->AFR[1] |= 0x50;
		
		SPI5->CR1 = 0x00;
		SPI5->CR2 = 0x00;
		SPI5->I2SCFGR = 0x00;
		SPI5->CR1 = 0x344;

		PowerOn();
	}
	
	void WriteByte(uint8_t byte) {
		
		SPI5->DR = byte;
		while (0 == (SPI5->SR & 0x02));
		while (0 != (SPI5->SR & 0x80));
	}
	
	void LcdWriteCommand(uint8_t command) {
		
		SelectCommand();
		ChipSelect();
		WriteByte(command);
		ChipNoSelect();
	}
	
	void LcdWriteData(uint8_t data) {
		
		SelectData();
		ChipSelect();
		WriteByte(data);
		ChipNoSelect();
	}
	
	void PowerOn(void) {
	
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

}










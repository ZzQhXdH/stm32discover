#ifndef		__LTDC_H
#define		__LTDC_H

#include "stm32f4xx.h"

namespace Ltdc {

	void Init(void);
	void setColor(uint32_t color);
	void setPixel(uint32_t x0, uint32_t y0, uint32_t color);
	void drawRect(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint32_t color);
	uint32_t getPixel(uint32_t x0, uint32_t y0);
}




#endif

#ifndef		__LCD_H
#define		__LCD_H

#include "Periph/LTDC.h"

namespace Lcd {

	extern uint32_t mCurX0;
	extern uint32_t mCurY0;
	void drawText(char c, uint32_t x0, uint32_t y0);
}






#endif

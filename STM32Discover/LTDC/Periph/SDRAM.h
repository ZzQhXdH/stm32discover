#ifndef		__SD_RAM_H
#define		__SD_RAM_H

#include "stm32f4xx.h"

namespace SDRAM {

	void Init(void);
	constexpr uint32_t StartAddr = 0xD0000000;
	constexpr uint32_t Size = 1024 * 1024 * 8;
}




#endif

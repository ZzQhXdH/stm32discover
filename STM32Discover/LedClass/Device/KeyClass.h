#ifndef		__KEY_CLASS_H
#define		__KEY_CLASS_H

#include "Periph/GpioClass.h"

class KeyClass:private GpioClass {

public:
	
	enum Status {
	
		Press = 0x01,
		Release = 0x00,
	};

	KeyClass(const GpioClass &io):
		GpioClass(io)
	{
		
	}
	
	KeyClass(GpioTypeDef *port, Pin pin, PuPd pupd = Up):
		GpioClass(port, pin, Input, Pp, pupd)
	{
		
	}
	
	Status getStatus(void) {
		
		if (!read()) {
			return Press;
		} else {
			return Release;
		}
	}
	

private:
	
};








#endif

#ifndef		__LED_CLASS_H
#define		__LED_CLASS_H

#include "Periph/GpioClass.h"

class LedClass:private GpioClass {
	
public:
	enum Status {
		
		Off = 0x00,
		On = 0x01,
	};
	
	LedClass(GpioClass &io):
		GpioClass(io)
	{
		
	}
	
	LedClass(GpioTypeDef *port, Pin pin):
		GpioClass(port, pin, Output, Pp)
	{
		
	}
	
	LedClass(GpioTypeDef *port, Pin pin, Status sta):
		GpioClass(port, pin, Output, Pp)
	{
		setStatus(sta);
	}
	
	inline Status getStatus(void) {
		if (isSet()) {
			return On;
		} else {
			return Off;
		}
	}
	
	inline void setStatus(Status sta) {
		if (sta == On) {
			set();
		} else {
			reset();
		}
	}
	
	inline void open(void) {
		set();
	}
	
	inline void close(void) {
		reset();
	}
	
	inline void toggle(void) {
		if (getStatus()) {
			reset();
		} else {
			set();
		}
	}
	
private:
	
};







#endif

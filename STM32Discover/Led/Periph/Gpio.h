#ifndef		__GPIO_H
#define		__GPIO_H

#include "stm32f4xx.h"

namespace Gpio {
	
	typedef int GpioPin;
	
	const GpioPin Pin0 = 0x01;
	const GpioPin Pin1 = 0x02;
	const GpioPin Pin2 = 0x04;
	const GpioPin Pin3 = 0x08;
	const GpioPin Pin4 = 0x10;
	const GpioPin Pin5 = 0x20;
	const GpioPin Pin6 = 0x40;
	const GpioPin Pin7 = 0x80;
	const GpioPin Pin8 = 0x100;
	const GpioPin Pin9 = 0x200;
	const GpioPin Pin10 = 0x400;
	const GpioPin Pin11 = 0x800;
	const GpioPin Pin12 = 0x1000;
	const GpioPin Pin13 = 0x2000;
	const GpioPin Pin14 = 0x4000;
	const GpioPin Pin15 = 0x8000;
	
	enum GpioPinEnum {
		
		Pin_0 = 0x01,
		Pin_1 = 0x02,
		Pin_2 = 0x04,
		Pin_3 = 0x08,
		
		Pin_4 = 0x10,
		Pin_5 = 0x20,
		Pin_6 = 0x40,
		Pin_7 = 0x80,
		
		Pin_8 = 0x100,
		Pin_9 = 0x200,
		Pin_10 = 0x400,
		Pin_11 = 0x800,
		
		Pin_12 = 0x1000,
		Pin_13 = 0x2000,
		Pin_14 = 0x4000,
		Pin_15 = 0x8000,
	};
	
	enum GpioMode {
		
		In = 0x00,
		Out = 0X01,
		Af = 0x02,
		Analog = 0x03,
	};
	
	enum GpioPupd {
		
		Float = 0x00,
		Up = 0x01,
		Down = 0x02,
	};
	
	enum GpioSpeed {
		
		Speed_2MHz = 0x00,
		Speed_25MHz = 0x01,
		Speed_50MHz = 0x02,
		Speed_100MHz = 0x03,
	};
	
	enum GpioOutputType {
	
		Pp = 0x00,
		Od = 0x01,
	};
	
	enum GpioAf {
		
		AF0 = 0x00,
		AF1 = 0x01,
		AF2 = 0x02,
		AF3 = 0x03,
		AF4 = 0x04,
		AF5 = 0x05,
		AF6 = 0x06,
		AF7 = 0x07,
		AF8 = 0x08,
		AF9 = 0x09,
		AF10 = 0x0A,
		AF11 = 0x0B,
		AF12 = 0x0C,
		AF13 = 0x0D,
		AF14 = 0x0E,
		AF15 = 0x0F,
	};
	
	inline void set(GPIO_TypeDef *port, GpioPin pin) {
		port->BSRRL |= pin;
	}
	
	inline void reset(GPIO_TypeDef *port, GpioPin pin) {
		port->BSRRH |= pin;
	}
	
	inline bool isSet(GPIO_TypeDef *port, GpioPin pin) {
		if ((port->ODR & pin) != 0) {
			return true;
		} else {
			return false;
		}
	}
	
	inline bool read(GPIO_TypeDef *port, GpioPin pin) {
		if ((port->IDR & pin) != 0) {
			return true;
		} else {
			return false;
		}
	}
	
	inline GPIO_TypeDef *operator<<(GPIO_TypeDef *port, GpioPinEnum pin) {
		set(port, pin);
		return port;
	}
	
	void GpInit(GPIO_TypeDef *port, 
				GpioPin pin, 
				GpioMode mode = Out,
				GpioPupd pupd = Up, 
				GpioOutputType type = Pp,
				GpioSpeed speed = Speed_100MHz);
	void setMode(GPIO_TypeDef *port, GpioPin pin, GpioMode mode);
	void setOutputType(GPIO_TypeDef *port, GpioPin pin, GpioOutputType type);
	void setInType(GPIO_TypeDef *port, GpioPin pin, GpioPupd pupd);
	void setAf(GPIO_TypeDef *port, GpioPin pin, GpioAf af);
}





#endif

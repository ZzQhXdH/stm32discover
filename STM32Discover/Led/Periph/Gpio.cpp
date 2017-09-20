#include "Periph/Gpio.h"


namespace Gpio {
	
	void setAf(GPIO_TypeDef *port, GpioPin pin, GpioAf af) 
	{
		uint32_t highMask = port->AFR[1];
		uint32_t lowMask = port->AFR[0];
		for (uint32_t i = 0; i < 8; i ++) {
			if (pin & (0x01 << i)) {
				lowMask &= ~(0x0F << i * 4);
				lowMask |= af << (4 * i);
			}
			if (pin & (0x100 << i)) {
				highMask &= ~(0x0F << i * 4);
				highMask |= af << (4 * i);
			}
		}
		port->AFR[0] = lowMask;
		port->AFR[1] = highMask;
	}
	
	void setInType(GPIO_TypeDef *port, GpioPin pin, GpioPupd pupd) 
	{
		uint32_t mask = port->MODER;
		for (uint32_t i = 0; i < 16; i ++) {
			if (pin & (0x01 << i)) {
				mask &= ~(0x03 << i * 2);
				mask |= pupd << i * 2;
			}
		}
		port->MODER = mask;
	}
	
	void setOutputType(GPIO_TypeDef *port, GpioPin pin, GpioOutputType type)
	{
		uint32_t mask = port->OTYPER;
		for (uint32_t i = 0; i < 16; i ++) {
			if (pin & (0x01 << i)) {
				mask &= ~(1 << i);
				mask |= type << i;
			}
		}
	}
	
	void setMode(GPIO_TypeDef *port, GpioPin pin, GpioMode mode) 
	{
		uint32_t mask = port->MODER;
		for (uint32_t i = 0; i < 16; i ++) {
			if (pin & (0x01 << i)) {
				mask &= ~(0x03 << i * 2);
				mask |= mode << i * 2;
			}
		}
		port->MODER = mask;
	}
	
	void GpInit(GPIO_TypeDef *port, 
				GpioPin pin, 
				GpioMode mode,
				GpioPupd pupd, 
				GpioOutputType type,
				GpioSpeed speed) 
	{
		uint32_t speedMask = port->OSPEEDR;
		uint32_t pupdMask = port->PUPDR;
		uint32_t modeMask = port->MODER;
		uint32_t typeMask = port->OTYPER;
		for (uint32_t i = 0; i < 16; i ++) {
			if (pin & (0x01 << i)) {
				speedMask &= ~(3 << i);
				speedMask |= (speed << i * 2);
				
				pupdMask &= ~(3 << i * 2);
				pupdMask |= (pupd << i * 2);
				
				modeMask &= ~(3 << i * 2);
				modeMask |= (mode << i * 2);
				
				typeMask &= ~(1 << i);
				typeMask |= (type << i);
			}
		}
		port->MODER = modeMask;
		if ((mode == Out) || (mode == Af)) {
		
			port->OSPEEDR = speedMask;
			port->OTYPER = typeMask;
		}
		port->PUPDR = pupdMask;
		
	}
}












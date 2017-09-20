#include "Device/LedClass.h"
#include "Device/KeyClass.h"
#include "Periph/SysClockClass.h"
#include "Periph/SerialPortClass.h"

int main(void)
{	
	SysClock::setAHB1Periph(GpioA);
	SysClock::setAHB1Periph(GpioG);
	SysClock::setAHB1Periph(GpioB);
	SysClock::setAPB2Periph(Usart1);
	
	LedClass led1(GPIOG, Pin13);
	LedClass led2(GPIOG, Pin14);
	led1.open();
	led2.open();

	
	KeyClass key(GPIOA, Pin0, GpioClass::Float);
	
	GpioClass tx(GPIOB, Pin6, GpioClass::Af7);
	GpioClass rx(GPIOB, Pin7, GpioClass::Af7);
	SerialPortClass port(USART1, tx, rx);
	port.open(SerialPortClass::WriteRead).write("Hello Word\r\n");
	
	while (true) {
	
		if (key.getStatus() == KeyClass::Press) {
			led1.close();
		//	port.write("button press\r\n");
		} else {
			led1.open();
		}

	}
}













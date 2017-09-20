#ifndef		__SERIAL_PORT_H
#define		__SERIAL_PORT_H

#include "Periph/GpioClass.h"

typedef USART_TypeDef SerialPort;

class SerialPortClass {

public:
	enum Parity {
		
		NoParity = 0x00,
		EvenParity = 0x400,
		OddParity = 0x600,
	};
	
	enum FlowControl {
		
		NoFlowControl = 0x00,
		FlowControl_CTS = 0x200,
		FlowControl_RTS = 0x100,
		FlowControl_CTS_RTS = 0x300,
	};
	
	enum DataBits {
		
		Data8 = 0x00,
		Data9 = 0x1000,
	};
	
	enum Baudrate {
		
		Baudrate_9600 = 9600,
		Baudrate_115200 = 115200,
	};
	
	enum StopBits {
		
		OneStopBits = 0x00,
		_0_5_StopBits = 0x1000,
		TwoStopBits = 0x2000,
		_1_5_StopBits = 0x3000,
	};
	
	enum Direction {
	
		WriteOnly = 0x08 | 0x2000,
		ReadOnly = 0x04 | 0x2000,
		WriteRead = 0x0C | 0x2000,
	};
	
	SerialPortClass(SerialPort *port, GpioClass &tx, GpioClass &rx):
		mTxPin(tx), 
		mRxPin(rx), 
		mPort(port),
		mParity(NoParity),
		mBaudrate(Baudrate_115200),
		mFlowControl(NoFlowControl),
		mDataBits(Data8),
		mStopBits(OneStopBits)
	{
		mPort->CR1 = 0x00;
		mPort->CR2 = mStopBits;
		mPort->CR3 = mFlowControl;
		mPort->BRR = 90000000 / mBaudrate;
		mPort->CR1 = mParity | mDataBits;
	}
	
	SerialPortClass(SerialPort *port, GpioTypeDef *tPort, Pin tPin, GpioTypeDef *rPort, Pin rPin):
		mTxPin(tPort, tPin, GpioClass::Af7), 
		mRxPin(rPort, rPin, GpioClass::Af7), 
		mPort(port),
		mParity(NoParity),
		mBaudrate(Baudrate_115200),
		mFlowControl(NoFlowControl),
		mDataBits(Data8),
		mStopBits(OneStopBits)
	{
		mPort->CR1 = 0x00;
		mPort->CR2 = mStopBits;
		mPort->CR3 = mFlowControl;
		mPort->BRR = 90000000 / mBaudrate;
		mPort->CR1 = mParity| mDataBits;
	}
	
	SerialPortClass(SerialPort *port,
					GpioClass &tx,
					GpioClass &rx,
					Baudrate baudrate,
					DataBits dataBits = Data8,
					Parity parity = NoParity,
					StopBits stopBits = OneStopBits,
					FlowControl flowControl = NoFlowControl):
		mTxPin(tx), mRxPin(rx), mPort(port), mParity(NoParity), 
		mBaudrate(baudrate), mFlowControl(flowControl), 
		mDataBits(dataBits), mStopBits(stopBits)
	{
		mPort->CR1 = 0x00;
		mPort->CR2 = mStopBits;
		mPort->CR3 = mFlowControl;
		mPort->BRR = 90000000 / mBaudrate;
		mPort->CR1 = mParity | mDataBits;
	}
	
	inline SerialPortClass &setParity(Parity parity) {
		
		mPort->CR1 &= ~0x600;
		mPort->CR1 |= parity;
		return *this;
	}
	
	inline SerialPortClass &setStopBits(StopBits stopBits) {
	
		mPort->CR2 &= ~0x3000;
		mPort->CR2 |= stopBits;
		return *this;
	}
	
	inline SerialPortClass &setFlowControl(FlowControl flowControl) {
	
		mPort->CR3 &= ~0x300;
		mPort->CR3 |= flowControl;
		return *this;
	}
	
	inline SerialPortClass &setBaudrate(Baudrate baudrate) {
		
		mPort->BRR = 90000000 / baudrate;
		return *this;
	}
	
	inline SerialPortClass &setDataBits(DataBits dataBits) {
	
		mPort->CR1 &= ~0x1000;
		mPort->CR1 |= dataBits;
		return *this;
	}
	
	inline SerialPortClass &open(Direction dir) {
	
		mPort->CR1 &= ~WriteRead;
		mPort->CR1 |= dir;
		return *this;
	}
	
	inline SerialPortClass &close(void) {
	
		mPort->CR1 = 0x00;
		return *this;
	}
	
	inline SerialPortClass &write(uint8_t byte) {
	
		while (0 == (mPort->SR & 0x80));
		mPort->DR = byte;
		return *this;
	}
	
	SerialPortClass &write(const uint8_t *bytes, uint32_t length = 0) {
	
		if (length == 0) {
			while (*bytes != '\0') {
			
				write(*bytes);
				bytes ++;
			}
			return *this;
		}
		
		while (length --) {
		
			write(*bytes);
			bytes ++;
		}
		
		return *this;
	}
	
	SerialPortClass &write(const char *string) {
	
		while (*string != '\0') {
			write((uint8_t) *string ++);
		}
		return *this;
	}
	
	
private:
	GpioClass mTxPin;
	GpioClass mRxPin;
	SerialPort *mPort;
	Parity mParity;
	Baudrate mBaudrate;
	FlowControl mFlowControl;
	DataBits mDataBits;
	StopBits mStopBits;
};






#endif

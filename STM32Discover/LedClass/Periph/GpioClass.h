#ifndef		__GPIO_CLASS_H
#define		__GPIO_CLASS_H


#include "stm32f4xx.h"

typedef GPIO_TypeDef GpioTypeDef;

enum Pin {
		
	Pin0 = 0x00,
	Pin1 = 0x01,
	Pin2 = 0x02,
	Pin3 = 0x03,
	Pin4 = 0x04,
	Pin5 = 0x05,
	Pin6 = 0x06,
	Pin7 = 0x07,
	Pin8 = 0x08,
	Pin9 = 0x09,
	Pin10 = 0x0A,
	Pin11 = 0x0B,
	Pin12 = 0x0C,
	Pin13 = 0x0D,
	Pin14 = 0x0E,
	Pin15 = 0x0F,
};

class GpioClass {

public:
		
	enum Mode {
		
		Input = 0x00,
		Output = 0x01,
		Af = 0x02,
		Analog = 0x03,
	};
		
	enum PuPd {
		
		Float = 0x00,
		Up = 0x01,
		Down = 0x02,
	};
		
	enum OutputType {
		
		Pp = 0x00,
		Od = 0x01,
	};
		
	enum OutputSpeed {
		
		Speed_2MHz = 0x00,
		Speed_25MHz = 0x01,
		Speed_50MHz = 0x02,
		Speed_100MHz = 0x03,
	};
		
	enum AfIo {
	
		Af0 = 0x00,
		Af1 = 0x01,
		Af2 = 0x02,
		Af3 = 0x03,
		Af4 = 0x04,
		Af5 = 0x05,
		Af6 = 0x06,
		Af7 = 0x07,
			
		Af8 = 0x08,
		Af9 = 0x09,
		Af10 = 0x0A,
		Af11 = 0x0B,
		Af12 = 0x0C,
		Af13 = 0x0D,
		Af14 = 0x0E,
		Af15 = 0x0F,
	};
		
	inline GpioClass(GpioTypeDef *port, Pin pin): 
		mPort(port), mPin(pin), mMode(Input), mType(Pp), mPupd(Float), mSpeed(Speed_2MHz), mIsSet(false)
	{
		
	}
		
	GpioClass(GpioTypeDef *port, Pin pin, 
			  Mode mode,  
			  OutputType type = Pp,
			  PuPd pupd = Up,
			  OutputSpeed speed = Speed_100MHz):
		mPort(port), mPin(pin), mMode(mode), mType(type), mPupd(pupd), mSpeed(speed), mIsSet(false)
	{
		port->MODER &= ~(3 << 2 * pin);
		port->MODER |= mode << 2 * pin;
			
		port->PUPDR &= ~(3 << 2 * pin);
		port->PUPDR |= pupd << 2 * pin;
		
		if (mode == Input) {
		
			return;
		}
		
		port->OTYPER &= ~(1 << pin);
		port->OTYPER |= type << pin;
			
		port->OSPEEDR &= ~(3 << 2 * pin);
		port->OSPEEDR |= speed << 2 * pin;
	}
		
	GpioClass(GpioTypeDef *port, Pin pin, 
			  AfIo af,
			  Mode mode = Af, 
			  PuPd pupd = Up, 
			  OutputType type = Pp, 
			  OutputSpeed speed = Speed_100MHz):
		mPort(port), mPin(pin), mMode(mode), mType(type), mPupd(pupd), mSpeed(speed), mAf(af), mIsSet(false)
	{
		port->MODER &= ~(3 << 2 * pin);
		port->MODER |= mode << 2 * pin;
			
		port->PUPDR &= ~(3 << 2 * pin);
		port->PUPDR |= pupd << 2 * pin;
			
		port->OTYPER &= ~(1 << pin);
		port->OTYPER |= type << pin;
			
		port->OSPEEDR &= ~(3 << 2 * pin);
		port->OSPEEDR |= speed << 2 * pin;
			
		if (pin >= 8) {
			port->AFR[1] &= ~(0x0F << (pin - 8) * 4);
			port->AFR[1] |= af << (pin - 8) * 4;
		} else {
			port->AFR[0] &= ~(0x0F << pin * 4);
			port->AFR[0] |= af << pin * 4;
		}
	}
	
	inline GpioClass(const GpioClass &other):
		mPort(other.mPort),
		mPin(other.mPin),
		mMode(other.mMode),
		mType(other.mType),
		mPupd(other.mPupd),
		mSpeed(other.mSpeed),
		mAf(other.mAf),
		mIsSet(other.mIsSet)
	{
		
	}
	
	inline void setMode(Mode mode) {
		mPort->MODER &= ~(3 << mPin * 2);
		mPort->MODER |= mode << mPin * 2;
		mMode = mode;
	}
		
	inline void setSpeed(OutputSpeed speed) {
		mPort->OSPEEDR &= ~(3 << mPin * 2);
		mPort->OSPEEDR |= speed << mPin * 2;
		mSpeed = speed;
	}
		
	inline void setOutputType(OutputType type) {
		mPort->OTYPER &= ~(1 << mPin);
		mPort->OTYPER |= type << mPin;
		mType = type;
	}
		
	inline void setPupd(PuPd pupd) {
		mPort->PUPDR &= ~(3 << mPin * 2);
		mPort->PUPDR |= pupd << mPin * 2;
		mPupd = pupd;
	}
		
	inline void setAfIo(AfIo af) {
		if (mPin >= 8) {
			mPort->AFR[1] &= ~(0x0F << (mPin - 8) * 4);
			mPort->AFR[1] |= af << (mPin - 8) * 4;
		} else {
			mPort->AFR[0] &= ~0x0F << mPin * 4;
			mPort->AFR[0] |= af << mPin * 4;
		}
		mAf = af;
	}
		
	inline void set(void) {
		mPort->BSRRL = 0x01 << mPin;
		mIsSet = true;
	}
		
	inline void reset(void) {
		mPort->BSRRH = 0x01 << mPin;
		mIsSet = false;
	}
		
	inline bool isSet(void) const {
		return mIsSet;
	}
	
	inline bool read(void) const {
		if (mPort->IDR & (1 << mPin)) {
			return true;
		} else {
			return false;
		}
	}
		
	inline GpioClass &operator <<(Mode mode) {
		setMode(mode);
		return *this;
	}
		
	inline GpioClass &operator <<(PuPd pupd) {
		setPupd(pupd);
		return *this;
	}
		
	inline GpioClass &operator <<(OutputType type) {
		setOutputType(type);
		return *this;
	}
		
	inline GpioClass &operator <<(AfIo af) {
		setAfIo(af);
		return *this;
	}
		
	inline GpioClass &operator <<(OutputSpeed speed) {
		setSpeed(speed);
		return *this;
	}
		
	inline GpioClass &operator <<(bool is) {
		if (is) {
			set();
		} else {
			reset();
		}
		return *this;
	}
	
private:
	GpioTypeDef * const mPort;
	const Pin mPin;
	Mode mMode;
	OutputType mType;
	PuPd mPupd;
	OutputSpeed mSpeed;
	AfIo mAf;
	bool mIsSet;
	
	/**
	 * @brief 将operator =函数设置成私有 禁止对象进行赋值操作
	 */
	GpioClass operator =(const GpioClass &other) { 
		GpioClass io(other.mPort, other.mPin);
		return io;
	}
};









#endif













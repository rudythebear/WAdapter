#ifndef W_SWITCH_H_
#define W_SWITCH_H_

#include "WPin.h"

#define MODE_BUTTON 0
#define MODE_BUTTON_LONG_PRESS 1
#define MODE_SWITCH 2
#define SWITCH_PRESSED_PEGEL HIGH

const long SWITCH_SENSITIVENESS = 20;


class WSwitch: public WPin {
public:
	typedef std::function<void()> THandlerFunction;
	WSwitch(int switchPin, byte mode)
	: WPin(switchPin, INPUT) {
		startTime = 0;
		_pressed = false;
		_pressedLong = false;
		triggerProperty = nullptr;
		this->mode = mode;
		longPressDuration = 5000;
		switchChangeDuration = 1000;
		if (this->isInitialized()) {
			state = digitalRead(this->getPin());
			if (state == SWITCH_PRESSED_PEGEL) {
				_pressed = true;
			}
		}

	}
	void loop(unsigned long now) {
		if (this->isInitialized()) {
			bool currentState = digitalRead(this->getPin());
			if (triggerProperty != nullptr) {
				triggerProperty->setBoolean(false);
			}
			if (currentState == SWITCH_PRESSED_PEGEL) { // buttons has been pressed
				// starting timer. used for switch sensitiveness
				if (startTime == 0) {
					startTime = now;
				}
				if (now - startTime >= SWITCH_SENSITIVENESS) {
					// switch pressed, sensitiveness taken into account
					if (!_pressed) {
						// This is set only once when switch is pressed
						state = !state;
						_pressed = true;
						if ((this->mode == MODE_BUTTON) || (this->mode == MODE_SWITCH)) {
							//log("Switch pressed short. pin:" + String(this->getPin()));
							if (triggerProperty != nullptr) {
								triggerProperty->setBoolean(true);
							}
							if (getProperty() != nullptr) {
								getProperty()->setBoolean(!getProperty()->getBoolean());
							}
							notify();
						}
					}
					if (this->mode == MODE_BUTTON_LONG_PRESS) {
						if (now - startTime >= longPressDuration && !_pressedLong) {
							_pressedLong = true;
							//log("Switch pressed long. pin:" + String(this->getPin()));
							notify();
						}
					}
				}
			} else if ((currentState == (!SWITCH_PRESSED_PEGEL)) && (startTime > 0)) {
				if ((_pressed) && (!_pressedLong) &&
					((this->mode == MODE_BUTTON_LONG_PRESS) || ((this->mode == MODE_SWITCH) && (now - startTime >= switchChangeDuration)))) {
					//log("Switch pressed short. pin:" + String(this->getPin()));
					if (triggerProperty != nullptr) {
						triggerProperty->setBoolean(true);
					}
					if (getProperty() != nullptr) {
						getProperty()->setBoolean(!getProperty()->getBoolean());
					}
					//notify(false);
				}
				startTime = 0;
				_pressedLong = false;
				_pressed = false;
			}
		}
	}

	void setOnPressed(THandlerFunction onPressed) {
		this->onPressed = onPressed;
	}

	void setTriggerProperty(WProperty* triggerProperty) {
		if (this->triggerProperty != triggerProperty) {
			this->triggerProperty = triggerProperty;
		}
	}

private:
	THandlerFunction onPressed;
	byte mode;
	int longPressDuration, switchChangeDuration;
	bool state;
	unsigned long startTime;
	bool _pressed;
	bool _pressedLong;
	WProperty* triggerProperty;

	void notify() {
		if (onPressed) {
			onPressed();
		}
 	}
};

#endif

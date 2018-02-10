/*
 * keyboard.h
 *
 *  Created on: Feb 10, 2018
 *      Author: dipsy
 */

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#include <cassert>
#include <cstring>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <libsc/led.h>
#include <vector>

#include <string>
#include "libsc/k60/touchscreen_lcd.h"

using libsc::System;
using std::string;
using libsc::k60::TouchScreenLcd;

class Keyboard{
public:
	Keyboard(TouchScreenLcd* pLcd):pLcd(pLcd){}

	/**
	 * Showkeyboard and return entered string
	 */
	string ShowKeyboard();

private:
	TouchScreenLcd* pLcd;
	uint16_t ox=1, oy=321,		//offset left, top
			kw = 48, kh = 70;	//key width key height
	char str[35]="";
	uint8_t len=0;
	bool cap_lock = false;
	const string keys[5] = {
			"1234567890",
			"qwertyuiop",
			"asdfghjkl",
			"\tzxcvbnm\b\\",
			",(   ).\n\\\\"
	};
	const uint8_t pl[5] = {0,0,24,0,0};

	void RenderKeyboard(bool is_upper_case = false);

	/**
	 * get character at this location
	 */
	char GetKey(uint16_t x, uint16_t y);

	char ToUpperCase(const char& c);
};

#endif /* INC_KEYBOARD_H_ */

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
Keyboard(TouchScreenLcd* pLcd):pLcd(pLcd){

}

void RenderKeyboard(bool is_upper_case = false){
	for(int i=0; i<5; i++){
		const string& s = keys[i];
		for(int j=s.size()-1; j>=0;j--){
			char c = s[j];
			uint16_t x = pl[i]+ox+j*kw, y = oy+i*kh;
			pLcd->Fill(x, y, x+kw, y+kh, 0xFFFF);
			pLcd->POINT_COLOR = 0x0000;
			pLcd->BACK_COLOR = 0xFFFF;
			if(c!='\\'){
				pLcd->DrawLine(x,y,x,y+kh);
			}
			pLcd->DrawLine(x,y,x+kw,y);
			pLcd->DrawLine(x,y+kh,x+kw,y+kh);
			switch(c){
			default:
				if(is_upper_case)
					c = ToUpperCase(c);
				pLcd->ShowChar(x+5, y+kh-48,c,48,0);
				break;
			case '\b':
				pLcd->ShowString(x+5, y+kh-48, 400, 48, 48, "<-", 0);
				break;
			case '\t':
				pLcd->ShowString(x+5, y+kh-48, 400, 48, 48, "^", 0);
				break;
			case '\n':
				pLcd->ShowString(x+5, y+kh-48, 400, 48, 48, "Done", 0);
				break;
			case '\\':
				break;
			}

		}
	}
}

string ShowKeyboard(){
	uint8_t sum=0;
	time_t last_tap = 0;
	RenderKeyboard();
	while(1){
		if(pLcd->Scan(0) && pLcd->touch_status!=4 && System::Time()>last_tap+500){
			last_tap = System::Time();
			sum++;
			char key = GetKey(pLcd->touch_x[0],pLcd->touch_y[0]);
			switch(key){
			default:
				if(len<34){
					str[len++] = (cap_lock ? ToUpperCase(key) : key);
					str[len]='\0';
				}
				break;
			case '\b':
				if(len>0){
					str[--len] = '\0';
				}
				break;
			case '\t':
				RenderKeyboard(cap_lock = !cap_lock);
				break;
			case '\n':
				return string(str);
			case 0:
				break;
			}
			pLcd->Fill(0,0,400,100,0x0000);
			pLcd->ShowString(0, 0, 408, 48, 48, str, 0);
			if(len>17)
				pLcd->ShowString(0, 50, 408, 48, 48, str+17, 0);
		}
		pLcd->ShowNum(0,100,len,4,48);
		pLcd->ShowNum(0,150,last_tap,4,48);
		pLcd->ShowNum(0,200,System::Time(),4,48);
		pLcd->ShowNum(0,250,sum,4,48);
	}
}

char GetKey(uint16_t x, uint16_t y){
	uint8_t i = (y-oy)/kh;
	uint8_t j = (x-ox-pl[i])/kw;
	if(i<0||j<0||i>=5||j>=keys[i].size())return 0;
	else return (keys[i][j] == '\\' ? GetKey(x-48,y) : keys[i][j]);
}

char ToUpperCase(const char& c){
	if(c>='a'&&c<='z')return c-'a'+'A';
	else return c;
}

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
};

#endif /* INC_KEYBOARD_H_ */

/*
 * main.cpp
 *
 * Author:
 * Copyright (c) 2014-2015 HKUST SmartCar Team
 * Refer to LICENSE for details
 */

#include <cassert>
#include <cstring>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <libsc/led.h>
#include <vector>

#include "libbase/k60/flash.h"
#include "libsc/k60/touchscreen_lcd.h"
#include "libsc/k60/dk100.h"
#include "libutil/touch_menu.h"

#include "keyboard.h"

namespace libbase
{
	namespace k60
	{

		Mcg::Config Mcg::GetMcgConfig()
		{
			Mcg::Config config;
			config.external_oscillator_khz = 50000;
			config.core_clock_khz = 150000;
			return config;
		}

	}
}

//using libsc::System;
using namespace libsc;
using namespace libbase::k60;
using libsc::k60::Dk100;
using std::vector;
using libsc::k60::TouchScreenLcd;
using libutil::Touch_Menu;



int yo = 0;
int sum=0;

int main(){
	System::Init();
	Led::Config led_config;
//	led_config.id = 2;
//	Led led2(led_config);
//	led_config.id = 3;
//	Led led3(led_config);

	Dk100::Config config;
	config.id = 0;
	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k9600;
	config.rx_isr = [&](const Byte* buff, const size_t& size){
		sum++;
		return true;
	};
	Dk100 nfc(config);

	bool show = false;


	TouchScreenLcd lcd;
	lcd.SetTouchingInterrupt([&](Gpi*,TouchScreenLcd* pLcd){
		yo++;
		show = true;
	});
	Keyboard keyboard(&lcd);
//	Flash::Config flash_config;
//	Flash flash(flash_config);
////	lcd.DisplayOn();
////	lcd.ShowNum(0,0,1234,4,48);
////	lcd.DrawCircle(50,50,50);
//	Touch_Menu menu(&lcd,&flash);
//	uint8_t h=0;
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.AddItem("hi",&h,&menu.main_menu);
//	menu.EnterMenu(&menu.main_menu,0,0,480,800,48);
	keyboard.ShowKeyboard();
	while(1){
//			led2.Switch();
		nfc.SendStr("A");
		lcd.ShowNum(0,0,yo,4,48);
		lcd.ShowNum(0,50,lcd.touch_status,1,48);
		lcd.ShowNum(0,100,System::Time(),4,48);
		lcd.ShowNum(0,150,lcd.Scan(0),1,48);
		lcd.ShowNum(0,200,sum,4,48);
		if(lcd.touch_status!=4){
			lcd.DrawPoint(lcd.touch_x[0],lcd.touch_y[0]);
		}
	}
}


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

using libsc::System;
using namespace libsc;
using namespace libbase::k60;
using libsc::k60::Dk100;
using std::vector;
using libsc::k60::TouchScreenLcd;
using libutil::Touch_Menu;

namespace Sectors{
const Byte
kIdBalance = 0x04,
kName = 0x05,
kChecksum = 0x0D,
kHistory = 0x0E,
kCursor = 0xE0,
kLastTap = 0xE1;
}

Dk100 *pNfc = nullptr;

Byte a=0,b=0,c=0,d=0,know=0;
uint32_t buf_;
int write_success_time = 0, read_success_time = 0;

uint16_t card_id = 0;
int16_t balance = 0;

uint32_t last_tap = 0, checksum = 0;

uint32_t A = 4219, B = 97729, C = 543217;
uint8_t last_sector = 0x0E;

inline uint32_t CalChecksum(uint16_t id, int16_t balance, uint32_t timestamp){
	return (B+(A*((id+balance+timestamp)%C))%C)%C;
}

void OnWrite(const Byte& sector){
	write_success_time++;
}

void OnRead(const Byte& sector, const Byte *data){
	read_success_time++;
	a=data[0];
	b=data[1];
	c=data[2];
	d=data[3];
}

void WriteCard(){

}

bool ReadCard(){
	//card id and balance
	if(!pNfc->SendRead(0x04))return false;
	buf_ = pNfc->GetData();
	memcpy(&card_id, &buf_,2);
	memcpy(&balance, &buf_+2,2);

	//checksum
	pNfc->SendRead(0x0D);
	buf_ = pNfc->GetData();
	memcpy(&checksum, &buf_, 4);

	//last tap timestamp
	buf_ = pNfc->SendRead(0xE1);
	memcpy(&last_tap, &buf_, 4);

	//return if this card is valid
	return true;
}

bool FormatCard(uint16_t id, int16_t balance){
	Byte temp[4];
	memset(temp, 0, 4);

	for(uint8_t i = 0x04; i<=0xE1; i++){
		if(pNfc->SendWrite(i,temp))return false;
	}

	memcpy(temp,&id,2);
	memcpy(temp+2,&balance,2);
	pNfc->SendWrite(0x04,temp);

	uint32_t time = 0;
	memcpy(temp, &time, 4);
	pNfc->SendWrite(Sectors::kLastTap,temp);

	uint32_t cs = CalChecksum(id,balance,time);
	memcpy(temp, &cs, 4);
	pNfc->SendWrite(0x0D, temp);

	uint8_t cursor = 0;
	memset(temp, 0, 4);
	memcpy(temp, &cursor, 1);
	pNfc->SendWrite(Sectors::kCursor, temp);
	return true;
}

int yo = 0;

int main(){
	System::Init();
	Led::Config led_config;
//	led_config.id = 2;
//	Led led2(led_config);
//	led_config.id = 3;
//	Led led3(led_config);

	bool show = false;

	TouchScreenLcd lcd([&](Gpi*,TouchScreenLcd* pLcd){
		yo++;
		show = true;
	});
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
	while(1){
//			led2.Switch();
		lcd.ShowNum(0,0,yo,4,48);
		lcd.ShowNum(0,50,lcd.touch_status,1,48);
		lcd.ShowNum(0,100,System::Time(),4,48);
		lcd.ShowNum(0,150,lcd.Scan(0),1,48);
	}
}

int main_(void)
{
	System::Init();

	//type your code here
	Led::Config led_config;
    led_config.id = 0;
    Led led0(led_config);
    led_config.id = 1;
	Led led1(led_config);
	led_config.id = 2;
	Led led2(led_config);
	led_config.id = 3;
	Led led3(led_config);

	led0.SetEnable(1);
	led1.SetEnable(1);
	led2.SetEnable(1);
	led3.SetEnable(1);

    Dk100::Config config;
    config.id = 0;
    config.baud_rate = libbase::k60::Uart::Config::BaudRate::k9600;
    config.rx_isr = [&](const Byte* buff, const size_t& size){
    	return pNfc->Listener(buff,size);
    };
    Dk100 nfc(config);
    pNfc = &nfc;

//    Byte buf[7] = {0xAA, 0x04, 0x95, 1, 14, 0x02};
//    nfc.SendBuffer(buf,6);

//    Byte buf[8] = {0xAA, 0x06, 0x0A, 0x04, 0x01, 0x01, 0x01, 0x01 };
//    nfc.SendBuffer(buf,8);

//        Byte buf[4] = {0xAA, 0x02, 0x09, 0x04 };
//        nfc.SendBuffer(buf,4);

//    SendRead(0x06);
    Byte to_[4] = {0x01,0x02,0x03,0x04};

    nfc.SetWriteSuccessHandler(OnWrite);
    nfc.SetReadSuccessHandler(OnRead);

//    nfc.SendWrite(0x06,to_);
//    nfc.SendRead(0x06);

    if(ReadCard()){

    }

	while (true){
		led0.Switch();
		System::DelayMs(250);
//		know=a;
//		know=b;
//		know=c;
//		know=d;
	}

	return 0;
}

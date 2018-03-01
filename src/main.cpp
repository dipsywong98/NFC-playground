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

#include "libutil/touch_keyboard.h"
#include "nfc.h"
#include "ui.h"

namespace libbase{
namespace k60{
Mcg::Config Mcg::GetMcgConfig(){
	Mcg::Config config;
	config.external_oscillator_khz = 50000;
	config.core_clock_khz = 150000;
	return config;
}}}

using namespace libsc;
using namespace libbase::k60;
using libsc::k60::Dk100;
using std::vector;
using libsc::k60::TouchScreenLcd;
using libutil::Touch_Menu;
using libutil::TouchKeyboard;

int yo = 0;
int sum=0;

Dk100* pNfc;
uint32_t know = 0;
Byte a=0,b=0,c=0,d=0;

int main(){
	System::Init();

	Dk100::Config config;
	config.id = 0;
	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k9600;
	config.rx_isr = [&](const Byte* buff, const size_t& size){
		sum++;
		return pNfc->Listener(buff,size);
	};
	Dk100 nfc(config);
	pNfc = &nfc;
	nfc.SetReadSuccessHandler([&](const Byte& sector, const Byte *data){
		a=data[0];
		b=data[1];
		c=data[2];
		d=data[3];
	});
	Nfc nfcMgr(&nfc);

	TouchScreenLcd lcd;
	lcd.SetTouchingInterrupt([&](Gpi*,TouchScreenLcd* pLcd){
		yo++;
	});
	TouchKeyboard keyboard(&lcd);

	Flash::Config flash_config;
	Flash flash(flash_config);
	Touch_Menu menu(&lcd,&flash);

	Ui ui(&lcd,&menu,&nfcMgr);
	ui.GoMainMenu();

	Byte x[4] = {0,0,0,0};
	nfc.SendWrite(0x04,x);
	nfc.SendWrite(0x05,x);
	nfc.SendWrite(0x06,x);
	nfc.SendWrite(0x07,x);
	nfc.SendWrite(0x08,x);
	nfc.SendWrite(0x09,x);
	nfc.SendWrite(0x0A,x);

	Byte y[12] = {'A','B','C','D','E','F','G','H','I','J','K','L'};
	Byte z[13] = {};

//	pNfc->SendWrite(0x05,(Byte*)"ABCD");
//	pNfc->SendWrite(0x06,(Byte*)"EFGH");
//	pNfc->SendWrite(0x07,(Byte*)"IJKL");

//	pNfc->SendRead(0x05,z);
//	pNfc->SendRead(0x06,z+4);
//	pNfc->SendRead(0x07,z+8);
//	z[12] = '\0';
//	lcd.ShowString(0,50,480,48,48,(char*)z,0);

//	pNfc->SendRead(0x05);
//	a = pNfc->GetData()[0];
//	b = pNfc->GetData()[1];
//	c = pNfc->GetData()[2];
//	d = pNfc->GetData()[3];
//	lcd.ShowChar(0,0,a,48,0);
//	lcd.ShowChar(30,0,b,48,0);
//	lcd.ShowChar(60,0,c,48,0);
//	lcd.ShowChar(90,0,d,48,0);

//	pNfc->SendRead(0x06);
//	a = pNfc->GetData()[0];
//	b = pNfc->GetData()[1];
//	c = pNfc->GetData()[2];
//	d = pNfc->GetData()[3];
//	lcd.ShowChar(120,0,a,48,0);
//	lcd.ShowChar(150,0,b,48,0);
//	lcd.ShowChar(180,0,c,48,0);
//	lcd.ShowChar(210,0,d,48,0);

//	pNfc->SendRead(0x07);
//	a = pNfc->GetData()[0];
//	b = pNfc->GetData()[1];
//	c = pNfc->GetData()[2];
//	d = pNfc->GetData()[3];
//	lcd.ShowChar(240,0,a,48,0);
//	lcd.ShowChar(270,0,b,48,0);
//	lcd.ShowChar(300,0,c,48,0);
//	lcd.ShowChar(330,0,d,48,0);

//	nfcMgr.SendReadMulti(0x05,z,3);

//	pNfc->SendRead(0x06);
//	a = z[0];
//	b = z[1];
//	c = z[2];
//	d = z[3];
//	lcd.ShowChar(0,50,a,48,0);
//	lcd.ShowChar(30,50,b,48,0);
//	lcd.ShowChar(60,50,c,48,0);
//	lcd.ShowChar(90,50,d,48,0);
//	a = z[4];
//	b = z[5];
//	c = z[6];
//	d = z[7];
//	lcd.ShowChar(120,50,a,48,0);
//	lcd.ShowChar(150,50,b,48,0);
//	lcd.ShowChar(180,50,c,48,0);
//	lcd.ShowChar(210,50,d,48,0);
//	a = z[8];
//	b = z[9];
//	c = z[10];
//	d = z[11];
//	lcd.ShowChar(240,50,a,48,0);
//	lcd.ShowChar(270,50,b,48,0);
//	lcd.ShowChar(300,50,c,48,0);
//	lcd.ShowChar(330,50,d,48,0);
	while(1);

//	Byte buf[4] = {1,2,3,4};
//	pNfc->SendWrite(0x04,buf);
//	pNfc->SendRead(0x04);
//	Byte* y = pNfc->GetData();
//	lcd.ShowNum(0,250,y[0],4,48);
//	lcd.ShowNum(0,300,y[1],4,48);
//	lcd.ShowNum(0,350,y[2],4,48);
//	lcd.ShowNum(0,400,y[3],4,48);
//	memcpy(buf,&know,4);
//	know = know + 0;

	while(1){
//		nfc.SendStr("A");
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


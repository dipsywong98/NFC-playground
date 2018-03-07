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
#include "bluetooth.h"
#include "protocol.h"

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
	config.id = 1;
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
//	ui.GoMainMenu();

//	Bluetooth bt(1,1);
//	bt.QueuePackage({Comm::PkgType::kRequestIp,0,{}});

	Protocol protocol;
	protocol.RequestIp();

	while(1){
		lcd.ShowNum(0,0,protocol.m_bt.GetQueueLength(),1,48);
		char buf[14];
		sprintf(buf,"%d.%d.%d.%d  ",protocol.ip[0],protocol.ip[1],protocol.ip[2],protocol.ip[3]);
		lcd.ShowString(0,50,480,48,48,buf,0);
		lcd.ShowNum(0,100,protocol.m_bt.receive_package_count,4,48);
		lcd.ShowNum(0,150,protocol.m_bt.delete_package_count,4,48);
		lcd.ShowNum(0,700,System::Time()/100%100,2,48);
	}

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


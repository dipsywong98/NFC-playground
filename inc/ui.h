/*
 * ui.h
 *
 *  Created on: Feb 10, 2018
 *      Author: dipsy
 */

#ifndef INC_UI_H_
#define INC_UI_H_

#include <string>
#include <vector>

#include "libbase/k60/flash.h"
#include "libsc/k60/touchscreen_lcd.h"
#include "libsc/k60/dk100.h"
#include "libutil/touch_menu.h"

#include "nfc.h"

using libsc::System;
using namespace libsc;
using namespace libbase::k60;
using libsc::k60::Dk100;
using std::vector;
using libsc::k60::TouchScreenLcd;
using libutil::Touch_Menu;

class Ui{
public:
uint16_t temp_card_id = 0;
int16_t temp_balance = 0;
std::string temp_name = "YOO";
TouchScreenLcd* pLcd;
Touch_Menu* pMenu;
Nfc* pNfcMgr;

bool terminate = false;

Ui(TouchScreenLcd* pLcd, Touch_Menu* pMenu, Nfc* pNfcMgr):
pLcd(pLcd),pMenu(pMenu),pNfcMgr(pNfcMgr)
{

}

void GoMainMenu(){
	//format submenu
	pMenu->AddItem("format card",&pMenu->main_menu);
	libutil::Touch_Menu::Menu* formatMenu = pMenu->main_menu.menu_items[0].sub_menu;
	pMenu->AddItem("id",&temp_card_id,formatMenu);
	pMenu->AddItem("balance",&temp_balance,formatMenu);
	pMenu->AddItem("name",&temp_name,formatMenu);
	pMenu->AddItem("Format",[&](){FormatCardDisplay();},formatMenu);

	//readcard
	pMenu->AddItem("read card",[&](){this->ReadCardDisplay();},&pMenu->main_menu);


	pMenu->EnterMenu(&pMenu->main_menu,0,0,480,800,48);
}

void FormatCardDisplay(){
	terminate = false;
	StartCancelNfcListener();
	if(pNfcMgr->FormatCard(temp_card_id,temp_balance)){
		pLcd->ShowString(0,0,480,48,48,"successfully formatted",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void ReadCardDisplay(){
	terminate = false;
	StartCancelNfcListener();
	if(pNfcMgr->ReadCard()){
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		char buf[20];
		sprintf(buf,"id: %d",pNfcMgr->card_id);
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->balance);
		pLcd->ShowString(0,100,480,48,48,buf,0);
		while(!terminate){
			sprintf(buf,"scan %d",pLcd->Scan(0));
			pLcd->ShowString(0,150,480,48,48,buf,0);
			sprintf(buf,"status %d",pLcd->touch_status);
			pLcd->ShowString(0,200,480,48,48,buf,0);
		}
	} else {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void StartCancelNfcListener(){
	pLcd->ShowString(0,0,480,48,48,"Touch to cancel",0);
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){
		pNfcMgr->Cancel();
		terminate = true;
	});
}

void CancelCancelNfcListener(){
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){});
}

};
#endif /* INC_UI_H_ */

/*
 * ui.cpp
 *
 *  Created on: Mar 2, 2018
 *      Author: dipsy
 */

#include "ui.h"

Ui::Ui(TouchScreenLcd* pLcd, Touch_Menu* pMenu, Nfc* pNfcMgr):
pLcd(pLcd),pMenu(pMenu),pNfcMgr(pNfcMgr)
{

}

void Ui::GoMainMenu(){
	//format submenu
	pMenu->AddItem("format card",&pMenu->main_menu);
	libutil::Touch_Menu::Menu* formatMenu = pMenu->main_menu.menu_items[0].sub_menu;
	pMenu->AddItem("id",&temp_card_id,formatMenu);
	pMenu->AddItem("balance",&temp_balance,formatMenu);
	pMenu->AddItem("name",&temp_name,formatMenu);
	pMenu->AddItem("Format",[&](){FormatCardDisplay();},formatMenu);

	//readcard
	pMenu->AddItem("read card",[&](){this->ReadCardDisplay();},&pMenu->main_menu);

	//clearcard
	pMenu->AddItem("clear card",[&](){this->ClearCardDisplay();},&pMenu->main_menu);

	pMenu->EnterMenu(&pMenu->main_menu,0,0,480,800,48);
}

void Ui::FormatCardDisplay(){
	terminate = false;
	StartCancelNfcListener();
	if(pNfcMgr->FormatCard(temp_card_id,temp_balance,temp_name)){
		pLcd->ShowString(0,0,480,48,48,"successfully formatted",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::ReadCardDisplay(){
	terminate = false;
	StartCancelNfcListener();
	if(pNfcMgr->ReadWholeCard()){
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		char buf[20];
		sprintf(buf,"id: %d",pNfcMgr->m_card_id);
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->m_balance);
		pLcd->ShowString(0,100,480,48,48,buf,0);
		sprintf(buf,"name: %s;",pNfcMgr->m_name);
		pLcd->ShowString(0,150,480,48,48,buf,0);
		while(!terminate){
			pLcd->ShowNum(0,700,System::Time()/100%10,1,48);
		}
	} else {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::ClearCardDisplay(){
	terminate = false;
	StartCancelNfcListener();
	if(pNfcMgr->ClearWholeCard()){
		pLcd->ShowString(0,0,480,48,48,"successfully cleared",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::StartCancelNfcListener(){
	pLcd->ShowString(0,0,480,48,48,"Touch to cancel",0);
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){
		pNfcMgr->Cancel();
		terminate = true;
	});
}

void Ui::CancelCancelNfcListener(){
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){});
}



/*
 * ui.cpp
 *
 *  Created on: Mar 2, 2018
 *      Author: dipsy
 */

#include "ui.h"

Ui::Ui(TouchScreenLcd* pLcd, Touch_Menu* pMenu, Nfc* pNfcMgr, Protocol* pProtocol):
pLcd(pLcd),pMenu(pMenu),pNfcMgr(pNfcMgr),pProtocol(pProtocol)
{

	pProtocol->RequestIp();
	pProtocol->RequestProducts();

	pMenu->AddItem("Product List",&pMenu->main_menu);
	libutil::Touch_Menu::Menu* productMenu = pMenu->main_menu.menu_items[0].sub_menu;

	//format submenu
	pMenu->AddItem("format card",&pMenu->main_menu);
	libutil::Touch_Menu::Menu* formatMenu = pMenu->main_menu.menu_items[1].sub_menu;
	pMenu->AddItem("id",&temp_card_id,formatMenu);
	pMenu->AddItem("balance",&temp_balance,formatMenu);
	pMenu->AddItem("name",&temp_name,formatMenu);
	pMenu->AddItem("Format",[&](){FormatCardDisplay();},formatMenu);

	//readcard
	pMenu->AddItem("read card",[&](){this->ReadCardDisplay();},&pMenu->main_menu);

	//clearcard
	pMenu->AddItem("clear card",[&](){this->ClearCardDisplay();},&pMenu->main_menu);

	pLcd->ShowString(0,0,480,50,48,"crawling data...",0);

	ip = pProtocol->AwaitRequestIp(pLcd);
	pLcd->ShowString(0,50,480,50,48,(char*)("ip"+ip).c_str(),0);

	products = pProtocol->AwaitRequestProducts(pLcd);
	for(const Product& product: products){
		pMenu->AddItem((char*)product.name,[&](){this->PurchaseProductDisplay(product);},(libutil::Touch_Menu::Menu*)productMenu);
	}
}

void Ui::GoMainMenu(){
	pMenu->EnterMenu(&pMenu->main_menu,0,0,480,800,48);
}

void Ui::FormatCardDisplay(){
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

void Ui::PurchaseProductDisplay(const Product& product){
	char message[20] = "tap card buy";
	strcat(message,product.name);
	StartKillAwaitListener(message);
	if(pNfcMgr->ReadCard()){
		const uint16_t card_id = pNfcMgr->m_card_id;
		int16_t balance = pNfcMgr->m_balance;
		uint32_t checksum = pNfcMgr->checksum;
		if(balance>=product.price){
			pProtocol->RequestPurchase(card_id,product.id,product.price,checksum);
			uint32_t time = pProtocol->AwaitRequestPurchase(pLcd);
			if(!terminate){
				balance -= product.price;
				if(pNfcMgr->UpdateBalance(card_id,balance,time)){
					pLcd->ShowString(0,0,480,48,48,"purchase success    ",0);
					char buf[20];
					sprintf(buf,"balance %d    ",balance);
					pLcd->ShowString(0,50,480,48,48,buf,0);
					System::DelayMs(3000);
				} else {
					pLcd->ShowString(0,0,480,48,48,"update balance fail",0);
					System::DelayMs(2000);
				}
			} else {
				pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
				System::DelayMs(1000);
			}
		} else {
			pLcd->ShowString(0,0,480,48,48,"not enough budget  ",0);
			char buf[20];
			sprintf(buf,"required %d",product.price);
			pLcd->ShowString(0,50,480,48,48,buf,0);
			sprintf(buf,"you have %d",balance);
			pLcd->ShowString(0,100,480,48,48,buf,0);
			System::DelayMs(3000);
		}
	} else if(terminate) {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"invalid card  ",0);
		System::DelayMs(2000);
	}
	StopKillAwaitListener();
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

void Ui::StartKillAwaitListener(const string& message){
	terminate = false;
	pLcd->ShowString(0,0,480,48,48,(char*)message.c_str(),0);
	pLcd->ShowString(0,50,480,48,48,"tap screen to cancel",0);
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){
		pNfcMgr->Cancel();
		pProtocol->CancelAwait();
		terminate = true;
	});
}

void Ui::StopKillAwaitListener(){
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){});
}

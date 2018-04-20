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


	//readcard
	pMenu->AddItem("read card history",[&](){this->ReadCardDisplay();},&pMenu->main_menu);
	pMenu->AddItem("read card balance",[&](){this->ReadCardBalanceDisplay();},&pMenu->main_menu);


	//admin tools submenu
	pMenu->AddItem("Admin Tools (Need Auth)", &pMenu->main_menu);
	libutil::Touch_Menu::Menu* adminMenu = pMenu->main_menu.menu_items[3].sub_menu;

	//format submenu
	pMenu->AddItem("format card",adminMenu);
	libutil::Touch_Menu::Menu* formatMenu = adminMenu->menu_items[0].sub_menu;
	pMenu->AddItem("id",&temp_card_id,formatMenu);
	pMenu->AddItem("balance",&temp_balance,formatMenu);
	pMenu->AddItem("name",&temp_name,formatMenu);
	pMenu->AddItem("Format",[&](){FormatCardDisplay();},formatMenu);

	//clearcard
	pMenu->AddItem("clear card",[&](){this->ClearCardDisplay();},adminMenu);

	pLcd->ShowString(0,0,480,50,48,"crawling data...",0);

	ip = pProtocol->AwaitRequestIp(pLcd);
	pLcd->ShowString(0,50,480,50,48,(char*)("ip"+ip).c_str(),0);

	//initialize product list
	products = pProtocol->AwaitRequestProducts(pLcd);
	for(const Product& product: products){
		pMenu->AddItem((char*)product.showText,[&](){this->PurchaseProductDisplay(product);},(libutil::Touch_Menu::Menu*)productMenu);
	}
}

void Ui::GoMainMenu(){
	pMenu->EnterMenu(&pMenu->main_menu,0,0,480,800,48);
}

void Ui::FormatCardDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"format card",0);
	if(pNfcMgr->FormatCard(temp_card_id,temp_balance,temp_name)){
		pLcd->ShowString(0,0,480,48,48,"successfully formatted",0);
		char buf[20];
//		sprintf(buf,"cs: %d",pNfcMgr->checksum);
//		pLcd->ShowString(0,50,480,48,48,buf,0);
//		while(!terminate){
//
//		}
	} else {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::ReadCardDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"read card history",0);
	if(pNfcMgr->ReadWholeCard(pLcd)){
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		char buf[20];
		sprintf(buf,"id: %d",pNfcMgr->m_card_id);
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->m_balance);
		pLcd->ShowString(0,100,480,48,48,buf,0);
		sprintf(buf,"name: %s;",pNfcMgr->m_name);
		pLcd->ShowString(0,150,480,48,48,buf,0);
		sprintf(buf,"last tap: %d;",pNfcMgr->last_tap);
		pLcd->ShowString(0,200,480,48,48,buf,0);
//		sprintf(buf,"cs: %d;",pNfcMgr->checksum);
//		pLcd->ShowString(0,250,480,48,48,buf,0);
//		sprintf(buf,"CS: %d;",pNfcMgr->calChecksum);
//		pLcd->ShowString(0,300,480,48,48,buf,0);
		for(int i = 0; i<pNfcMgr->purchases.size();i++){
			const Purchase& purchase = pNfcMgr->purchases[i];
			sprintf(buf,"%d,$%d:%s",purchase.timestamp,purchase.product.price,purchase.product.name);
			pLcd->ShowString(0,250+i*50,480,48,48,buf,0);
		}
		while(!terminate){
			pLcd->ShowNum(0,750,System::Time()/100%10,1,48);
		}
	} else if(terminate) {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"invalid",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::ReadCardBalanceDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"read card balance",0);
	if(pNfcMgr->ReadCard()){
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		char buf[20];
		sprintf(buf,"id: %d",pNfcMgr->m_card_id);
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->m_balance);
		pLcd->ShowString(0,100,480,48,48,buf,0);
		sprintf(buf,"last tap: %d;",pNfcMgr->last_tap);
		pLcd->ShowString(0,200,480,48,48,buf,0);
		while(!terminate){
			pLcd->ShowNum(0,750,System::Time()/100%10,1,48);
		}
	} else if(terminate) {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"invalid",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::ClearCardDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"clear card",0);
	if(pNfcMgr->ClearWholeCard(pLcd)){
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
					pNfcMgr->AddPurchaseHistory(product,time);
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
	terminate = false;
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

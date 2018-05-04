/*
 * ui.cpp
 *
 *  Created on: Mar 2, 2018
 *      Author: dipsy
 */

#include "ui.h"

Ui::Ui(TouchScreenLcd* pLcd, Touch_Menu* pMenu, Nfc* pNfcMgr, Protocol* pProtocol, Touch_Menu* pSecretMenu):
pLcd(pLcd),pMenu(pMenu),pNfcMgr(pNfcMgr),pProtocol(pProtocol), pSecretMenu(pSecretMenu)
{
	pProtocol->SetResendPeriod(100);
	pProtocol->RequestIp();
	pProtocol->RequestProducts();

	pMenu->AddItem("Product List",&pMenu->main_menu);
	libutil::Touch_Menu::Menu* productMenu = pMenu->main_menu.menu_items[0].sub_menu;


	//readcard
	pMenu->AddItem("read card history",[&](){this->ReadCardDisplay();},&pMenu->main_menu);
	pMenu->AddItem("read card balance",[&](){this->ReadCardBalanceDisplay();},&pMenu->main_menu);


	//admin tools submenu
	pMenu->AddItem("Admin Tools (Need Auth)", [&](){this->Auth();}, &pMenu->main_menu);

	//clearcard
	pSecretMenu->AddItem("clear card",[&](){this->ClearCardDisplay();},&pSecretMenu->main_menu);

	//format submenu
	pMenu->AddItem("format card",&pSecretMenu->main_menu);
	libutil::Touch_Menu::Menu* formatMenu = pSecretMenu->main_menu.menu_items[1].sub_menu;
	pSecretMenu->AddItem("id",&temp_card_id,formatMenu);
	pSecretMenu->AddItem("balance",&add_value_amount,formatMenu);
	pSecretMenu->AddItem("name",&temp_name,formatMenu);
	pSecretMenu->AddItem("Format",[&](){FormatCardDisplay();},formatMenu);

	pSecretMenu->AddItem("Set Add Value", &add_value_amount, &pSecretMenu->main_menu);
	pSecretMenu->AddItem("Add Value", [&](){this->AddValueDisplay();}, &pSecretMenu->main_menu);

	pSecretMenu->AddItem("hack checksum",[&](){this->HackCheckSumDisplay();},&pSecretMenu->main_menu);


	pLcd->ShowString(0,0,480,50,48,"crawling data...",0);

	ip = pProtocol->AwaitRequestIp(pLcd);
	pLcd->ShowString(0,50,480,50,48,(char*)("ip"+ip).c_str(),0);

	//initialize product list
	products = pProtocol->AwaitRequestProducts(pLcd);

	if(pProtocol->products_count!=products.size()){
		std::sort(products.begin(),products.end(),[](const Product& a, const Product& b){return a.id < b.id;});
		for(int i = products.size()-1, j = products.size()-2;j>=0;i--,j--){
			if(products[i].id == products[j].id)products.erase(products.begin()+i);
		}
	}
	for(const Product& product: products){
		pMenu->AddItem((char*)product.showText,[&](){this->PurchaseProductDisplay(product);},(libutil::Touch_Menu::Menu*)productMenu);
	}
	pProtocol->SetResendPeriod(1000);
}

void Ui::GoMainMenu(){
	pMenu->EnterMenu(&pMenu->main_menu,0,0,480,800,48);
}

void Ui::FormatCardDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"format card",0);
	if(pNfcMgr->FormatCard(temp_card_id,0,temp_name)){
		if(add_value_amount!=0){
			AddValueDisplay();
		}
		pLcd->ShowString(0,0,480,48,48,"successfully formatted",0);
		char buf[20];
//		sprintf(buf,"cs: %d",pNfcMgr->GetChecksum());
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
		pLcd->Fill(0,0,480,1000,0x0000);
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		char buf[20];
		char buf2[20];
		sprintf(buf,"id: %d",pNfcMgr->GetCardId());
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->GetBalance());
		pLcd->ShowString(0,100,480,48,48,buf,0);
		sprintf(buf,"name: %s;",pNfcMgr->m_name);
		pLcd->ShowString(0,150,480,48,48,buf,0);
		FormatTime(buf,pNfcMgr->GetLastTap(),12);
		sprintf(buf2,"last buy:%s;",buf);
		pLcd->ShowString(0,200,480,48,48,buf2,0);
		bool page = 0;
		while(!terminate){
			ReadCardPage(page);
			page = !page;
			System::DelayMs(2000);
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

void Ui::ReadCardPage(bool page){
	char buf[20];
	char buf2[20];
	const int size = pNfcMgr->purchases.size();
	for(int i = 0; i<size/2;i++){
		pLcd->DrawLine(0,250+i*100,480,250+i*100);
		const Purchase& purchase = pNfcMgr->purchases[i+size/2*page];
		FormatTime(buf,purchase.timestamp,26);
		pLcd->ShowString(0,252+i*100,480,48,48,buf,0);
		sprintf(buf,"$%d:%s",purchase.product.price,purchase.product.name);
		pLcd->ShowString(0,300+i*100,480,48,48,buf,0);
	}
	pLcd->ShowString(100,750,480,48,48,buf,0);
}

void Ui::ReadCardBalanceDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"read card balance",0);
	if(pNfcMgr->ReadCard()){
		pLcd->Fill(0,0,480,1000,0x0000);
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		char buf[20];
		char buf2[30];
		sprintf(buf,"id: %d",pNfcMgr->GetCardId());
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->GetBalance());
		pLcd->ShowString(0,100,480,48,48,buf,0);
		sprintf(buf,"last buy:");
		pLcd->ShowString(0,200,480,48,48,buf,0);
		FormatTime(buf,pNfcMgr->GetLastTap(),20);
		pLcd->ShowString(0,250,480,48,48,buf,0);
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
		pLcd->Fill(0,0,480,1000,0x0000);
		pLcd->ShowString(0,0,480,48,48,"successfully cleared",0);
		System::DelayMs(1000);
	} else {
		pLcd->Fill(0,0,480,1000,0x0000);
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	}
	CancelCancelNfcListener();
}

void Ui::PurchaseProductDisplay(const Product& product){
	char message[20] = "buy ";
	strcat(message,product.name);
	StartKillAwaitListener(message);
	if(pNfcMgr->ReadCard()){
		const uint16_t card_id = pNfcMgr->GetCardId();
		int16_t balance = pNfcMgr->GetBalance();
		uint32_t checksum = pNfcMgr->GetChecksum();
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

void Ui::AddValueDisplay(){
	Product product(0,"Add Value",add_value_amount);
	char message[20];
	sprintf(message,"Add $%d", add_value_amount);
	StartKillAwaitListener(message);
	if(pNfcMgr->ReadCard()){
		const uint16_t card_id = pNfcMgr->GetCardId();
		int16_t balance = pNfcMgr->GetBalance();
		uint32_t checksum = pNfcMgr->GetChecksum();

		pProtocol->RequestPurchase(card_id,product.id,product.price,checksum);
		uint32_t time = pProtocol->AwaitRequestPurchase(pLcd);
		if(!terminate){
			balance += add_value_amount;
			if(pNfcMgr->UpdateBalance(card_id,balance,time)){
				pNfcMgr->AddPurchaseHistory(product,time);
				pLcd->ShowString(0,0,480,48,48,"add value success    ",0);
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

	} else if(terminate) {
		pLcd->ShowString(0,0,480,48,48,"operation canceled  ",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"invalid card  ",0);
		System::DelayMs(2000);
	}
	StopKillAwaitListener();
}

void Ui::HackCheckSumDisplay(){
	StartCancelNfcListener();
	pLcd->ShowString(0,48,480,48,48,"hack checksum",0);
	uint32_t oldChecksum = pNfcMgr->GetChecksum();
	if(pNfcMgr->HackChecksum()){
		char buf[20];
		char buf2[30];
		pLcd->Fill(0,0,480,1000,0x0000);
		pLcd->ShowString(0,0,480,48,48,"Record (Tap to leave):",0);
		sprintf(buf,"id: %d",pNfcMgr->GetCardId());
		pLcd->ShowString(0,50,480,48,48,buf,0);
		sprintf(buf,"balance: %d",pNfcMgr->GetBalance());
		pLcd->ShowString(0,100,480,48,48,buf,0);
		sprintf(buf,"last buy:");
		pLcd->ShowString(0,200,480,48,48,buf,0);
		FormatTime(buf,pNfcMgr->GetLastTap(),20);
		pLcd->ShowString(0,250,480,48,48,buf,0);
		sprintf(buf,"old %d",oldChecksum);
		pLcd->ShowString(0,300,480,48,48,buf,0);
		sprintf(buf,"new %d",pNfcMgr->GetChecksum());
		pLcd->ShowString(0,350,480,48,48,buf,0);
		while(!terminate){
			pLcd->ShowNum(0,750,System::Time()/100%10,1,48);
		}
	} else if (terminate) {
		pLcd->ShowString(0,0,480,48,48,"Operation Cancelled",0);
		System::DelayMs(1000);
	} else {
		pLcd->ShowString(0,0,480,48,48,"Did not hack checksum",0);
		System::DelayMs(1000);
	}
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
	pLcd->ShowString(0,0,480,48,48,"tap screen to cancel",0);
	pLcd->ShowString(0,50,480,48,48,(char*)message.c_str(),0);
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){
		pNfcMgr->Cancel();
		pProtocol->CancelAwait();
		terminate = true;
	});
}

void Ui::StopKillAwaitListener(){
	pLcd->SetTouchingInterrupt([&](libbase::k60::Gpi*,TouchScreenLcd*){});
}

void Ui::Auth(){
	TouchKeyboard kb(pLcd);
	kb.SetPasswordMode(1);
	if(kb.ShowKeyboard()=="BoyGod Leslie") {
		pSecretMenu->EnterMenu(&pSecretMenu->main_menu,0,0,480,800,48);
	}
}

void Ui::FormatTime(char* buf, time_t t, size_t max_length){
	t+=28800;
	time (&t);
	struct tm * timeinfo = localtime (&t);
	if(max_length>26){
		strftime (buf, max_length, "%d-%b-%Y %H:%M:%S (%a)",timeinfo);
	}
	else if(max_length>20){
		strftime (buf, max_length, "%d-%b-%Y %H:%M:%S",timeinfo);
	} else if (max_length>=12) {
		strftime (buf, max_length, "%d/%m %H:%M",timeinfo);
	} else {
		//max length too short impossible to format time
		assert(0);
	}
}

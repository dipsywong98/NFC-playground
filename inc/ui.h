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
#include <cstring>
#include <ctime>
#include <algorithm>

#include "libbase/k60/flash.h"
#include "libsc/k60/touchscreen_lcd.h"
#include "libsc/k60/dk100.h"
#include "libutil/touch_menu.h"

#include "purchase.h"
#include "product.h"
#include "protocol.h"
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
	int16_t add_value_amount = 0;
	std::string temp_name = "YOO";
	TouchScreenLcd* pLcd;
	Touch_Menu* pMenu;
	Touch_Menu* pSecretMenu;
	Nfc* pNfcMgr;
	Protocol* pProtocol;
	bool clicked_bottom = false;
	uint8_t pkg_id;

	string ip;
	vector<Product> products;

	bool terminate = false;

	Ui(TouchScreenLcd* pLcd, Touch_Menu* pMenu, Nfc* pNfcMgr, Protocol* pProtocol, Touch_Menu* pSecretMenu);

	void GoMainMenu();

	void FormatCardDisplay();

	void ReadCardDisplay();

	void ReadCardPage(bool page);

	void ReadCardBalanceDisplay();

	void ClearCardDisplay();

	void PurchaseProductDisplay(const Product& product);

	void AddValueDisplay();

	void HackCheckSumDisplay();

	void StartCancelNfcListener();

	void CancelCancelNfcListener();

	//cancel Nfc and bluetooth await
	void StartKillAwaitListener(const string& message);

	void StopKillAwaitListener();

	void Auth();

	void FormatTime(char* buf, time_t time, size_t max_length);
};
#endif /* INC_UI_H_ */

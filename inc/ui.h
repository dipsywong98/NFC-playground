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

	Ui(TouchScreenLcd* pLcd, Touch_Menu* pMenu, Nfc* pNfcMgr);

	void GoMainMenu();

	void FormatCardDisplay();

	void ReadCardDisplay();

	void ClearCardDisplay();

	void StartCancelNfcListener();

	void CancelCancelNfcListener();

};
#endif /* INC_UI_H_ */

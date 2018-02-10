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

void GetMainMenu(Touch_Menu* pMenu){
	pMenu->AddItem("format card",[](){FormatCard(0,0);})
}

libutil::Touch_Menu::Menu* FormatCardMenu(){
	libutil::Touch_Menu::Menu form
}


#endif /* INC_UI_H_ */

/*
 * nfc.h
 *
 *  Created on: Feb 10, 2018
 *      Author: dipsy
 */

#ifndef INC_NFC_H_
#define INC_NFC_H_

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

using libsc::k60::Dk100;

/**
 * Class for controlling nfc R/W actions
 */

class Nfc{
public:


	uint16_t card_id = 0;
	int16_t balance = 0;

	uint32_t last_tap = 0, checksum = 0;

	Nfc(Dk100 *pNfc);
	inline uint32_t CalChecksum(uint16_t id, int16_t balance, uint32_t timestamp);

	void OnWrite(const Byte& sector);

	void OnRead(const Byte& sector, const Byte *data);

	bool WriteCard();

	bool ReadCard();

	bool FormatCard(uint16_t id, int16_t balance);

	void Cancel(){
		pNfc->Cancel();
	}

private:

	Dk100 *pNfc = nullptr;

	Byte a=0,b=0,c=0,d=0,know=0;
	Byte* buf_ = nullptr;
	int write_success_time = 0, read_success_time = 0;

	uint32_t A = 4219, B = 97729, C = 543217;
	uint8_t last_sector = 0x0E;

};

#endif /* INC_NFC_H_ */

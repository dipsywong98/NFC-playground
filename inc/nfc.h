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
#include <algorithm>

#include "libbase/k60/flash.h"
#include "libsc/k60/touchscreen_lcd.h"
#include "libsc/k60/dk100.h"
#include "libutil/touch_menu.h"

#include "purchase.h"
#include "product.h"

using libsc::k60::Dk100;

/**
 * Class for controlling nfc R/W actions
 */

class Nfc{
public:


	char m_name[34];
	char temp[4];
	vector<Purchase> purchases;


	Nfc(Dk100 *pNfc);
	inline uint32_t CalChecksum(uint16_t id, int16_t balance, uint32_t timestamp);

	void OnWrite(const Byte& sector);

	void OnRead(const Byte& sector, const Byte *data);

	/**
	 * Send write to multiple sectors
	 * @param {data} data array, length need to be multiple of 4
	 * @param {len} number of sectors
	 * @return true for operation cancelled
	 */
	bool SendWriteMulti(const Byte& sector, const Byte* data, const uint8_t& len);

	/**
	 * Send read to multiple sectors
	 * @param {data} data array to accept returned data
	 * @param {len} number of sectors
	 * @return true for operation cancelled
	 */
	bool SendReadMulti(const Byte& sector, Byte* data, const uint8_t& len);

	/**
	 * Read the essential part of the card
	 * return false when canceled or invalid card
	 */
	bool ReadCard();

	/**
	 * Read all sectors of the card
	 */
	bool ReadWholeCard(TouchScreenLcd* pLcd);

	bool ReadName();

	bool FormatCard(uint16_t id, int16_t balance, std::string& name);

	bool UpdateBalance(uint16_t id, int16_t balance, uint32_t time);

	void Cancel(){
		pNfc->Cancel();
	}

	bool ClearWholeCard(TouchScreenLcd* pLcd);

	bool AddPurchaseHistory(const Product& product, uint32_t time);

	bool ReadPurchaseHistories(TouchScreenLcd* pLcd);

	/**
	 * make the card valid again by making the checksum compatible with the card id, balance and time on card
	 * return: false when didnt hack the checksum, true when hacked the checksum
	 */
	bool HackChecksum();

	uint16_t GetCardId(){return m_card_id;}

	int16_t GetBalance(){return m_balance;}

	uint32_t GetLastTap(){return last_tap;}

	uint32_t GetChecksum(){return checksum;}

	uint32_t GetCalChecksum(){return calChecksum;}

private:

	Dk100 *pNfc = nullptr;

	Byte a=0,b=0,c=0,d=0,know=0;
	Byte* buf_ = nullptr;
	int write_success_time = 0, read_success_time = 0;

	uint32_t A = 4219, B = 97729, C = 543217;
	uint8_t last_sector = 0x0E;

	uint16_t m_card_id = 0;
	int16_t m_balance = 0;
	uint32_t last_tap = 0, checksum = 0, calChecksum = 0;
};

#endif /* INC_NFC_H_ */

/*
 * nfc.cpp
 *
 *  Created on: Feb 10, 2018
 *      Author: dipsy
 */


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

#include "nfc.h"

using libsc::System;
using namespace libsc;
using namespace libbase::k60;
using libsc::k60::Dk100;
using std::vector;
using libsc::k60::TouchScreenLcd;
using libutil::Touch_Menu;

//struct Sectors{
//	static constexpr Byte kIdBalance = 0x04;
//	static constexpr Byte kName = 0x05;
//	static constexpr Byte kChecksum = 0x0D;
//	static constexpr Byte kHistory = 0x0E;
//	static constexpr Byte kCursor = 0xE0;
//	static constexpr Byte kLastTap = 0xE1;
//};

inline uint32_t Nfc::CalChecksum(uint16_t id, int16_t balance, uint32_t timestamp){
	return (B+(A*((id+balance+timestamp)%C))%C)%C;
}

void Nfc::OnWrite(const Byte& sector){
	write_success_time++;
}

void Nfc::OnRead(const Byte& sector, const Byte *data){
	read_success_time++;
	a=data[0];
	b=data[1];
	c=data[2];
	d=data[3];
}

bool Nfc::WriteCard(){
	return false;
}

bool Nfc::ReadCard(){
	//card id and balance
	if(!pNfc->SendRead(0x04))return false;
	buf_ = pNfc->GetData();
	memcpy(&card_id, buf_,2);
	memcpy(&balance, buf_+2,2);

	//checksum
	pNfc->SendRead(0x0D);
	buf_ = pNfc->GetData();
	memcpy(&checksum, buf_, 4);

	//last tap timestamp
	pNfc->SendRead(0xE1);
	buf_ = pNfc->GetData();
	memcpy(&last_tap, buf_, 4);

	//return if this card is valid
	return true;
}

bool Nfc::FormatCard(uint16_t id, int16_t balance){
	Byte temp[4];
	memset(temp, 0, 4);

//	for(uint8_t i = 0x04; i<=0xE1; i++){
//		if(pNfc->SendWrite(i,temp))return false;
//	}

	memcpy(temp,&id,2);
	memcpy(temp+2,&balance,2);
	if(!pNfc->SendWrite(0x04,temp)) return false;

//	uint32_t time = 0;
//	memcpy(temp, &time, 4);
//	pNfc->SendWrite(0xE1,temp);
//
//	uint32_t cs = CalChecksum(id,balance,time);
//	memcpy(temp, &cs, 4);
//	pNfc->SendWrite(0x0D, temp);
//
//	uint8_t cursor = 0;
//	memset(temp, 0, 4);
//	memcpy(temp, &cursor, 1);
//	pNfc->SendWrite(0xE0, temp);
	return true;
}


Nfc::Nfc(Dk100 *pNfc):pNfc(pNfc)
{

}

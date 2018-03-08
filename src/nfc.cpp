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

bool Nfc::SendWriteMulti(const Byte& sector, const Byte* data, const uint8_t& len){
	for(uint8_t i = 0; i<len; i++){
		if(!pNfc->SendWrite(sector+i,data + i*4)){
			return false;
		}
	}

//	memcpy(buf,data,4);
//	pNfc->SendWrite(0x05,buf);
//
//	memcpy(buf,data+4,4);
//	pNfc->SendWrite(0x06,buf);
//
//	memcpy(buf,data+8,4);
//	pNfc->SendWrite(0x07,buf);

	return true;
}

bool Nfc::SendReadMulti(const Byte& sector, Byte* data, const uint8_t& len){
	for(uint8_t i = 0; i<len; i++){
		if(!pNfc->SendRead(sector+i,data + i*4)){
			return false;
		}
	}
//	pNfc->SendRead(0x05);
//	memcpy(data,pNfc->GetData(),4);
//
//	pNfc->SendRead(0x06);
//	memcpy(data+4,pNfc->GetData(),4);
//
//	pNfc->SendRead(0x07);
//	memcpy(data+8,pNfc->GetData(),4);

	return true;
}

bool Nfc::ReadCard(){
	//card id and balance
	if(!pNfc->SendRead(0x04))return false;
	buf_ = pNfc->GetData();
	memcpy(&m_card_id, buf_,2);
	memcpy(&m_balance, buf_+2,2);

	//checksum
	pNfc->SendRead(0x0D);
	buf_ = pNfc->GetData();
	memcpy(&checksum, buf_, 4);

	//last tap timestamp
	pNfc->SendRead(0xE1);
	buf_ = pNfc->GetData();
	memcpy(&last_tap, buf_, 4);

	//return if this card is valid
	return true;//checksum == CalChecksum(m_card_id,m_balance,last_tap);
}

bool Nfc::ReadWholeCard(){
	if(!ReadCard())return false;
	if(!ReadName())return false;
	return true;
}

bool Nfc::ReadName(){
//	pNfc->SendRead(0x05);
//	memcpy(temp,pNfc->GetData(),4);
	if(SendReadMulti(0x05,(Byte*)m_name,8)){
		return true;
	} else {
		return false;
	}
}

bool Nfc::FormatCard(uint16_t id, int16_t balance, std::string& name){
	Byte temp[4];
	memset(temp, 0, 4);

//	for(uint8_t i = 0x04; i<=0xE1; i++){
//		if(!pNfc->SendWrite(i,temp))return false;
//	}

	memcpy(temp,&id,2);
	memcpy(temp+2,&balance,2);
	if(!pNfc->SendWrite(0x04,temp)) return false;

	char name_buf[34]={};
	sprintf(name_buf,"%s",name.c_str());
	if(!SendWriteMulti(0x05,(Byte*) name_buf,8)) return false;

	uint32_t time = 0;
	memcpy(temp, &time, 4);
	pNfc->SendWrite(0xE1,temp);

	uint32_t cs = CalChecksum(id,balance,time);
	memcpy(temp, &cs, 4);
	pNfc->SendWrite(0x0D, temp);

	uint8_t cursor = 0;
	memset(temp, 0, 4);
	memcpy(temp, &cursor, 1);
	pNfc->SendWrite(0xE0, temp);
	return true;
}

bool Nfc::UpdateBalance(uint16_t id,int16_t balance, uint32_t time){
	m_balance = balance;
	last_tap = time;
	Byte temp[4];
	memset(temp, 0, 4);

	memcpy(temp,&id,2);
	memcpy(temp+2,&balance,2);
	if(!pNfc->SendWrite(0x04,temp)) return false;

	memcpy(temp, &time, 4);
	pNfc->SendWrite(0xE1,temp);

	uint32_t cs = CalChecksum(id,balance,time);
	memcpy(temp, &cs, 4);
	pNfc->SendWrite(0x0D, temp);
	return true;
}

bool Nfc::ClearWholeCard(){
	Byte buf[4] = {0,0,0,0};
	for(Byte i = 0x04; i<=0xE1; i++){
		if(!pNfc->SendWrite(i,buf))return false;
	}
	return true;
}

Nfc::Nfc(Dk100 *pNfc):pNfc(pNfc)
{

}

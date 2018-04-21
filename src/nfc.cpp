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
	return true;
}

bool Nfc::SendReadMulti(const Byte& sector, Byte* data, const uint8_t& len){
	for(uint8_t i = 0; i<len; i++){
		if(!pNfc->SendRead(sector+i,data + i*4)){
			return false;
		}
	}
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
	calChecksum =  CalChecksum(m_card_id,m_balance,last_tap);
	return checksum == calChecksum;//checksum == CalChecksum(m_card_id,m_balance,last_tap);
}

bool Nfc::ReadWholeCard(TouchScreenLcd* pLcd){
	if(!ReadCard())return false;
	char buf[20] = "h";
	pLcd->ShowString(0,500,480,50,48,buf,0);
	if(!ReadName())return false;
	strcat(buf,"h");
	pLcd->ShowString(0,500,480,50,48,buf,0);
	if(!ReadPurchaseHistories(pLcd))return false;
//	purchases.clear();
//	pNfc->SendRead(0xE0);
//	uint8_t headsector;
//	Byte buf[45];
//	memcpy(&headsector,pNfc->GetData(),1);
//	SendReadMulti(14+10*headsector,buf,10);
//	Purchase purchase;
//	memcpy(&purchase.timestamp,buf,4);
//	memcpy(&purchase.product.price,buf+4,2);
//	memcpy(&purchase.product.name,buf+8,32);
//	purchases.push_back(purchase);
	return true;
}

bool Nfc::ReadName(){
	if(SendReadMulti(0x05,(Byte*)m_name,8)){
		return true;
	} else {
		return false;
	}
}

bool Nfc::FormatCard(uint16_t id, int16_t balance, std::string& name){
	Byte temp[4];
	memset(temp, 0, 4);

	memcpy(temp,&id,2);
	memcpy(temp+2,&balance,2);
	if(!pNfc->SendWrite(0x04,temp)) return false;

	char name_buf[34]={};
	sprintf(name_buf,"%s",name.c_str());
	if(!SendWriteMulti(0x05,(Byte*) name_buf,8)) return false;

	uint32_t time = 0;
	memcpy(temp, &time, 4);
	pNfc->SendWrite(0xE1,temp);

	checksum = CalChecksum(id,balance,time);
	memcpy(temp, &checksum, 4);
	pNfc->SendWrite(0x0D, temp);

	uint8_t cursor = 9;
	memset(temp, 0, 4);
	memcpy(temp, &cursor, 1);
	pNfc->SendWrite(0xE0, temp);
	return true;
}

bool Nfc::UpdateBalance(uint16_t id,int16_t balance, uint32_t time){
	m_balance = balance;
	last_tap = std::max(time,last_tap);
	Byte temp[4];
	memset(temp, 0, 4);

	memcpy(temp,&id,2);
	memcpy(temp+2,&balance,2);
	if(!pNfc->SendWrite(0x04,temp)) return false;

	memcpy(temp, &time, 4);
	pNfc->SendWrite(0xE1,temp);

	checksum = CalChecksum(id,balance,time);
	memcpy(temp, &checksum, 4);
	pNfc->SendWrite(0x0D, temp);
	return true;
}

bool Nfc::ClearWholeCard(TouchScreenLcd* pLcd){
	Byte buf[4] = {0,0,0,0};
	for(Byte i = 0x04; i<=0xE1; i++){
		if(!pNfc->SendWrite(i,buf))return false;
		char buf[20];
		sprintf(buf,"%d/%d",i-0x04,0xE1-0x04);
		pLcd->ShowString(0,500,480,50,48,buf,0);
	}
	return true;
}

bool Nfc::AddPurchaseHistory(const Product& product, uint32_t time){
	if(!pNfc->SendRead(0xE0)) return false;
	uint8_t cursor = 0;
	memcpy(&cursor, pNfc->GetData(),1);
	Byte buf[40];
	memcpy(buf,&time,4);
	memcpy(buf+4,&product.price,2);
	memcpy(buf+8,product.name,32);
	if(!SendWriteMulti(14+ cursor*10,buf,10)) return false;
	cursor = (cursor+1)%10;
	memcpy(buf,&cursor,1);
	pNfc->SendWrite(0xE0,buf);
	return true;
}

bool Nfc::ReadPurchaseHistories(TouchScreenLcd* pLcd){
	purchases.clear();
	for(uint8_t s_id = 0x0E; s_id<0xA7; s_id+=10){
		Byte buf[40];
		memset(buf,'\0',40);
		if(SendReadMulti(s_id,buf,10)){
			char buf2[20];
			sprintf(buf2,"hh%d",s_id-0x0E);
			pLcd->ShowString(0,500,480,50,48,buf2,0);
			Purchase purchase;
			memcpy(&purchase.timestamp,buf,4);
			if(purchase.timestamp && purchase.timestamp>20588733 && purchase.timestamp == purchase.timestamp){
				memcpy(&purchase.product.price,buf+4,2);
				memcpy(&purchase.product.name,buf+8,32);
				purchases.push_back(purchase);
			}
		} else {
			return false;
		}
	}
	Purchase d;
	for(int i=0; i<10; i++){
		d = purchases[0];
	}
//	std::sort(purchases.begin(),purchases.end(),[](const Purchase& a, const Purchase&b){
//		return a.timestamp<b.timestamp;
//	});
	Byte debug;
	return true;
}

Nfc::Nfc(Dk100 *pNfc):pNfc(pNfc)
{

}

/*
 * main.cpp
 *
 * Author: 
 * Copyright (c) 2014-2015 HKUST SmartCar Team
 * Refer to LICENSE for details
 */

#include <cassert>
#include <cstring>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <libsc/led.h>
#include <vector>

#include "libsc/k60/dk100.h"

namespace libbase
{
	namespace k60
	{

		Mcg::Config Mcg::GetMcgConfig()
		{
			Mcg::Config config;
			config.external_oscillator_khz = 50000;
			config.core_clock_khz = 150000;
			return config;
		}

	}
}

using libsc::System;
using namespace libsc;
using namespace libbase::k60;
using libsc::k60::Dk100;
using std::vector;

Led* pLed = nullptr;

int success_time = 0;
int recieve_time = 0;
int write_time = 0;
int read_time = 0;
int data = 0;
Dk100 *pNfc;

uint8_t a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0,k=0,l=0,know=0;
int g_size = 0;

vector<Byte> buffer;
int destroy_time = 0, handle_time = 0;
int send_time = 0;

bool waiting_write = false;
Byte sending_buffer[8];
void SendWriteHelper(){
	pNfc->SendBuffer(sending_buffer,8);
}
void SendWrite(Byte sector, Byte *to_write){
	waiting_write = true;
	Byte buf[8] = {0xAA, 0x06, 0x0A, sector};
	memcpy(sending_buffer,buf,4);
	memcpy(sending_buffer+4,to_write,4);
	SendWriteHelper();
}

bool waiting_read = false;
void SendReadHelper(){
	pNfc->SendBuffer(sending_buffer,4);
}
void SendRead(Byte sector){
	waiting_read = true;
	Byte buf[4] = {0xAA, 0x02, 0x09, sector};
	memcpy(sending_buffer,buf,4);
	SendReadHelper();
}

void handler(const vector<Byte>& v){
	handle_time++;
	if(v.size()>0) a=v[0];
	if(v.size()>1) b=v[1];
	if(v.size()>2) c=v[2];
	if(v.size()>3) d=v[3];
	if(v.size()>4) e=v[4];
	if(v.size()>5) f=v[5];
	if(v.size()>6) g=v[6];
	if(v.size()>7) h=v[7];
	if(v.size()>8) i=v[8];
	if(v.size()>9) j=v[9];
	if(v.size()>10) k=v[10];
	if(v.size()>11) l=v[11];

	//trigger when card tab
	if(v[2] == 0x01){
		send_time++;
//		Byte buf[7] = {0xAA, 0x01, 0x15};
//		pNfc->SendBuffer(buf,3);
		Byte to_send[4] = {0x01,0x02,0x03,0x04};
		SendWrite(0x06,to_send);
	}

	//trigger when write success
	if(waiting_write && v[2] == 0xFE){
		waiting_write = false;
	}

	//trigger when read success
	if(v[2] == 0x90){
		waiting_read = false;
	}
}

bool listener(const Byte* data, const size_t size){
	know = a;	//AA
	know = b;	//length
	know = c;	//cmd
	know = d;	//1
	know = e;	//2
	know = f;	//3
	know = g;	//4
	know = h;
	know = i;
	recieve_time++;
	for(uint8_t i=0; i<size; i++){
		buffer.push_back(data[i]);
		if(buffer.size() > 1){
			if(buffer.size() == buffer[1]+2){
				handler(buffer);
				buffer.clear();
			} else if (buffer.size() > buffer[1]+2) {
				destroy_time++;
				buffer.clear();
			}
		}
	}
	return true;
}

int main(void)
{
	System::Init();

	//type your code here
	Led::Config led_config;
    led_config.id = 0;
    Led led0(led_config);
    led_config.id = 1;
	Led led1(led_config);
	led_config.id = 2;
	Led led2(led_config);
	led_config.id = 3;
	Led led3(led_config);

	led0.SetEnable(1);
	led1.SetEnable(1);
	led2.SetEnable(1);
	led3.SetEnable(1);

	pLed = &led1;

    Dk100::Config config;
    config.id = 0;
    config.baud_rate = libbase::k60::Uart::Config::BaudRate::k9600;
    config.rx_isr = listener;
    Dk100 nfc(config);
    pNfc = &nfc;

//    Byte buf[7] = {0xAA, 0x04, 0x95, 1, 14, 0x02};
//    nfc.SendBuffer(buf,6);

//    Byte buf[8] = {0xAA, 0x06, 0x0A, 0x04, 0x01, 0x01, 0x01, 0x01 };
//    nfc.SendBuffer(buf,8);

//        Byte buf[4] = {0xAA, 0x02, 0x09, 0x04 };
//        nfc.SendBuffer(buf,4);

//    SendRead(0x06);
    Byte to_[4] = {0x01,0x02,0x03,0x04};
    SendWrite(0x06,to_);
	while (true){
	    led1.SetEnable(!waiting_write);
	    led2.SetEnable(!waiting_read);
	    if(System::Time()%10){

			if(waiting_write){
				SendWriteHelper();
			}
			else if(waiting_read==false){
				SendRead(0x06);
			}
			if(waiting_read){
				SendReadHelper();
			}
	    }
	}

	return 0;
}

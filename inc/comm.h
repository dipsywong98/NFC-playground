/*
 * comm.h
 *
 *  Created on: Feb 28, 2018
 *      Author: dipsy
 */

#ifndef INC_COMM_H_
#define INC_COMM_H_

#include <queue>
#include <vector>
#include <cstring>

#include "libbase/k60/mcg.h"
#include "libsc/system.h"
#include "libsc/led.h"
#include "libsc/joystick.h"
#include "libsc/k60/ov7725.h"
#include "libsc/battery_meter.h"
#include "libsc/lcd_console.h"
#include "libsc/st7735r.h"
#include "libsc/system.h"
#include "libsc/lcd_typewriter.h"
#include "libsc/futaba_s3010.h"
#include "libsc/dir_motor.h"
#include "libsc/dir_encoder.h"
#include "libsc/k60/jy_mcu_bt_106.h"
#include "libsc/lcd_console.h"

using libsc::System;
using namespace libsc;
using namespace libsc::k60;
using namespace libbase::k60;
using std::queue;
using std::pair;
using std::vector;

class Comm{
public:

	int last_send;

	struct PkgType{
		static const Byte
				kACK = 0x00,
				kRequestProducts = 0x01,
				kProductCount = 0x02,
				kCatalogItem = 0x03,
				kPurchase = 0x04;
	};

	struct Package{
		Byte type;
		vector<Byte> data;
	};

	Comm():last_send(0){}
	virtual ~Comm() = 0;

	int CalChecksum(Byte* buf,int length){
		int sum = 0;
		for(int i=0; i<length; i++){
			if(i == length-2)continue;
			sum+=buf[i];
		}
		return sum;
	}

	void SendPackage(const Package& pkg){
		const size_t len = 5 + pkg.data.size();
		Byte* buf = new Byte[len];
		memset(buf,0,len);
		buf[0] = 0xAA;
		buf[1] = len;
		buf[2] = pkg.type;
		memcpy(buf+3,&pkg.data[0],pkg.data.size());
		buf[len-1] = 0xFF;
		buf[len-2] = CalChecksum(buf,len);
		SendBuffer(buf,len);
		delete buf;
	}

	void RequestProductList(){
//		constexpr Byte checksum = 0xAA+0x05+0x01+0xFF;
//		Byte buf[] = {0xAA,0x05,0x01,checksum,0xFF};
		m_sendqueue.push({(Byte)PkgType::kRequestProducts,{}});
		SendFirst();
	}
	void SendFirst(){
		if(m_sendqueue.size()>0){
			SendPackage(m_sendqueue.front());
		}
	}
	void Period(){
		if(m_sendqueue.size()>0){
			SendFirst();
		}
	}
	bool Listener(const Byte* data, const size_t& size){
		for(uint8_t i=0; i<size; i++){
			if(data[i]==0xAA)buffer.clear();
			buffer.push_back(data[i]);
			if(buffer.size() > 1){
				if(data[i]==0xFF && buffer.size() == buffer[1]){
					BuildBufferPackage(buffer);
					buffer.clear();
				} else if (buffer.size() > buffer[1]+2) {
					buffer.clear();
				}
			}
			if(data[i]==0xFF){
				buffer.clear();
			}
		}
	}
	void BuildBufferPackage(const vector<Byte>& buffer){
		Package pkg = {buffer[2],{}};
		for(int i=3; i<buffer.size()-2;i++){
			pkg.data.push_back(buffer[i]);
		}
		Handler(pkg);
	}
	void Handler(const Package& pkg){
		Byte type = pkg.type;
		if(type == PkgType::kACK){
			HandleAck(type);
		}
		else {
			switch(type){
			case PkgType::kProductCount:
				break;

			}
			SendPackage({PkgType::kACK,{type}});
		}
	}
	void HandleAck(const Byte& type){
		if(type==m_sendqueue.front().type){
			m_sendqueue.pop();
			SendFirst();
		}
	}

	virtual void SendBuffer(const Byte* buf, const size_t& size) = 0;
private:

	queue<Package> m_sendqueue;
	vector<Byte> buffer;

};



#endif /* INC_COMM_H_ */

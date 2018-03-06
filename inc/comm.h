/*
 * comm.h
 *
 *  Created on: Feb 28, 2018
 *      Author: dipsy
 */

#ifndef INC_COMM_H_
#define INC_COMM_H_

#include <algorithm>
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
using std::vector;

class Comm{
public:

	int lol=0;
	int last_send;
	uint32_t historic_package_sum = 0;

	struct PkgType{
		static const Byte
				kACK = 0x00,
				kRequestProducts = 0x01,
				kProductCount = 0x02,
				kPurchase = 0x02,
				kRequestIp = 0x03;
	};

	struct Package{
		Byte type;
		uint16_t id;
		vector<Byte> data;
	};

	Comm():last_send(0){}
	virtual ~Comm(){};

	uint16_t CalChecksum(Byte* buf,int length);
	uint16_t CalChecksum(const vector<Byte>& buf);

	void QueuePackage(Package pkg);

	void SendPackageImmediate(const Package& pkg);

	void Period();

	bool Listener(const Byte* data, const size_t& size);

	void BuildBufferPackage(const vector<Byte>& buffer);

	void Handler(const Package& pkg);

	virtual void SendBuffer(const Byte* data, const size_t& size) = 0;

	int GetQueueLength(){return m_sendqueue.size();}

private:

	vector<Package> m_sendqueue;
	vector<Byte> buffer;
};



#endif /* INC_COMM_H_ */

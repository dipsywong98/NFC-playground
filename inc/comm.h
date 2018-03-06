/*
 * comm.h
 *
 *  Created on: Feb 28, 2018
 *      Author: dipsy
 */

#ifndef INC_COMM_H_
#define INC_COMM_H_

#include <vector>
#include <cstring>
#include <functional>

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
		uint8_t id;
		vector<Byte> data;
	};

	Comm(){}
	virtual ~Comm(){};

	/**
	 * Calculate checksum
	 */
	uint8_t CalChecksum(Byte* buf,int length);
	uint8_t CalChecksum(const vector<Byte>& buf);

	/**
	 * QueuePackage
	 * all of them send every 10ms
	 * remove on received ACK with corresponding id
	 * return the unique id assigned
	 */
	uint8_t QueuePackage(Package pkg);

	/**
	 * SendPackage
	 * convert the package to Byte buffer and send immediately
	 */
	void SendPackageImmediate(const Package& pkg);

	/**
	 * should be called every 10ms
	 */
	void Period();

	/**
	 * bluetooth isr
	 * parse the received bytes
	 */
	bool Listener(const Byte* data, const size_t& size);

	/**
	 * parse the byte array into package
	 */
	void BuildBufferPackage(const vector<Byte>& buffer);

	/**
	 * handle the package
	 */
	void Handler(const Package& pkg);

	/**
	 * send buffer implementation
	 */
	virtual void SendBuffer(const Byte* data, const size_t& size) = 0;

	int GetQueueLength(){return m_sendqueue.size();}

private:

	vector<Package> m_sendqueue;
	vector<Byte> buffer;

	//for generating unique id
	uint32_t historic_package_sum = 0;
};



#endif /* INC_COMM_H_ */

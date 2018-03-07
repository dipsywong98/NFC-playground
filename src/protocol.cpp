/*
 * protocol.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#include "protocol.h"

void Protocol::RequestIp(){
	ip_ready = false;
	m_bt.QueuePackage({Bluetooth::PkgType::kRequestIp,0,{}});
}

void Protocol::RequestProducts(){
	products_ready = false;
	products_count = -1;
	m_bt.QueuePackage({Bluetooth::PkgType::kRequestProducts,0,{}});
}

void Protocol::Handler(const Bluetooth::Package& pkg){
	Byte debug[100];
	memset(debug,0,100);
	memcpy(debug,&*pkg.data.begin(),pkg.data.size());
	switch(pkg.type){
	case Bluetooth::PkgType::kRequestIp:
		IpHandler(pkg);
		break;
	case Bluetooth::PkgType::kRequestProducts:
		ProductsHandler(pkg);
		break;
//	case Bluetooth::PkgType::kPurchase:
//		PurchaseHandler(pkg);
	}
}

void Protocol::IpHandler(const Bluetooth::Package& pkg){
	ip_ready = true;
	memcpy(ip,&*pkg.data.begin(),4);
}

void Protocol::ProductsHandler(const Bluetooth::Package& pkg){
	uint8_t id = pkg.data[0];
	if(id == 0){
		products_count = pkg.data[1];
	}
	else{
		char name[33];

	}

}

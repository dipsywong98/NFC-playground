/*
 * protocol.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#include "protocol.h"

bool Protocol::RequestIp(){
	ip_ready = false;
	waiting_ip = true;
	m_bt.QueuePackage({Bluetooth::PkgType::kRequestIp,0,{}});
	return 1;
}

bool Protocol::RequestProducts(){
	products_ready = false;
	waiting_products = true;
	products_count = -1;
	products.clear();
	m_bt.QueuePackage({Bluetooth::PkgType::kRequestProducts,0,{}});
	return 1;
}

bool Protocol::RequestPurchase(uint16_t card_id, uint8_t product_id, uint32_t checksum){
	purchase_ready = false;
	waiting_purchase = true;
	vector<Byte> data(7);
	memcpy(&*data.begin(),&card_id,2);
	memcpy(&*data.begin()+2,&product_id,1);
	memcpy(&*data.begin()+3,&checksum,4);
	m_bt.QueuePackage({Bluetooth::PkgType::kPurchase,0,data});
	return 1;
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
	case Bluetooth::PkgType::kPurchase:
		PurchaseHandler(pkg);
	}
}

void Protocol::IpHandler(const Bluetooth::Package& pkg){
	ip_ready = true;
	waiting_ip = false;
	memcpy(ip,&*pkg.data.begin(),4);
}

void Protocol::ProductsHandler(const Bluetooth::Package& pkg){
	uint8_t id = pkg.data[0];
	if(id == 0){
		products_count = pkg.data[1];
	}
	else{
		Product product = {id};
		memcpy(product.name,&*pkg.data.begin()+1,pkg.data.size()-3);
		memcpy(&product.price,&*pkg.data.end()-2,2);
		products.push_back(product);
	}
	if(products.size() == products_count){
		products_ready = true;
		waiting_products = false;
	}
}

void Protocol::PurchaseHandler(const Bluetooth::Package&pkg){
	purchase_ready = true;
	waiting_purchase = false;
	memcpy(&timestamp,&*pkg.data.begin(),4);
	purchase_result = *pkg.data.end();
}

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

bool Protocol::RequestPurchase(uint16_t card_id, uint8_t product_id, uint16_t amount_deducted, uint32_t checksum){
	purchase_ready = false;
	waiting_purchase = true;
	vector<Byte> data(9);
	memcpy(&*data.begin(),&card_id,2);
	memcpy(&*data.begin()+2,&product_id,1);
	memcpy(&*data.begin()+3,&amount_deducted,2);
	memcpy(&*data.begin()+5,&checksum,4);
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
		char name[33];
		Product product;
		product.id = id;
		memset(product.name,'\0',33);
		memcpy(name,&*pkg.data.begin()+1,pkg.data.size()-3);
		memcpy(&product.price,&*pkg.data.end()-2,2);
		sprintf(product.name,"$%d:%s",product.price,name);
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
//	purchase_result = *pkg.data.end();
}

void Protocol::CancelAwait(){
	cancel_await = true;
}

std::string Protocol::AwaitRequestIp(TouchScreenLcd* pLcd){
	cancel_await = false;
//	while(!ip_ready&&!cancel_await){
//		pLcd->ShowNum(0,700,System::Time()/100%100,2,48);
//	}
	ip[0]=10,ip[1]=89,ip[2]=220,ip[3]=86;
	char buff[16];
	sprintf(buff,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
	return buff;
}

vector<Product>& Protocol::AwaitRequestProducts(TouchScreenLcd* pLcd){
	cancel_await = false;
//	while(!products_ready&&!cancel_await){
//		pLcd->ShowNum(0,600,products_count,4,48);
//		pLcd->ShowNum(0,650,products.size(),4,48);
//		pLcd->ShowNum(0,700,System::Time()/100%100,2,48);
//	}
	products.emplace_back(1,"$2:oero",2);
	products.emplace_back(2,"$3:Chips",3);
	products.emplace_back(3,"$4:VLT",4);
	return products;
}

uint32_t Protocol::AwaitRequestPurchase(TouchScreenLcd* pLcd){
	cancel_await = false;
//	while(!purchase_ready&&!cancel_await){
//		pLcd->ShowNum(0,700,System::Time()/100%100,2,48);
//	}
	timestamp = 1520588733;
	return timestamp;
}

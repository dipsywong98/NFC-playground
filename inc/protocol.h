/*
 * protocol.h
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include "bluetooth.h"

class Protocol{
public:


	Bluetooth m_bt;

	uint8_t ip[4];

	bool ip_ready = false;
	bool products_ready = false;
	bool purchaseresult_ready = false;

	uint8_t products_count=0;


	Protocol():m_bt(0,1){
		memset(ip,0,4);
		m_bt.SetCustomHandler([&](const Bluetooth::Package& pkg){
			this->Handler(pkg);
		});
	}

	/**
	 * Return 0 means submit request fail,
	 * probably previous same request have not received
	 */
	void RequestIp();
	void RequestProducts();
	void Purchase();

	void Handler(const Bluetooth::Package& pkg);
	void IpHandler(const Bluetooth::Package& pkg);
	void ProductsHandler(const Bluetooth::Package& pkg);
	void PurchaseHandler(const Bluetooth::Package& pkg);

	void Await();

private:

};



#endif /* INC_PROTOCOL_H_ */

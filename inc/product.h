/*
 * product.h
 *
 *  Created on: Mar 6, 2018
 *      Author: dipsy
 */

#ifndef INC_PRODUCT_H_
#define INC_PRODUCT_H_

struct Product{
	uint8_t id;
	char name[33];
	uint16_t price;
	Product(uint8_t id, const char name[], uint16_t price):id(id),price(price){strcpy(this->name,name);}
	Product(){}
};



#endif /* INC_PRODUCT_H_ */

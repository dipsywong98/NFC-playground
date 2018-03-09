/*
 * purchase.h
 *
 *  Created on: Mar 9, 2018
 *      Author: dipsy
 */

#ifndef INC_PURCHASE_H_
#define INC_PURCHASE_H_

#include "product.h"

struct Purchase {
	uint32_t timestamp;
	Product product;
};



#endif /* INC_PURCHASE_H_ */

/*
 * comm.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: dipsy
 */

#include "comm.h"

//inline void Comm::Send(){
//	uint8_t len = System::Time()%32;
//	char* buf = new char[len];
//	buf[0] = 0xAA;
//	buf[1] = len;
//	for(uint8_t i=2; i<len-2; i++){
//		buf[i] = System::Time()*i%255;
//		buf[len-2] += buf[i];
//	}
//	buf[len-1] = 0xFF;
//	buf[len-2] += 0xAA + len + 0xFF;
//	m_bt->SendBuffer((Byte*)buf,len);
//	last_send = System::Time();
//	delete buf;
//}


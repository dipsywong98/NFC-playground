/*
 * nfc.h
 *
 *  Created on: Feb 10, 2018
 *      Author: dipsy
 */

#ifndef INC_NFC_H_
#define INC_NFC_H_

#include <cassert>
#include <cstring>
#include <libbase/k60/mcg.h>
#include <libsc/system.h>
#include <libsc/led.h>
#include <vector>

#include "libbase/k60/flash.h"
#include "libsc/k60/touchscreen_lcd.h"
#include "libsc/k60/dk100.h"
#include "libutil/touch_menu.h"

inline uint32_t CalChecksum(uint16_t id, int16_t balance, uint32_t timestamp);

void OnWrite(const Byte& sector);

void OnRead(const Byte& sector, const Byte *data);

void WriteCard();

bool ReadCard();

bool FormatCard(uint16_t id, int16_t balance);

int main_(void);

#endif /* INC_NFC_H_ */

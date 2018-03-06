/*
 * bluetooth.cpp
 *
 *  Created on: Mar 2, 2018
 *      Author: dipsy
 */

#include "bluetooth.h"

Bluetooth::Bluetooth(int bt_id, int pit_channel):
Comm(),m_bt(GetBtConfig(bt_id)),m_pit(GetPitConfig(pit_channel)){

}

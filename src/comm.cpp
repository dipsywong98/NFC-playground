/*
 * comm.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: dipsy
 */

#include "comm.h"

uint8_t Comm::CalChecksum(Byte* buf,int length){
	uint8_t sum = 0;
	for(int i=0; i<length; i++){
		if(i == length-2)continue;
		sum+=buf[i];
	}
	return sum;
}

uint8_t Comm::CalChecksum(const vector<Byte>& buf){
	uint8_t sum = 0, length = buf.size();
	for(int i=0; i<length; i++){
		if(i == length-2)continue;
		sum+=buf[i];
	}
	return sum;
}

uint8_t Comm::QueuePackage(Package pkg){
	pkg.id = ++historic_package_sum;
	m_sendqueue.push_back(pkg);
	return pkg.id;
}

void Comm::SendPackageImmediate(const Package& pkg){
	sent_package_count++;
	const size_t len = 6 + pkg.data.size();
	Byte* buf = new Byte[len];
	Byte debug[100];
	memset(debug,0,100);
	memset(buf,0,len);
	buf[0] = 0xAA;
	buf[1] = len;
	buf[2] = pkg.type;
	buf[3] = pkg.id;
	memcpy(buf+4,&pkg.data[0],pkg.data.size());
	buf[len-1] = 0xFF;
	buf[len-2] = CalChecksum(buf,len);
	memcpy(debug,buf,len);
	SendBuffer(buf,len);
	delete buf;
}

void Comm::Period(){
	for(const Package& pkg:m_sendqueue){
		SendPackageImmediate(pkg);
	}
}

bool Comm::Listener(const Byte* data, const size_t& size){
	Byte debug[100];
	memset(debug,0,100);
	memcpy(debug,&*buffer.begin(),buffer.size());
	for(uint8_t i=0; i<size; i++){
		buffer.push_back(data[i]);
		if(buffer.size() > 1){
			if(data[i]==0xFF && buffer.size() == buffer[1]){
				receive_package_count++;
				BuildBufferPackage(buffer);
			} else if(data[i]==0xAA && buffer.size() > buffer[1]){
				delete_package_count++;
				buffer.clear();
				buffer.push_back(0xAA);
			}
		}
	}
	return true;
}

void Comm::BuildBufferPackage(const vector<Byte>& buffer){
	Byte debug[100];
	memset(debug,0,100);
	memcpy(debug,&*buffer.begin(),buffer[1]);

	uint8_t checksum = 0;
	for(int i=0;i<buffer.size();i++){
		if(i==buffer.size()-2)continue;
		checksum += buffer[i];
	}
	if(checksum!=CalChecksum(buffer)){
		return;
	}
	Package pkg = {buffer[2],buffer[3],{}};
	for(int i=4; i<buffer.size()-2;i++){
		pkg.data.push_back(buffer[i]);
	}
	Handler(pkg);
}

void Comm::Handler(const Package& pkg){
	if(pkg.type == PkgType::kACK){
		for(uint8_t i=0; i<m_sendqueue.size(); i++){
			if(m_sendqueue[i].id == pkg.id){
				m_sendqueue.erase(m_sendqueue.begin()+i);
				break;
			}
		}
	} else {
		SendPackageImmediate({PkgType::kACK,pkg.id,{}});
	}
	if(m_CustomHandler){
		m_CustomHandler(pkg);
	}
}

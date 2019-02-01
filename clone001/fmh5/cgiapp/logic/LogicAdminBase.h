/*
 * LogicParams.h
 *
 *  Created on: 2017-8-17
 *      Author: dawx62fac
 */

#ifndef LOGICPARAMS_H_
#define LOGICPARAMS_H_

#include "LogicInc.h"
#include <stdexcept>


class AdminBaseUnit
{
public:
	AdminBaseUnit(const std::string& name, const std::string& skey, int level);
	virtual ~AdminBaseUnit() {}
protected:
	const ::google::protobuf::Message* SendPacket(unsigned uid, Message* msg);
private:
	void _Send(unsigned uid, Message* msg);
protected:
	CSG17Packet replay_packet_;
};



#endif /* LOGICPARAMS_H_ */

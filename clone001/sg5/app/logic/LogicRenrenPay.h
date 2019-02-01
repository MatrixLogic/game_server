#ifndef __LOGIC_RENREN_PAY_H__
#define __LOGIC_RENREN_PAY_H__

#include "LogicInc.h"

class CLogicRenrenPay
{
public:
	int PrePay(const string &openid, const string &openkey, int type, string &orderId, string &token);

	int DeliverGood(const string &openid, const string &sig, const string orderId, int &amount);
};

#endif

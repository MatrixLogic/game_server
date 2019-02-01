#include "LogicInc.h"

int main()
{
	if (!Kernel::Init())
	{
		cout << "init kernel fail" << endl;
		return 1;
	}

	DataPayHistory payHis;
	payHis.channel = PCT_RENREN;
	payHis.channel_pay_id = "";
	payHis.count = 1;
	payHis.credit = 5;
	payHis.open_id = "2312131";
	payHis.status = PST_PENDING;
	payHis.type = 301;
	payHis.uid = 10000070;
	payHis.pay_id = 0;
	payHis.time = 0;
	CLogicPay logicPay;
	int ret = logicPay.AddPayHistory(payHis);
	if (ret != 0)
		return ret;

	return 0;
}

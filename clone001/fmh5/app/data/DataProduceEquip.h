#include "Kernel.h"

#define DataProduceequip_queuedata_COUNT 36
#define DataProduceequip_shelfdata_COUNT 36

struct DataProduceequip {
	uint32_t uid;
	uint32_t finish_time;
	uint16_t id;
	uint8_t queuenum;
	int8_t status;

	char queuedata[DataProduceequip_queuedata_COUNT];  //生产队列
	char shelfdata[DataProduceequip_shelfdata_COUNT];  //展示架

	DataProduceequip()
	{
		uid = 0;
		finish_time = 0;
		id = 0;
		status = status_free;
		queuenum = 0;
		memset(queuedata, 0, sizeof(queuedata));
		memset(shelfdata, 0, sizeof(shelfdata));
	}

	void SetMessage(ProtoProduce::ProduceEquipCPP *msg)
	{
		msg->set_ud(id);
		msg->set_status(status);
		msg->set_finish_time(finish_time);
		msg->set_queuenum(queuenum);

		//队列数据
		for(int i = 0; i < queuenum; ++i)
		{
			int propsid = *(reinterpret_cast<int*>(queuedata + i * sizeof(int)) );

			if (0 == propsid)
			{
				break;
			}

			msg->add_queuedata(propsid);
		}

		//展示架数据
		for(int i = 0; i < queuenum; ++i)
		{
			int propsid = *(reinterpret_cast<int*>(shelfdata + i * sizeof(int)));

			if (0 == propsid)
			{
				break;
			}

			msg->add_shelfdata(propsid);
		}
	}

	int GetQueueFront()
	{
		//获取生产队列的头部元素
		int * pprops = reinterpret_cast<int*>(queuedata);

		return *pprops;
	}

	int GetWorkQueueNum(char * pdata)
	{
		//获取工作队列的长度
		for(int i = 0; i < queuenum; ++i)
		{
			int propsid = *(reinterpret_cast<int*>(pdata + i*sizeof(int) ));

			if (0 == propsid)
			{
				return i;
			}
		}

		return queuenum;
	}

	bool InsertQueue(char * pdata, int productid)
	{
		int num = GetWorkQueueNum(pdata);

		//在队列的末尾加上数据
		if (num >= queuenum)
		{
			return false;
		}

		//队列还有空余空间，插入.num可以直接当下标使用
		int * pprops = reinterpret_cast<int*>(pdata + num*sizeof(int));

		*pprops = productid;

		return true;
	}

	int GetPosValue(char * pdata, int pos)
	{
		int productid = *(reinterpret_cast<int*>(pdata + pos*sizeof(int)));

		return productid;
	}

	int PopArray(char * pdata, int pos)
	{
		//弹出数组中指定位置的值，后续数据自动前移
		//第一个队列生产的产品
		int productid = *(reinterpret_cast<int*>(pdata + pos*sizeof(int)));

		//队列为空，就不做移动操作了
		if (0 == productid)
		{
			return 0;
		}

		//完成当前任务，将后续任务依次往前移动
		int * curprops = NULL;
		int * nextprops = NULL;

		for(int i = pos; i < queuenum - 1; ++i)
		{
			curprops = reinterpret_cast<int*>(pdata + i*sizeof(int));
			nextprops = reinterpret_cast<int*>(pdata + (i+1)*sizeof(int));

			*curprops = *nextprops;
		}

		int * endprops = reinterpret_cast<int*>(pdata + (queuenum - 1)*sizeof(int));
		*endprops = 0;

		return productid;
	}

	void FinishCurrentJob()
	{
		//获取展示架上的已有物品长度
		int num = GetWorkQueueNum(shelfdata);

		if (num >= queuenum)
		{
			//展示架已满，暂停
			status = status_suspend;
			finish_time = 0;

			return ;
		}

		//展示架上有位置可以摆放.做如下操作
		//1.将队列头部的数据取出，后续数据往前移动
		int productid = PopArray(queuedata, 0);
		//2.将取出的数据插入展示架.
		InsertQueue(shelfdata, productid);

		//状态设置为空闲,结束时间重置为0
		status = status_free;
		finish_time = 0;
	}
};

class CDataProduceequip: public DBCBase<DataProduceequip, DB_PRODUCEEQUIP>
{
public:
	virtual int Get(DataProduceequip &data)
	{
		DBCREQ_DECLARE(DBC::GetRequest, data.uid);
		DBCREQ_SET_KEY(data.uid);
		DBCREQ_SET_CONDITION(EQ, id, data.id);
		DBCREQ_NEED_BEGIN();
		DBCREQ_NEED(uid);
		DBCREQ_NEED(id);
		DBCREQ_NEED(status);
		DBCREQ_NEED(queuenum);
		DBCREQ_NEED(queuedata);
		DBCREQ_NEED(shelfdata);
		DBCREQ_NEED(finish_time);
		DBCREQ_EXEC;
		DBCREQ_IFNULLROW;
		DBCREQ_IFFETCHROW;

		DBCREQ_GET_BEGIN();
		DBCREQ_GET_INT(data, uid);
		DBCREQ_GET_INT(data, id);
		DBCREQ_GET_INT(data, status);
		DBCREQ_GET_INT(data, queuenum);
		DBCREQ_GET_CHAR(data, queuedata, DataProduceequip_queuedata_COUNT);
		DBCREQ_GET_CHAR(data, shelfdata, DataProduceequip_shelfdata_COUNT);
		DBCREQ_GET_INT(data, finish_time);
		return 0;
	}

	virtual int Get(vector<DataProduceequip> &data)
	{
		if (data.empty())
			return R_ERROR;

		DBCREQ_DECLARE(DBC::GetRequest, data[0].uid);
		DBCREQ_SET_KEY(data[0].uid);
		data.clear();
		DBCREQ_NEED_BEGIN();
		DBCREQ_NEED(uid);
		DBCREQ_NEED(id);
		DBCREQ_NEED(status);
		DBCREQ_NEED(queuenum);
		DBCREQ_NEED(queuedata);
		DBCREQ_NEED(shelfdata);
		DBCREQ_NEED(finish_time);

		DBCREQ_EXEC;

		DBCREQ_ARRAY_GET_BEGIN(data);
		DBCREQ_ARRAY_GET_INT(data, uid);
		DBCREQ_ARRAY_GET_INT(data, id);
		DBCREQ_ARRAY_GET_INT(data, status);
		DBCREQ_ARRAY_GET_INT(data, queuenum);
		DBCREQ_ARRAY_GET_CHAR(data, queuedata,	DataProduceequip_queuedata_COUNT);
		DBCREQ_ARRAY_GET_CHAR(data, shelfdata,	DataProduceequip_shelfdata_COUNT);
		DBCREQ_ARRAY_GET_INT(data, finish_time);

		DBCREQ_ARRAY_GET_END();
		return 0;
	}

	virtual int Add(DataProduceequip &data)
	{
		DBCREQ_DECLARE(DBC::InsertRequest, data.uid);
		DBCREQ_SET_KEY(data.uid);
		DBCREQ_SET_INT(data, id);
		DBCREQ_SET_INT(data, status);
		DBCREQ_SET_INT(data, queuenum);
		DBCREQ_SET_CHAR(data, queuedata, DataProduceequip_queuedata_COUNT);
		DBCREQ_SET_CHAR(data, shelfdata, DataProduceequip_shelfdata_COUNT);
		DBCREQ_SET_INT(data, finish_time);

		DBCREQ_EXEC;

		return 0;
	}

	virtual int Set(DataProduceequip &data)
	{
		DBCREQ_DECLARE(DBC::UpdateRequest, data.uid);
		DBCREQ_SET_KEY(data.uid);
		DBCREQ_SET_CONDITION(EQ, id, data.id);
		DBCREQ_SET_INT(data, id);
		DBCREQ_SET_INT(data, status);
		DBCREQ_SET_INT(data, queuenum);
		DBCREQ_SET_CHAR(data, queuedata, DataProduceequip_queuedata_COUNT);
		DBCREQ_SET_CHAR(data, shelfdata, DataProduceequip_shelfdata_COUNT);
		DBCREQ_SET_INT(data, finish_time);

		DBCREQ_EXEC;

		return 0;
	}

	virtual int Del(DataProduceequip &data)
	{
		DBCREQ_DECLARE(DBC::DeleteRequest, data.uid);
		DBCREQ_SET_KEY(data.uid);
		DBCREQ_SET_CONDITION(EQ, id, data.id);
		DBCREQ_EXEC;
		return 0;
	}
};

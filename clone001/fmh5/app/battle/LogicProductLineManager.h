#ifndef LOGIC_PRODUCTLINE_MANAGER_H
#define LOGIC_PRODUCTLINE_MANAGER_H

#include "LogicQueueManager.h"

//地块生产
class DataCropProduceRoutine : public DataQueueRoutine
{
public:
	DataCropProduceRoutine(unsigned uid, unsigned endts, vector<unsigned> & uds):
		DataQueueRoutine(uid, endts, uds)
	{

	}

	virtual void CheckUd(unsigned buildud);

	virtual void GetPriceAndATime(unsigned buildud, int & cash, int & diffts);

	//定时任务结束
	virtual void SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg);
};

//设备生产
class DataProduceEquipRoutine : public DataQueueRoutine
{
public:
	DataProduceEquipRoutine(unsigned uid, unsigned endts, vector<unsigned> & uds):
		DataQueueRoutine(uid, endts, uds)
	{

	}

	virtual void CheckUd(unsigned buildud);

	virtual void GetPriceAndATime(unsigned buildud, int & cash, int & diffts);

	virtual void SingleRoutineEnd(unsigned buildud, ProtoPush::PushBuildingsCPP * msg);

	//升星
	int EquipmentStarUpgrade(DataProduceequip & equipment, ProtoPush::PushBuildingsCPP * msg);
};

//动物生产
class DataAnimalRoutine : public DataQueueRoutine
{
public:
	DataAnimalRoutine(unsigned uid, unsigned endts, vector<unsigned> & uds):
		DataQueueRoutine(uid, endts, uds)
	{

	}

	virtual void CheckUd(unsigned animalud);

	virtual void GetPriceAndATime(unsigned animalud, int & cash, int & diffts);

	virtual void SingleRoutineEnd(unsigned animalud, ProtoPush::PushBuildingsCPP * msg);
};

class LogicProductLineManager :public BattleSingleton, public CSingleton<LogicProductLineManager>
{
private:
	friend class CSingleton<LogicProductLineManager>;
	LogicProductLineManager();

public:
	enum
	{
		init_queue_num = 2,  //默认队列长度
	};

	virtual void CallDestroy() { Destroy();}

	int NewUser(unsigned uid);

	int CheckLogin(unsigned uid);

	//建造相关的生产线
	int ProduceAfterBuild(unsigned uid, unsigned ud, unsigned type, bool ispush = true, ProtoBuilding::BuildResp * resp = NULL);

	//设备的下一步动作
	void ProduceEquipNextMove(DataProduceequip & equipment);

	//种植
	int Process(unsigned uid, ProtoProduce::PlantCropReq* req, ProtoProduce::PlantCropResp* resp);
	//收割
	int Process(unsigned uid, ProtoProduce::ReapCropReq* req, ProtoProduce::ReapCropResp* resp);
	//生产队列扩展
	int Process(unsigned uid, ProtoProduce::ExpandQueueReq* req, ProtoProduce::ExpandQueueResp* resp);
	//加入生产队列
	int Process(unsigned uid, ProtoProduce::JoinQueueReq* req, ProtoProduce::JoinQueueResp* resp);
	//取回物品
	int Process(unsigned uid, ProtoProduce::FetchProductReq* req, ProtoProduce::FetchProductResp* resp);

	//领养动物
	int Process(unsigned uid, ProtoProduce::AdoptAnimalReq* req, ProtoProduce::AdoptAnimalResp* resp);
	//喂养动物
	int Process(unsigned uid, ProtoProduce::FeedAnimalReq* req, ProtoProduce::FeedAnimalResp* resp);
	//获取物品
	int Process(unsigned uid, ProtoProduce::ObtainProductReq* req, ProtoProduce::ObtainProductResp* resp);
private:
	int OnlineCropland(unsigned uid);

	int OnlineEquipProduce(unsigned uid);

	int OnlineAnimal(unsigned uid);

	//地块种植
	int PlantCrop(unsigned uid, unsigned corpud, vector<unsigned> & lands, ProtoProduce::PlantCropResp *resp);

	//收割
	int ReapCrop(unsigned uid, vector<unsigned> & lands, ProtoProduce::ReapCropResp * resp);

	//扩充队列
	int ExpandQueue(unsigned uid, unsigned equipud, ProtoProduce::ExpandQueueResp * resp);

	//加入队列
	int JoinEquipQueue(unsigned uid, unsigned equipud, unsigned productid, ProtoProduce::JoinQueueResp * resp);

	//取回仓库
	int FetchBackStorage(unsigned uid, unsigned equipud, unsigned pos, ProtoProduce::FetchProductResp * resp);

	//领养动物
	int Adopt(unsigned uid, unsigned buildud, ProtoProduce::AdoptAnimalResp * resp);

	//喂养动物
	int Feed(unsigned uid, unsigned animalud, unsigned fodderud, ProtoProduce::FeedAnimalResp * resp);

	//获取物品
	int Obtain(unsigned uid, unsigned animalud, ProtoProduce::ObtainProductResp * resp);

	//获取经验奖励
	int GetExpReward(unsigned productid, unsigned count, CommonGiftConfig::CommonModifyItem &reward);

	//收获后的动作：更新收获次数，是否随机产生道具
	int ActAfterHarvest(unsigned uid, unsigned count, DataCommon::CommonItemsCPP * msg);

	unsigned GetBuildId(unsigned uid, unsigned equipud);
};

#endif //LOGIC_PRODUCTLINE_MANAGER_H

#ifndef __LOGIC_GATE_H__
#define __LOGIC_GATE_H__

#include "LogicInc.h"

struct GateRankData
{
	unsigned uid;
	int      score;
	string   name;
};
struct GateRank
{
	unsigned lastReadTime;
	vector<GateRankData> gateRankVec;
};

class CLogicGate
{
public:
	int GetGate(unsigned uid, Json::Value &data);
	int UpdateGate(unsigned uid, const Json::Value &data);

	int GetGateRank(unsigned uid, int gateId, Json::Value &json);
	int Load(unsigned uid, unsigned start, unsigned end, Json::Value &json);

	static const vector<GateRankData>& GetGateRankVec(int gateId);
private:
	static map<int, GateRank> gateRankMap;
};

#endif

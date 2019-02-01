/*
 * LogicBarbarianInvasion.h
 *
 *  Created on: 2012-12-17
 *      Author: Administrator
 */

#ifndef LOGICBARBARIANINVASION_H_
#define LOGICBARBARIANINVASION_H_
#include "LogicInc.h"
class CLogicBarbarianInvasion {
public:
	CLogicBarbarianInvasion();
	virtual ~CLogicBarbarianInvasion();

	int UpdateRank(DataPlayerItem const &playerItem, unsigned aid);
	int GetTeamPlayersRank(DataPlayersPoints &playerRank, unsigned aid);
	int GetTeamRank(DataTeamItem teamRank[],  int &teamsNum);

protected:
	CDataBarbarianInvasion *GetCDataBarbarianInvasion();
	vector<int> m_begin_ts;
	vector<int> m_end_ts;
};

#endif /* LOGICBARBARIANINVASION_H_ */

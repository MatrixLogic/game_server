#ifndef LOGICUSERNOMENATE_H_
#define LOGICUSERNOMENATE_H_

#include "LogicInc.h"
#include "DataNewUserRecList.h"

class CLogicUserNomenate {
public:
	int InitializeUserNomenate(unsigned uid, DataUserNomenate *pData = NULL);
	int AddUserNomenate(const DataUserNomenate &data);
	int GetUserNomenate(unsigned uid, DataUserNomenate &data);
	int SetUserNomenate(unsigned uid, const DataUserNomenate &data);
	int DeleteUserNomenate(unsigned uid);

	int setFriendRecList(unsigned uid, const string &rec_list);
	int getFriendRecList(unsigned uid, string &rec_list);
	int setFriendAddList(unsigned uid, const string &add_list);
	int getFriendAddList(unsigned uid, string &add_list);
	int setFriendBlackList(unsigned uid, const string &black_list);
	int getFriendBlackList(unsigned uid, string &black_list);
	int setFriendList(unsigned uid, const string &friend_list);
	int getFriendList(unsigned uid, string &friend_list);

public:
	int GetNewUserRecList(string &list);
	int JoinNewUserRecList(unsigned uid);
private:
	static CDataNewUserRecList * GetCDataNewUserRecList(void);
};

#endif /* LOGICUSERNOMENATE_H_ */

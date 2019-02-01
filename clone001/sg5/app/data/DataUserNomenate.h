#ifndef DATAUSERNOMENATE_H_
#define DATAUSERNOMENATE_H_

#include "Kernel.h"

struct DataUserNomenate{
	unsigned uid;
	string rec_list;
	string add_list;
	string black_list;
	string friend_list;
	unsigned update_time;
};
class CDataUserNomenate : public CDataBaseDBC{
public:
	CDataUserNomenate(int table = DB_USER_NOMENATE):CDataBaseDBC(table){}
public:
	int addNomenate(const DataUserNomenate &data);
	int setNomenate(unsigned uid, const DataUserNomenate &data);
	int getNomenate(unsigned uid, DataUserNomenate &data);
	int deleteNomenate(unsigned uid);

	int setRecList(unsigned uid, const string &rec_list);
	int getRecList(unsigned uid, string &rec_list);

	int setAddList(unsigned uid, const string &add_list);
	int getAddList(unsigned uid, string &add_list);

	int setBlackList(unsigned uid, const string &black_list);
	int getBlackList(unsigned uid, string &black_list);

	int setFriendList(unsigned uid, const string &friend_list);
	int getFriendList(unsigned uid, string &friend_list);

};

#endif /* DATAUSERNOMENATE_H_ */

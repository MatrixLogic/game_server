#ifndef DATAUSERTEXT_H_
#define DATAUSERTEXT_H_

#include "Kernel.h"

class CDataUserText : public CDataBaseDBC
{
public:
	CDataUserText(int table = DB_USER_TEXT) : CDataBaseDBC(table) {}

public:
	int AddData(unsigned uid, const string &data);
	int SetData(unsigned uid, const string &data);
	int GetData(unsigned uid, string &data);
	int RemoveData(unsigned uid);

};

#endif /* DATAUSERTEXT_H_ */

#ifndef __DATA_BROADCAST_H__
#define __DATA_BROADCAST_H__

#include "Kernel.h"

struct BroadcastMessage
{
	unsigned id;
	string content;
};

#define BROADCAST_AMOUNT_MAX 10
#define BROADCAST_CONTENT_SIZE_MAX 512

enum BroadcastStatus
{
	BS_IDLE,
	BS_READING
};

struct BroadcastItem
{
	unsigned		id;
	BroadcastStatus	status;
	char			content[BROADCAST_CONTENT_SIZE_MAX];
};
struct BroadcastDATA
{
	unsigned		cur_id;
	BroadcastItem	items[BROADCAST_AMOUNT_MAX];
};

class CDataBroadcast
{
public:
	CDataBroadcast();
	int Init(const string &path, semdat sem=sem_broadcast);
	int AddBdMessage(const string &msg);
	int GetBdMessage(vector<BroadcastMessage> &msgs);

	int GetBroadcastData(BroadcastDATA &data);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif

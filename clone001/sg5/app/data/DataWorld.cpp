/*
 * DataWorld.cpp
 *
 *  Created on: 2012-8-21
 *      Author: Administrator
 */

#include "DataWorld.h"

CDataWorld::CDataWorld() {
	// TODO Auto-generated constructor stub
	m_init = false;
}

CDataWorld::~CDataWorld() {
	// TODO Auto-generated destructor stub
}

int CDataWorld::Init(const std::string &worldData, semdat sem)
{
	if(m_init)
	{
		return 0;
	}

	int semgroup = 0;
	int semserver = 0;
	Config::GetDomain(semserver);
	if(!semserver)
		Config::GetDB(semserver);
	if(!m_sh.CreateOrOpen(worldData.c_str(), sizeof(World), SEM_ID(sem,semgroup,semserver)))
	{
		error_log("[init_world_fail][path=%s]", worldData.c_str());
		return R_ERROR;
	}
	CAutoLock lock(&m_sh, true);
	if(!m_sh.HasInit())
	{

		World *pdata = (World *)m_sh.GetAddress();
		//	初始化坐标系统
		memset(pdata, 0, sizeof(World));
		short int x = 0, y = 0;
		for(;x < WORLD_LENGTH_OF_SIDE; x++)
		{
			for(y = 0;y < WORLD_LENGTH_OF_SIDE; y++)
			{
				pdata->worldPoints[x][y].x = x + 1;
				pdata->worldPoints[x][y].y = y + 1;
			}
		}
		pdata->curr_cid = MIN_CID;
		m_sh.SetInitDone();
	}
	m_init = true;
	return 0;
}
int CDataWorld::Move(short int begin_x,short int begin_y,short int end_x, short int end_y,vector<WorldPoint> &vWorldPoints)
{

	World *pdata = (World *)m_sh.GetAddress();
	CAutoLock lock(&m_sh, true);
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	short int x = 0;
	short int y = 0;
	for(x = begin_x; x <= end_x;x++)
	{
		for(y = begin_y; y <= end_y; y++)
		{
			vWorldPoints.push_back(pdata->worldPoints[x][y]);
		}
	}
	return 0;
}
int CDataWorld::AddUser(const WorldPoint &worldpoint, int unsigned &world_pos)
{

	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if(pdata->curr_cid >= MIN_CID + WORLD_SIZE )
	{
		error_log("map full ,cann't add user ");
		return R_ERR_DB;
	}
	int iCount = 0;
	short unsigned x = Math::GetRandomInt(NEW_USER_INIT_SIZE - 1);
	short unsigned y = Math::GetRandomInt(NEW_USER_INIT_SIZE - 1);
	while(pdata->worldPoints[x][y].uid != 0 || pdata->worldPoints[x][y].map_flag != 0 || pdata->worldPoints[x][y].top_worldpos != 0)
	{

		if(x < NEW_USER_INIT_SIZE - 1 && y < NEW_USER_INIT_SIZE - 1)
		{
			y++;
		}
		else if (y >= NEW_USER_INIT_SIZE - 1 && x < NEW_USER_INIT_SIZE - 1)
		{
			x++;
			y = 0;
		}else{
			x = 0;
			y = 0;

		}
		iCount++;
		if(iCount >= (NEW_USER_INIT_SIZE * NEW_USER_INIT_SIZE))
		{
			error_log("map full ,cann't add user ");
			return R_ERR_DB;
		}
	}

	world_pos = (x + 1)* 1000 + y + 1;
	if (0 == pdata->worldPoints[x][y].cid)
	{
		pdata->worldPoints[x][y] = worldpoint;
		pdata->worldPoints[x][y].cid = ++pdata->curr_cid;
		pdata->worldPoints[x][y].x = x + 1;
		pdata->worldPoints[x][y].y = y + 1;
		pdata->worldPoints[x][y].map_flag = USER_MAIN_CITY;
	}
	return 0;
}

int CDataWorld::AddNpc(unsigned npcid,short x, short y,int npcType,int citysize,char name[32])
{
	if(x + citysize >= WORLD_LENGTH_OF_SIDE || y + citysize >= WORLD_LENGTH_OF_SIDE)
	{
		error_log("out of range x = %d,y = %d",x,y);
		return R_ERR_DB;
	}
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if(pdata->worldPoints[x][y].uid != 0)
	{
		error_log("add npc failed x = %d,y =%d", x, y);
		return R_ERR_DB;
	}
	int index_x = 0;
	int index_y = 0;

	for(index_x = x;index_x < x + citysize; index_x++)
	{
		for(index_y = y;index_y < y + citysize; index_y++)
		{
			if(pdata->worldPoints[index_x][index_y].uid != 0 || pdata->worldPoints[index_x][index_y].top_worldpos != 0)
			{
				error_log("add npc failed x=%d,y=%d,index_x=%d,index_y=%d uid=%u,top_world_pos = %u,citysize=%d",x,y,index_x,index_y,pdata->worldPoints[index_x][index_y].uid,
						pdata->worldPoints[index_x][index_y].top_worldpos,citysize);
				return -1;
			}
		}
	}
	if(index_x >= x + citysize && index_y >= y + citysize)
	{
		unsigned worldpos = (x + 1) * 1000 + y + 1;
		for(index_x = x;index_x < x + citysize; index_x++)
		{
			for(index_y = y;index_y < y + citysize; index_y++)
			{
				pdata->worldPoints[index_x][index_y].top_worldpos = (x + 1) * 1000 + y + 1;
			}
		}
		pdata->worldPoints[x][y].uid = npcid;
		pdata->worldPoints[x][y].map_flag = npcType;
		pdata->curr_cid++;
		memcpy(pdata->worldPoints[x][y].name,name,sizeof(pdata->worldPoints[x][y].name));
		memcpy(pdata->worldPoints[x][y].cityname,name,sizeof(pdata->worldPoints[x][y].cityname));
	}
	return 0;
}

int CDataWorld::AddNpc(unsigned npcid,short begin_x, short begin_y,short end_x,short end_y,int npcType,unsigned &worldpos,int citysize,const string &name )
{
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	int iCount = 0;
	short  x = Math::GetRandomInt(end_x - begin_x) + begin_x;
	short  y = Math::GetRandomInt(end_y - begin_y) + begin_y;
	int  subarenaSize= (end_x + 1) * (end_y + 1) - begin_x * begin_y;
	bool usable = false;
	int index_x = 0;
	int index_y = 0;
	while(1)
	{
		if(pdata->worldPoints[x][y].uid == 0 && pdata->worldPoints[x][y].top_worldpos == 0)
		{
			if(x + citysize < end_x && y + citysize < end_y)
			{
				for(index_x = x;index_x < x + citysize;index_x++)
				{
					for(index_y = y;index_y <  y + citysize;index_y++)
					{
						if(pdata->worldPoints[index_x][index_y].uid != 0 || pdata->worldPoints[index_x][index_y].top_worldpos != 0)
						{
							break;
						}
					}
					if(index_y < y + citysize)
					{
						break;
					}
				}
				if(index_x >= x + citysize && index_y >= y + citysize)
				{
					usable = true;
				}
			}
		}
		if(usable)
		{
			break;
		}
		if( (x >= begin_x ) && (x < end_x ) && (y >= begin_y ) && (x < end_y ) )
		{
			y++;
		}
		else if ((y >= end_x) && (x >= begin_x ) && (x < end_x ))
		{
			x++;
			y = begin_y;

		}else{
			x = begin_x;
			y = begin_y;
		}
		if(x + citysize > end_x )
		{
			x = begin_x;
		}
		if(y + citysize > end_y)
		{
			y = begin_y;
		}
		iCount++;
		if(iCount >= subarenaSize)
		{
			error_log("map full ,cann't add user ");
			return R_ERR_DB;
		}
	}
	// init npc
	pdata->worldPoints[x][y].uid = npcid;
	pdata->worldPoints[x][y].map_flag = npcType;
	pdata->curr_cid++;
	memcpy(pdata->worldPoints[x][y].cityname,name.c_str(),sizeof(pdata->worldPoints[x][y].cityname) - 1);
	memcpy(pdata->worldPoints[x][y].name,name.c_str(),sizeof(pdata->worldPoints[x][y].name) - 1);
	worldpos = 1000 * (x + 1) +y + 1;
	for(index_x = x;index_x < x + citysize;index_x++)
	{
		for(index_y = y;index_y <  y + citysize;index_y++)
		{
			pdata->worldPoints[index_x][index_y].top_worldpos = worldpos;
		}
	}
	return 0;
}

int CDataWorld::RemoveNpc(unsigned npcid,short x,short y,int npcType,int citysize)
{
	int index_x = 0;
	int index_y = 0;
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if(pdata->worldPoints[x][y].map_flag == npcType )
	{
		memset(&pdata->worldPoints[x][y],0x00,sizeof(pdata->worldPoints[x][y]));
		pdata->worldPoints[x][y].x = x + 1;
		pdata->worldPoints[x][y].y = y + 1;
		for(index_x = x;index_x < x + citysize;index_x++)
		{
			for(index_y = y;index_y < y + citysize;index_y++)
			{
				pdata->worldPoints[index_x][index_y].top_worldpos = 0;
			}

		}
	}else{
		error_log("remove npc failed, npcid = %d,",npcid);
		return R_ERR_DB;
	}
	if(pdata->curr_cid > 0)
	{
		pdata->curr_cid--;
	}
	return 0;
}
int CDataWorld::RemoveNpc(unsigned npcid,short begin_x, short begin_y,short end_x,short end_y,int npcType,int citysize)
{
	short x = 0, y = 0;
	int index_x = 0;
	int index_y = 0;
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	for(x = begin_x;x <= end_x;x++)
	{

		for( y = begin_y;y <= end_y;y++)
		{

			if(pdata->worldPoints[x][y].map_flag == npcType )
			{
				memset(&pdata->worldPoints[x][y], 0x00,sizeof(pdata->worldPoints[x][y]));
				pdata->worldPoints[x][y].x = x + 1;
				pdata->worldPoints[x][y].y = y + 1;
				pdata->curr_cid--;
				for(index_x = x;index_x < x + citysize;index_x++)
				{
					for(index_y = y;index_y <  y + citysize;index_y++)
					{
						pdata->worldPoints[index_x][index_y].top_worldpos = 0;
					}
				}
			}
		}
	}
	if(pdata->curr_cid > 0)
	{
		pdata->curr_cid--;
	}
	return 0;

}

int CDataWorld::RemoveNpcByType(short x,short y,int npctype,int citysize)
{
	int index_x = 0;
	int index_y = 0;
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if(!IsValidIndex(x,y))
	{
		return R_ERR_DB;
	}
	if(pdata->worldPoints[x][y].map_flag == npctype)
	{
		memset(&pdata->worldPoints[x][y], 0x00,sizeof(pdata->worldPoints[x][y]));
		pdata->worldPoints[x][y].x = x + 1;
		pdata->worldPoints[x][y].y = y + 1;
		if(pdata->curr_cid > 0)
		{
			pdata->curr_cid--;
		}
		for(index_x = x;index_x < x + citysize;index_x++)
		{
			for(index_y = y;index_y < y + citysize;index_y++)
			{
				pdata->worldPoints[index_x][index_y].top_worldpos = 0;
			}

		}
	}
	return 0;
}
int CDataWorld::RemoveNpcByType(short begin_x,short begin_y,short end_x,short end_y,int npctype,int citysize)
{
	int index_x = 0;
	int index_y = 0;
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if((!IsValidIndex(begin_x,begin_y)) || (!IsValidIndex(end_x,end_y)))
	{
		return R_ERR_DB;
	}
	short x = 0; short y = 0;
	for(x = begin_x; x < end_x;x++)
	{

		for(y = begin_y; y < end_y; y++)
		{

			if(pdata->worldPoints[x][y].map_flag == npctype)
			{
				memset(&pdata->worldPoints[x][y], 0x00,sizeof(pdata->worldPoints[x][y]));
				pdata->worldPoints[x][y].x = x + 1;
				pdata->worldPoints[x][y].y = y + 1;
				if(pdata->curr_cid > 0)
				{
					pdata->curr_cid--;
				}
				for(index_x = x;index_x < x + citysize;index_x++)
				{
					for(index_y = y;index_y < y + citysize;index_y++)
					{
						pdata->worldPoints[index_x][index_y].top_worldpos = 0;
					}
				}
			}
		}
	}
	return 0;
}
int CDataWorld::GetNpc(short begin_x,short begin_y,short end_x,short end_y,int npctype,vector<DataNpc> &vNpc )
{
	DataNpc dataNpc;
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if((!IsValidIndex(begin_x,begin_y)) || (!IsValidIndex(end_x,end_y)))
	{
		return R_ERR_DB;
	}
	short x = 0; short y = 0;
	for(x = begin_x; x < end_x;x++)
	{
		for(y = begin_y; y < end_y; y++)
		{
			if(pdata->worldPoints[x][y].map_flag == npctype && pdata->worldPoints[x][y].uid != 0)
			{
				dataNpc.npcId = pdata->worldPoints[x][y].uid;
				dataNpc.npctype = pdata->worldPoints[x][y].map_flag;
				dataNpc.worldpos = (x + 1) * 1000 + y + 1;
				dataNpc.cityname = string(pdata->worldPoints[x][y].name);
				vNpc.push_back(dataNpc);
			}
		}
	}
	return 0;
}

int CDataWorld::SetWorldPoint(unsigned uid, int unsigned &world_pos)
{
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	short unsigned x = world_pos / 1000;
	short unsigned y = world_pos % 1000;;
	memset(&pdata->worldPoints[x][y],0x00,sizeof(pdata->worldPoints[x][y]));
	pdata->worldPoints[x][y].uid = uid;
	// if(pdata->worldPoints[x][y].cid = ++pdata->curr_cid)
	if(0 == pdata->worldPoints[x][y].cid)
	{

	//	pdata->worldPoints[x][y].map_flag = FIELD_CITY;

	}
	return 0;
}

int CDataWorld::ChangeWorldPointType(short int x, short int y,short int map_flag)
{
		World *pdata = (World *)m_sh.GetAddress();
		if(pdata == NULL)
		{
			return R_ERR_DB;
		}
		CAutoLock lock(&m_sh, true);
		pdata->worldPoints[x][y].map_flag = map_flag;
		return 0;
}


int CDataWorld::ReplaceUser(const WorldPoint &worldpoint, int x,int y)
{

	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	if(pdata->curr_cid >= MIN_CID + WORLD_SIZE )
	{
		error_log("map full ,cann't add user ");
		return R_ERR_DB;
	}
	pdata->worldPoints[x][y].uid = worldpoint.uid;
	//memcpy(pdata->worldPoints[x][y].username,worldpoint.username,sizeof(pdata->worldPoints[x][y].username) - 1);
	pdata->worldPoints[x][y].map_flag = worldpoint.map_flag;
	return 0;
}
int CDataWorld::MoveCity(unsigned uid,const unsigned oldWorldPos,const unsigned toWorldPos)
{
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	CAutoLock lock(&m_sh, true);
	short old_x = (oldWorldPos - 1001) / 1000;
	short old_y = (oldWorldPos - 1001) % 1000;
	short to_x = (toWorldPos - 1001) / 1000;
	short to_y = (toWorldPos - 1001) % 1000;
	if(pdata->worldPoints[old_x][old_y].uid != uid)
	{
		error_log("move city failed,uid miss match, olduid= %u,touid=%u",uid,pdata->worldPoints[old_x][old_y].uid);
		return R_ERR_DB;
	}
	if(pdata->worldPoints[to_x][to_y].uid != 0)
	{
		error_log("move city failed city is being using,uid= %d, oldpos = %u,newpos = %d");
		return R_ERR_DB;
	}
	pdata->worldPoints[to_x][to_y]=pdata->worldPoints[old_x][old_y];
	pdata->worldPoints[to_x][to_y].x = to_x + 1;
	pdata->worldPoints[to_x][to_y].y = to_y + 1;
	memset(&(pdata->worldPoints[old_x][old_y]),0x00,sizeof(pdata->worldPoints[old_x][old_y]));
	pdata->worldPoints[old_x][old_y].x = old_x + 1;
	pdata->worldPoints[old_x][old_y].y = old_y + 1;
	return 0;
}

int CDataWorld::ReplaceUser(const WorldPoint &worldpoint,unsigned worldpos)
{
	int x = (worldpos - 1001) / 1000;
	int y = (worldpos - 1001) % 1000;
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}

	CAutoLock lock(&m_sh, true);
	pdata->worldPoints[x][y].uid = worldpoint.uid;
	if(IsValidWorldType(worldpoint.map_flag))
	{
		pdata->worldPoints[x][y].map_flag = worldpoint.map_flag;
	}
	pdata->worldPoints[x][y].last_collect_ts = worldpoint.last_collect_ts;

	return 0;
}
int CDataWorld::ViewMap()
{
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	int x = 0, y = 0;
	CAutoLock lock(&m_sh, true);
	for(x = 0; x < WORLD_LENGTH_OF_SIDE;x++)
	{
		for( y = 0; y < WORLD_LENGTH_OF_SIDE;y++)
		{

			cout<<"uid = "<<pdata->worldPoints[x][y].uid<<" \t map_type="<<pdata->worldPoints[x][y].map_flag << "topworldpos="<<pdata->worldPoints[x][y].top_worldpos;
			cout<<"x = "<<pdata->worldPoints[x][y].x<<"\t y ="<<pdata->worldPoints[x][y].y<<endl;
		}
	}
	return 0;
}

int CDataWorld::GetWorldUser(const int world_pos,WorldPoint &worldPoint){
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	int x = 0, y = 0;
	CAutoLock lock(&m_sh, true);

	x = (world_pos - 1001) / 1000;
	y = (world_pos - 1001) % 1000;
	worldPoint = pdata->worldPoints[x][y];
	return 0;
}

int CDataWorld::GetAllWorldUser(vector<WorldPoint> &vWorldPoint)
{
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	int x = 0, y = 0;
	CAutoLock lock(&m_sh, true);
	for(x = 0; x < WORLD_LENGTH_OF_SIDE;x++)
	{
		for(y = 0; y < WORLD_LENGTH_OF_SIDE;y++)
		{
			if(pdata->worldPoints[x][y].uid != 0)
			{
				vWorldPoint.push_back(pdata->worldPoints[x][y]);
			}
		}
	}
	return 0;
}
int CDataWorld::SetWorldPoint(const int world_pos,WorldPoint &worldPoint)
{
	World *pdata = (World *)m_sh.GetAddress();
	if(pdata == NULL)
	{
		return R_ERR_DB;
	}
	int x = 0, y = 0;
	CAutoLock lock(&m_sh, true);
	x = (world_pos - 1001) / 1000;
	y = (world_pos - 1001) % 1000;

	if(!(x >= 0 && x < 500 && y >= 0 && y < 500))
	{
		// cout<<"world_pos out of range"<<endl;
		return R_ERR_DB;
	}
	if(pdata->worldPoints[x][y].uid ==0)
	{
		pdata->worldPoints[x][y] = worldPoint;
		// cout<<"worldpos = "<<world_pos<<"  had been used"<<endl;
	}

	return 0;
}

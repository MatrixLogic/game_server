/*
 * BattleSocket.h
 *
 *  Created on: 2016-9-17
 *      Author: Ralf
 */

#ifndef BATTLESOCKET_H_
#define BATTLESOCKET_H_


#include "Common.h"
#include "Packet1.h"

class BattleSocket
{
public:
	static int Send(unsigned server, CSG17Packet* packet);
	static int Receive(unsigned server, CSG17Packet* packet);

	BattleSocket(unsigned server):serverid(server){}
	~BattleSocket(){}
private:
	int Connect();
	int Send(CSG17Packet* packet);
	int Receive(CSG17Packet* packet);
	bool IsClosed(){return socket.IsClosed();}

	CSocket socket;
	unsigned serverid;

	static map<unsigned, BattleSocket*> m_map;
};


#endif /* BATTLESOCKET_H_ */

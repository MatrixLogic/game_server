/*
 * Packet.h
 *
 *  Created on: 2012-2-10
 *      Author: dada
 */

#ifndef _LIP_FIRE_PACKET_H_
#define _LIP_FIRE_PACKET_H_

#include "Common.h"
#include "IPacket.h"

#define PACKET_HEADER_SIZE sizeof(BattleHead)
#define MAX_PROTOCOL_BODY_SIZE 0x20000
#define MAX_PACKET_SIZE (PACKET_HEADER_SIZE + MAX_PROTOCOL_BODY_SIZE + sizeof(byte))


// 协议包头数据
class CFirePacket: public IPacket, public IEncodable, public IDecodable {
public:

	int ChannelId;	//通道ID

	uint16_t head;
	uint32_t bodyLen;
	uint16_t cmd;
	uint32_t fd;
	uint32_t time;
	uint32_t microTime;
	uint32_t clientIp;	//原始客户端的ip

	uint32_t uid;
	bool delmsg;
	set<unsigned>* group;

	CStaticBuffer<MAX_PROTOCOL_BODY_SIZE> body;	//包体


	CFirePacket(uint16_t c = 0, bool d = true);
	~CFirePacket();

	virtual bool Encode(IBuffer *pBuffer);
	virtual bool Decode(IBuffer *pBuffer);
	bool SetBody(IBuffer* pBuf);

	uint32_t GetDecodeSize() {
		return m_decodeSize;
	}

	IMPLEMENT_TO_STRING("cid=%d&len=%u&cmd=%u&fd=%u&time=%u&microTime=%u",ChannelId, bodyLen,cmd,fd,time,microTime);


	Message* m_msg;
	bool DecodePB();
	bool EncodePB();

private:
	uint32_t m_decodeSize;

};

DEFINE_RECEIVE_PACKET_BEGIN(CReceiveProtocol)
	Message* msg;
	IMPLEMENT_PACKET_DECODE_BEGIN
		PACKET_DECODE(PBMsg, msg);
	IMPLEMENT_PACKET_DECODE_END
DEFINE_RECEIVE_PACKET_END


DEFINE_SEND_PACKET_BEGIN(CSendCProtocol)
	Message* msg;
	set<unsigned>* group;
	IMPLEMENT_PACKET_ENCODE_BEGIN
		if(group && group->size())
		{
			PACKET_ENCODE(UInt32, group->size());
			for(set<unsigned>::iterator it=group->begin();it!=group->end();++it)
			{
				PACKET_ENCODE(UInt32, *it);
			}
		}
		PACKET_ENCODE(PBMsg, msg);
	IMPLEMENT_PACKET_ENCODE_END
DEFINE_SEND_PACKET_END

#endif /* PACKET_H_ */

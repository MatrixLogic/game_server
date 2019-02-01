/*
 * Packet.cpp
 *
 *  Created on: 2012-2-10
 *      Author: dada
 */

#include "FirePacket.h"

#define PACKET_HEAD_MAGIC_NUMBER 21961

CFirePacket::CFirePacket(uint16_t c, bool d)
{
	ChannelId = -1;
	head = PACKET_HEAD_MAGIC_NUMBER;
	bodyLen = 0;
	cmd = c;
	fd = 0;
	time = 0;
	microTime = 0;
	clientIp = 0;
	m_decodeSize = 0;
	uid = 0;
	delmsg = d;

	m_msg = NULL;
	group = NULL;
}

CFirePacket::~CFirePacket()
{
	if(delmsg && m_msg)
		delete m_msg;
}

bool CFirePacket::Encode(IBuffer *pBuffer)
{
	if(cmd == PROTOCOL_ACCESS_ANSWER
	|| cmd == PROTOCOL_ACCESS_BROAD_CAST
	|| cmd == PROTOCOL_ACCESS_SEND
	|| cmd == PROTOCOL_ACCESS_GROUP_SEND
	|| cmd == PROTOCOL_DELIVER
	|| cmd == PROTOCOL_ADMIN
	|| cmd == PROTOCOL_BOT)
	{
		if(!EncodePB())
			return false;
	}

	if(pBuffer == NULL)
	{
		return false;
	}
	pBuffer->Clear();
	CBufferWriter writer(pBuffer);
//	writer.SetSwapByte(true);

	PACKET_ENCODE(UInt16, PACKET_HEAD_MAGIC_NUMBER);
	PACKET_ENCODE(UInt32,body.GetSize());
	PACKET_ENCODE(UInt16,cmd);
	PACKET_ENCODE(UInt32,fd);
	PACKET_ENCODE(UInt32,time);
	PACKET_ENCODE(UInt32,microTime);
	PACKET_ENCODE(UInt32,clientIp); //client ip
	PACKET_ENCODE(Bytes, body.GetConstBuffer(), body.GetSize());

	//info_log("%s",String::b2s((const char*)pBuffer->GetConstBuffer(),pBuffer->GetSize()).c_str());

	return true;
}

bool CFirePacket::Decode(IBuffer *pBuffer)
{
	m_decodeSize = 0;
	if(pBuffer == NULL)
	{
		return false;
	}
	CBufferReader reader(pBuffer);
//	reader.SetSwapByte(true);

	PACKET_DECODE(UInt16, head);
	if(head != PACKET_HEAD_MAGIC_NUMBER)
	{
		error_log("get head failed:%x",head);
		m_decodeSize = 2;
		return false;
	}

	if(PACKET_HEADER_SIZE > pBuffer->GetSize())
	{
		return false;
	}

	PACKET_DECODE(UInt32, bodyLen);
	PACKET_DECODE(UInt16, cmd);
	PACKET_DECODE(UInt32, fd);
	PACKET_DECODE(UInt32, time);
	PACKET_DECODE(UInt32, microTime);
	PACKET_DECODE(UInt32, clientIp);

	if((unsigned)(PACKET_HEADER_SIZE + bodyLen) > pBuffer->GetSize()){
		return false;
	}

	m_decodeSize = bodyLen + PACKET_HEADER_SIZE;

	if(!body.SetSize(bodyLen)){
		error_log("buf capacity insufficent expect %u but only %u",bodyLen,body.GetCapacity());
		return true;
	}

	PACKET_DECODE(Bytes, body.GetNativeBuffer(), bodyLen);

	//debug_log("body len=%u-%u:\n%s",bodyLen,body.GetSize(),body.ToString().c_str());

	if(cmd == PROTOCOL_ACCESS_TRANSFER
	|| cmd == PROTOCOL_DELIVER
	|| cmd == PROTOCOL_ADMIN
	|| cmd == PROTOCOL_BOT)
		return DecodePB();

	return true;
}


bool CFirePacket::SetBody(IBuffer* pBuf){
	if(pBuf == NULL){
		return false;
	}

	if(pBuf->GetSize() > MAX_PROTOCOL_BODY_SIZE){
		return false;
	}

	body.Clear();
	CBufferWriter writer(&body);
	writer.AppendBytes(pBuf->GetConstBuffer(),pBuf->GetSize());

	return true;
}

bool CFirePacket::DecodePB()
{
	CReceiveProtocol rp;
	if(!rp.Decode(&body))
		return false;
	m_msg = rp.msg;
	return true;
}
bool CFirePacket::EncodePB()
{
	CSendCProtocol sp;
	sp.msg = m_msg;
	sp.group = group;
	return sp.Encode(&body);
}

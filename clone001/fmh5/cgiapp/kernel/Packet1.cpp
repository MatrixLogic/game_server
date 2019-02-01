/*
 * Packet.cpp
 *
 *  Created on: 2012-2-10
 *      Author: dada
 */

#include "Packet1.h"

#define PACKET_HEAD_MAGIC_NUMBER	0x81
#define PACKET_TAIL_MAGIC_NUMBER	0x7e

static byte PacketCheckSum(const byte *buffer, uint32_t size)
{
	static byte s_salt[7] = {0x74, 0xe8, 0xa6, 0x97, 0x2e, 0x3c,  0x59};
	byte sum = 0;
	for(uint32_t i = 0; i < size; i++)
	{
		sum ^= buffer[i];
	}
	sum ^= s_salt[size % sizeof(s_salt)];
	return sum;
}

CPacket1::CPacket1() :
	ChannelId(-1),
	PacketType(PT_Unknow),
	PacketFlag(0),
	From(0),
	To(0),
	SequenceNo(0),
	RetryTime(0),
	UserId(0),
	Key(0),
	ProtocolType(0),
	ProtocolFlag(0),
	ProtocolResult(0),
	m_decodeSize(0)
{
}

static byte s_baReserveData[8] = {0};

bool CPacket1::Encode(IBuffer *pBuffer)
{
	if(pBuffer == NULL)
	{
		return false;
	}
	pBuffer->Clear();
	CBufferWriter writer(pBuffer);
	writer.SetSwapByte(true);
	uint32_t packetLength = PACKET_HEADER_SIZE + ProtocolBody.GetSize() + sizeof(byte);
	if(packetLength > UINT16_MAX)
	{
		return false;
	}
	PACKET_ENCODE(UInt16, (uint16_t)packetLength);
	PACKET_ENCODE(Byte, PACKET_HEAD_MAGIC_NUMBER);
	PACKET_ENCODE(Byte, 0);
	PACKET_ENCODE(Byte, PacketType);
	PACKET_ENCODE(Byte, PacketFlag);
	PACKET_ENCODE(Byte, From);
	PACKET_ENCODE(Byte, To);
	PACKET_ENCODE(UInt32, SequenceNo);
	PACKET_ENCODE(Byte, RetryTime);
	PACKET_ENCODE(Bytes, s_baReserveData, 3);
	PACKET_ENCODE(UInt32, UserId);
	PACKET_ENCODE(UInt32, Key);
	PACKET_ENCODE(UInt16, ProtocolBody.GetSize());
	PACKET_ENCODE(Byte, PACKET_HEADER_SIZE);
	PACKET_ENCODE(Byte, 0);
	PACKET_ENCODE(Byte, MainProtocolType);
	PACKET_ENCODE(Byte, SubProtocolType);
	PACKET_ENCODE(Byte, ProtocolFlag);
	PACKET_ENCODE(Byte, ProtocolResult);
	PACKET_ENCODE(Bytes, ProtocolBody.GetConstBuffer(), ProtocolBody.GetSize());
	PACKET_ENCODE(Byte, PACKET_TAIL_MAGIC_NUMBER);
	pBuffer->SetAt(27, PacketCheckSum(pBuffer->GetConstBuffer() + 28, pBuffer->GetSize() - 29));
	pBuffer->SetAt(3, PacketCheckSum(pBuffer->GetConstBuffer() + 4, pBuffer->GetSize() - 5));
	return true;
}

bool CPacket1::Decode(IBuffer *pBuffer)
{
	m_decodeSize = 0;
	if(pBuffer == NULL)
	{
		return false;
	}
	CBufferReader reader(pBuffer);
	reader.SetSwapByte(true);
	uint16_t packetLength = 0;
	PACKET_DECODE(UInt16, packetLength);
	if(packetLength > pBuffer->GetSize())
	{
		return false;
	}
	if(packetLength < PACKET_HEADER_SIZE + sizeof(byte))
	{
		return false;
	}
	byte head = 0;
	PACKET_DECODE(Byte, head);
	if(head != PACKET_HEAD_MAGIC_NUMBER)
	{
		return false;
	}
	byte tail = pBuffer->GetAt(packetLength - sizeof(byte));
	if(tail != PACKET_TAIL_MAGIC_NUMBER)
	{
		return false;
	}
	m_decodeSize = packetLength;
	byte checkSum = 0;
	PACKET_DECODE(Byte, checkSum);
	if(PacketCheckSum(pBuffer->GetConstBuffer() + 4, packetLength - 5) != checkSum)
	{
		return false;
	}
	byte packetType = 0;
	PACKET_DECODE(Byte, packetType);
	PacketType = (EPacketType)packetType;
	PACKET_DECODE(Byte, PacketFlag);
	PACKET_DECODE(Byte, From);
	PACKET_DECODE(Byte, To);
	PACKET_DECODE(UInt32, SequenceNo);
	PACKET_DECODE(Byte, RetryTime);
	PACKET_DECODE_SKIP(3);
	PACKET_DECODE(UInt32, UserId);
	PACKET_DECODE(UInt32, Key);
	uint16_t bodySize = 0;
	PACKET_DECODE(UInt16, bodySize);
	byte bodyOffset = 0;
	PACKET_DECODE(Byte, bodyOffset);
	if(bodyOffset == 0)
	{
		bodyOffset = PACKET_HEADER_SIZE;
	}
	else if(bodyOffset < PACKET_HEADER_SIZE)
	{
		return false;
	}
	if(bodyOffset + bodySize >= packetLength)
	{
		return false;
	}
	byte bodyCheckSum = 0;
	PACKET_DECODE(Byte, bodyCheckSum);
	if(PacketCheckSum(pBuffer->GetConstBuffer() + 28, bodyOffset + bodySize - 28) != bodyCheckSum)
	{
		return false;
	}
	PACKET_DECODE(Byte, MainProtocolType);
	PACKET_DECODE(Byte, SubProtocolType);
	PACKET_DECODE(Byte, ProtocolFlag);
	PACKET_DECODE(Byte, ProtocolResult);
	ProtocolBody.Clear();
	if(bodyOffset != 0)
	{
		PACKET_DECODE_SKIP(bodyOffset - PACKET_HEADER_SIZE);
		if(!ProtocolBody.SetSize(bodySize))
		{
			return false;
		}
		PACKET_DECODE(Bytes, ProtocolBody.GetNativeBuffer(), bodySize);
	}
	PACKET_DECODE_SKIP(1);
	return true;
}

uint32_t CPacket1::GetDecodeSize()
{
	return m_decodeSize;
}


/******************************************************************/

#define SG17_PACKET_HEAD_MAGIC_NUMBER 21961

CSG17Packet::CSG17Packet(uint16_t c)
{
	ChannelId = -1;
	head = SG17_PACKET_HEAD_MAGIC_NUMBER;
	bodyLen = 0;
	cmd = c;
	fd = 0;
	time = 0;
	microTime = 0;
	clientIp = 0;
	m_decodeSize = 0;

	m_msg = NULL;
}

CSG17Packet::~CSG17Packet()
{
	if(m_msg)
		delete m_msg;
}

bool CSG17Packet::Encode(IBuffer *pBuffer)
{
	if(cmd == PROTOCOL_DELIVER
	|| cmd == PROTOCOL_ADMIN
	|| cmd == PROTOCOL_BOT
	|| cmd == PROTOCOL_ACCESS_TRANSFER)
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

	PACKET_ENCODE(UInt16, SG17_PACKET_HEAD_MAGIC_NUMBER);
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

bool CSG17Packet::Decode(IBuffer *pBuffer)
{
	m_decodeSize = 0;
	if(pBuffer == NULL)
	{
		return false;
	}
	CBufferReader reader(pBuffer);
//	reader.SetSwapByte(true);

	PACKET_DECODE(UInt16, head);
	if(head != SG17_PACKET_HEAD_MAGIC_NUMBER)
	{
		error_log("get head failed:%x",head);
		m_decodeSize = 2;
		return false;
	}

	if(SG17_PACKET_HEADER_SIZE > pBuffer->GetSize())
	{
		return false;
	}

	PACKET_DECODE(UInt32, bodyLen);
	PACKET_DECODE(UInt16, cmd);
	PACKET_DECODE(UInt32, fd);
	PACKET_DECODE(UInt32, time);
	PACKET_DECODE(UInt32, microTime);
	PACKET_DECODE(UInt32,clientIp);

	if((unsigned)(SG17_PACKET_HEADER_SIZE + bodyLen) > pBuffer->GetSize()){
		return false;
	}

	m_decodeSize = bodyLen + SG17_PACKET_HEADER_SIZE;

	if(!body.SetSize(bodyLen)){
		error_log("buf capacity insufficent expect %u but only %u",bodyLen,body.GetCapacity());
		return true;
	}

	PACKET_DECODE(Bytes, body.GetNativeBuffer(), bodyLen);

	//debug_log("body len=%u-%u:\n%s",bodyLen,body.GetSize(),body.ToString().c_str());

	if(cmd == PROTOCOL_DELIVER
	|| cmd == PROTOCOL_ADMIN
	|| cmd == PROTOCOL_BOT
	|| cmd == PROTOCOL_ACCESS_ANSWER
	|| cmd == PROTOCOL_ACCESS_SEND
	|| cmd == PROTOCOL_ACCESS_BROAD_CAST)
		return DecodePB();

	return true;
}


bool CSG17Packet::SetBody(IBuffer* pBuf){
	if(pBuf == NULL){
		return false;
	}

	if(pBuf->GetSize() > SG17_MAX_PROTOCOL_BODY_SIZE){
		return false;
	}

	body.Clear();
	CBufferWriter writer(&body);
	writer.AppendBytes(pBuf->GetConstBuffer(),pBuf->GetSize());

	return true;
}

bool CSG17Packet::DecodePB()
{
	CSG17ReceiveProtocol rp;
	if(!rp.Decode(&body))
		return false;
	m_msg = rp.msg;
	return true;
}
bool CSG17Packet::EncodePB()
{
	CSG17SendCProtocol sp;
	sp.msg = m_msg;
	return sp.Encode(&body);
}

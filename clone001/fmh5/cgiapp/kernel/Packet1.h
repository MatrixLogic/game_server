/*
 * Packet.h
 *
 *  Created on: 2012-2-10
 *      Author: dada
 */

#ifndef PACKET_H_
#define PACKET_H_

#include "Common.h"

#define PACKET_HEADER_SIZE 32
#define MAX_PROTOCOL_BODY_SIZE 0x20000
#define MAX_PACKET_SIZE (PACKET_HEADER_SIZE + MAX_PROTOCOL_BODY_SIZE + sizeof(byte))

class IPacket : public IBase
{
public:
	virtual string ToString() const
	{
		return "";
	}
};

class IEncodable : public IBase
{
public:
	virtual bool Encode(IBuffer *pBuffer) = 0;
};

class IDecodable : public IBase
{
public:
	virtual bool Decode(IBuffer *pBuffer) = 0;
};

class IPacketSend : public IPacket, public IEncodable
{
};

class IPacketReceive : public IPacket, public IDecodable
{
};

///协议包结构
//WORD	PacketLength	包体长度，包含PacketLength和Tail
//BYTE	Head	包头=0x81
//BYTE	CheckSum	校验和，不包含PacketLength、Head、CheckSum和Tail，其他字节异或
//BYTE	PacketType	包类型
//BYTE	PacketFlag	包标记
//BYTE	From	来源
//BYTE	To	目标
//DWORD	SequenceNo	序列号
//BYTE	RetryTime	重试次数
//BYTE[3]	Reserve	保留
//DWORD	UserId	用户ID
//DWORD	Key	会话KEY
//WORD	BodyLength 协议数据长度
//BYTE	BodyOffset	协议数据位置，从Head开始的偏移
//BYTE	BodyCheckSum	协议数据未加密校验和，从MainProtocolType到Body所有字节异或
//BYTE	MainProtocolType	主协议号
//BYTE	SubProtocolType	子协议号
//BYTE	ProtocolFlag	协议标记
//BYTE	ProtocolResult	协议结果
//BYTE[BodyLength]	Body	包体
//BYTE	Tail	包尾=0x7e

///协议包类型
// +--+--+--+--+--+--+--+--+
// |VA|     RS    |FS|TS|RQ|
// +--+--+--+--+--+--+--+--+
//   7  6  5  4  3  2  1  0
// RQ: 是否答复，0为请求，1为答复
// FS: 原始请求发送方是否服务器，0为客户端，1为服务器
// TS: 原始请求接收方是否服务器，0为客户端，1为服务器
// VA: 是否有效协议包类型，1为有效
// RS: 保留
enum EPacketType
{
	PT_Unknow = 0,	//未知协议
	PT_CCRequest = 0x80,	//C->C请求
	PT_CCReply = 0x81,	//C->C答复
	PT_CSRequest = 0x82,	//C->S请求
	PT_CSReply = 0x83,	//S->C答复，发送方为服务器，接收方为客户端
	PT_SCRequest = 0x84,	//S->C请求
	PT_SCReply = 0x85,	//C->S答复，发送方为客户端，接收方为服务器
	PT_SSRequest = 0x86,	//S->S请求
	PT_SSReply = 0x87	//S->S答复
};

#define COMBINE_PROTOCOL_TYPE(MainProtocolType, SubProtocolType) (((uint16_t)MainProtocolType<<8) | (uint16_t)SubProtocolType)


/// 协议编解码

#define DEFINE_SEND_PACKET_BEGIN(Name)	\
	class Name : public IPacketSend	\
	{	\
	public:	\

#define DEFINE_SEND_PACKET_END	\
	};	\

#define DEFINE_EMPTY_SEND_PACKET(Name)	\
	class Name : public IPacketSend	\
	{	\
	public:	\
		virtual bool Encode(IBuffer *pBuffer) { return true; }	\
	};	\

#define IMPLEMENT_PACKET_ENCODE_BEGIN	\
	virtual bool Encode(IBuffer *pBuffer)	\
	{	\
		if(pBuffer == NULL)	\
		{	\
			return false;	\
		}	\
		pBuffer->Clear();	\
		CBufferWriter writer(pBuffer);	\
		writer.SetSwapByte(true);	\

#define IMPLEMENT_PACKET_ENCODE_END	\
		return true;	\
	}	\

#define PACKET_ENCODE(type, ...)	if(!writer.Append##type(__VA_ARGS__)) return false

#define DEFINE_RECEIVE_PACKET_BEGIN(Name)	\
	class Name : public IPacketReceive	\
	{	\
	public:	\

#define DEFINE_RECEIVE_PACKET_END	\
	};	\

#define DEFINE_EMPTY_RECEIVE_PACKET(Name)	\
	class Name : public IPacketReceive	\
	{	\
	public:	\
		virtual bool Decode(IBuffer *pBuffer) { return true; }	\
	};	\

#define IMPLEMENT_PACKET_DECODE_BEGIN	\
	virtual bool Decode(IBuffer *pBuffer)	\
	{	\
		if(pBuffer == NULL)	\
		{	\
			return false;	\
		}	\
		CBufferReader reader(pBuffer);	\
		reader.SetSwapByte(true);	\

#define IMPLEMENT_PACKET_DECODE_END	\
		return true;	\
	}	\

#define PACKET_DECODE(type, ...)	if(!reader.Get##type(__VA_ARGS__)) return false

#define PACKET_DECODE_SKIP(offset)	if(!reader.Skip(offset)) return false

#define DEFINE_PROTOCOL_TYPE(MainType, SubType)	\
	static const byte MainProtocolType = MainType;	\
	static const byte SubProtocol = SubType;	\
	static const uint16_t ProtocolType = COMBINE_PROTOCOL_TYPE(MainType, SubType);	\

#define IMPLEMENT_TO_STRING(format, ...)	\
	virtual string ToString() const	\
	{	\
		string s;	\
		String::Format(s, format, __VA_ARGS__);	\
		return s;	\
	}	\

/// 协议包头数据
class CPacket1 :
	public IPacket,
	public IEncodable,
	public IDecodable
{

public:

	int ChannelId;	//通道ID
	EPacketType PacketType;	//协议包类型协议版本
	byte PacketFlag;	//包标记
	byte From;	//来源
	byte To;	//目标
	uint32_t SequenceNo;	//序列号
	byte RetryTime;	//重试次数
	uint32_t UserId;	//用户ID
	uint32_t Key;	//会话KEY
	union
	{
		uint16_t ProtocolType;	//协议类型
		struct
		{
			byte SubProtocolType;	//子协议号
			byte MainProtocolType;	//主协议号
		};
	};
	byte ProtocolFlag;	//协议标记
	byte ProtocolResult;	//协议结果
	CStaticBuffer<MAX_PROTOCOL_BODY_SIZE> ProtocolBody;	//包体

public:

	CPacket1();

	virtual bool Encode(IBuffer *pBuffer);
	virtual bool Decode(IBuffer *pBuffer);

	uint32_t GetDecodeSize();

	virtual bool IsRequest() { return !(PacketType & 0x1); }
	virtual bool IsReply() { return PacketType & 0x1; }

	IMPLEMENT_TO_STRING("cid=%d&t=0x%02X&sn=%u&uid=%u&key=0x%08X&pt=0x%04hX&pr=%u&bs=%u",
			ChannelId, (unsigned)PacketType, SequenceNo, UserId, Key, ProtocolType, (unsigned)ProtocolResult, ProtocolBody.GetSize())

private:
	uint32_t m_decodeSize;

};

/*******************************************************************************/

#define SG17_PACKET_HEADER_SIZE sizeof(BattleHead)
#define SG17_MAX_PROTOCOL_BODY_SIZE 0x20000
#define SG17_MAX_PACKET_SIZE (SG17_PACKET_HEADER_SIZE + SG17_MAX_PROTOCOL_BODY_SIZE + sizeof(byte))


// 协议包头数据
class CSG17Packet: public IPacket, public IEncodable, public IDecodable {
public:

	int ChannelId;	//通道ID

	uint16_t head;
	uint32_t bodyLen;
	uint16_t cmd;
	uint32_t fd;
	uint32_t time;
	uint32_t microTime;
	uint32_t clientIp;	//原始客户端的ip

	CStaticBuffer<SG17_MAX_PROTOCOL_BODY_SIZE> body;	//包体


	CSG17Packet(uint16_t c = 0);
	~CSG17Packet();

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

DEFINE_RECEIVE_PACKET_BEGIN(CSG17ReceiveProtocol)
	Message* msg;
	IMPLEMENT_PACKET_DECODE_BEGIN
	reader.SetSwapByte(false);
		PACKET_DECODE(PBMsg, msg);
	IMPLEMENT_PACKET_DECODE_END
DEFINE_RECEIVE_PACKET_END


DEFINE_SEND_PACKET_BEGIN(CSG17SendCProtocol)
	Message* msg;
	IMPLEMENT_PACKET_ENCODE_BEGIN
	writer.SetSwapByte(false);
		PACKET_ENCODE(PBMsg, msg);
	IMPLEMENT_PACKET_ENCODE_END
DEFINE_SEND_PACKET_END


#endif /* PACKET_H_ */

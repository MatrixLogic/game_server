/*
 * CBroadcast.h
 *
 *  Created on: 2012-8-20
 *      Author: Administrator
 */

#ifndef CBROADCAST_H_
#define CBROADCAST_H_
#include "Kernel.h"
#define UPDATE_SAVE_INTERVAL 30
#define SESSION_SECRET_KEY_SIZE 16
#define PROTOCOL_TYPE_REFRESH COMBINE_PROTOCOL_TYPE(1, 3)
#define PROTOCOL_TYPE_LOGIN COMBINE_PROTOCOL_TYPE(1, 1)
#define PROTOCOL_TYPE_CHAT_SUBMIT COMBINE_PROTOCOL_TYPE(4, 1)
#define PROTOCOL_TYPE_FORBID_CHAT COMBINE_PROTOCOL_TYPE(5, 1)
		DEFINE_RECEIVE_PACKET_BEGIN(CLoginReceive)
			uint32_t SessionKey;
			CStaticBuffer<SESSION_SECRET_KEY_SIZE> SecretKey;
			IMPLEMENT_PACKET_DECODE_BEGIN
			PACKET_DECODE(UInt32, SessionKey);
			PACKET_DECODE(Bytes, SecretKey.GetNativeBuffer(), SecretKey.GetFreeCapacity());
			IMPLEMENT_PACKET_DECODE_END
			// IMPLEMENT_TO_STRING("Platform=%u&Name=%s", (unsigned)SessionKey,  );
		DEFINE_RECEIVE_PACKET_END


		DEFINE_SEND_PACKET_BEGIN(CLoginSend)
			DEFINE_PROTOCOL_TYPE(1, 1)
			byte platform;
			string name;
			IMPLEMENT_PACKET_ENCODE_BEGIN
			writer.SetSwapByte(false);
				PACKET_ENCODE(Byte, platform);
				PACKET_ENCODE(String,name);
			IMPLEMENT_PACKET_ENCODE_END
			//IMPLEMENT_TO_STRING("SessionKey=0x%08X&SecretKey=%s", SessionKey, SecretKey.ToString().c_str())
		DEFINE_SEND_PACKET_END



DEFINE_SEND_PACKET_BEGIN(CChatRequest)
	byte ToType;
	uint32_t ToId;
	string Message;
	IMPLEMENT_PACKET_ENCODE_BEGIN
	writer.SetSwapByte(false);
		PACKET_ENCODE(Byte, ToType);
		PACKET_ENCODE(UInt32, ToId);
		PACKET_ENCODE(String, Message);
	IMPLEMENT_PACKET_ENCODE_END
	IMPLEMENT_TO_STRING("ToType=%u&ToId=%u&Message=%s", (unsigned)ToType, ToId, Message.c_str())
DEFINE_SEND_PACKET_END

DEFINE_RECEIVE_PACKET_BEGIN(CChatReply)
	string Message;
	IMPLEMENT_PACKET_DECODE_BEGIN
		PACKET_DECODE(String, Message);
	IMPLEMENT_PACKET_DECODE_END
	IMPLEMENT_TO_STRING("Message=%s", Message.c_str())
DEFINE_RECEIVE_PACKET_END

//5-1
//指定用户禁言

DEFINE_SEND_PACKET_BEGIN(CForbidChatRequest)
	uint32_t ForbidUid;
	uint32_t ForbidTs;
	IMPLEMENT_PACKET_ENCODE_BEGIN
	//writer.SetSwapByte(false);
	PACKET_ENCODE(UInt32, ForbidUid);
	PACKET_ENCODE(UInt32, ForbidTs);
	IMPLEMENT_PACKET_ENCODE_END
	IMPLEMENT_TO_STRING("ForbidUid=%u&ForbidTs=%u", ForbidUid, ForbidTs)
DEFINE_SEND_PACKET_END

DEFINE_RECEIVE_PACKET_BEGIN(CForbidChatReply)
	uint32_t Result;
	string Message;
	IMPLEMENT_PACKET_DECODE_BEGIN
	PACKET_DECODE(UInt32, Result);
	PACKET_DECODE(String, Message);
	IMPLEMENT_PACKET_DECODE_END
	IMPLEMENT_TO_STRING("Message=%s,Result = %u", Message.c_str(),Result)
DEFINE_RECEIVE_PACKET_END

#endif /* CBROADCAST_H_ */

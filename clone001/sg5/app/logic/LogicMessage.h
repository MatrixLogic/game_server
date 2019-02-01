#ifndef __LOGIC_MESSAGE_H__
#define __LOGIC_MESSAGE_H__

#include "LogicInc.h"

enum MessageType
{
	MSG_SYS = 0,
	MSG_CHAT,
	MSG_REQUEST
};
enum TowardType
{
	TOWARD_RECV = 0,
	TOWARD_SEND,
	TOWARD_TYPE_ALL
};
enum UserMsgStatus
{
	UMS_UN_READ = 0,	//未读
	UMS_READED,			//已读
	UMS_REMOVE	//删除
};
enum MessageStatus
{
	MS_UN_PROCESS = 0,
	MS_PROCESSED,
	MS_ACCEPT,
	MS_REJECT,
	MS_CANCEL,
	MS_STATUS_ALL
};

enum MessageUserType
{
	MUT_NONE = 0,	//无
	MUT_ALL = 1,	//所有用户
	MUT_NON_APP_USER = 2	//非应用用户
};

class CLogicMessage
{
public:
	int SetMessageContent(uint64_t messageId, const string &content);

	int AddSysMessage(const DataUserBasic &user, const string &content);
	int AddSysMessage(unsigned uid, const string &content);
	int GetNewSysMessage(unsigned uid, vector<DataMessage> &msgVec);

	int Chat(unsigned uid, unsigned toUid, const string &message);
	int GetChat(unsigned uid, int toward, vector<DataMessage> &msgVec);
	int RemoveChat(unsigned uid, uint64_t messageId);

	int AddRequest(uint64_t &messageId, unsigned uid, unsigned toUid, const Json::Value &content);
	int AddRequest(unsigned uid, unsigned toUid, const Json::Value &content);
	int ReplyRequest(unsigned uid, unsigned toUid, uint64_t message_id, MessageStatus mstatus, const Json::Value &content);
	int CancelRequest(unsigned uid, const string &mtype, uint64_t message_id = 0);
	int CancelRequest(unsigned uid, TowardType toward = TOWARD_TYPE_ALL);

	int GetRequest(unsigned uid, const set<string> &mtypes, int toward, int status, vector<DataMessage> &msgVec);
	int GetRequest(unsigned uid, const string &mtype, int toward, int status, vector<DataMessage> &msgVec);
	int GetRequestJson(unsigned uid, const set<string> &mtypes, int toward, int status, Json::Value &requests);

	int AddPlatformRequest(uint64_t &messageId, unsigned fromUid, const OPUserInfo &toUser, const Json::Value &content);
	int SetMessagesTarget(vector<uint64_t> messageIds, unsigned toUid);
	int CheckAllianceJoinMsg(unsigned aid);

protected:
	int GetMessageId(uint64_t &msgid);

	//增加一个单向的消息,如系统消息
	int AddMessage(uint64_t &msgid, const DataUserBasic &user, UserMsgStatus status,
			MessageType type, const string &content, MessageStatus mstatus);
	int AddMessage(const DataUserBasic &user, UserMsgStatus status,
			MessageType type, const string &content, MessageStatus mstatus);

	//增加一个双向的消息
	int AddMessage(uint64_t &msgid, const DataUserBasic &user, UserMsgStatus status,
			const DataUserBasic &toUser, UserMsgStatus toStatus,
			MessageType type, const string &content,
			MessageStatus mstatus, uint64_t reply_id = 0);
	int AddMessage(const DataUserBasic &user, UserMsgStatus status,
			const DataUserBasic &toUser, UserMsgStatus toStatus,
			MessageType type, const string &content,
			MessageStatus mstatus, uint64_t reply_id = 0);

	int AutoRejectRequest(DataMessage &message);
};

#endif

/*
 * CgiEmail.cpp
 *
 *  Created on: 2012-7-26
 *      Author: aaron
 */

#include "LogicInc.h"
class CCgiEmail: public CCgiBase {
public:
	CCgiEmail() :
			CCgiBase("email") {
		// TODO Auto-generated constructor stub
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_GET_UID);
		SetFeature(CF_CHECK_SESSION);
		SetFeature(CF_CHECK_PLATFORM);
		SetFeature(CF_CHECK_HASH);
		SetFeature(CF_CHECK_VERSION);
		SetFeature(CF_APPEND_HASH);
		SetFeature(CF_PARSE_DATA);
		SetFeature(CF_CRYPT_DATA);
		SetFeature(CF_DECRYPT_DATA);
		SetFeature(CF_COMPRESS_OUT);
	}
	CGI_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("getinboxemaillist", GetInboxEmailList)
	CGI_SET_ACTION_MAP("getoutboxemaillist", GetOutboxEmailList)
	CGI_SET_ACTION_MAP("postemail",PostEmail)
	CGI_SET_ACTION_MAP("reademail",ReadEmail)
	CGI_SET_ACTION_MAP("deleteemail",DeleteEmail)
	CGI_SET_ACTION_MAP("getattachments",GetAttachments)
	CGI_ACTION_MAP_END


	~CCgiEmail() {
		// TODO Auto-generated destructor stub
	}
		// 获取收件箱列表
	int GetInboxEmailList() {
		int ret;
		unsigned uid;
		if (!Json::GetUInt(m_data, "uid", uid)) {
			return R_ERR_PARAM;
		}

		CLogicEmail logicEmail;
		vector<DataEmail> vEmail;
		ret = logicEmail.GetInboxEmailListJson(uid, vEmail ,m_jsonResult);
		if (ret != 0) {
			error_log("[GetEmailList fail][ret=%d,uid=%u]", ret, uid);
			DB_ERROR_RETURN_MSG("get_email_list_fail");
			return ret;
		}
		CGI_SEND_LOG(
				"action=GetEmailListJson&uid=%u&rcount=%u", m_uid,   (unsigned)m_jsonResult["emaillist"].size());
		return 0;
	}

	// 获取发件箱列表
	int GetOutboxEmailList() {
		int ret;
		unsigned uid;
		if (!Json::GetUInt(m_data, "uid", uid)) {
			return R_ERR_PARAM;
		}
		CLogicEmail logicEmail;
		vector<DataEmail> vEmail;
		ret = logicEmail.GetOutboxEmailListJson(uid, vEmail ,m_jsonResult);
		if (ret != 0) {
			error_log("[GetEmailList fail][ret=%d,uid=%u]", ret, uid);
			return ret;
		}

		CGI_SEND_LOG(
				"action=GetEmailListJson&uid=%u&rcount=%u", m_uid,   (unsigned)m_jsonResult["emaillist"].size());
		return R_SUCCESS;
	}
	// 发邮件
	int PostEmail()
	{
		int ret = 0;
		CLogicEmail clogicemail;
		DataEmail data;
		vector<uint64_t> vto_uid;
		uint64_t uto_uid = 0;
		Json::Value attachments;
		Json::Value to_uid;
		CLogicIdCtrl logicIdCtrl;
		//uint64_t curId;
		//ret = logicIdCtrl.GetCurrentId(KEY_UID_CTRL,curId);

		if(!Json::GetArray(m_data,"to_uid",to_uid))
		{
			return R_ERR_PARAM;
		}
		unsigned int iCount = to_uid.size();
		// 获取多个收件人
		for( unsigned int i = 0; i < iCount; i++)
		{
			if(!Json::GetUInt64(to_uid,i,uto_uid))
			{
				return R_ERR_PARAM;
			}
			//if(uto_uid <= curId && uto_uid >= Config::GetIntValue(CONFIG_UID_MIN))
			{
				vto_uid.push_back(uto_uid);
			}
		}
		if(vto_uid.size() < 1)
		{
			PARAM_ERROR_RETURN_MSG("check_uid_faile");
		}
		if(!Json::GetString(m_data,"title",data.title))
		{
			return R_ERR_PARAM;
		}
		if(!Json::GetString(m_data,"text",data.text))
		{
			return R_ERR_PARAM;
		}
		Json::FastWriter writer;
		string attach_flag="0";
		if(!Json::GetString(m_data,"attach_flag",attach_flag))
		{
			//error_log("[parse fail]");
			//return R_ERR_PARAM;
		}
		if(attach_flag == "1")
		{
			if(!Json::GetObjectOrArray(m_data,"attachments",attachments))
			{
				//error_log("[parse fail]");
				//data.attach_flag = 0;
				attach_flag="0";
				//return R_ERR_PARAM;
			}
			data.attachments = writer.write(attachments);
		}
		data.attach_flag = atoi(attach_flag.c_str());
		data.post_ts = Time::GetGlobalTime();
		data.read_ts = 0;
		data.uid=m_uid;
		ret = clogicemail.AddEmail(data,vto_uid,OpenPlatform::GetType());
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}
	// 读邮件
	int ReadEmail()
	{
		int ret = 0;
		CLogicEmail clogicemail;
		DataEmail data;
		if(!Json::GetUInt64(m_data,"email_id",data.email_id))
		{
			return R_ERR_PARAM;
		}
		ret = clogicemail.ReadEmail(m_uid,data.email_id,data);
		if(0 != ret)
		{
			return ret;
		}
		m_jsonResult["email_id"]=Convert::UInt64ToString(data.email_id);
		m_jsonResult["title"]=data.title;
		m_jsonResult["text"]=data.text;
		m_jsonResult["from_name"]=data.from_name;
		m_jsonResult["to_name"]=data.to_name;
		m_jsonResult["post_ts"]=data.post_ts;
		m_jsonResult["read_ts"]=data.read_ts;
		if(data.post_flag == 1)
		{
			m_jsonResult["from_uid"]=data.uid;
			m_jsonResult["to_uid"]=data.opposite_uid;
		}else{
			m_jsonResult["from_uid"]=data.opposite_uid;
			m_jsonResult["to_uid"]=data.uid;
		}

		return  0;
	}
	// 删除邮件
	int DeleteEmail()
	{
		int ret;
		unsigned uid;
		vector<uint64_t> vEmailIdList;
		uint64_t email_id;
		CLogicEmail clogicemail;
		if (!Json::GetUInt(m_data, "uid", uid)) {
			return R_ERR_PARAM;
		}
		Json::Value value;

		if(!Json::GetArray(m_data,"emailidlist",value))
		{
			return R_ERR_PARAM;
		}
		int iCount = value.size();
		vEmailIdList.resize(iCount);
		for(int i = 0; i < iCount; i++)
		{
			// 获取传入的要删除的email_id列表，存入vector容器vEmailIdList中
			if(!Json::GetUInt64(value,i,email_id))
			{
					return R_ERR_PARAM;
			}
			vEmailIdList.push_back(email_id);
		}
		ret = clogicemail.DeleteEmails(m_uid,vEmailIdList);
		if(0 != ret)
		{
			return ret;
		}
		return 0;
	}
	// 提取附件
	// 潜在bug:
	// 客户端取附件后，如未能放入背包中，会丢失附件
	int GetAttachments()
	{
		int ret;
		uint64_t email_id = 0;
		vector<uint64_t> vEmailIdList;
		vector<string> vattachments;
		CLogicEmail clogicemail;
		Json::Reader reader;
		Json::Value value;
		if(!Json::GetArray(m_data,"emailidlist",value))
		{
			return R_ERR_PARAM;
		}
		unsigned int uCount = value.size();
		//vEmailIdList.resize(uCount);
		//vattachments.resize(uCount);
		for(unsigned int i = 0; i < uCount ; i++)
		{
			if(!Json::GetUInt64(value,i,email_id))
			{
				return R_ERR_PARAM;
			}
			vEmailIdList.push_back(email_id);
		}

		ret = clogicemail.GetAttachments(m_uid,vEmailIdList,vattachments);
		if(0 != ret)
		{
			return ret;
		}
		uCount = vattachments.size();

		for(vector<string>::size_type i= 0;i < uCount; i++)
		{
			if(!reader.parse(vattachments[i],m_jsonResult["attachments"][i]))
			{
				 error_log("[parse fail][uid=%u,attachments=%s,i=%u]",m_uid,vattachments[i].c_str(),i);
			}

		}

		return 0;
	}
private:
};

CGI_MAIN(CCgiEmail)



















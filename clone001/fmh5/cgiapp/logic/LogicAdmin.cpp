#include "LogicAdmin.h"
#include "CBroadcast.h"
#include <sys/stat.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


map<string, AdminInfo> CLogicAdmin::adminList;
map<string, AdminSession> CLogicAdmin::adminSession;

int CLogicAdmin::AutoLogin(const string &name, string &skey){
	string url = Config::GetValue(CONFIG_AUTO_LOGIN_URL);
	if(url.empty())
		return 0;
	url += name;
	string response;
	if (!Network::HttpGetRequest(response, url))
		return 0;

	Json::Value result;
	Json::Reader reader;
	if (response.empty() || !reader.parse(response, result)
		|| !result.isMember("skey") || !result["skey"].isString())
		return 0;

	string skey1 = result["skey"].asString();
	if(skey == skey1){
		int level = GetLevel(name);
		if(level){
			AdminSession sess;
			sess.skey = skey;
			sess.activeTime = Time::GetGlobalTime();
			adminSession[name] = sess;

			return level;
		}
	}
	return 0;
}

int CLogicAdmin::UpdateAdminList()
{
	static time_t lastMTime = 0;
	string adminFile = Config::GetValue("admin_list");
	if (adminFile.empty())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	struct stat st;
	if (stat(adminFile.c_str(), &st) != 0)
	{
		REFUSE_RETURN_MSG("System error");
	}
	if (lastMTime != st.st_mtime)
	{
		FILE *fp = fopen(adminFile.c_str(), "r");
		if (!fp)
		{
			REFUSE_RETURN_MSG("System error");
		}
		char line[256];
		char *pch, *name, *pwd, *level;
		while (fgets(line, sizeof(line), fp) != NULL)
		{
            name = pwd = level = NULL;
            pch = line;
            while (*pch ==' ' || *pch == '\t') pch++;
            name = pch;
            while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != ' ' && *pch != '\t') pch++;
            if (*pch == '\0' || *pch == '\r' || *pch == '\n') continue;
            *pch++ = '\0';
            while (*pch ==' ' || *pch == '\t') pch++;
            pwd = pch;
            while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != ' ' && *pch != '\t') pch++;
            if (*pch == '\0' || *pch == '\r' || *pch == '\n') continue;
            *pch++ = '\0';
            while (*pch ==' ' || *pch == '\t') pch++;
            level = pch;
            while (*pch != '\0' && *pch != '\r' && *pch != '\n'  && *pch != ' ' && *pch != '\t') pch++;
            if (*pch != '\0') *pch = '\0';

            AdminInfo info;
            info.pwd = pwd;
            info.level = atoi(level);
            adminList[string(name)] = info;
		}
		lastMTime = st.st_mtime;
	}
	return 0;
}

int CLogicAdmin::CheckSession(const string &name, const string &skey)
{
	if (name.empty())
	{
		SESS_ERROR_RETURN_MSG("Name is empty");
	}
	map<string, AdminSession>::iterator it = adminSession.find(name);
	if (it == adminSession.end())
	{
		SESS_ERROR_RETURN_MSG("Session error");
	}
	if (Time::GetGlobalTime() - (it->second).activeTime > 3600)
	{
		SESS_ERROR_RETURN_MSG("Session timeout");
	}
	return 0;
}



int CLogicAdmin::LoginCheck(string const &name,string const &skey){
	map<string, AdminInfo>::iterator it1 = adminList.find(name);
	map<string, AdminSession>::iterator it2 = adminSession.find(name);
	if(it1 == adminList.end()){
		REFUSE_RETURN_MSG("The user not exist");
	}
	if(it2 == adminSession.end()){
		REFUSE_RETURN_MSG("The Session not exist");
	}
	else
	{
		if(it2 -> second.skey != skey)
		{
			REFUSE_RETURN_MSG("The skey not exist");
		}
	}
	return 0;
}

int CLogicAdmin::GetLevel(const string &name)
{
	map<string, AdminInfo>::iterator it = adminList.find(name);
	if (it == adminList.end())
		return ADMIN_LEVEL_0;
	return (it->second).level;
}

int CLogicAdmin::CheckLevel(const string &name, ADMIN_LEVEL needLevel)
{
	if (GetLevel(name) < needLevel)
	{
		REFUSE_RETURN_MSG("Insufficient permissions");
	}
	return 0;
}

int CLogicAdmin::Login(const string &name, const string &passwd, string &skey)
{
	int ret = UpdateAdminList();
	if (ret != 0)
		return ret;
	map<string, AdminInfo>::iterator it = adminList.find(name);
	if (it == adminList.end())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	if ((it->second).pwd != passwd)
	{
		REFUSE_RETURN_MSG("Passwd error");
	}

	AdminSession sess;
	sess.skey = Session::CreateSessionKey(0);
	sess.activeTime = Time::GetGlobalTime();
	adminSession[name] = sess;

	skey = sess.skey;
	return 0;
}

int CLogicAdmin::ModifyPassword(const string &name, const string &password, const string &newpwd)
{
	int ret = UpdateAdminList();
	if (ret != 0)
		return ret;
	map<string, AdminInfo>::iterator it = adminList.find(name);
	if (it == adminList.end())
	{
		REFUSE_RETURN_MSG("The user not exist");
	}
	if ((it->second).pwd != password)
	{
		REFUSE_RETURN_MSG("Passwd error");
	}
	(it->second).pwd = newpwd;
	string adminFile = Config::GetValue("admin_list");
	if (adminFile.empty())
	{
		REFUSE_RETURN_MSG("System error");
	}
	FILE *fp = fopen(adminFile.c_str(), "w");
	if (!fp)
	{
		REFUSE_RETURN_MSG("System error");
	}
	for (it = adminList.begin(); it != adminList.end(); it++)
	{
		fprintf(fp, "%s %s %d\n", (it->first).c_str(), (it->second).pwd.c_str(), (it->second).level);
	}
	fclose(fp);

	return 0;
}

int CLogicAdmin::ChangeBlockTs(const string &openid,unsigned block_time, const string &close_reason)
{
	if(block_time < gm_max && block_time != gm_none)
		return 0;

	int ret = 0;

	vector<unsigned> uids;
	CDataUserMapping dbUserMapping;
	ret = dbUserMapping.GetAllMapping(openid,uids);
	if(ret == 0){
		for(int i=0;i<uids.size();++i)
			AddForbidUser(uids[i],block_time);
			sleep(3);
	}

	CDataBlackUser dbBlackUser;
	DataBlackUser blackUser;
	ret = dbBlackUser.GetBlackUser(openid, blackUser);
	if (0 == ret)
	{
		blackUser.block_time = block_time;
		blackUser.close_reason = close_reason;
		ret = dbBlackUser.SetBlackUser(blackUser);
		if (0 != ret)
		{
			error_log("[SetBlackUser fail][ret=%d,openid=%s]", ret,openid.c_str());
			DB_ERROR_RETURN_MSG("set_black_user_fail!");
		}
		return 0;
	}
	else if (R_ERR_NO_DATA == ret)
	{
		blackUser.open_id = openid;
		blackUser.close_reason = close_reason;
		blackUser.block_time = block_time;
		ret = dbBlackUser.AddBlackUser(blackUser);
		if (0 != ret)
		{
			error_log("[AddBlackUser fail][ret=%d,openid=%s]", ret,openid.c_str());
			DB_ERROR_RETURN_MSG("add_black_user_fail!");
		}
		return 0;
	}
	return R_ERROR;
}

int CLogicAdmin::GetBlockTs(const string &openid, Json::Value &data)
{
	CDataBlackUser dbBlackUser;
	DataBlackUser blackUser;
	int ret = dbBlackUser.GetBlackUser(openid, blackUser);
	if (0 == ret)
	{
		data["blockts"] = blackUser.block_time;
		data["close_reson"] = blackUser.close_reason;
		return 0;
	}
	else if (R_ERR_NO_DATA == ret)
	{
		data["blockts"] = 0;
		data["close_reson"] = "";
		return 0;
	}
	return ret;
}

int CLogicAdmin::AddResourceOrEquip(unsigned uid, unsigned type, unsigned count, string reason, unsigned eqid)
{
	int ret = R_SUCCESS;

	unsigned realtype = type - 1; //参数类型到后台实际类型的转换

	Admin::AsycAdd * addmsg = new Admin::AsycAdd;
	Admin::AsycItem*  singleitem = addmsg->add_item();

	singleitem->set_uid(uid);
	singleitem->set_count(count);

	if (realtype >= e_asyn_max)  //装备
	{
		if (eqid < realtype)  //校验eqid的正确性，避免与资源类型无法区分
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}

		singleitem->set_id(eqid);
	}
	else  //资源
	{
		singleitem->set_id(realtype);
	}

	//发送协议
	CSG17Packet packet;
	packet.cmd = PROTOCOL_ADMIN;
	packet.m_msg = addmsg;

	ret = BattleSocket::Send(Config::GetZoneByUID(uid), &packet);

	if(ret)
	{
		ERROR_RETURN_MSG(5, "send system error");
	}

	CSG17Packet reply;
	ret = BattleSocket::Receive(Config::GetZoneByUID(uid), &reply);

	if(ret)
	{
		ERROR_RETURN_MSG(5, "recv system error");
	}

	Admin::AsycAddResp* rmsg = (Admin::AsycAddResp*) reply.m_msg;
	ret = rmsg->ret();

	if(ret)
	{
		ERROR_RETURN_MSG(5, "reply error.");
	}

	return 0;
}

void CLogicAdmin::Log(const string &admin, const string &op, const string &reason,
		unsigned uid, const string &value, const string &detail)
{
	string dir = Config::GetPath(ADMIN_OP_LOG_DIR);
	if (dir.empty())
		return;
	if (dir[dir.length()-1] != '/') dir += "/";
	char buf[256];
	time_t now;
	time( &now );
	struct tm *ptm;
	ptm = localtime(&now);
	snprintf(buf, sizeof(buf), "%sop_%04d%02d%02d.log", dir.c_str(), ptm->tm_year+1900,
			ptm->tm_mon+1, ptm->tm_mday);
	FILE *fp = fopen(buf, "a");
	if (!fp) return;
	fprintf(fp, "%04d%02d%02d %02d:%02d:%02d|%s|%s|%s|%u|%s\n", ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
			ptm->tm_hour,ptm->tm_min,ptm->tm_sec,admin.c_str(),op.c_str(),reason.c_str(),uid,value.c_str());
	fclose(fp);
	if (!detail.empty())
	{
		snprintf(buf, sizeof(buf), "%sdetail_%04d%02d%02d.log", dir.c_str(), ptm->tm_year+1900,
				ptm->tm_mon+1, ptm->tm_mday);
		fp = fopen(buf, "a");
		if (!fp) return;
		fprintf(fp, "%04d%02d%02d %02d:%02d:%02d|%s|%s|%u|%s\n", ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
				ptm->tm_hour,ptm->tm_min,ptm->tm_sec,admin.c_str(),op.c_str(),uid,detail.c_str());
		fclose(fp);
	}
}

void CLogicAdmin::AddCheckLog(const string &admin, const string &ip, const string &op, string &reason, unsigned uid, unsigned id, int count)
{
	reason.erase( remove( reason.begin(), reason.end(), '\r\n' ), reason.end());
	reason.erase( remove( reason.begin(), reason.end(), '\n' ), reason.end());
	string dir = "/data/release/sgonline/pi/webroot/monitor/op_";
	char buf[256];
	time_t now;
	time( &now );
	struct tm *ptm;
	ptm = localtime(&now);
	snprintf(buf, sizeof(buf), "%s%04d%02d%02d.log",
			dir.c_str(),
			ptm->tm_year+1900,
			ptm->tm_mon+1,
			ptm->tm_mday);
	FILE *fp = fopen(buf, "a");
	if (!fp)
	{
		return;
	}
	fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d|%s|%s|%u|%u|0|%s|%s|%u|%d|%s\n",
			ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,
			admin.c_str(),
			ip.c_str(),
			Config::GetZoneByUID(uid),
			uid,
			"name",
			op.c_str(),
			id,
			count,
			reason.c_str()
			);
	fclose(fp);
	return;
}

int CLogicAdmin::BroadCastByTool(string message, int repeats, int interval,string serverid)
{
	if(repeats == 1)
		BroadCast(message, repeats, interval, serverid);
	else
	{
		string path = Config::GetPath("tools/");
		string file = path + "broadcast.dat";
		string cmd = "cd " + path + " && nohup ./BroadCast "
				+ CTrans::ITOS(repeats) + " "
				+ CTrans::ITOS(interval) + " "
				+ serverid + " &";
		File::Write(file, message);
		system(cmd.c_str());
	}
	return 0;
}

int CLogicAdmin::BroadCast(string message, int repeats, int interval,string serverid)
{
	string splatform = Convert::UIntToString(OpenPlatform::GetType());
	string tcpserver = Config::GetValue(CONFIG_TCP_HOST);
	if(serverid.empty())
		Config::GetValue(serverid, CONFIG_SERVER_ID);


	CPacket1 cpacket;
	cpacket.UserId = 1999999999;
	cpacket.ProtocolType = PROTOCOL_TYPE_LOGIN;
	cpacket.MainProtocolType = 1;
	cpacket.SubProtocolType = 1;
	cpacket.PacketType = PT_CSRequest;
	cpacket.ProtocolBody.Clear();

	CLoginSend cloginsend;
	cloginsend.name = "{\"name\":\"系统管理员\",\"hname\":\"\",\"hlvl\":0}";
	cloginsend.platform = (byte)splatform[0];
	error_log("platform = %u, name = %s, server=%s",cloginsend.platform, cloginsend.name.c_str(),serverid.c_str());

	if (!cloginsend.Encode(&cpacket.ProtocolBody))
	{
		error_log(" body encode failed");
		return R_ERR_DATA;
	}

	CStaticBuffer<1000> buffer;
	buffer.Clear();
	if (!cpacket.Encode(&buffer))
	{
		error_log(" packet encode failed");
		return R_ERR_DATA;
	}

	CSocket socket;
	struct sockaddr_in server;
	if (!socket.Socket(SOCK_STREAM, 0, AF_INET))
	{
		error_log("create socket error");
		return R_ERROR;
	}

	size_t pos = tcpserver.find(':');
	string host = tcpserver.substr(0, pos);
	string port = tcpserver.substr(pos + 1, tcpserver.length() - pos - 1);
	socket.Socket(SOCK_STREAM, 0, AF_INET);
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_port = htons(atoi(port.c_str()));
	server.sin_family = AF_INET;
	memset(server.sin_zero, 0x00, sizeof(server.sin_zero));

	if (!socket.Connect((struct sockaddr *) &server,sizeof(struct sockaddr)))
	{
		error_log("connect socket error,addr = %s, port=%s,errno = %d, errmsg = %s",
				host.c_str(),port.c_str(),errno, strerror(errno));
		socket.Close();
		return R_ERR_REFUSE;
	}

	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	if (!socket.SetSockOpt(SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout,
			sizeof(struct timeval))
			|| !socket.SetSockOpt(SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
					sizeof(struct timeval)))
		error_log("set time out wrong");

	string firstpack = "tgw_l7_forward\r\nHost: " + tcpserver + " serverid:"
			+ serverid + "\r\n\r\n";
	int firstpackLen = firstpack.length();
	byte packbyte[firstpackLen];
	memset(packbyte, 0x00, firstpackLen);
	memcpy(packbyte, firstpack.c_str(), firstpackLen);
	if (!(socket.Send(packbyte, firstpackLen, 0))) {
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0)))
	{
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	unsigned int recvsize = 0;
	buffer.Clear();
	recvsize = socket.Receive(buffer.GetNativeBuffer(),
	buffer.GetFreeCapacity());
	if (recvsize < 0)
	{
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	buffer.SetSize(recvsize);
	CPacket1 loginreply;
	if (!loginreply.Decode(&buffer))
	{
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	CLoginReceive cloginrec;
	if (!cloginrec.Decode(&(loginreply.ProtocolBody)))
	{
		error_log("decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	info_log("clogicrec.sessionkey = %u", cloginrec.SessionKey);

	CPacket1 cpacketChartRequest;
	cpacketChartRequest = loginreply;
	cpacketChartRequest.ProtocolType = PROTOCOL_TYPE_CHAT_SUBMIT;
	cpacketChartRequest.MainProtocolType = 4;
	cpacketChartRequest.SubProtocolType = 1;
	cpacketChartRequest.PacketType = PT_CSRequest;
	cpacketChartRequest.Key = cloginrec.SessionKey;
	cpacketChartRequest.UserId = 1999999999;

	CChatRequest chatrequest;
	chatrequest.Message = message;
	chatrequest.ToId = 10000005;
	chatrequest.ToType = 1;
	buffer.Clear();
	chatrequest.Encode(&cpacketChartRequest.ProtocolBody);
	cpacketChartRequest.Encode(&buffer);
	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0)))
	{
		error_log("chat send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	if (repeats > 1)
	{
		CStaticBuffer<100> UpdateSavebuffer;
		int tmpinterval = interval;
		int sendsize;
		cpacketChartRequest.ProtocolType = PROTOCOL_TYPE_REFRESH;
		cpacketChartRequest.ProtocolBody.Clear();
		cpacketChartRequest.ProtocolBody.Append((byte *)"\0",1);
		while (--repeats)
		{
			cpacketChartRequest.Encode(&UpdateSavebuffer);
			while (tmpinterval > UPDATE_SAVE_INTERVAL)
			{
				sleep(UPDATE_SAVE_INTERVAL);
				tmpinterval -= UPDATE_SAVE_INTERVAL;
				sendsize = (socket.Send(UpdateSavebuffer.GetConstBuffer(),UpdateSavebuffer.GetSize(), 0));
				if (sendsize < 0)
				{
					error_log("updatesave  error sendsize = %d",sendsize);
					socket.Close();
					return R_ERR_REFUSE;
				}
			}
			sleep(tmpinterval);
			sendsize = socket.Send(buffer.GetConstBuffer(),buffer.GetSize(), 0);
			if(sendsize < 0 )
			{
				error_log("chart send socket error,sendsize = %d",sendsize);
				socket.Close();
				return R_ERR_REFUSE;
			}
			info_log("auto to send success sendsize = %d",sendsize);
			tmpinterval = interval;
		}
	}

	buffer.Clear();
	recvsize = socket.Receive(buffer.GetNativeBuffer(), buffer.GetFreeCapacity());
	if (recvsize < 0) {
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	buffer.SetSize(recvsize);

	CPacket1 cpacketChartReply;
	if (!cpacketChartReply.Decode(&buffer))
	{
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}
	if (cpacketChartReply.ProtocolResult != 0) {
		error_log("protocol error");
		socket.Close();
		return R_ERROR;
	}

	socket.Close();
	return R_SUCCESS;
}

int CLogicAdmin::AddForbidUser(unsigned uid, unsigned forbidts, string serverid, string splatform) {
	string tcpserver = Config::GetValue(CONFIG_TCP_HOST);

	int serveridint = Config::GetZoneByUID(uid);
	serveridint = MainConfig::GetMergedDomain(serveridint);
	if(!serveridint)
		serveridint = Config::GetZoneByUID(uid);
	else
		serverid = Convert::IntToString(serveridint);
	if(serverid.empty())
		serverid = Convert::IntToString(serveridint);

	CPacket1 cpacket;
	cpacket.UserId = 1999999999;
	cpacket.ProtocolType = PROTOCOL_TYPE_LOGIN;
	cpacket.MainProtocolType = 1;
	cpacket.SubProtocolType = 1;
	cpacket.PacketType = PT_CSRequest;
	cpacket.ProtocolBody.Clear();

	CLoginSend cloginsend;
	cloginsend.name = "{\"name\":\"系统管理员\",\"hname\":\"\",\"hlvl\":0}";
	cloginsend.platform = (byte) splatform[0];
	error_log("platform = %u, name = %s, server=%s",cloginsend.platform, cloginsend.name.c_str(),serverid.c_str());

	if (!cloginsend.Encode(&cpacket.ProtocolBody)) {
		error_log(" body encode failed");
		return R_ERR_DATA;
	}

	CStaticBuffer<1000> buffer;
	buffer.Clear();
	if (!cpacket.Encode(&buffer)) {
		error_log(" packet encode failed");
		return R_ERR_DATA;
	}

	CSocket socket;
	struct sockaddr_in server;
	if (!socket.Socket(SOCK_STREAM, 0, AF_INET)) {
		error_log("create socket error");
		return R_ERROR;
	}

	//string tcpserver = Config::GetValue(CONFIG_TCP_SERVER);
	size_t pos = tcpserver.find(':');
	string hostname = tcpserver.substr(0, pos);
	string port = tcpserver.substr(pos + 1, tcpserver.length() - pos - 1);
	bool isIp = CheckIp(hostname);
	string host = isIp ? hostname : string(inet_ntoa(*(struct in_addr*) *(gethostbyname(hostname.c_str())->h_addr_list)));
	socket.Socket(SOCK_STREAM, 0, AF_INET);
	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_port = htons(atoi(port.c_str()));
	server.sin_family = AF_INET;
	memset(server.sin_zero, 0x00, sizeof(server.sin_zero));

	if (!socket.Connect((struct sockaddr *) &server, sizeof(struct sockaddr))) {
		error_log("connect socket error,addr = %s, port=%s,errno = %d, errmsg = %s",
				host.c_str(),port.c_str(),errno, strerror(errno));
		socket.Close();
		return R_ERR_REFUSE;
	}

	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	if (!socket.SetSockOpt(SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout,
			sizeof(struct timeval))
			|| !socket.SetSockOpt(SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout,
					sizeof(struct timeval)))
		error_log("set time out wrong");

	//if (isIp && serverid != "") {
	string firstpack = "tgw_l7_forward\r\nHost: " + tcpserver + " serverid:"
			+ serverid + "\r\n\r\n";
	int firstpackLen = firstpack.length();
	byte packbyte[firstpackLen];
	memset(packbyte, 0x00, firstpackLen);
	memcpy(packbyte, firstpack.c_str(), firstpackLen);
	if (!(socket.Send(packbyte, firstpackLen, 0))) {
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	//}
	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0))) {
		error_log("send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	unsigned int recvsize = 0;
	buffer.Clear();
	recvsize = socket.Receive(buffer.GetNativeBuffer(),
			buffer.GetFreeCapacity());
	if (recvsize < 0) {
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	buffer.SetSize(recvsize);
	CPacket1 loginreply;
	if (!loginreply.Decode(&buffer)) {
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	CLoginReceive cloginrec;
	if (!cloginrec.Decode(&(loginreply.ProtocolBody))) {
		error_log("decode error");
		socket.Close();
		return R_ERR_DATA;
	}

	error_log("clogicrec.sessionkey = %u", cloginrec.SessionKey);

	CPacket1 cpacketForbidChartRequest;
	cpacketForbidChartRequest = loginreply;
	cpacketForbidChartRequest.ProtocolType = PROTOCOL_TYPE_FORBID_CHAT;
	cpacketForbidChartRequest.MainProtocolType = 5;
	cpacketForbidChartRequest.SubProtocolType = 1;
	cpacketForbidChartRequest.PacketType = PT_CSRequest;
	cpacketForbidChartRequest.Key = cloginrec.SessionKey;
	cpacketForbidChartRequest.UserId = 1999999999;

	CChatRequest chatrequest;
	CForbidChatRequest forbidrequest;
	//	forbidrequest.Message = "222232121";
	forbidrequest.ForbidUid = uid;
	forbidrequest.ForbidTs = forbidts;
	//	chatrequest.Message = "whut_psl";
	//	chatrequest.ToId = uid;
	//	chatrequest.ToType = 1;
	buffer.Clear();
	cpacketForbidChartRequest.ProtocolBody.Clear();
	forbidrequest.Encode(&cpacketForbidChartRequest.ProtocolBody);
	error_log("message = %s",forbidrequest.ToString().c_str());
	cpacketForbidChartRequest.Encode(&buffer);
	if (!(socket.Send(buffer.GetConstBuffer(), buffer.GetSize(), 0))) {
		error_log("chart send socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}
	//
	//	buffer.Clear();
	//	recvsize = 0;
	recvsize = socket.Receive(buffer.GetNativeBuffer(),
			buffer.GetFreeCapacity());
	if (recvsize <= 0) {
		error_log("receive socket error");
		socket.Close();
		return R_ERR_REFUSE;
	}

	buffer.SetSize(recvsize);

	CPacket1 cpacketChartReply;
	if (!cpacketChartReply.Decode(&buffer)) {
		error_log("cpacket decode error");
		socket.Close();
		return R_ERR_DATA;
	}
	if (cpacketChartReply.ProtocolResult != 0) {
		error_log("protocol error");
		return R_ERROR;
	}

	return 0;
}

int CLogicAdmin::SetFlag(string &openid, unsigned flag)
{
	if(flag >= gm_max)
		return 0;

	CDataBlackUser dbBlackUser;
	DataBlackUser blackUser;
	blackUser.open_id = openid;
	blackUser.block_time = flag;
	return dbBlackUser.SetBlackUser(blackUser);
}
int CLogicAdmin::CheckGM(string &openid, unsigned &flag)
{
	flag =  gm_none;
	CDataBlackUser dbBlackUser;
	DataBlackUser data;
	int ret = dbBlackUser.GetBlackUser(openid, data);
	if(ret)
		return ret;

	if (data.block_time > Time::GetGlobalTime())
		flag = gm_forbid;
	else if(data.block_time < gm_max)
		flag = data.block_time;

	return 0;
}

bool CLogicAdmin::CheckIp(const string &host){
	bool isIp = true;
	for(int i = 0 ; i < host.length();i++)
	{
		isIp = isIp&&((host[i]>='0'&&host[i]<='9')||host[i]=='.');
	}
	return isIp;
}


string CLogicAdmin::GetActTime()
{
	string path = Config::GetPath("conf/ActivityTime.json");
	string conf;
	int ret = File::Read(path,conf);
	if(ret)
		return "Read file fail";

	return conf;
}
string CLogicAdmin::SetActTime(unsigned id, unsigned bts, unsigned ets, unsigned ver)
{
	string path = Config::GetPath("conf/ActivityTime.json");
	string conf;
	int ret = File::Read(path,conf);
	if(ret)
		return "Read file fail";

	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value con;
	if(!reader.parse(conf, con) || !con.isMember("act") || !con["act"].isArray())
	if(ret)
		return "parse file fail";

	bool add = true;
	for(unsigned i=0;i<con["act"].size();++i)
	{
		if(con["act"][i].isMember("id") && con["act"][i]["id"].asUInt() == id)
		{
			con["act"][i]["bts"] = bts;
			con["act"][i]["ets"] = ets;
			con["act"][i]["ver"] = ver;
			add = false;
			break;
		}
	}
	if(add)
	{
		Json::Value temp;
		temp["id"] = id;
		temp["bts"] = bts;
		temp["ets"] = ets;
		temp["ver"] = ver;
		con["act"].append(temp);
	}

	string res =  writer.write(con);
	ret = File::Write(path, res);
	if(ret)
		return "write file fail";

	return res;
}

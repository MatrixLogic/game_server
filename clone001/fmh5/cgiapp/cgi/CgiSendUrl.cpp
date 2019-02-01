#include "CgiSendUrl.h"
#include "Buffer.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcgi_stdio.h>
#include "Packet1.h"
#include "ProtoInc.h"
#include "DataUserMapping.h"
#include <fstream>

using namespace std;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int len = strlen((char *)ptr);
    if (len > 999999) 
        return 0;

    ((std::string*)stream)->append((char*)ptr, size * nmemb); 

    return size*nmemb;
}

SimulationWebClient::SimulationWebClient(string openid, string serverip, string reqname, string & data):
		m_openid(openid),
		m_serverip(serverip),
		m_reqname(reqname),
		m_data(data),
		m_bodylen(0),
		m_sockfd(-1),
		m_epollfd(-1),
		m_event(0),
		m_isStop(false),
		m_isLogin(false),
		m_isProcess(false),
		m_isNoReply(false),
		m_events(NULL),
		m_recvbuffer(SG17_MAX_PACKET_SIZE),
		m_sendbuffer(SG17_MAX_PACKET_SIZE)
{
}

SimulationWebClient::~SimulationWebClient()
{
	delete[] m_events;
	close(m_sockfd);
	close(m_epollfd);
}

int SimulationWebClient::Run()
{
    //openid查询uid的值
	CDataUserMapping dbUserMapping;
	unsigned uid = 0;

	int ret = dbUserMapping.GetMapping(m_openid, PT_TEST, uid);

	if(ret && ret != R_ERR_NO_DATA)
	{
		m_errmsg = "get mapping error";
		return 1;
	}

	//用户不存在时，继续发送登录cgi,会注册新用户
	Common::Login * loginmsg = new Common::Login;

	ret = LoginCGI(uid, loginmsg);

	if (ret)
	{
		delete loginmsg;
		return ret;
	}

	ret = ConnectServer();

	if (ret)
	{
		return ret;
	}

	ret = SetPoll();

	if (ret)
	{
		return ret;
	}

	m_packet.cmd = PROTOCOL_ACCESS_TRANSFER;  //发送给battle
	m_packet.m_msg = loginmsg;
	m_sendbuffer.Clear();

	if(!m_packet.Encode(&m_sendbuffer))
	{
		m_errmsg = "packet encode failed";
		return R_ERR_DATA;
	}

	ret = ListenPoll();

	if (ret)
	{
		return ret;
	}


	return 0;
}

int SimulationWebClient::LoginCGI(unsigned uid, Common::Login * msg)
{
    //模拟登陆
    char szUrl[100] = {0};
    sprintf(szUrl, "http://%s:7000/cgi/login?openid=%s&openkey=&pf=", m_serverip.c_str(), m_openid.c_str());

    CURL *curl;
    CURLcode  res;

    curl = curl_easy_init();//对curl进行初始化

    string str;

    curl_easy_setopt(curl, CURLOPT_URL, szUrl); //设置访问地址
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 12);//设置超时时间
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);//设置写数据的变量
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    res = curl_easy_perform(curl);//执行登录

    if (CURLE_OK != res)
    {
    	m_errmsg = "登录失败";
        return 1;
    }

    Json::Reader reader;
    Json::Value value;

    string sdata(str);

    //判断sdata是否是正常的json格式的数据, {}

    if (sdata[0] != '{' || sdata[sdata.size() - 1] != '}')
    {
    	m_errmsg = "登录失败，错误原因:";
    	m_errmsg += sdata;

        return 2;
    }

    //int len = sdata.size();

	if( !reader.parse( sdata, value ) )
	{
		m_errmsg ="解析json失败";
        return 5;
	}

    /*
    if (!value.isMember("skey") || !Json::GetString(value, "skey", strskey)
        || !value.isMember("lasttime") ||  !GetUInt(value, "lasttime", lasttime)
        || !value.isMember("lastseq") ||  !GetUInt(value, "lastseq", seqid)
        || !value.isMember("server_ts") ||  !GetUInt(value, "server_ts", ts) )
    {
        errmsg="获取必要参数失败";
        return 6;
    }
	*/

	//设置login信息
	const Json::Value & jsonmsg = value["msg"];
	msg->set_uid(jsonmsg["uid"].asUInt());
	msg->set_openid(jsonmsg["openid"].asString());
	msg->set_openkey(jsonmsg["openkey"].asString());
	msg->set_ts(jsonmsg["ts"].asUInt());
	msg->set_sig(jsonmsg["sig"].asString());
	msg->set_fig(jsonmsg["fig"].asString());
	msg->set_isnew(jsonmsg["isnew"].asBool());
	msg->set_platform(jsonmsg["platform"].asUInt());
	//msg->set_kingdom(jsonmsg["kindom"].asUInt());
	//msg->set_ttss_level(jsonmsg["ttss_level"].asUInt());
	//msg->set_ttss_level_name(jsonmsg["ttss_level_name"].asString());
	//msg->set_ttss_grow_level(jsonmsg["ttss_grow_level"].asUInt());
	//msg->set_ttss_grow_value(jsonmsg["ttss_grow_value"].asUInt());
	//msg->set_is_blue_vip(jsonmsg["is_blue_vip"].asUInt());
	//msg->set_is_blue_year_vip(jsonmsg["is_blue_year_vip"].asUInt());
	//msg->set_is_super_blue_vip(jsonmsg["is_super_blue_vip"].asUInt());
	//msg->set_blue_vip_level(jsonmsg["blue_vip_level"].asUInt());
	//msg->set_is_yellow_vip(jsonmsg["is_yellow_vip"].asUInt());
	//msg->set_is_yellow_year_vip(jsonmsg["is_yellow_year_vip"].asUInt());
	//msg->set_yellow_vip_level(jsonmsg["yellow_vip_level"].asUInt());

    return 0;
}

int SimulationWebClient::ConnectServer()
{
	int fd = 0;

	if((fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		m_errmsg = "make tcp socket error<<endl";
		return 1;
	}

	struct sockaddr_in serveraddr;

	bzero((void*)&serveraddr, sizeof(serveraddr));
	const int server_port = 7301;

	m_serverip = "127.0.0.1";

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr  = inet_addr(m_serverip.c_str());
	serveraddr.sin_port = htons(server_port);

	int ret = connect(fd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));

	if (-1 == ret)
	{
		m_errmsg = "connect server error";
		return 1;
	}

	m_sockfd = fd;
	int flags = 0;

    if((flags = fcntl(m_sockfd, F_GETFL, 0))==-1)
    {
    	m_errmsg = "fcntl F_GETFL fail:";
        return 1;
    }

    flags |= O_NONBLOCK;

	fcntl(m_sockfd, F_SETFL, O_NONBLOCK);

	return 0;
}

int SimulationWebClient::SetPoll()
{
	if (-1 == m_sockfd)
	{
		return 1;
	}

	//对连接的套接字进行epoll事件模型
	int efd = epoll_create(100);

	if (-1 == efd)
	{
		m_errmsg = "epoll create fail";
		return 1;
	}

	m_event = EPOLLIN | EPOLLOUT | EPOLLET;
	struct epoll_event epollevent;
	epollevent.events = m_event;
	int ret = epoll_ctl (efd, EPOLL_CTL_ADD, m_sockfd, &epollevent);

	if (-1 == ret)
	{
		m_errmsg ="epoll ctl set error.";
		return 1;
	}

	m_epollfd = efd;

	return 0;
}

int SimulationWebClient::ListenPoll()
{
	if (-1 == m_epollfd)
	{
		m_errmsg = "epoll fd error.";
		return 1;
	}

	const int EPOLL_WAIT_TIMEOUT = 60;
	int ret = 0;
	m_events = new epoll_event[10];

	while(true)  //监听事件
	{
		if (m_isStop)
		{
			break;
		}

		int nReady = epoll_wait(m_epollfd, m_events, 10, EPOLL_WAIT_TIMEOUT);  //可以读取的事件个数

		if(nReady != -1)
		{
			if (0 == nReady)
			{
				continue;
			}

			for(int i = 0; i < nReady; ++i)
			{
				if (m_events[i].events & EPOLLIN)  //可读
				{
					ret = InputNotify();

					if (ret)
					{
						return 1;
					}
				}
				else if (m_events[i].events & EPOLLOUT)  //非阻塞发送
				{
					//发送数据
					ret = OutPutNotify();

					if (ret)
					{
						return ret;
					}
				}
				else
				{
					m_errmsg ="not in mode";
					return 1;
				}
			}
		}
		else if (errno == EINTR)
		{
			m_errmsg = "error happen when recv data";
			return 1;
		}
	}

	return 0;
}

int SimulationWebClient::OutPutNotify()
{
	SendData();   //发送数据

	return 0;
}

int SimulationWebClient::InputNotify()
{
	//接收数据
	//先解包
	CBuffer bufftemp(SG17_MAX_PACKET_SIZE);
	unsigned int recvsize = 0;

	recvsize = recv(m_sockfd, bufftemp.GetNativeBuffer(), bufftemp.GetFreeCapacity(), 0);

	if (recvsize < 0)
	{
		if (errno == EAGAIN)
		{
			return 0;
		}
		else
		{
			m_errmsg  = "error happen";
			return 1;
		}
	}
	else if (0 == recvsize)
	{
		m_errmsg  = "recv no data";
		return 1;
	}

	//1.第一个包中有A和B两个包，但是B不完整。未处理
	//2.第一个包中包含完整的A和B，但处理错误

	bufftemp.SetSize(recvsize);

	//FCGI_printf("recv_size:%u<br>", recvsize);

	//收到的数据未必是完整的
	if (0 == m_bodylen)
	{
		bool issuccess = m_recvbuffer.CopyFromBuffer(&bufftemp);

		if (!issuccess)
		{
			m_errmsg = "copy fail";
			return 1;;
		}

		CBufferReader reader(&m_recvbuffer);

		bool isok = DecodePacketHead(reader);

		if (!isok)
		{
			return 1;
		}

		//FCGI_printf("body_len:%u<br>", m_bodylen);
	}
	else  //在旧的数据下新增新的数据
	{
		unsigned leftlen = SG17_PACKET_HEADER_SIZE + m_bodylen - m_recvbuffer.GetSize();

		if (bufftemp.GetSize() > leftlen)   //下一个数据包丢弃
		{

			//FCGI_printf("leftlen:%u,recv-size=%u<br>", leftlen, m_recvbuffer.GetSize());
			byte * pdata = new byte[leftlen + 1];
			bufftemp.GetData(pdata, leftlen, 0);

			m_recvbuffer.Append(pdata, leftlen);

			delete[] pdata;
		}
		else  //收到的内容全都是这个报文的
		{
			//FCGI_printf("leftlen-complete:%u,recv-size=%u<br>", leftlen, m_recvbuffer.GetSize());
			m_recvbuffer.AppendBuffer(&bufftemp);
		}
	}

	//解析收到的报文
	//什么时候处理这个结束呢?? 顺序: login->再处理连续数据
	if (SG17_PACKET_HEADER_SIZE + m_bodylen > m_recvbuffer.GetSize())   //还有数据待接收
	{
		;
	}
	//处理一个报文中，可能包含的多个别的报文数据
	else if (SG17_PACKET_HEADER_SIZE + m_bodylen <= m_recvbuffer.GetSize())  //收到完整数据或者超过一个报文数据
	{
		//先处理第一个包
		//不修改recv_buff中的数据
		//FCGI_printf("lenth:%u<br>", m_recvbuffer.GetSize());

		while(m_recvbuffer.GetSize() >= SG17_PACKET_HEADER_SIZE + m_bodylen)
		{
			unsigned len = SG17_PACKET_HEADER_SIZE + m_bodylen;

			byte * pdata = new byte[SG17_PACKET_HEADER_SIZE + m_bodylen + 1];
			m_recvbuffer.GetData(pdata, len, 0);
			m_recvbuffer.Remove(0, len);
			CBuffer buffnow(SG17_MAX_PACKET_SIZE);

			buffnow.Append(pdata, len);
			delete[] pdata;

			//m_recvbuffer.Append(pdata, leftlen);

			//如何解析接收到的数据呢??
			m_packet.m_msg = NULL;

			bool issuccee = m_packet.Decode(&buffnow);

			if (!issuccee)
			{
				m_errmsg = "decode error";
				return 1;
			}

			int ret = HandleResp(m_packet.m_msg);

			if (ret)
			{
				return 1;
			}

			//重新解析接下来的报文头
			if (0 != m_recvbuffer.GetSize())
			{
				//对recv_buff进行解析报文头时，修改了这里的数据
				CBufferReader reader(&m_recvbuffer);

				bool isok = DecodePacketHead(reader);

				if (!isok)
				{
					return 1;
				}
			}
		}

		//重置接收信息
		if (0 == m_recvbuffer.GetSize())
		{
			m_bodylen = 0;
		}

		//是否停止
		if (m_isLogin && m_isProcess && 0 == m_bodylen)  //非登录时，才停止
		{
			m_isStop = true;
		}
	}

	return 0;
}

bool SimulationWebClient::DecodePacketHead(CBufferReader & reader)
{
	uint16_t head = 0;
	uint32_t bodyLen = 0;
	uint16_t cmd = 0;

	PACKET_DECODE(UInt16, head);

	const uint16_t SG17_PACKET_HEAD_MAGIC_NUMBER = 21961;

	if(head != SG17_PACKET_HEAD_MAGIC_NUMBER)
	{
		m_errmsg = "get head failed";
		return false;
	}

	//解析报文头
	PACKET_DECODE(UInt32, bodyLen);
	PACKET_DECODE(UInt16, cmd);

	m_bodylen = bodyLen;

	return true;
}

int SimulationWebClient::HandleResp(Message * msg)
{
	if (NULL == msg)
	{
		m_errmsg = "messages error";
		return 1;
	}
	//判断是否是user
	string respname = msg->GetTypeName();

	//FCGI_printf("Handle:%s<br>", respname.c_str());

	if ("User.User" == respname)
	{
		int ret = ProcessLogin();

		if (ret)
		{
			return ret;
		}

		m_isLogin = true;
		m_isProcess = false;

		if (m_isNoReply)  //没有响应，发送完请求包后，即可停止接收服务端数据
		{
			m_isProcess = true;
		}
	}
	else
	{
		Json::Value result;
		_p2j(msg, result);

		Json::FastWriter writer;
		string resultdata = writer.write(result);

		FCGI_printf(resultdata.c_str());
		FCGI_printf("<br>------------------next resp---------------------------------------<br>");
		m_isProcess = true;
	}

	return 0;
}

int SimulationWebClient::ProcessLogin()
{
	//收到user响应后，开始发送要请求的数据
	int ret = SendReqProtocol();

	if (ret)
	{
		return ret;
	}

	return 0;
}

int SimulationWebClient::SendReqProtocol()
{
	Message * pmsg = NULL;

    Json::Reader reader;
    Json::Value value;

    if (m_data.empty())
    {
    	m_data="{}";
    }

	if( !reader.parse( m_data, value ) )
	{
		m_errmsg = "解析data-json失败";
        return 5;
	}

	int ret = CreateProto(&pmsg);

	if (ret)
	{
		return ret;
	}

	//将json内容转换成proto
	bool isok = _j2p(pmsg, value);

	if (!isok)
	{
		return 1;
	}

	//组装包，发送数据
	m_packet.cmd = PROTOCOL_ACCESS_TRANSFER;  //发送给battle
	m_packet.m_msg = pmsg;

	m_sendbuffer.Clear();

	if(!m_packet.Encode(&m_sendbuffer))
	{
		m_errmsg = "packet encode failed";
		return R_ERR_DATA;
	}

	ret = SendData();

	if (ret)
	{
		m_errmsg = "send error";
		return 1;
	}

	return 0;
}

int SimulationWebClient::CreateProto(Message ** msg)
{
	//根据包名，自动创建proto类
	const Descriptor * pdescrip = DescriptorPool::generated_pool()->FindMessageTypeByName(m_reqname);

	if (pdescrip)
	{
		const Message * psg = MessageFactory::generated_factory()->GetPrototype(pdescrip);

		*msg = psg->New();
	}
	else
	{
		m_errmsg = "reqproto error:" + m_reqname;
		return 1;
	}

	return 0;
}

int SimulationWebClient::SendData()
{
	if (0 == m_sendbuffer.GetSize()) //去掉epollout事件
	{
		DisableOutPut();
		return 0;
	}

	int sendlen = send(m_sockfd, m_sendbuffer.GetConstBuffer(), m_sendbuffer.GetSize(), 0);

	if (sendlen < 0 && errno != EAGAIN)  //非阻塞，且缓冲区已满，设置epoll事件
	{
		m_errmsg = "发送数据失败.";
		return 1;;
	}

	if (sendlen >= m_sendbuffer.GetSize())  //数据发送完全,移除epoll写事件
	{
		DisableOutPut();
	}
	else
	{
		m_sendbuffer.Remove(0, sendlen);
	}

	return 0;
}

int SimulationWebClient::EnableOutPut()
{
	m_event |= EPOLLOUT;
	struct epoll_event epollevent;
	epollevent.events = m_event;

	epoll_ctl (m_epollfd, EPOLL_CTL_MOD, m_sockfd, &epollevent);

	return 0;
}

int SimulationWebClient::DisableOutPut()
{
	m_event &= ~EPOLLOUT;
	struct epoll_event epollevent;
	epollevent.events = m_event;

	epoll_ctl (m_epollfd, EPOLL_CTL_MOD, m_sockfd, &epollevent);

	return 0;
}

int SimulationWebClient::EnableInput()
{
	m_event |= EPOLLIN;
	struct epoll_event epollevent;
	epollevent.events = m_event;

	epoll_ctl (m_epollfd, EPOLL_CTL_MOD, m_sockfd, &epollevent);

	return 0;
}

int SimulationWebClient::DisableInput()
{
	m_event &= ~EPOLLIN;
	struct epoll_event epollevent;
	epollevent.events = m_event;

	epoll_ctl (m_epollfd, EPOLL_CTL_MOD, m_sockfd, &epollevent);

	return 0;
}

bool SimulationWebClient::_j2p(Message* msg, Json::Value &json)
{
	const Descriptor* des = msg->GetDescriptor();
	const Reflection* ref = msg->GetReflection();
	for (int i = 0; i < des->field_count(); ++i)
	{
		const FieldDescriptor* field = des->field(i);
		const string& key = field->name();

		if (field->is_required())
		{
			if(!json.isMember(key))
			{
				m_errmsg = "[ConfigPBBase] key is not exists. key="+ key;
				return false;
			}
		}

		if(field->is_repeated() && json.isMember(key))
		{
			if(!json[key].isArray())
				return false;
			for(unsigned j = 0; j < json[key].size(); ++j)
			{
				switch (field->cpp_type())
				{
				case FieldDescriptor::CPPTYPE_DOUBLE:
					ref->AddDouble(msg, field, json[key][j].asDouble());
					break;
				case FieldDescriptor::CPPTYPE_FLOAT:
					ref->AddFloat(msg, field, (float)json[key][j].asDouble());
					break;
				case FieldDescriptor::CPPTYPE_INT64:
					ref->AddInt64(msg, field, (int64_t)json[key][j].asDouble());
					break;
				case FieldDescriptor::CPPTYPE_UINT64:
					ref->AddUInt64(msg, field, (uint64_t)json[key][j].asDouble());
					break;
				case FieldDescriptor::CPPTYPE_INT32:
					ref->AddInt32(msg, field, json[key][j].asInt());
					break;
				case FieldDescriptor::CPPTYPE_UINT32:
					ref->AddUInt32(msg, field, json[key][j].asUInt());
					break;
				case FieldDescriptor::CPPTYPE_BOOL:
					ref->AddBool(msg, field, json[key][j].asBool());
					break;
				case FieldDescriptor::CPPTYPE_STRING:
					ref->AddString(msg, field, json[key][j].asString());
					break;
				case FieldDescriptor::CPPTYPE_MESSAGE:
					if(!_j2p(ref->AddMessage(msg, field), json[key][j]))
						return false;
					break;
				default:
					break;
				}
			}
		}
		else if (json.isMember(key))
		{
			switch (field->cpp_type())
			{
			case FieldDescriptor::CPPTYPE_DOUBLE:
				ref->SetDouble(msg, field, json[key].asDouble());
				break;
			case FieldDescriptor::CPPTYPE_FLOAT:
				ref->SetFloat(msg, field, (float)json[key].asDouble());
				break;
			case FieldDescriptor::CPPTYPE_INT64:
				ref->SetInt64(msg, field, (int64_t)json[key].asDouble());
				break;
			case FieldDescriptor::CPPTYPE_UINT64:
				ref->SetUInt64(msg, field, (uint64_t)json[key].asDouble());
				break;
			case FieldDescriptor::CPPTYPE_INT32:
				ref->SetInt32(msg, field, json[key].asInt());
				break;
			case FieldDescriptor::CPPTYPE_UINT32:
				ref->SetUInt32(msg, field, json[key].asUInt());
				break;
			case FieldDescriptor::CPPTYPE_BOOL:
				ref->SetBool(msg, field, json[key].asBool());
				break;
			case FieldDescriptor::CPPTYPE_STRING:
				ref->SetString(msg, field, json[key].asString());
				break;
			case FieldDescriptor::CPPTYPE_MESSAGE:
				if(!_j2p(ref->MutableMessage(msg, field), json[key]))
					return false;
				break;
			default:
				break;
			}
		}
	}

	return true;
}

bool SimulationWebClient::_p2j(const Message* msg, Json::Value &json)
{
	const Descriptor* des = msg->GetDescriptor();
	const Reflection* ref = msg->GetReflection();

	for (int i = 0; i < des->field_count(); ++i)
	{
		const FieldDescriptor* field = des->field(i);
		const string& key = field->name();
		if(field->is_repeated())
		{
			json[key].resize(0);
			for(int j = 0; j < ref->FieldSize(*msg, field); ++j)
			{
				Json::Value temp;
				switch (field->cpp_type())
				{
				case FieldDescriptor::CPPTYPE_DOUBLE:
					temp = ref->GetRepeatedDouble(*msg, field, j);
					break;
				case FieldDescriptor::CPPTYPE_FLOAT:
					temp = ref->GetRepeatedFloat(*msg, field, j);
					break;
				case FieldDescriptor::CPPTYPE_INT64:
					temp = double(ref->GetRepeatedInt64(*msg, field, j));
					break;
				case FieldDescriptor::CPPTYPE_UINT64:
					temp = double(ref->GetRepeatedUInt64(*msg, field, j));
					break;
				case FieldDescriptor::CPPTYPE_INT32:
					temp = ref->GetRepeatedInt32(*msg, field, j);
					break;
				case FieldDescriptor::CPPTYPE_UINT32:
					temp = ref->GetRepeatedUInt32(*msg, field, j);
					break;
				case FieldDescriptor::CPPTYPE_BOOL:
					temp = ref->GetRepeatedBool(*msg, field, j);
					break;
				case FieldDescriptor::CPPTYPE_STRING:
					temp = ref->GetRepeatedString(*msg, field, j);
					break;
				case FieldDescriptor::CPPTYPE_MESSAGE:
					_p2j(&ref->GetRepeatedMessage(*msg, field, j), temp);
					break;
				default:
					break;
				}
				json[key].append(temp);
			}
		}
		else
		{
			switch (field->cpp_type())
			{
			case FieldDescriptor::CPPTYPE_DOUBLE:
				json[key] = ref->GetDouble(*msg, field);
				break;
			case FieldDescriptor::CPPTYPE_FLOAT:
				json[key] = ref->GetFloat(*msg, field);
				break;
			case FieldDescriptor::CPPTYPE_INT64:
				json[key] = double(ref->GetInt64(*msg, field));
				break;
			case FieldDescriptor::CPPTYPE_UINT64:
				json[key] = double(ref->GetUInt64(*msg, field));
				break;
			case FieldDescriptor::CPPTYPE_INT32:
				json[key] = ref->GetInt32(*msg, field);
				break;
			case FieldDescriptor::CPPTYPE_UINT32:
				json[key] = ref->GetUInt32(*msg, field);
				break;
			case FieldDescriptor::CPPTYPE_BOOL:
				json[key] = ref->GetBool(*msg, field);
				break;
			case FieldDescriptor::CPPTYPE_STRING:
				json[key] = ref->GetString(*msg, field);
				break;
			case FieldDescriptor::CPPTYPE_MESSAGE:
				_p2j(&ref->GetMessage(*msg, field), json[key]);
				break;
			default:
				break;
			}
		}
	}

	return true;
}

int main()
{
	//const message = NULL;
	if(!Kernel::Init())
	{
		cerr<<("[Kernel::Init fail][server=Toywar]");
		return false;
	}

    while(FCGI_Accept() >= 0)
    {   
        send_html_start();
        
        int len;
        char *lenstr, poststr[1000];
        
        lenstr = getenv("CONTENT_LENGTH");

        if (NULL == lenstr)
        {
            FCGI_printf("传递的参数错误!");
            send_html_end();
            continue;
        }
        
        len = atoi(lenstr);
 
        memset(poststr, 0, sizeof(poststr));
        
        FCGI_fread( poststr, len+1, 1, FCGI_stdin);

        //cout<<"上报的内容长度是:"<<len<<"内容是:"<<poststr<<"[]"<<endl;
        
        vector<string> vctstr;
        map<string, string> mapParam;
        
        stringsplit(poststr, "&", vctstr);
        string key;
        string val;
        char dst[1000] = {0};

        //FCGI_printf("长度是:%d,内容是:%s", len, poststr);
        FCGI_printf("<br>");
        
        for (vector<string>::iterator itItem = vctstr.begin(); itItem != vctstr.end(); ++itItem)
        {
            string::size_type cpos = itItem->find("=");
            //cout<<"原始内容是:"<<*itItem<<"[]<br>"<<endl;
            if (cpos != string::npos)
            {
                key = itItem->substr(0, cpos);
                val = itItem->substr(cpos + 1);
            }
            else
            {
                continue;
            }

            
            //cout<<"first:key:"<<key<<",value:"<<val<<"||\n<br>"<<endl;
            
            //FCGI_printf("first:key:%s,value:%s,\n", key.c_str(), val.c_str());
            StringReplace(key, "+", " ");
            StringReplace(val, "+", " ");

            {
                char * ptemp = const_cast<char*> (key.c_str());
                memset(dst, 0, sizeof(dst));
                unencode(ptemp, ptemp+key.size(), dst);
                key = dst;
                //cout<<key.size()<<"\n";
                //key[key.size()-1] = '\0';
                //cout<<key.size()<<"\n"<<endl;
            }
            
            {
                char * ptemp = const_cast<char*> (val.c_str());
                memset(dst, 0, sizeof(dst));
                unencode(ptemp, ptemp+val.size(), dst);
                val = dst;
                //cout<<val.size()<<"\n";
                //val[val.size()-1] = '\0';
                //cout<<val.size()<<"\n"<<endl;
            }
            
            //cout<<"key:"<<key<<",value:"<<val<<","<<"\n<br>"<<endl;
            
            //FCGI_printf("key:%s,value:%s,<br>", key.c_str(), val.c_str());
            mapParam.insert(make_pair(key, val));
        }
        
        //从map中取出key。包括uid,cgi
        string cgival = mapParam["cgiaddr"];
        string openidval = mapParam["openid"];
        string reqval = mapParam["reqproto"];
        string dataval = mapParam["data"];
           
        //判断传过来的参数是否合理
        if (cgival.empty() || openidval.empty() || reqval.empty())
        {
            FCGI_printf("传递的参数错误!每个参数都必填");
            send_html_end();
            continue;   
        }
        
        //获得服务器ip
        int fpos = cgival.find_first_of(':');
        int spos = cgival.find(':', fpos+1);
        
        if (spos == string::npos)  //如果不存在端口
        {
            spos = cgival.find('/', fpos+3);
        }
        
        string serverip = cgival.substr(fpos+3, spos - fpos - 3);
        
        //模拟发送登录包

        int ret = 0;
        SimulationWebClient simulationclient(openidval, serverip, reqval, dataval);
        
        ret = simulationclient.Run();

        if (0 != ret)
        {
        	FCGI_printf("模拟发送失败<br>");
            FCGI_printf(simulationclient.GetErrmsg().c_str());
        }
        else
        {
        	FCGI_printf("<br>模拟发送成功");
        }

        send_html_end();
    }
   
    return 0;
}

int send_html_end()
{
    FCGI_printf("</BODY></HTML>\n");

    return 0;
}

int send_html_start()
{
    FCGI_printf("Content-type: text/html\r\n\r\n\n");

    FCGI_printf("<HTML><HEAD>\n"
        "<TITLE>Simulation Post Result</TITLE>\n"
        "</HEAD><br>");

    FCGI_printf("<meta content=\"text/html; charset=UTF-8\" http-equiv=\"Content-Type\">");
    FCGI_printf("<BODY>\n");

    return 0;
}

int stringsplit(string sData, string sDelim, vector<string> &vItems)
{
	vItems.clear();

	string::size_type bpos = 0;
	string::size_type epos = 0;
	string::size_type nlen = sDelim.size();
    
	while ((epos=sData.find(sDelim, epos)) != string::npos)
	{
		vItems.push_back(sData.substr(bpos, epos-bpos));
		epos += nlen;
		bpos = epos;
	}

	vItems.push_back(sData.substr(bpos, sData.size()-bpos));
    
    return 0;
}

int StringReplace(string& sData, const string& sSrc, const string& sDst)
{
	string::size_type pos = 0;
	string::size_type slen = sSrc.size();
	string::size_type dlen = sDst.size();
    
	while ((pos = sData.find(sSrc, pos)) != string::npos)
	{
		sData.replace(pos, slen, sDst);
		pos += dlen;
	}

	return 0;
}

int unencode(char *src, char *last, char *dest)
{
    for(; src != last; src++, dest++)
    {
        if(*src == '+')
        {
            *dest = ' ';
        }
        else if(*src == '%')
        {
             int code;
             //取两位十六进制，写入到code中。
             if(sscanf(src+1, "%2x", &code) != 1) code = '?';
             *dest = code;
             src +=2; 
        }     
        else
        {
            *dest = *src;
        }
    }
    
    //*dest = '\n';
    *dest = '\0';
    
    return 0;
}

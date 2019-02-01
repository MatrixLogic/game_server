//#include "ServerInc.h"
#include <curl/curl.h>
#include "LogicInc.h"
#include <sys/epoll.h>
#include <fcntl.h>

using namespace std;


int send_html_end();
int send_html_start();

int stringsplit(string data, string delim, vector<string> &vctstr);
int StringReplace(string& sData, const string& sSrc, const string& sDst);
int unencode(char *src, char *last, char *dest);

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::MessageFactory;
using google::protobuf::Reflection;

class SimulationWebClient
{
public:
	SimulationWebClient(string openid, string serverip, string reqname, string &data);

	~SimulationWebClient();

	int Init();

	int Run();

	//发送请求协议
	int SendReqProtocol();

	string GetErrmsg() {return m_errmsg;}
private:
	int ConnectServer();

	int SetPoll();  //设置epoll

	int ListenPoll();  //监听epoll

	int LoginCGI(unsigned uid, Common::Login * msg);

	int OutPutNotify();

	int InputNotify();

	int HandleResp(Message * msg);

	int ProcessLogin();

	bool DecodePacketHead(CBufferReader & reader);

	bool _j2p(Message* msg, Json::Value &json);

	bool _p2j(const Message* msg, Json::Value &json);

	int SendData();

	int EnableOutPut();

	int DisableOutPut();

	int EnableInput();

	int DisableInput();

	//创建发送的proto协议
	int CreateProto(Message **);

private:
	string	m_openid;
	string	m_serverip;
	string  m_reqname;
	string 	m_errmsg;
	string &	m_data;
	unsigned m_bodylen;
	int m_sockfd;
	int m_epollfd;
	int m_event;
	bool m_isStop;
	bool m_isLogin;
	bool m_isProcess;

	bool m_isNoReply;
	struct epoll_event* m_events;
	CSG17Packet m_packet;
	CBuffer m_recvbuffer;
	CBuffer m_sendbuffer;
};

int SendData(int sockfd);


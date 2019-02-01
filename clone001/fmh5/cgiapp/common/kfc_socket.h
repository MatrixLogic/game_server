#ifndef __KFC_SOCKET_H__
#define __KFC_SOCKET_H__

#include <string>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <stdexcept>

namespace kfc
{
	struct socket_error: public std::runtime_error
	{
		socket_error(const std::string& s):std::runtime_error(s){}
	};
	struct socket_again: public std::runtime_error
	{
		socket_again(const std::string& s):std::runtime_error(s){}
	};
	struct socket_intr: public std::runtime_error
	{
		socket_intr(const std::string& s):std::runtime_error(s){}
	};

	typedef in_addr_t numeric_ipv4_t;
	typedef sa_family_t family_t;
	typedef uint16_t port_t;

	class CSocket
	{
	public:
		// Construction
		CSocket():_socket_fd(INVALID_SOCKET){}
		~CSocket(){close();}
		void create(int protocol_family = PF_INET, int socket_type = SOCK_STREAM);
		void attach(int fd){_socket_fd = fd;}

	public:
		// Attributes, gettor & settor
		int fd() const {return _socket_fd;}
		bool socket_is_ok() const {return(_socket_fd != INVALID_SOCKET);}

		void get_peer_name(numeric_ipv4_t& peer_address, port_t& peer_port);
		void get_sock_name(numeric_ipv4_t& socket_address, port_t& socket_port);

	public:
		// Operations
		void bind(port_t port);
		void bind(numeric_ipv4_t addr, port_t port);
		void listen(int backlog = 32);
		void connect(numeric_ipv4_t addr, port_t port);
		void accept(CSocket& client_socket);
		void shutdown(int how = SHUT_RDWR);
		void close();

		int receive   (void * buf, size_t len, int flag = 0);
		int send(const void * buf, size_t len, int flag = 0);

		int receive_from (void* buf, size_t len,
						  numeric_ipv4_t& from_address, port_t& from_port, int flags = 0);
		int send_to(const void* msg, size_t len,
					numeric_ipv4_t to_address, port_t to_port, int flags = 0);

		void set_nonblock();
		int  get_sockerror();
		void set_reuseaddr();
		static void ignore_pipe_signal() {signal(SIGPIPE, SIG_IGN);}

	protected:
		static const int INVALID_SOCKET = -1;
		int _socket_fd;

	private:
		CSocket(const CSocket& sock); // no implementation
		CSocket& operator=(const CSocket& sock); // no implementation
	};

	//////////////////////////////////////////////////////////////////////////

	class CSocketAddr
	{
	public:
		CSocketAddr():_len(sizeof(struct sockaddr_in))
		{memset(&_addr, 0, sizeof(struct sockaddr_in));}

		struct sockaddr * addr()
		{
			return(struct sockaddr *)(&_addr);
		}
		struct sockaddr_in * addr_in()
		{
			return &_addr;
		}
		socklen_t& length(){return _len;}

		numeric_ipv4_t get_numeric_ipv4(){return _addr.sin_addr.s_addr;}
		void set_numeric_ipv4(numeric_ipv4_t ip){_addr.sin_addr.s_addr = ip;}

		bool set_ipv4(const std::string& ip);
		std::string get_ipv4();

		port_t get_port(){return ntohs(_addr.sin_port);}
		void set_port(port_t port){_addr.sin_port = htons(port);}

		family_t get_family(){return _addr.sin_family;}
		void set_family(family_t f){_addr.sin_family = f;}

	private:
		struct sockaddr_in _addr;
		socklen_t _len;
	};


	//////////////////////////////////////////////////////////////////////////
	class CSimpleSocketTcp : public CSocket
	{
	public:
		// Construction
		CSimpleSocketTcp(){}
		~CSimpleSocketTcp(){}

		void create(){CSocket::create(PF_INET, SOCK_STREAM);}

		// Operations
		void bind   (const std::string& addr, port_t  port);
		void connect(const std::string& addr, port_t  port);

		//	recv & send existed, use CSocket version

		// Attributes, gettor
		std::string get_peer_ip();
		std::string get_sock_ip();
		port_t get_peer_port();
		port_t get_sock_port();
	};

	class CSimpleSocketUdp : public CSocket
	{
	public:
		CSimpleSocketUdp(){}
		~CSimpleSocketUdp(){}

		void create(){CSocket::create(PF_INET, SOCK_DGRAM);}
		void bind   (const std::string& addr, port_t  port);

		int receive_from (void* buf, size_t len,
						  std::string& from_address, port_t& from_port, int flags=0);
		int send_to(const void* msg, size_t len,
					const std::string& to_address, port_t to_port, int flags=0);
	};

	static std::string in_n2s(numeric_ipv4_t addr);
	static numeric_ipv4_t in_s2n(const std::string& addr);

	inline std::string in_n2s(numeric_ipv4_t addr)
	{
		char buf[INET_ADDRSTRLEN];
		const char* p = inet_ntop(AF_INET, &addr, buf, sizeof(buf));
		return p ? p : std::string();
	}

	inline numeric_ipv4_t in_s2n(const std::string& addr)
	{
		struct in_addr sinaddr;
		errno = 0;
		int ret = inet_pton(AF_INET, addr.c_str(), &sinaddr);

		if(ret < 0)
			throw socket_error("inet_pton: " + addr + strerror(errno));
		else if(ret == 0)
			throw socket_error(std::string("CSocketAddr::in_s2n: inet_pton: "
										   "does not contain a character string representing a valid "
										   "network address in: ") + addr);

		return sinaddr.s_addr;
	}

}

#endif

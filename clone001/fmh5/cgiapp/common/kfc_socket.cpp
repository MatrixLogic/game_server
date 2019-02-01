

#include "kfc_socket.h"

using namespace std;

void kfc::CSocket::create(int protocol_family /* = PF_INET */, int socket_type /* = SOCK_STREAM */)
{
	//	release fd first
	close();

	//	create and check
	_socket_fd =::socket(protocol_family, socket_type, 0);
	if (_socket_fd < 0)
	{
		throw kfc::socket_error(strerror(errno));
		_socket_fd = INVALID_SOCKET;
	}
}

void kfc::CSocket::bind(kfc::port_t port)
{
	bind(htonl(INADDR_ANY), port);
}

void kfc::CSocket::bind(kfc::numeric_ipv4_t ip, kfc::port_t port)
{
	kfc::CSocketAddr addr;
	addr.set_family(AF_INET);
	addr.set_port(port);
	addr.set_numeric_ipv4(ip);

	errno = 0;
	if (::bind(_socket_fd, addr.addr(), addr.length()) < 0)
		throw kfc::socket_error(strerror(errno));
}

void kfc::CSocket::listen(int backlog /*=32*/)
{
	if (::listen(_socket_fd, backlog) < 0)
		throw kfc::socket_error(strerror(errno));
}

void kfc::CSocket::accept(kfc::CSocket & client_socket)
{
	client_socket.close();
	int client_fd = INVALID_SOCKET;
	errno = 0;
	kfc::CSocketAddr addr;
	client_fd =::accept(_socket_fd, addr.addr(), &addr.length());
	if (client_fd < 0)	//	INVALID_SOCKET < 0
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw kfc::socket_again(strerror(errno));
		else if (errno == EINTR)
			throw kfc::socket_intr (strerror(errno));
		else
			throw kfc::socket_error(strerror(errno));
	}
	client_socket._socket_fd = client_fd;
}

void kfc::CSocket::connect(kfc::numeric_ipv4_t addr, kfc::port_t port)
{
	kfc::CSocketAddr add;
	add.set_family(AF_INET);
	add.set_port(port);
	add.set_numeric_ipv4(addr);
	errno = 0;
	if (::connect(_socket_fd, add.addr(), add.length()) < 0)
		throw kfc::socket_error(string("kfc::CSocket::connect") + strerror(errno));
}

void kfc::CSocket::shutdown(int how /*=SHUT_WR*/)
{
	if (::shutdown(_socket_fd, how) < 0)
		throw kfc::socket_error(strerror(errno));
}

void kfc::CSocket::close()
{
	if (_socket_fd != INVALID_SOCKET)
	{
		//::shutdown(_socket_fd, SHUT_RDWR);
		::close(_socket_fd);
		_socket_fd = INVALID_SOCKET;
	}
}

int kfc::CSocket::receive(void *buf, size_t len, int flag /*=0*/)
{
	errno = 0;
	int bytes =::recv(_socket_fd, buf, len, flag);
	if (bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw socket_again(strerror(errno));
		else if (errno == EINTR)
			throw socket_intr (strerror(errno));
		else
			throw kfc::socket_error(strerror(errno));
	}
	return bytes;
}

int kfc::CSocket::send(const void *buf, size_t len, int flag /*=0*/)
{
	errno = 0;
	int bytes = ::send(_socket_fd, buf, len, flag);
	if (bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw socket_again(strerror(errno));
		else if (errno == EINTR)
			throw socket_intr (strerror(errno));
		else
			throw kfc::socket_error(strerror(errno));
	}
	
	return bytes;
}

int kfc::CSocket::receive_from(void * buf, size_t len, kfc::numeric_ipv4_t& from_address,
								 kfc::port_t& from_port, int flags /*=0*/)
{
	kfc::CSocketAddr addr;
	errno = 0;
	int bytes =::recvfrom(_socket_fd, buf, len, flags, addr.addr(), &addr.length());
	if (bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw socket_again(strerror(errno));
		else if (errno == EINTR)
			throw socket_intr (strerror(errno));
		else
			throw kfc::socket_error(strerror(errno));
	}
	from_address = addr.get_numeric_ipv4();
	from_port = addr.get_port();
	return bytes;
}

int kfc::CSocket::send_to(const void *msg, size_t len,
							kfc::numeric_ipv4_t to_address, kfc::port_t to_port, int flags /*=0*/)
{
	kfc::CSocketAddr addr;
	addr.set_family(AF_INET);
	addr.set_numeric_ipv4(to_address);
	addr.set_port(to_port);
	errno = 0;
	int bytes =::sendto(_socket_fd, msg, len, flags, addr.addr(), addr.length());
	if (bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			throw socket_again(strerror(errno));
		else if (errno == EINTR)
			throw socket_intr (strerror(errno));
		else
			throw kfc::socket_error(strerror(errno));
	}
	return bytes;
}

void kfc::CSocket::get_peer_name(kfc::numeric_ipv4_t & peer_address, kfc::port_t & peer_port)
{
	kfc::CSocketAddr addr;
	if (::getpeername(_socket_fd, addr.addr(), &addr.length()) < 0)
		throw kfc::socket_error(strerror(errno));
	peer_address = addr.get_numeric_ipv4();
	peer_port = addr.get_port();
}

void kfc::CSocket::get_sock_name(kfc::numeric_ipv4_t& socket_address, kfc::port_t & socket_port)
{
	kfc::CSocketAddr addr;
	if (::getsockname(_socket_fd, addr.addr(), &addr.length()) < 0)
		throw kfc::socket_error(strerror(errno));
	socket_address = addr.get_numeric_ipv4();
	socket_port = addr.get_port();
}

void kfc::CSocket::set_reuseaddr()
{
	int optval = 1;
	size_t optlen = sizeof(optval);
	if (::setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen) < 0)
		throw kfc::socket_error(strerror(errno));
}

int kfc::CSocket::get_sockerror()
{
	int error = 0;
	socklen_t optlen = sizeof(error);
	if (::getsockopt(_socket_fd, SOL_SOCKET, SO_ERROR, &error, &optlen) < 0)
		throw kfc::socket_error(strerror(errno));
	return error;
}

void kfc::CSocket::set_nonblock()
{
	int val = fcntl(_socket_fd, F_GETFL, 0);
	if (val == -1)
		throw kfc::socket_error(strerror(errno));
	if (val & O_NONBLOCK)
		return;
	
	if (fcntl(_socket_fd, F_SETFL, val | O_NONBLOCK | O_NDELAY) == -1)
		throw kfc::socket_error(strerror(errno));
}

bool kfc::CSocketAddr::set_ipv4(const  string & ip)
{
	try {
		set_numeric_ipv4(in_s2n(ip));
		return true;
	} catch(socket_error &e) {
		return false;
	}
}

std::string kfc::CSocketAddr::get_ipv4()
{
	return in_n2s(_addr.sin_addr.s_addr);
}

void kfc::CSimpleSocketTcp::connect(const string& addr, kfc::port_t  port)
{
	kfc::CSocket::connect(in_s2n(addr), port);
}
 
void kfc::CSimpleSocketTcp::bind(const string& addr, kfc::port_t port)
{
	kfc::numeric_ipv4_t ip = (addr.empty() || addr== "*") ?
		htonl(INADDR_ANY) : in_s2n(addr);
	kfc::CSocket::bind(ip, port);
}
/*
int kfc::CSyncSocket::sendrecv(const void *sendbuf,int sendlen,void * recvbuf,int recvsize,int flag)
{
	
}
*/
string kfc::CSimpleSocketTcp::get_peer_ip()
{
	kfc::numeric_ipv4_t ip;
	kfc::port_t port;
	get_peer_name(ip, port);
	return in_n2s(ip);
}

string kfc::CSimpleSocketTcp::get_sock_ip()
{
	kfc::numeric_ipv4_t ip;
	kfc::port_t port;
	get_sock_name(ip, port);
	return in_n2s(ip);
}

kfc::port_t kfc::CSimpleSocketTcp::get_peer_port()
{
	kfc::numeric_ipv4_t ip;
	kfc::port_t port;
	get_peer_name(ip, port);
	return port;
}

kfc::port_t kfc::CSimpleSocketTcp::get_sock_port()
{
	kfc::numeric_ipv4_t ip;
	kfc::port_t port;
	get_sock_name(ip, port);
	return port;
}

//////////////////////////////////////////////////////////////////////////

void kfc::CSimpleSocketUdp::bind(const string& addr, kfc::port_t port)
{
	kfc::numeric_ipv4_t ip = (addr.empty() || addr== "*") ?
		htonl(INADDR_ANY) : in_s2n(addr);
	kfc::CSocket::bind(ip, port);
}

int kfc::CSimpleSocketUdp::receive_from(void * buf, size_t len,
										  string& addr, kfc::port_t& port, int flags/* = 0 */)
{
	kfc::numeric_ipv4_t ip;
	int ret = kfc::CSocket::receive_from(buf, len, ip, port, flags);
	addr = in_n2s(ip);
	return ret;
}

int kfc::CSimpleSocketUdp::send_to(const void *msg, size_t len,
									 const string& addr, kfc::port_t port, int flags /* = 0 */)
{
	return kfc::CSocket::send_to(msg, len, in_s2n(addr), port, flags);
}



#include "tproxy_server.h"
#include <linux/netfilter_ipv4.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>


tproxy_server::tproxy_server(boost::asio::io_service &io_service, unsigned short port) :
	server(io_service, port)
{
	int fd = (int)acceptor_.native_handle();
	int opt = 1;
	int result = setsockopt(fd, SOL_IP, IP_TRANSPARENT, &opt, sizeof(opt));
	std::cout << "fd " << fd << ", result " << result << std::endl;
}

tproxy_server::~tproxy_server()
{
}

tcp::endpoint tproxy_server::get_remote()
{
	int fd = (int)socket_.native_handle();
	sockaddr_in remote_addr;
	size_t addr_len = sizeof(remote_addr);
	getsockname(fd, (sockaddr*)&remote_addr, &addr_len);

	unsigned short port = ntohs(remote_addr.sin_port);
	std::uint32_t ip_addr = (std::uint32_t)ntohl(remote_addr.sin_addr.s_addr);
	std::cout << "fd=" << fd << ", ip=" << inet_ntoa(remote_addr.sin_addr) << ", port=" << port << std::endl;
	auto result = tcp::endpoint(address_v4(ip_addr), port);

	return result;
}

tcp::socket tproxy_server::make_connect_socket()
{
	auto socket = server::make_connect_socket();
	int fd = (int)socket.native_handle();
	int opt = 1;
	//int result = setsockopt(fd, SOL_IP, IP_TRANSPARENT, &opt, sizeof(opt));
	return socket;
}
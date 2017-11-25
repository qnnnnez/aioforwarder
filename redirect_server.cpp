#include "redirect_server.h"
#include <linux/netfilter_ipv4.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

redirect_server::redirect_server(boost::asio::io_service &io_service, unsigned short port):
	server(io_service, port)
{
}

redirect_server::~redirect_server()
{
}

tcp::endpoint redirect_server::get_remote()
{
	int fd = (int)socket_.native_handle();
	sockaddr_in remote_addr;
	size_t addr_len = sizeof(remote_addr);
	getsockopt(fd, SOL_IP, SO_ORIGINAL_DST, &remote_addr, &addr_len);
	unsigned short port = ntohs(remote_addr.sin_port);
	std::uint32_t ip_addr = (std::uint32_t)ntohl(remote_addr.sin_addr.s_addr);
	std::cout << "fd=" << fd << ", ip=" << inet_ntoa(remote_addr.sin_addr) << ", port=" << port << std::endl;
	auto result = tcp::endpoint(address_v4(ip_addr), port);
	if (result.address() == socket_.local_endpoint().address())
		result = tcp::endpoint(address_v4(), 0);
	return result;
}
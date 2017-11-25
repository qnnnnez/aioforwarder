#include <memory>
#include <functional>
#include <queue>
#include <iostream>
#include "asio.h"
#include "forward_server.h"

#include <linux/netfilter_ipv4.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main()
{
	boost::asio::io_service io_service;
	tcp::socket socket(io_service);
	//socket.connect()

	forward_server s(io_service, 2333,
		tcp::endpoint(address_v4::from_string("202.202.43.42"), 80));
	io_service.run();
	return 0;
}

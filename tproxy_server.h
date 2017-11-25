#pragma once
#include "server.h"

class tproxy_server :
	public server
{
public:
	tproxy_server(boost::asio::io_service &io_service, unsigned short port);
	~tproxy_server();

protected:
	virtual tcp::endpoint get_remote();
	virtual tcp::socket make_connect_socket();
};

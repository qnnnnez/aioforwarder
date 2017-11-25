#pragma once
#include "server.h"
class forward_server :
	public server
{
public:
	forward_server(boost::asio::io_service &io_service, unsigned short listen_port, tcp::endpoint remote);
	~forward_server();

protected:
	tcp::endpoint remote;

	virtual tcp::endpoint get_remote();
};


#pragma once
#include "server.h"
class redirect_server :
	public server
{
public:
	redirect_server(boost::asio::io_service &io_service, unsigned short port);
	~redirect_server();

protected:
	virtual tcp::endpoint get_remote();
};


#include "forward_server.h"



forward_server::forward_server(boost::asio::io_service &io_service, unsigned short listen_port, tcp::endpoint remote_):
	server(io_service, listen_port), remote(remote_)
{
}


forward_server::~forward_server()
{
}


tcp::endpoint forward_server::get_remote()
{
	return remote;
}
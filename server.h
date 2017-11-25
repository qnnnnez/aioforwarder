#pragma once
#include <memory>
#include "asio.h"

class server
{
public:
	server(boost::asio::io_service &io_service, unsigned short port);
	virtual ~server();

protected:
	virtual void do_accept();
	virtual void do_connect(tcp::socket src, tcp::endpoint remote);
	virtual tcp::endpoint get_remote() = 0;
	virtual tcp::socket make_connect_socket();

	boost::asio::io_service& io_service();

	tcp::acceptor acceptor_;
	tcp::socket socket_;
};

template<typename ConstBufferSequence>
void write_and_close(tcp::socket socket, const ConstBufferSequence data);